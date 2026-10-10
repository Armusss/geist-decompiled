/* MSL time.c (only time() is linked in Geist) */
typedef unsigned long time_t;
time_t __get_time(void);

time_t time(time_t* timer) {
    time_t t = __get_time();
    if (timer) {
        *timer = t;
    }
    return t;
}
