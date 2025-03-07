#ifndef __MONOTIME_HH
#define __MONOTIME_HH

#ifdef _WIN32
    #include <Windows>
inline unsigned long long mstime() { //==... 待修正
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    double time_in_seconds = (double)counter.QuadPart / frequency.QuadPart;
    printf("High-precision time: %f seconds\n", time_in_seconds);
}
inline unsigned long long nstime() {
}
#else
    #include <ctime>
inline unsigned long long nstime() {
    timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
        return 0;
    }
    return (tp.tv_sec * 1'000'000'000u + tp.tv_nsec);
}
inline unsigned long long mstime() {
    return (nstime() / 1'000'000u);
}
#endif

#endif