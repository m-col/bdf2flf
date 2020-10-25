#include <stdio.h>
#include <stdlib.h>

#define PROGNAME "bdf2flf"


void bdf2flf(FILE * in, FILE * out)
{
}


int main(int argc, char *argv[])
{
    if (argc > 1) {
	printf(
	    PROGNAME "< font.bdf > font.flf\n"
	    "bdf data is read from stdin and figlet font data is output to stdout.\n"
	);
	exit(EXIT_SUCCESS);
    }
    
    bdf2flf(stdin, stdout);

    (void)(argv);  // shush the compiler
    return 0;
}
