#include "Char16Iterator.h"
#include "shrinkTextWidth.h"

#include <regex>
#include <sstream>

using namespace std;

namespace
{
    xstring
        findNextPart(const xstring & postfix, int width, const xchar *& start);

    xstring
        findNextPart(const xstring & postfix, int width, const xchar *& start)
    {
        const xchar * partStart = start;
        int count = 0;
        const xchar * partEnd = partStart;
        char16_t prevChar = '\0';
        const xchar * postfixEnd = postfix.c_str() + postfix.length();

        start = nullptr;
        for (Char16Iterator iterator(partStart);; iterator++, ++count)
        {
            if (iterator >= postfixEnd)
            {
                if (prevChar != ' ') partEnd = iterator;
                start = nullptr;
                break;
            }
            char16_t ch = *iterator;
            if (prevChar == L'\x2010' || (prevChar != ' ' && ch == ' '))
                partEnd = iterator;
            if (count >= width)
            {
                // Got part end and new part start after it, can break.
                if (start >= partEnd) break;

                // Part end got or not, looking for new part start.
            }
            if (prevChar == L'\x2010' || (prevChar == ' ' && ch != ' '))
            {
                start = iterator;
                if (count >= width) break;
            }
            prevChar = ch;
        }
        return xstring(partStart, partEnd);
    }
}

xstring shrinkTextWidth(const xchar * text, int width)
{
    xostringstream ostream;
    {
        xregex wordRE(XSTR("(\\s*)(\\S.*?)\\s*$"));
        xstring line;
        xistringstream istream(text);
        while (getline(istream, line))
        {
            xcmatch wordMatch;
            regex_search(line.c_str(), wordMatch, wordRE);
            xstring prefix = wordMatch[1];
            xstring postfix = wordMatch[2];
            int partWidth = width - static_cast<int>(prefix.length());
            const xchar * start = postfix.c_str();
            do
            {
                xstring part = findNextPart(postfix, partWidth, start);
                ostream << prefix << part << XSTR('\n');
            }
            while (start);
        }
    }
    return ostream.str();
}
