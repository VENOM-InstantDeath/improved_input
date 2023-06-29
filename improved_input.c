#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include "vector.h"
#include "getkey.h"

void cbreak(struct termios *tty) {
	tty->c_cc[VTIME] = 0; tty->c_cc[VMIN] = 1;
	tty->c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSADRAIN, tty);
}

void getpos(int *pos) {
	struct pollfd rdfd;
	rdfd.fd = 0;
	rdfd.events = POLLIN;
	fflush(stdin);
	printf("\033[6n");
	fflush(stdout);
	char x[5]; char y[5];
	char p=0; char px=0;
	for (;;) {
		char buff[1];
		poll(&rdfd, 1, 1000);
		read(0, buff, 1);
		switch (buff[0]) {
			case 27:
			case 91:
				continue;
			case 82:
				break;
			case 59:
				p++;
				px=0;
				continue;
			default:
				if (!p) {y[px] = buff[0];px++;}
				else {x[px] = buff[0];px++;}
		}
		if (buff[0] == 82) break;
	}
	pos[0] = atoi(x); pos[1] = atoi(y);
}

struct Args {
	char* str;
	int size;
	char** SRC;
	int SRC_SIZE;
};

void* search(void* argp) {
	struct Args *args = (struct Args*)argp;
	int p = 0;
	int start = 0;
	int state = 0;
	char* res = NULL;
	for (int e=0; e<args->size; e++) {
		for (int i=start; i<args->SRC_SIZE; i++) {
			if (args->SRC[i][p] == args->str[p]) {
				res = args->SRC[i];
				start = i;
				state = 1;
				break;
			}
		}
		if (!state) return 0;
		else state = 0;
		p++;
	}
	printf("\033[%d`", args->size);
	printf("\033[38;2;85;85;85m");
	for (int i=args->size; i<strlen(res); i++) {
		printf("\033[%d`%c", i+1, res[i]);
	}
	printf("\033[0m");
	return res;
}

int improved_input(struct string *str, char**SRC, int src_size) {
	struct termios tty, old;
	tcgetattr(0, &old);
	tty = old;
	cbreak(&tty);
	fflush(stdin);
	int pos[2];getpos(pos);
	int p = 0;
	pthread_t T;
	struct Key key;
	printf("\033[0`");fflush(stdout);
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
				printf("\033[0K");
				for (int i=p; i<str->size; i++) {printf("\033[%d`%c", i+1,str->str[i]);}
				fflush(stdout);
				if (SRC != NULL) {
					struct Args args = {str->str, str->size, SRC, src_size};
					pthread_create(&T, NULL, search, &args);
					pthread_join(T, NULL);
				}
				p++;
				printf("\033[%d`", p+1);
				fflush(stdout);
		}
	}
	tcsetattr(0, TCSADRAIN, &old);
	return 1;
}
