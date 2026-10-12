// Path splitting (0x80015A44-0x80015B64), like _splitpath: backslashes become slashes, then the path is
// cut into directory, file name and extension. The drive part is always empty.
// Names are descriptive guesses; the code is matched against the retail DOL.

extern "C" {
char* strcpy(char* dst, const char* src);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
}

// The drive argument is declared int* (File.cpp's prototype) but written as a byte.
void LevelLoadNames(const char* path, int* drive, char* dir, char* name, char* ext) {
    char buf[512];
    char* p;

    strcpy(buf, path);
    while ((p = strchr(buf, '\\')) != 0) {
        *p = '/';
    }

    p = strrchr(buf, '.');
    if (p) {
        if (ext) {
            strcpy(ext, p);
        }
        *p = 0;
    } else if (ext) {
        *ext = 0;
    }

    p = strrchr(buf, '/');
    if (p) {
        if (name) {
            strcpy(name, p + 1);
        }
        p[1] = 0;
        if (dir) {
            strcpy(dir, buf);
        }
    } else {
        if (name) {
            strcpy(name, buf);
        }
        if (dir) {
            *dir = 0;
        }
    }

    if (drive) {
        *(char*)drive = 0;
    }
}
