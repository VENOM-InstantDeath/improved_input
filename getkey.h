#ifndef GETKEY_H
#define GETKEY_H
struct Key {
	char key;
	char esc;
	char arrow;
};
void right(struct string *str, int *p);
void left(struct string *str, int* p);
void backspace(struct string* str, int *p);
void getkey(struct Key *key);
#endif
