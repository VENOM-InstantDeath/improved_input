#include <stdio.h>
#include <termios.h>
#include "improved_input.h"
#include "vector.h"

int main() {
	struct string str; string_init(&str);
	improved_input(&str);
	printf("You entered: %s\n", str.str);
}
