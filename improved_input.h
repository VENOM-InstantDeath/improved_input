#include <poll.h>
#include "vector.h"
void cbreak(struct termios *tty);
int handleEsc(struct pollfd *rdfd, struct string *str, int p);
int improved_input(struct string *str);
