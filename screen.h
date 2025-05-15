#ifndef SCREEN_H
#define SCREEN_H

#define EXIT "/exit"
int init_main_screen();
int init_username_screen(char *username, int max_size);
int init_message_box();
int init_text_box();
int read_new_message(char *message, int max_size);
int show_message(char *message);

#endif