#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	char *name = malloc(sizeof(char) * 1025);
	fgets(name, 1024, stdin);
	name[strcspn(name, "\n")] = '\0';
	printf("Hello, %s!\n", name);
	fflush(NULL);
	return -1;
}
