#pragma once

#include "xsys.h"

#include <iterator>

// Using this class on a malformed input string may result in undefined behavior
// and produce a segmentation fault.

class Char16Iterator: public std::iterator<std::input_iterator_tag, char16_t>
{
public:
    explicit Char16Iterator(const xchar * ptr);
    char16_t operator * () const;
    Char16Iterator & operator ++ ();
    void operator ++ (int);
    operator const xchar * ();
private:
    const xchar * ptr;
};
