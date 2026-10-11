/* MSL time.gamecube.c: seconds since 1900 from the OS tick counter (bus clock / 4 = 40.5 MHz). */
typedef unsigned long time_t;
typedef long long OSTime;
OSTime OSGetTime(void);

time_t __get_time(void) {
    return (time_t)(OSGetTime() / 40500000) + 0xBC17C200;
}
