#include "extras.h"
#include <cctype>

int stricmp(const char* str1, const char* str2) {
	char a_var;
	char b_var;

	do {
		b_var = tolower(*str1++);
		a_var = tolower(*str2++);

		if (b_var < a_var) {
			return -1;
		}
		if (b_var > a_var) {
			return 1;
		}
	} while (b_var != 0);

	return 0;
}

// Geist's extras.c is built at -O4 with MSL's inline ctype (MSL_USE_INLINES), unlike the rest of MSL common.
// The 0x44-byte function before stricmp is strupr (in-place toupper), not strnicmp.
char* strupr(char* str) {
    char* p = str;

    while (*p) {
        *p = toupper(*p);
        p++;
    }

    return str;
}
