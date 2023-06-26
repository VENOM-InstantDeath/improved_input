#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include "vector.h"

void arr_pr(char *arr, int size) {
	printf("{");
	for (int i=0; i<size-1; i++) {
		printf("%d, ", arr[i]);
	}
	printf("%d}\n", arr[size-1]);
}

void cbreak(struct termios *tty) {
	tty->c_cc[VTIME] = 0; tty->c_cc[VMIN] = 1;
	tty->c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSADRAIN, tty);
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
	if (key[0]==91 && key[1]==67) {
		if ((*p) < str->size) {
			printf("\033[1C");
			fflush(stdout);
			(*p)++;
		}
	}
	else if (key[0]==91 && key[1]==68) {
		if (p) {
			printf("\033[1D");
			fflush(stdout);
			(*p)--;
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
		if (ch[0] == 4) {  /*Ctrl+D*/
			string_free(str);
			str->str = NULL;
			printf("\n");
			tcsetattr(0, TCSADRAIN, &old);
			return 0;
		}
		if (ch[0] == 10) {  /*Enter*/
			printf("\n");
			printf("%d\n", str->size);
			string_addch(str, 0);
			tcsetattr(0, TCSADRAIN, &old);
			return 1;
		}
		if (ch[0] == 127) {  /*Backspace*/
			printf("\033[1D \033[1D");
			fflush(stdout);
			if (string_popat(str, p-1)) {
				string_free(str);
				str->str = NULL;
				return 0;
			}
			p--;
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
