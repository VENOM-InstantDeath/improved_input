#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "vector.h"
#include "getkey.h"

void cbreak(struct termios *tty) {
	tty->c_cc[VTIME] = 0; tty->c_cc[VMIN] = 1;
	tty->c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSADRAIN, tty);
}

int improved_input(struct string *str) {
	struct termios tty, old;
	tcgetattr(0, &old);
	tty = old;
	cbreak(&tty);
	int p = 0;
	struct Key key;
	for (;;) {
		getkey(&key);
		if (key.esc) {
			string_free(str);
			str->str = NULL;
			printf("\n");
			tcsetattr(0, TCSADRAIN, &old);
			return 0;
		}
		switch (key.arrow) {
			case 1: right(str, &p);continue;
			case 2: left(str, &p);continue;
			case 3:
				while (p != str->size && str->str[p+1] != ' ') right(str, &p);
				while (p != str->size && str->str[p+1] == ' ') right(str, &p);
				if (p != str->size) right(str, &p);
				continue;
			case 4:
				while (p && str->str[p-1] == ' ') left(str, &p);
				while (p && str->str[p-1] != ' ') left(str, &p);
				continue;
		}
		switch (key.key) {
			case 4:
				string_free(str);
				str->str = NULL;
				printf("\n");
				tcsetattr(0, TCSADRAIN, &old);
				return 0;
			case 10:
				printf("\n");
				printf("%d\n", str->size);
				string_addch(str, 0);
				tcsetattr(0, TCSADRAIN, &old);
				return 1;
			case 127:
			case 8:
				backspace(str, &p);
				continue;
			case 23:
				while (p && str->str[(p)-1] == ' ') backspace(str, &p);
				while (p && str->str[(p)-1] != ' ') backspace(str, &p);
				continue;
			default:
				string_addchat(str, key.key, p);
				printf("\033[s\033[0K");
				for (int i=p; i<str->size; i++) {printf("%c", str->str[i]);}
				printf("\033[u\033[1C");
				fflush(stdout);
				p++;
		}
	}
	tcsetattr(0, TCSADRAIN, &old);
	return 1;
}
