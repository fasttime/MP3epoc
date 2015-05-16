#include "IMP3AttributeSetFormatInfo.h"
#include "toUpperASCII.h"

#include <stdexcept>

using namespace std;

namespace MP3epoc
{
    const xchar * DefaultMP3AttributeSetFormatInfo::getInvalidSymbol() const
    {
        // While the IMP3AttributeSetFormatInfo interface does not require
        // culture invariance, having this accessor return a localized string
        // would make round-trip parsing difficult.
        return XSTR("Invalid");
    }

    xchar DefaultMP3AttributeSetFormatInfo::getWholeFileChar() const
    {
        return XSTR('*');
    }

    xchar
        DefaultMP3AttributeSetFormatInfo::getAttributeChar(
        MP3Attribute attribute)
        const
    {
        switch (attribute)
        {
        case MP3Attribute::Private:
            return XSTR('P');
        case MP3Attribute::Copyright:
            return XSTR('C');
        case MP3Attribute::Original:
            return XSTR('O');
        case MP3Attribute::Emphasis:
            return XSTR('E');
        default:
            throw out_of_range("MP3Attribute out of range");
        }
    }

    xchar
        DefaultMP3AttributeSetFormatInfo::getCompactFormatChar(
        MP3AttributeInfoBox attributeInfo)
        const
    {
        int status = attributeInfo.getStatus();
        if (status == 0) return XSTR(' ');

        const type_info & statusType = attributeInfo.getStatusType();
        char formatChar;
        if (statusType == typeid(BinaryAttributeStatus))
        {
            switch (static_cast<BinaryAttributeStatus>(status))
            {
            case BinaryAttributeStatus::Unspecified:
                return XSTR('.');
            case BinaryAttributeStatus::NotSet:
                formatChar = XSTR('n');
                break;
            case BinaryAttributeStatus::Set:
                formatChar = XSTR('y');
                break;
            default:
                goto argument_exception;
            }
        }
        else if (statusType == typeid(EmphasisAttributeStatus))
        {
            switch (static_cast<EmphasisAttributeStatus>(status))
            {
            case EmphasisAttributeStatus::Unspecified:
                return XSTR('.');
            case EmphasisAttributeStatus::None:
                formatChar = XSTR('n');
                break;
            case EmphasisAttributeStatus::e50_15_µs:
                formatChar = XSTR('m');
                break;
            case EmphasisAttributeStatus::CCITT_j_17:
                // c may be confused with copyright, use j instead.
                formatChar = XSTR('j');
                break;
            case EmphasisAttributeStatus::Invalid:
                formatChar = XSTR('x');
                break;
            default:
                goto argument_exception;
            }
        }
        else
            goto argument_exception;
        return
            attributeInfo.isWholeFile() ?
            lowerToUpperASCII(formatChar) : formatChar;

    argument_exception:
        throw invalid_argument("MP3AttributeInfoBox invalid");
    }
}
