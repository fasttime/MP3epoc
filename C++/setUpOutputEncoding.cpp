#include "setUpOutputEncoding.h"

#if defined(_WIN32)

#include <cstdio>
#include <fcntl.h>
#include <io.h>

void setUpOutputEncoding()
{
    void (* setUpOutputEncoding)(FILE * file) = [] (FILE * file)
    {
        int handle = _fileno(file);
        _setmode(handle, _isatty(handle) ? _O_WTEXT : _O_U8TEXT);
    };
    setUpOutputEncoding(stdout);
    setUpOutputEncoding(stderr);
}

#elif defined(__APPLE__) // #if defined(_WIN32)

void setUpOutputEncoding()
{
    // no satisfactory implementation yet
}

#endif // #if defined(_WIN32)
