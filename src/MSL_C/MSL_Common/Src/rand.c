/* MSL rand.c (Geist's MSL build) */
static unsigned long random_next = 1;

void srand(unsigned int seed) {
    random_next = seed;
}

int rand(void) {
    random_next = random_next * 1103515245 + 12345;
    return (random_next >> 16) & 0x7fff;
}
