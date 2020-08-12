#include "../include/manager.h"
#include "../include/file.h"

int ext_y_max, ext_x_max, int_y_max, int_x_max;

int filter_names(const struct dirent *entry) {
    return strcmp(entry->d_name, ".") != 0;
}

void init_screen(working_wnd *windows[]) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    start_color();
    init_pair(2, COLOR_YELLOW, COLOR_CYAN); // для неактивного указателя
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // для активного указателя
    init_pair(1, COLOR_BLACK, COLOR_CYAN); // для обычного файла (не исполняемого)
    init_pair(6, COLOR_GREEN, COLOR_CYAN); // для исполняемого файла
    init_pair(3, COLOR_WHITE, COLOR_CYAN); // для каталогов
    init_pair(5, COLOR_BLACK, COLOR_WHITE); // для текущего каталога
    init_pair(7, COLOR_WHITE, COLOR_RED); // для информационного окна
    init_windows(windows);
}

void init_windows(working_wnd *windows[]) {
    int y_max, x_max;
    getmaxyx(stdscr, y_max, x_max);

    msg_wnd = newwin(y_max / 2, x_max / 2, y_max / 4, x_max / 4);
    windows[0]->wnd = newwin(y_max, x_max / 2, 0, 0);
    windows[1]->wnd = newwin(y_max, x_max / 2, 0, x_max / 2);
    windows[0]->subwnd = derwin(windows[0]->wnd, y_max - 3, x_max / 2 - 2, 2, 1);
    windows[1]->subwnd = derwin(windows[1]->wnd, y_max - 3, x_max / 2 - 2, 2, 1);
    wbkgd(msg_wnd, COLOR_PAIR(7) | A_BOLD);

    for (int i = 0; i <= 1; i++) {   
        wbkgd(windows[i]->wnd, COLOR_PAIR(1));
        wbkgd(windows[i]->subwnd, COLOR_PAIR(1));
        keypad(windows[i]->subwnd, TRUE);
        scrollok(windows[i]->subwnd, TRUE);
        windows[i]->file_num = 0;
        windows[i]->change_dir = 1;
        strcpy(windows[i]->pwd, "/");
        getmaxyx(windows[i]->wnd, ext_y_max, ext_x_max);
        getmaxyx(windows[i]->subwnd, int_y_max, int_x_max);
        draw_ext_window(windows[i]);
    }

    windows[0]->is_active = 1;
    windows[1]->is_active = 0;
}

void *show_message(void *mes) {
    unsigned char *message = (unsigned char *) mes;
    int mes_wnd_x, mes_wnd_y;
    getmaxyx(msg_wnd, mes_wnd_y, mes_wnd_x);
    int str_x = (strlen(message) >= mes_wnd_x) ? 1 : (mes_wnd_x - strlen(message)) / 2;
    wclear(msg_wnd);
    mvwprintw(msg_wnd, (mes_wnd_y / 2) - 1, str_x, "%s", message);
    wrefresh(msg_wnd);
}

void draw_ext_window(working_wnd *window) {
    wclear(window->wnd);
    box(window->wnd, 0, 0);
    wmove(window->wnd, 0, 2);
    color_print(window->wnd, window->pwd, 5);
    wmove(window->wnd, 1, 4);
    color_print(window->wnd, "Name", 2);
    wmove(window->wnd, 1, (ext_x_max / 2) - 2);
    color_print(window->wnd, "Size", 2);
    wmove(window->wnd, 1, ext_x_max - 15);
    color_print(window->wnd, "Modif. time", 2);
    wrefresh(window->wnd);
}

void draw_int_window(working_wnd *window) {
    if (window->change_dir) {
        window->entry_cnt = scandir(window->pwd, &window->namelist,
            filter_names, alphasort);
        window->change_dir = 0;
        print_all_files(window);
    }
    else print_file(window, window->file_num);
}

void draw_pointer(working_wnd *window) {
    int y, x, color;
    getyx(window->subwnd, y, x);

    if (window->is_active) color = 4;
    else color = 2;
    
    if (y != 0) {
        wmove(window->subwnd, y - 1, 0);
        color_print(window->subwnd, "  ", 1);
    }
    if (y != int_y_max - 1) {
        wmove(window->subwnd, y + 1, 0);
        color_print(window->subwnd, "  ", 1);
    }

    wmove(window->subwnd, y, 0);
    color_print(window->subwnd, "->", color);
}

void run() {
    working_wnd **windows = (working_wnd **) malloc(2 * sizeof(working_wnd *));
    for (int index = 0; index <= 1; index++) {
        windows[index] = (working_wnd *) malloc(sizeof(working_wnd));
    }

    setlocale(LC_ALL, "");
    init_screen(windows);
    draw_int_window(windows[0]);
    draw_int_window(windows[1]);

    int ch, subwnd_y, subwnd_x, cur_wnd = 0;
    while (1) {
        getyx(windows[cur_wnd]->subwnd, subwnd_y, subwnd_x);
        switch (ch = wgetch(windows[cur_wnd]->subwnd)) {
            case KEY_UP: {
                if (windows[cur_wnd]->file_num > 0) {
                    windows[cur_wnd]->file_num--;
                    if (subwnd_y == 0) wscrl(windows[cur_wnd]->subwnd, UP);
                    else wmove(windows[cur_wnd]->subwnd, subwnd_y - 1, 0);
                }
            } break;
            case KEY_DOWN: {
                if (windows[cur_wnd]->file_num < windows[cur_wnd]->entry_cnt - 1) {
                    windows[cur_wnd]->file_num++;
                    if (subwnd_y == ext_y_max - 4) wscrl(windows[cur_wnd]->subwnd, DOWN);
                    else wmove(windows[cur_wnd]->subwnd, subwnd_y + 1, 0);
                }
            } break;
            case KEY_F(5): {
                int unactive_wnd = cur_wnd ^ 1;
                unsigned char file_name[256], message[256];
                struct path paths;

                if (define_file_type(windows[cur_wnd], windows[cur_wnd]->file_num) == REG) {
                    strcpy(file_name, windows[cur_wnd]->namelist[windows[cur_wnd]->file_num]->d_name);
                    sprintf(paths.src, "%s/%s", windows[cur_wnd]->pwd, file_name);
                    sprintf(paths.dst, "%s/%s", windows[unactive_wnd]->pwd, file_name);
                    strcpy(message, "Копирование файла...");

                    if (check_access(paths)) {
                        pthread_t copy_tid, message_tid;
                        pthread_create(&copy_tid, NULL, copy_file, (void *) &paths);
                        pthread_create(&message_tid, NULL, show_message, (void *) message);
                        pthread_join(copy_tid, NULL);
                        usleep(500);
                    }
                    else {
                        show_message("Не удалось выполнить копирование");
                        sleep(1);
                    }

                    for (int i = 0; i <= 1; i++) {
                        windows[i]->file_num = 0;
                        windows[i]->change_dir = 1;
                        draw_ext_window(windows[i]);
                        draw_int_window(windows[i]);
                    }
                    continue;
                }
            } break;
            case 10: {
                unsigned char file_name[FILENAME_MAX + 256];
                sprintf(file_name, "%s/%s", windows[cur_wnd]->pwd,
                windows[cur_wnd]->namelist[windows[cur_wnd]->file_num]->d_name);
                int file_type = define_file_type(windows[cur_wnd], windows[cur_wnd]->file_num);
                switch (file_type) {
                    case DIRECT: {
                        get_next_dir(windows[cur_wnd], file_name);
                        draw_ext_window(windows[cur_wnd]);
                        windows[cur_wnd]->file_num = 0;
                        windows[cur_wnd]->change_dir = 1;
                    } break;
                    case EXEC: {
                        if (fork() == 0) {
                            execl("/usr/bin/xterm", "/usr/bin/xterm",
                                "-hold", "-e", file_name, NULL);
                            exit(EXIT_FAILURE);
                        }
                        else wait(NULL);
                    } break;
                }
            } break;
            case 9: {
                windows[cur_wnd]->is_active = 0;
                draw_int_window(windows[cur_wnd]);
                cur_wnd ^= 1;
                windows[cur_wnd]->is_active = 1;
            } break;
            case 27: {
                for (int i = 0; i <= 1; i++) {
                    delwin(windows[i]->wnd);
                    delwin(windows[i]->subwnd);
                    for (int j = 0; j < windows[i]->entry_cnt; j++) {
                        free(windows[i]->namelist[j]);
                    }
                    free(windows[i]->namelist);
                    free(windows[i]);
                }
                delwin(msg_wnd);
                endwin();
                exit(EXIT_SUCCESS);
            } break;
        }
        draw_int_window(windows[cur_wnd]);
    }
}