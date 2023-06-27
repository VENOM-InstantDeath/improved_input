#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include "vector.h"

void cbreak(struct termios *tty) {
	tty->c_cc[VTIME] = 0; tty->c_cc[VMIN] = 1;
	tty->c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSADRAIN, tty);
}

void backspace(struct string* str, int *p) {
	if (!(*p)) return;;
	string_popat(str, (*p)-1);
	(*p)--;
	printf("\033[1D\033[s\033[0K");
	for (int i=*p; i<str->size; i++) {printf("%c", str->str[i]);}
	printf("\033[u");
	fflush(stdout);
}

void right(struct string *str, int *p) {
	if ((*p) < str->size) {
		printf("\033[1C");
		fflush(stdout);
		(*p)++;
	}
}

void left(struct string *str, int* p) {
	if (p) {
		printf("\033[1D");
		fflush(stdout);
		(*p)--;
	}
}

int handleEsc(struct pollfd *rdfd, struct string *str, int *p) {
	if (!poll(rdfd, 1, 5)) {
		string_free(str);
		str->str = NULL;
		printf("\n");
		return 0;
	}
	char key[2]; int keyp=0;
	while (keyp < 2) {
		char ch[1];
		read(0, ch, 1);
		key[keyp] = ch[0]; keyp++;
	}
	if (key[0]==91) {
		if (key[1]==67) right(str, p);
		else if (key[1]==68) left(str, p);
		else if (key[1]==49) {
			char ch[3];
			read(0, ch, 3);
			if (ch[2] == 67) {
				while (*p != str->size && str->str[(*p)+1] != ' ') right(str, p);
				while (*p != str->size && str->str[(*p)+1] == ' ') right(str, p);
				if (*p != str->size) right(str, p);
			}
			else if (ch[2] == 68) {
				while (*p && str->str[(*p)-1] == ' ') left(str, p);
				while (*p && str->str[(*p)-1] != ' ') left(str, p);
			}
		}
	}
	return 1;
}

int improved_input(struct string *str) {
	struct termios tty, old;
	tcgetattr(0, &old);
	tty = old;
	cbreak(&tty);
	int p = 0;
	struct pollfd rdfd[1];
	rdfd[0].fd = 0;
	rdfd[0].events = POLLIN;
	for (;;) {
		char ch[1];
		poll(rdfd, 1, -1);
		read(0, ch, 1);
		if (ch[0] == 27) {  /*ESC*/
			if (!handleEsc(rdfd, str, &p)) {
				tcsetattr(0, TCSADRAIN, &old);
				return 0;
			}
			continue;
		}
		else if (ch[0] == 4) {  /*Ctrl+D*/
			string_free(str);
			str->str = NULL;
			printf("\n");
			tcsetattr(0, TCSADRAIN, &old);
			return 0;
		}
		else if (ch[0] == 10) {  /*Enter*/
			printf("\n");
			printf("%d\n", str->size);
			string_addch(str, 0);
			tcsetattr(0, TCSADRAIN, &old);
			return 1;
		}
		else if (ch[0] == 127 || ch[0] == 8) {  /*Backspace*/
			backspace(str, &p);
			continue;
		}
		else if (ch[0] == 23) {  /*Ctrl+W*/
			while (p && str->str[(p)-1] == ' ') backspace(str, &p);
			while (p && str->str[(p)-1] != ' ') backspace(str, &p);
			continue;

		}
		string_addchat(str, ch[0], p);
		printf("\033[s\033[0K");
		for (int i=p; i<str->size; i++) {printf("%c", str->str[i]);}
		printf("\033[u\033[1C");
		fflush(stdout);
		p++;
	}
}
