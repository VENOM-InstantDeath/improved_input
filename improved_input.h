#ifndef IMPROVED_INPUT_H
#define IMPROVED_INPUT_H
#include <poll.h>
#include "vector.h"
void cbreak(struct termios *tty);
int improved_input(struct string *str);
#endif
