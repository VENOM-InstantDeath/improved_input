#include <stdio.h>
#include <termios.h>
#include "improved_input.h"
#include "vector.h"

char *SRC[13] = {
	"ada","algol","amiga","ampl",
	"bash","basic",
	"c","c++","carbon",
	"java", "javascript",
	"python",
	"rust"
};

int main() {
	struct string str; string_init(&str);
	improved_input(&str, SRC, 13);
	printf("You entered: %s\n", str.str);
}
