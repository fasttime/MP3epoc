#include "Char16Iterator.h"

Char16Iterator::Char16Iterator(const xchar * ptr): ptr(ptr)
{ }

Char16Iterator & Char16Iterator::operator ++ ()
{
    (*this)++;
    return *this;
}

Char16Iterator::operator const xchar * ()
{
    return ptr;
}

#ifdef _WIN32

char16_t Char16Iterator::operator * () const
{
    return *ptr;
}

void Char16Iterator::operator ++ (int)
{
    ++ptr;
}

#else // #ifdef _WIN32

char16_t Char16Iterator::operator * () const
{
    int ch = static_cast<signed char>(*ptr);
    if (ch >= 0) return static_cast<char16_t>(ch);
    if (ch < static_cast<signed char>(0xc0))
        return static_cast<char16_t>((ch & 0x1f) << 6 | (ptr[1] & 0x3f));
    return
        static_cast<char16_t>
        ((ch & 0x0f) << 12 | (ptr[1] & 0x3f) << 6 | (ptr[2] & 0x3f));
}

void Char16Iterator::operator ++ (int)
{
    while (static_cast<signed char>(*++ptr) < static_cast<signed char>(0xc0));
}

#endif // #ifdef _WIN32
