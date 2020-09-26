#ifndef DEFINES
#define DEFINES

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/udp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <locale.h>
#include <unistd.h>
#include <getopt.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MTU 1500
#define SRC_PORT 6000
#define DEST_IP "192.168.2.186"
#define DEST_PORT 7000

unsigned char dest_mac[6] = { 0x08, 0x00, 0x27, 0x79, 0xdf, 0x5d };

#endif