#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "screen.h"


int HEIGHT, WIDTH;
static int cursor_y_pos = 1;
WINDOW *message_box;
WINDOW *text_box;

int init_main_screen() {
    initscr();
    getmaxyx(stdscr, HEIGHT, WIDTH);
}

int init_username_screen(char *username, int max_size) {
    int username_box_startx, username_box_starty, username_box_nlines, username_box_ncols;

    username_box_nlines = (1 * HEIGHT) / 5;
    username_box_ncols = (2 * WIDTH) / 5;
    username_box_startx = (WIDTH - username_box_ncols) / 2;
    username_box_starty = (HEIGHT - username_box_nlines) / 2;
    
    WINDOW *username_box = newwin(username_box_nlines, username_box_ncols, username_box_starty, username_box_startx);
    box(username_box, 0, 0);

    wrefresh(username_box);
    wmove(username_box, (username_box_nlines / 2) - 1, 5);
    wprintw(username_box, "Enter your username: ");
    wgetnstr(username_box, username, max_size - 1);  // NOTE: wgetstr excludes newline and replaces it with a null terminator
    delwin(username_box);
    refresh();
    return strlen(username);
    
}

int init_message_box() {
    
    int message_box_startx, message_box_starty, message_box_nlines, message_box_ncols;
    
    message_box_nlines = (2 * HEIGHT) / 3;
    message_box_ncols =  (2 * WIDTH) / 3;
    message_box_startx = (WIDTH - message_box_ncols) / 2;
    message_box_starty = (HEIGHT - message_box_nlines) / 3;
    
    // draw the windows 
    message_box = newwin(message_box_nlines, message_box_ncols, message_box_starty, message_box_startx);
    box(message_box, 0 , 0);
    
    wrefresh(message_box);

}

int init_text_box() {
    int text_box_startx, text_box_starty, text_box_nlines, text_box_ncols;

    // define window dimensions
    text_box_nlines = HEIGHT / 9;
    text_box_ncols = (2 * WIDTH) / 3;
    text_box_startx = (WIDTH - text_box_ncols) / 2;
    text_box_starty = (4 *  HEIGHT) / 5;

    text_box = newwin(text_box_nlines, text_box_ncols , text_box_starty, text_box_startx);
    box(text_box, 0 , 0);
    wmove(text_box, (text_box_nlines / 2), 1);

    wrefresh(text_box);
}


int read_new_message(char *message, int max_size) {
    wgetnstr(text_box, message, max_size - 1); // -1 for including at most MAXSIZE characters + the null terminator
    wclear(text_box);
    init_text_box();
    wrefresh(text_box);
    return strlen(message) + 1; // +1 for null terminator

}

int show_message(char *message) {
    int y, x;
    getyx(text_box, y, x);
    wmove(message_box, cursor_y_pos, 1);
    wprintw(message_box,"%s", message);
    wmove(text_box, y, x);
    wrefresh(message_box);
    wrefresh(text_box);
    cursor_y_pos++;
}
