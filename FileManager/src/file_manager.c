#include "../header/file_manager.h"

int y_max, x_max;

int filter_names(const struct dirent *entry) {
    return strcmp(entry->d_name, ".") != 0;
}

int cmp_names(const void *entry_1, const void *entry_2) {
    struct dirent **ent_1 = (struct dirent **) entry_1;
    struct dirent **ent_2 = (struct dirent **) entry_2;
    return strcmp((*ent_1)->d_name, (*ent_2)->d_name);
}

void init_screen() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_RED, COLOR_CYAN);
    init_pair(3, COLOR_WHITE, COLOR_CYAN);
    init_pair(4, COLOR_RED, COLOR_WHITE);
    getmaxyx(stdscr, y_max, x_max);
}

void init_windows(struct filelist *windows[]) {
    windows[0]->wnd = newwin(y_max, x_max / 2, 0, 0);
    windows[1]->wnd = newwin(y_max, x_max / 2, 0, x_max / 2);
    for (int i = 0; i <= 1; i++) {   
        box(windows[i]->wnd, 0, 0);
        wbkgd(windows[i]->wnd, COLOR_PAIR(1));
        keypad(windows[i]->wnd, TRUE);
        windows[i]->scrl_border[0] = 0;
        windows[i]->scrl_border[1] = y_max - 3;
        windows[i]->file_ptr = 0;
        windows[i]->change_dir = 1;
        strcpy(windows[i]->pwd, "/");
    }
    windows[0]->is_active = 1;
    windows[1]->is_active = 0;
}

void read_dir(struct filelist *window) {
    if (window->change_dir) {
        window->entry_cnt = scandir(window->pwd, &window->namelist, filter_names, NULL);
        window->change_dir = 0;
        define_file_types(window, window->entry_cnt);
        qsort(window->namelist, window->entry_cnt, sizeof(struct dirent *), cmp_names);
        if (window->entry_cnt < window->scrl_border[1]) {
            window->scrl_border[1] = window->entry_cnt;
        }
    }
    wclear(window->wnd);
    box(window->wnd, 0, 0);
    print_top_line(window);
    for (int i = window->scrl_border[0]; i < window->scrl_border[1]; i++) {
        print_file_inf(window, i);
    }
    wrefresh(window->wnd);
}

void define_file_types(struct filelist *window, int entry_cnt) {
    for (int i = 0; i < entry_cnt; i++) {
        struct stat file_stat = get_file_stat(window, i);
        if ((file_stat.st_mode & S_IFMT) == S_IFDIR) {
            string_shift(window->namelist[i]->d_name);
            window->namelist[i]->d_name[0] = '/';
        }
    }
}

void string_shift(char *str) {
    int str_size = strlen(str);
    str[str_size + 1] = 0;
    for (int i = str_size; i > 0; i--) {
        str[i] = str[i - 1];
    }
}

struct stat get_file_stat(struct filelist *window, int index) {
    unsigned char file_path[FILENAME_MAX];
    struct stat file_stat;
    strcpy(file_path, window->pwd);
    strcat(file_path, "/\0");
    strcat(file_path, window->namelist[index]->d_name);
    stat(file_path, &file_stat);
    return file_stat;
}

void get_next_dir(struct filelist *window, char *file_name) {
    strcat(window->pwd, file_name);
    chdir(window->pwd);
    getcwd(window->pwd, FILENAME_MAX);
}

void print_top_line(struct filelist *window) {
    int wnd_y, wnd_x;
    getmaxyx(window->wnd, wnd_y, wnd_x);
    wmove(window->wnd, 0, 2);
    wattron(window->wnd, A_STANDOUT);
    wprintw(window->wnd, "%s", window->pwd);
    wattroff(window->wnd, A_STANDOUT);
    wmove(window->wnd, 1, 4);
    wattron(window->wnd, COLOR_PAIR(2));
    wprintw(window->wnd, "Name");
    wmove(window->wnd, 1, (wnd_x / 2) - 2);
    wprintw(window->wnd, "Size");
    wmove(window->wnd, 1, wnd_x - 14);
    wprintw(window->wnd, "Modif. time");
    wattroff(window->wnd, COLOR_PAIR(2));
}

void print_file_inf(struct filelist *window, int index) {
    int y, x, wnd_y, wnd_x, color;
    unsigned char time_str[20];
    getyx(window->wnd, y, x);
    getmaxyx(window->wnd, wnd_y, wnd_x);
    if (window->file_ptr == index) {
        if (window->is_active) color = 4;
        else color = 2;
        wattron(window->wnd, COLOR_PAIR(color));
        wmove(window->wnd, y + 1, 1);
        wprintw(window->wnd, "->");
        wattroff(window->wnd, COLOR_PAIR(color));
    }
    wmove(window->wnd, y + 1, 4);
    if (window->namelist[index]->d_name[0] == '/') {
        wattron(window->wnd, COLOR_PAIR(3));
        wprintw(window->wnd, "%s", window->namelist[index]->d_name);
        wattroff(window->wnd, COLOR_PAIR(3));
    }
    else wprintw(window->wnd, "%s", window->namelist[index]->d_name);
    struct stat file_stat = get_file_stat(window, index);
    wmove(window->wnd, y + 1, (wnd_x / 2) - 2);
    wprintw(window->wnd, "%d", file_stat.st_size);
    wmove(window->wnd, y + 1, wnd_x - 14);
    struct tm *time = localtime(&file_stat.st_ctime);
    strftime(time_str, sizeof(time_str), "%d %b %H:%M", time);
    wprintw(window->wnd, "%s", time_str);
}

void run() {
    int ch, cur_wnd = 0;
    struct filelist **windows = (struct filelist **) malloc(2 * sizeof(struct filelist *));
    windows[0] = (struct filelist *) malloc(sizeof(struct filelist));
    windows[1] = (struct filelist *) malloc(sizeof(struct filelist));
    setlocale(LC_ALL, "");
    init_screen();
    init_windows(windows);
    read_dir(windows[0]);
    read_dir(windows[1]);
    while (1) {  
        switch (ch = wgetch(windows[cur_wnd]->wnd)) {
            case KEY_UP: {
                if (windows[cur_wnd]->file_ptr > windows[cur_wnd]->scrl_border[0]) {
                    windows[cur_wnd]->file_ptr--;
                }
                else if (windows[cur_wnd]->file_ptr > 0) {
                    windows[cur_wnd]->file_ptr--;
                    windows[cur_wnd]->scrl_border[0]--, windows[cur_wnd]->scrl_border[1]--;
                }
            } break;
            case KEY_DOWN: {
                if (windows[cur_wnd]->file_ptr < windows[cur_wnd]->scrl_border[1] - 1) {
                    windows[cur_wnd]->file_ptr++;
                }
                else if (windows[cur_wnd]->file_ptr < windows[cur_wnd]->entry_cnt - 1) {
                    windows[cur_wnd]->file_ptr++;
                    windows[cur_wnd]->scrl_border[0]++, windows[cur_wnd]->scrl_border[1]++;
                }
            } break;
            case 10: {
                unsigned char name[FILENAME_MAX];
                strcpy(name, windows[cur_wnd]->namelist[windows[cur_wnd]->file_ptr]->d_name);
                if (name[0] == '/') {
                    get_next_dir(windows[cur_wnd], name);
                    windows[cur_wnd]->file_ptr = 0;
                    windows[cur_wnd]->scrl_border[0] = 0;
                    windows[cur_wnd]->scrl_border[1] = y_max - 3;
                    windows[cur_wnd]->change_dir = 1;
                }
            } break;
            case 9: {
                windows[cur_wnd]->is_active = 0;
                read_dir(windows[cur_wnd]);
                if (cur_wnd == 0) cur_wnd = 1;
                else cur_wnd = 0;
                windows[cur_wnd]->is_active = 1;
            } break;
            case 27: {
                endwin();
                for (int i = 0; i <= 1; i++) {
                    delwin(windows[i]->wnd);
                    free(windows[i]->namelist);
                }
                free(windows);
                exit(EXIT_SUCCESS);
            } break;
        }
        read_dir(windows[cur_wnd]);
    }
}