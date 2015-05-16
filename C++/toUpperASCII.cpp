#include "toUpperASCII.h"

xchar lowerToUpperASCII(xchar ch)
{
    return ch + (XSTR('A') - XSTR('a'));
}

xchar toUpperASCII(xchar ch)
{
    return ch >= XSTR('a') && ch <= XSTR('z') ? lowerToUpperASCII(ch) : ch;
}
