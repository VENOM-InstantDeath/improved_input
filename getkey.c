#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "vector.h"
#include "getkey.h"

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
	if (*p) {
		printf("\033[1D");
		fflush(stdout);
		(*p)--;
	}
}

void handleEsc(struct pollfd *rdfd, struct Key *key) {
	if (!poll(rdfd, 1, 5)) {
		key->esc = 1;
		return;
	}
	char kch[2]; int keyp=0;
	while (keyp < 2) {
		char ch[1];
		read(0, ch, 1);
		kch[keyp] = ch[0]; keyp++;
	}
	if (kch[0]==91) {
		if (kch[1]==67) {key->arrow=1;return;}
		else if (kch[1]==68) {key->arrow=2;return;}
		else if (kch[1]==49) {
			char ch[3];
			read(0, ch, 3);
			if (ch[2] == 67) {key->arrow=3;return;}
			else if (ch[2] == 68) {key->arrow=4;return;}
		}
	}
	return;
}

void getkey(struct Key *key) {
	key->key = 0;
	key->arrow = 0;
	key->esc = 0;
	struct pollfd rdfd[1];
	rdfd[0].fd = 0;
	rdfd[0].events = POLLIN;
	char ch[1];
	poll(rdfd, 1, -1);
	read(0, ch, 1);
	if (ch[0] == 27) {  /*ESC*/
		handleEsc(rdfd, key);
		return;
	}
	key->key=ch[0];
	return;
}
