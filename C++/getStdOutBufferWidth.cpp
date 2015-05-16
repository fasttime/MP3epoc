#include "getStdOutBufferWidth.h"

#if defined(_WIN32)

#include "Windows API.h"

int getStdOutBufferWidth()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    return
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi) ?
        csbi.dwSize.X : -1;
}

#elif defined(__APPLE__) // #if defined(_WIN32)

#include <sys/ioctl.h>
#include <unistd.h>

int getStdOutBufferWidth()
{
    struct winsize w;
    return ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) >= 0 ? w.ws_col : -1;
}

#endif // #if defined(_WIN32)
