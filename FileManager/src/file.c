#include "../include/manager.h"
#include "../include/file.h"

extern int ext_y_max, ext_x_max, int_y_max, int_x_max;

void color_print(WINDOW *wnd, char *str, int color_num) {
    if (color_num == 1 || color_num == 5) wattron(wnd, COLOR_PAIR(color_num));
    else wattron(wnd, COLOR_PAIR(color_num) | A_BOLD);

    wprintw(wnd, "%s", str);

    if (color_num == 1 || color_num == 5) wattroff(wnd, COLOR_PAIR(color_num));
    else wattroff(wnd, COLOR_PAIR(color_num) | A_BOLD);
}

struct stat get_file_stat(working_wnd *window, int index) {
    unsigned char file_path[FILENAME_MAX + 256];
    struct stat file_stat;

    sprintf(file_path, "%s/%s", window->pwd, window->namelist[index]->d_name);
    stat(file_path, &file_stat);

    return file_stat;
}

mode_t get_file_perms(char *file) {
    struct stat file_stat;
    if (stat(file, &file_stat) < 0) return -1;
    else return
    (file_stat.st_mode & S_IRUSR) | (file_stat.st_mode & S_IWUSR) | (file_stat.st_mode & S_IXUSR) |
    (file_stat.st_mode & S_IRGRP) | (file_stat.st_mode & S_IWGRP) | (file_stat.st_mode & S_IXGRP) |
    (file_stat.st_mode & S_IROTH) | (file_stat.st_mode & S_IWOTH) | (file_stat.st_mode & S_IXOTH);
}

void get_next_dir(working_wnd *window, char *file_name) {
    chdir(file_name);
    getcwd(window->pwd, FILENAME_MAX);
}

int define_file_type(working_wnd *window, int index) {
    struct stat file_stat = get_file_stat(window, index);
    int type_mask = file_stat.st_mode & __S_IFMT;
    switch (type_mask) {
        case __S_IFDIR: return DIRECT; break;
        case __S_IFREG: {
            if (!(file_stat.st_mode & S_IXUSR)) return REG;
            else return EXEC;
        } break;
    }
}

int check_access(struct path paths) {
    int src_fd = open(paths.src, O_RDONLY);
    int dst_fd = open(paths.dst, O_CREAT | O_TRUNC | O_WRONLY,
        get_file_perms(paths.src));
    if (src_fd < 0 || dst_fd < 0) {
        close(src_fd);
        close(dst_fd);
        return 0;
    }
    else {
        close(src_fd);
        close(dst_fd);
        return 1;
    }
}

void *copy_file(void *args) {
    struct path *paths = (struct path *) args;
    unsigned char in_buffer[FILENAME_MAX];
    int src_fd = open(paths->src, O_RDONLY);
    int dst_fd = open(paths->dst, O_CREAT | O_TRUNC | O_WRONLY,
        get_file_perms(paths->src));
    int rbyte, wbyte;
    while (rbyte = read(src_fd, in_buffer, FILENAME_MAX)) {
        unsigned char *out_buffer = in_buffer;
        while (rbyte > 0) {
            wbyte = write(dst_fd, out_buffer, rbyte); 
            if (wbyte >= 0) {
                rbyte -= wbyte;
                out_buffer += wbyte;
            }  
        }
    }
    close(src_fd);
    close(dst_fd);
}

void print_all_files(working_wnd *window) {
    int y, x, border;

    wclear(window->subwnd);
    wmove(window->subwnd, 0, 0);

    if (int_y_max < window->entry_cnt) border = int_y_max;
    else border = window->entry_cnt;

    for (int index = 0; index < border; index++) {
        getyx(window->subwnd, y, x);
        print_file(window, index);
        wmove(window->subwnd, y + 1, 0);
    }

    wmove(window->subwnd, 0, 0);
}

void print_file(working_wnd *window, int index) {
    int y, x, text_color;
    unsigned char modif_time[20], file_size[20];
    
    getyx(window->subwnd, y, x);
    struct stat file_stat = get_file_stat(window, index);
    sprintf(file_size, "%ld", file_stat.st_size);
    struct tm *time_struct = localtime(&file_stat.st_ctime);
    strftime(modif_time, sizeof(modif_time), "%d %b %H:%M", time_struct);
    int file_type = define_file_type(window, index);

    if (window->file_num == index) draw_pointer(window);
    
    switch (file_type) {
        case 1: text_color = 3; break;
        case 2: text_color = 1; break;
        case 3: text_color = 6; break;
    }

    wmove(window->subwnd, y, 3);
    color_print(window->subwnd, window->namelist[index]->d_name, text_color);
    wmove(window->subwnd, y, (int_x_max / 2) - 2);
    color_print(window->subwnd, file_size, text_color);
    wmove(window->subwnd, y, int_x_max - 14);
    color_print(window->subwnd, modif_time, text_color);
    wrefresh(window->subwnd);
}