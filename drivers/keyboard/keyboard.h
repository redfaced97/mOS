#ifndef KEYBOARD_H
#define KEYBOARD_H

#define INPUT_BUFFER_SIZE 256

extern char input_buffer[INPUT_BUFFER_SIZE];
extern int buffer_idx;
extern int command_ready;

int kstrcmp(const char *s1, const char *s2);
void clear_input_buffer();
void keyboard_handler_main();

#endif
