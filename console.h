#ifndef CONSOLE_H
#define CONSOLE_H
enum {
    COLOR_WHITE,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_BLACK,
    COLOR_PURPLE,
    COLOR_YELLOW,
    COLOR_CYAN,
    COLOR_ORANGE,
    COLOR_BROWN,
    COLOR_LTRED,
    COLOR_GREY1,
    COLOR_GREY2,
    COLOR_LTGREEN,
    COLOR_LTBLUE,
    COLOR_GREY3,
};

class Console {
  public:
    Console() {

    };
    void clear_screen();
    void cursor_up();
    void cursor_down();
    void cursor_left();
    void cursor_right();
    void move_cursor(int x, int y);
    void set_color();
    void get_cursor(int *x, int *y);

#ifdef _WIN32
    int get_console_info(HANDLE *h, COORD *pos, DWORD *size);
#endif
};

#endif
