#include "../header/text_editor.h"

WINDOW *toolbar_wnd, *filename_wnd;

int fd, x_max, y_max;

unsigned char **content;
int inserting, cur_line, top_line, bot_line, content_size = 0;

void init_screen() {
    initscr();
    cbreak();
    noecho();
    start_color();
    scrollok(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(1);
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    getmaxyx(stdscr, y_max, x_max);
    content = (unsigned char **) malloc(MAX_STRING_COUNT * sizeof(unsigned char *));
    for (int i = 0; i < y_max - 1; i++) new_string(i);
    inserting = 0, cur_line = 1, top_line = 0, bot_line = y_max - 2;
}

void op_sv_file(int mode) {
    filename_wnd = subwin(stdscr, 1, x_max, y_max - 2, 0);
    wbkgd(filename_wnd, COLOR_PAIR(1));
    wclear(filename_wnd);
    wprintw(filename_wnd, "Enter filename: ");
    wrefresh(filename_wnd);
    unsigned char filename[MAX_STRING_COUNT];
    set_canon_mode();
    wscanw(filename_wnd, "%s", filename);
    set_uncanon_mode();
    if (mode == OPEN) fd = open(filename, O_RDWR);
    else if (mode == SAVE) fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd < 0) {
        wclear(filename_wnd);
        wprintw(filename_wnd, "Сould not open file!");
        wrefresh(filename_wnd);
    }
    else {
        if (mode == OPEN) {
            read_file();
            display_content(0, 0, ALL);
        }
        else if (mode == SAVE) {
            for (int i = 0; i < content_size; i++) {
                write(fd, content[i], strlen(content[i]));
            }
            close(fd);
            wclear(filename_wnd);
            wprintw(filename_wnd, "Content saved successfully!");
            wrefresh(filename_wnd);
        }
    }
    delwin(filename_wnd);
}

void read_file() {
    unsigned char ch;
    int str_num = 0, smb_num = 0;
    while (read(fd, &ch, 1)) {
        if (content[str_num] == NULL) new_string(str_num);
        content[str_num][smb_num++] = ch;
        if (ch == '\n' || smb_num == x_max) {
            str_num++;
            smb_num = 0;
        }
    }
    close(fd);
}

void scroll_text(int direction) {
    if (direction == UP) {
        if (top_line != 0) {
            scrl(UP);
            refresh();
            top_line--, bot_line--;
        }
    }
    else if (direction == DOWN) {
        if (bot_line != content_size) {
            scrl(DOWN);
            top_line++, bot_line++;
        }
        if (content[bot_line] == NULL) new_string(content_size);
    }
}

void display_content(int y, int x, int direction) {
    char empty_str[x_max];
    memset(empty_str, ' ', x_max);
    if (direction == UP) {
        move(0, 0);
        if (strlen(content[top_line]) == 0) addstr(empty_str);
        else addstr(content[top_line]);
        refresh();
    }
    else if (direction == DOWN) {
        move(y_max - 2, 0);
        if (strlen(content[bot_line]) == 0) addstr(empty_str);
        else addstr(content[bot_line]);
        refresh();
    }
    else if (direction == ALL) {
        move(0, 0);
        for (int i = top_line; i <= bot_line; i++) {
            if (strlen(content[i]) == 0) addstr(empty_str);
            else addstr(content[i]);
            refresh();
        }
    }
    move(y, x);
}

void draw_toolbar(int y, int x) {
    toolbar_wnd = subwin(stdscr, 1, x_max, y_max - 1, 0);
    wbkgd(toolbar_wnd, COLOR_PAIR(1));
    wclear(toolbar_wnd);
    wmove(toolbar_wnd, 0, 1);
    waddstr(toolbar_wnd, "F2 - OPEN FILE");
    wmove(toolbar_wnd, 0, 17);
    waddstr(toolbar_wnd, "F3 - SAVE FILE");
    wmove(toolbar_wnd, 0, 33);
    waddstr(toolbar_wnd, "F4 - EXIT");
    wmove(toolbar_wnd, 0, 50);
    if (inserting) waddstr(toolbar_wnd, "--INSERTING--");
    else waddstr(toolbar_wnd, "--VIEWING--");
    wmove(toolbar_wnd, 0, 70);
    wprintw(toolbar_wnd, "%d:%d", y, x + 1);
    wrefresh(toolbar_wnd);
    delwin(toolbar_wnd);
}

void set_canon_mode() {
    nocbreak();
    echo();
}

void set_uncanon_mode() {
    cbreak();
    if (inserting == 1) echo();
    else noecho();
}

void new_string(int index) {
    content[index] = (unsigned char *) malloc(x_max * sizeof(unsigned char));
    memset(content[index], 0, x_max);
    content_size++;
}

void run() {
    int ch, x = 0, y = 0;
    init_screen();
    draw_toolbar(cur_line, x);
    while (1) {
        switch (ch = getch()) {
            case KEY_UP: {
                move(y - 1, x);
                if (cur_line != 1) cur_line--;
                if (y == 0) {
                    scroll_text(UP);
                    display_content(y, x, UP);
                }
            } break;
            case KEY_DOWN: {
                move(y + 1, x);
                if (y != MAX_STRING_COUNT) {
                    cur_line++;
                    if (y == y_max - 2) {
                        scroll_text(DOWN);
                        display_content(y, x, DOWN);
                    }
                }
            } break;
            case KEY_LEFT: move(y, x - 1); break;
            case KEY_RIGHT: move(y, x + 1); break;
            case KEY_IC: {
                inserting = 1;
                echo();
            } break;
            case KEY_DC: {
                inserting = 0;
                noecho();
            } break;
            case KEY_F(2): {
                op_sv_file(OPEN);
            } break;
            case KEY_F(3): {
                op_sv_file(SAVE);
            } break;
            case KEY_F(4): {
                for (int i = 0; i < content_size; i++) {
                    free(content[i]);
                }
                free(content);
                endwin();
                exit(EXIT_SUCCESS);
            } break;
            case KEY_BACKSPACE: {
                if (x != 0 && inserting == 1) {
                    mvaddch(y, x - 1, ' ');
                    move(y, x - 1);
                    content[cur_line - 1][x - 1] = ' ';
                }
            } break;
            case 10: {
                if (inserting == 1) {
                    move(y + 1, 0);
                    if (y != MAX_STRING_COUNT) {
                        cur_line++;
                        if (y == y_max - 2) {
                            scroll_text(DOWN);
                            display_content(y, x, DOWN);
                        }
                    }
                    content[cur_line - 1][x] = '\n';
                }
            } break;
            default: {
                if ((ch >= 32 && ch <= 126) && inserting == 1) {
                    content[cur_line - 1][x] = ch;
                }
            } break;
        }
        getyx(stdscr, y, x);
        refresh();
        draw_toolbar(cur_line, x);
    }
}