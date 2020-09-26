#include "../include/defines.h"

void check_value(int val, const char *msg) {
    if (val < 0) {
        perror(msg);
        exit(-1);
    }
}

void get_eth_hdr(struct ethhdr *eth_hdr, struct ifreq ifr) {
    for (int i = 0; i < 6; i++) {
        eth_hdr->h_dest[i] = dest_mac[i];
        eth_hdr->h_source[i] = (unsigned char)
            ifr.ifr_ifru.ifru_hwaddr.sa_data[i];
    }
    eth_hdr->h_proto = htons(ETH_P_IP);
}

void get_ip_hdr(struct iphdr *ip_hdr, struct ifreq ifr) {
    ip_hdr->version = 4;
    ip_hdr->ihl = 5;
    ip_hdr->tos = 0;
    ip_hdr->id = 1;
    ip_hdr->frag_off = 0;
    ip_hdr->protocol = 17;
    ip_hdr->ttl = IPDEFTTL;
    ip_hdr->check = 0;
    ip_hdr->saddr = inet_addr(
        inet_ntoa(((struct sockaddr_in *) &ifr.ifr_ifru.ifru_addr)->sin_addr));
    ip_hdr->daddr = inet_addr(DEST_IP);
}

void get_udp_hdr(struct udphdr *udp_hdr) {
    udp_hdr->source = htons(SRC_PORT);
    udp_hdr->dest = htons(DEST_PORT);
    udp_hdr->check = 0;
}

uint16_t crc16(struct iphdr *ip_hdr) {
    int check_sum = 0;
    unsigned short *ptr = (unsigned short *) ip_hdr;
    for (int i = 0; i < 10; i++) {
        check_sum += *(ptr)++;
    }
    int tmp = check_sum >> 16;
    check_sum = (check_sum & 0xFFFF) + tmp;
    return (uint16_t) ~check_sum;
}

int main(int argc, char* argv[]) {
    system("clear");
    setlocale(LC_ALL, "");

    struct sockaddr_ll eth_server;
    struct sockaddr_in ip_server;

    eth_server.sll_family = AF_PACKET;
    eth_server.sll_ifindex = if_nametoindex("enp0s3");
    eth_server.sll_halen = 6;
    for (int i = 0; i < 6; i++) eth_server.sll_addr[i] = dest_mac[i];

    ip_server.sin_family = AF_INET;
    ip_server.sin_addr.s_addr = inet_addr(DEST_IP);
    ip_server.sin_port = htons(DEST_PORT);

    int sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    check_value(sock_fd, "socket");
    printf("RAW сокет успешно создан\n\n");

    struct ifreq ifr_mac, ifr_ip;
    memset(&ifr_mac, 0, sizeof(ifr_mac));
    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_mac.ifr_name, "enp0s3", IFNAMSIZ - 1);
    strncpy(ifr_ip.ifr_name, "enp0s3", IFNAMSIZ - 1);

    int ioctl_val = ioctl(sock_fd, SIOCGIFHWADDR, &ifr_mac);
    check_value(ioctl_val, "ioctl");

    ioctl_val = ioctl(sock_fd, SIOCGIFADDR, &ifr_ip);
    check_value(ioctl_val, "ioctl");

    int ethhdr_size = sizeof(struct ethhdr),
        iphdr_size = sizeof(struct iphdr), udphdr_size = sizeof(struct udphdr);

    unsigned char buf_in[MTU - ethhdr_size - iphdr_size - udphdr_size],
        buf_snd[MTU], buf_rcv[MTU];

    memset(buf_snd, 0, sizeof(buf_snd));

    struct ethhdr *eth_hdr = (struct ethhdr *) buf_snd;
    struct iphdr *ip_hdr = (struct iphdr *) (buf_snd + ethhdr_size);
    struct udphdr *udp_hdr = (struct udphdr *) (buf_snd + ethhdr_size + iphdr_size);
    
    get_eth_hdr(eth_hdr, ifr_mac);
    get_ip_hdr(ip_hdr, ifr_ip);
    get_udp_hdr(udp_hdr);

    memset(buf_in, 0, sizeof(buf_in));
    printf("$ ");
    fgets(buf_in, sizeof(buf_in), stdin);
    buf_in[strlen(buf_in) - 1] = 0;

    int packet_size = ethhdr_size + iphdr_size + udphdr_size;
    memcpy(buf_snd + packet_size, buf_in, strlen(buf_in));
    packet_size += strlen(buf_in);

    ip_hdr->tot_len = htons(packet_size - ethhdr_size);
    udp_hdr->len = htons(packet_size - ethhdr_size - iphdr_size);
    ip_hdr->check = crc16(ip_hdr);

    sendto(sock_fd, buf_snd, packet_size, 0, (const struct sockaddr *) &eth_server, sizeof(eth_server));

    int serv_st_size = sizeof(ip_server);
    while (1) {
        recvfrom(sock_fd, buf_rcv, sizeof(buf_rcv), 0, (struct sockaddr *) &ip_server, &serv_st_size);
        struct udphdr *rcv_udp_hdr = (struct udphdr *) (buf_rcv + ethhdr_size + iphdr_size);
        if (rcv_udp_hdr->source = DEST_PORT) {
            printf("Сервер прислал сообщение: %s\n\n", buf_rcv + ethhdr_size + iphdr_size + udphdr_size);
            break;
        }
    }

    return 0;
}