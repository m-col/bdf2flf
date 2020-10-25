#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <unistd.h> // ?
#include <limits.h> // ?
#include <errno.h> // ?

#define PROGNAME "bdf2flf"


void bdf2flf(FILE * in, FILE * out)
{
    char linebuf[1024];
    char *s;
    int widest = 0;
    int comments = 2;
    int baseline, old_layout, direction, full_layout, codetags;
    char font[1024];
    char copyright[1024];
    int chars = 0;
    int height = 0;
    int fbb_width = 0;
    int fbb_height = 0;
    int fbb_yoffset = 0;

    for (;;) {
	if (!fgets(linebuf, sizeof(linebuf), in)) {	// EOF
	    break;
	}
	if (!(s = strtok(linebuf, " \t\n\r"))) {	// empty line
	    break;
	}
	if (!strcasecmp(s, "FONTBOUNDINGBOX")) {
	    fbb_width = atoi(strtok(NULL, " \t\n\r"));
	    fbb_height = atoi(strtok(NULL, " \t\n\r"));
	    strtok(NULL, " \t\n\r");
	    fbb_yoffset = atoi(strtok(NULL, " \t\n\r"));

	} else if (!strcasecmp(s, "FONT")) {
	    strcpy(font, strtok(NULL, "\n"));

	} else if (!strcasecmp(s, "COPYRIGHT")) {
	    strcpy(copyright, strtok(NULL, "\n"));
	    comments = 3;

	} else if (!strcasecmp(s, "CHARS")) {
	    chars = atoi(strtok(NULL, " \t\n\r"));
	    break;
	}
    }

    // exit if data could not be read completely
    if (chars <= 0 || fbb_width <= 0 || fbb_height <= 0 || height <= 0) {
	fprintf(stderr, "Could not extract BDF data. Is the input a BDF file?\n");
	exit(EXIT_FAILURE);
    }

    // output header
    fprintf(out, "flf2a$ %i %i %i %i %i %i %i %i\n", height, baseline, widest+2,
	    old_layout, comments, direction, full_layout, codetags);
    fprintf(out, "FIGlet font derived from %s\n", font);
    if (comments > 2)
	fprintf(out, "which has copyright: %s\n", copyright);
    fprintf(out, "Converted to flf with bdf2flf (c) Matt Colligan.\n");

}


int main(int argc, char *argv[])
{
    if (argc > 1) {
	printf(PROGNAME "< input.bdf > output.flf\nbdf data is read from stdin"
		"and figlet font data is output to stdout.\n");
	exit(EXIT_SUCCESS);
    }
    
    bdf2flf(stdin, stdout);

    (void)(argv);  // shush the compiler
    return 0;
}
