#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROGNAME "bdf2flf"


// These are the characters required at the start of FIGfonts.
const unsigned int REQUIRED[] = {
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88,
    89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
    106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
    121, 122, 123, 124, 125, 126, 196, 214, 220, 228, 246, 252, 223
};


struct character {
    unsigned int width;
    unsigned int encoding;
    char *bitmap;
    _Bool printed;
};


void die(char *msg)
{
    fprintf(stderr, msg);
    exit(EXIT_FAILURE);
}


void put_pixel(unsigned int c, unsigned int bit, FILE *out)
{

    if (c & bit) {
	fputc('#', out);
    } else {
	fputc(' ', out);
    }
}


void print_character(FILE *out, char *bitmap, unsigned int width, unsigned int height)
{
    unsigned int y;
    unsigned int x;
    int c;
    unsigned int bit;
    size_t hex_chars = (width + 7) / 8 * 2;
    char *s;
    s = malloc(hex_chars);

    for (y = 0; y < height; y++) {
	memset(s, 0, strlen(s));
	if (y)
	    fprintf(out, "\n");
	for (x = 0; x < hex_chars; x++) {
	    strncat(s, bitmap, 1);
	    bitmap++;
	}

	c = (int)strtol(s, NULL, 16);
	bit = 0x80 * hex_chars / 2;
	for (x = 0; x < width; x++) {
	    put_pixel(c, bit, out);
	    if (bit == 1)
		break;
	    bit = bit / 2;
	}
	fprintf(out, "$@");
    }

    fprintf(out, "@\n");
    free(s);
}


void bdf2flf(FILE * in, FILE * out)
{
    char *s;
    char linebuf[1024];
    char font[1024];
    char copyright[1024];
    int baseline, codetags, i;
    int comments = 3;
    int num_chars = 0;
    int width = 0;
    int height = 0;
    char bitmap[4 * 32];
    int n = 0;
    struct character *chars;
    const unsigned int *j;

    // Some of the bdf-reading logic is derived from bdf2c (c) Lutz Sammer.
    // extract bdf headers
    for (;;) {
	if (!fgets(linebuf, sizeof(linebuf), in)) {	// EOF
	    break;
	}
	if (!(s = strtok(linebuf, " \t\n\r"))) {	// empty line
	    break;
	}
	if (!strcasecmp(s, "FONTBOUNDINGBOX")) {
	    width = atoi(strtok(NULL, " \t\n\r"));
	    height = atoi(strtok(NULL, " \t\n\r"));
	    strtok(NULL, " \t\n\r");
	    baseline = height + atoi(strtok(NULL, " \t\n\r"));

	} else if (!strcasecmp(s, "FONT")) {
	    strcpy(font, strtok(NULL, "\n"));

	} else if (!strcasecmp(s, "COPYRIGHT")) {
	    strcpy(copyright, strtok(NULL, "\n"));
	    comments = 4;

	} else if (!strcasecmp(s, "CHARS")) {
	    num_chars = atoi(strtok(NULL, " \t\n\r"));
	    break;
	}
    }

    if (num_chars <= 0 || width <= 0 || height <= 0)
	die("Could not extract BDF data. Is the input a BDF file?\n");

    if (width > 32 || height > 32)
	die("bdf fonts this large are not supported, sorry.\n");

    chars = malloc(num_chars * sizeof(struct character));
    if (!chars)
	die("Could not allocate memory.\n");

    // extract individual char data
    for (;;) {
	if (!fgets(linebuf, sizeof(linebuf), in))
	    break;
	if (!(s = strtok(linebuf, " \t\n\r")))
	    continue;
	if (!strcasecmp(s, "ENCODING")) {
	    chars[n].encoding = atoi(strtok(NULL, " \t\n\r"));
	} else if (!strcasecmp(s, "BBX")) {
	    chars[n].width = atoi(strtok(NULL, " \t\n\r"));
	} else if (!strcasecmp(s, "BITMAP")) {
	    if (n == num_chars)
		die("bdf font has incorrect CHARS value.\n");
	    i = 0;
	    chars[n].printed = 0;
	    chars[n].bitmap = malloc((width + 7) / 8 * height * 2);
	    chars[n].bitmap[0] = '\0';
	    for (;;) {
		fgets(linebuf, sizeof(linebuf), in);
		s = strtok(linebuf, "\n\r");
		if (!strcasecmp(s, "ENDCHAR")) {
		    break;
		}
		strncat(chars[n].bitmap, s, 4);
		i++;
	    }
	    n++;
	}
    }

    // output flf header
    codetags = 0;
    fprintf(out, "flf2a$ %i %i %i -1 %i 0 16384 %i\n", height, baseline, width+2,
	    comments, codetags);
    fprintf(out, "FIGlet font derived from %s\n", font);
    if (comments > 3)
	fprintf(out, "which has copyright: %s\n", copyright);
    fprintf(out, "Converted to flf with bdf2flf (c) 2020 Matt Colligan.\n\n");

    // print required characters first
    n = 0;
    j = &REQUIRED[0];
    i = 0;
    for (;;) {
	if (chars[n].encoding == *j) {
	    strncpy(bitmap, chars[n].bitmap, ((width + 7) / 8) * height * 2);
	    print_character(out, bitmap, chars[n].width, height);
	    chars[n].printed = 1;
	    if (*j == 223)
		break;
	    j++;
	    i = 0;
	}
	n++;
	i++;
	if (n == num_chars)
	    n = 0;
	if (i == num_chars)
	    break;
    }

    // print additional characters second
    for (n = 0; n < num_chars; n++) {
	if (!chars[n].printed) {
	    strncpy(bitmap, chars[n].bitmap, ((width + 7) / 8) * height * 2);
	    print_character(out, bitmap, chars[n].width, height);
	}
	free(chars[n].bitmap);
    }
}


int main(int argc, char *argv[])
{
    if (argc > 1) {
	printf(PROGNAME " < input.bdf > output.flf\nbdf data is read from stdin"
		" and figlet font data is output to stdout.\n");
	exit(EXIT_SUCCESS);
    }

    bdf2flf(stdin, stdout);

    (void)(argv);  // shush the compiler
    return 0;
}
