#include "MP3AttributeSet.h"

#include <sstream>
#include <stdexcept>

using namespace std;

namespace
{
    const unsigned statusMask       = 0x0f0f0f0f;
    const unsigned wholeFileMask    = 0x10101010;
}

namespace MP3epoc
{
    MP3AttributeSet::MP3AttributeSet(): data(0) { }

    MP3AttributeSet::MP3AttributeSet(uint32_t data): data(data) { }

    MP3AttributeInfoBox
        MP3AttributeSet::operator [] (MP3Attribute attribute) const
    {
        switch (attribute)
        {
        case MP3Attribute::Private:
            return MP3AttributeInfoBox(privateInfo);
        case MP3Attribute::Copyright:
            return MP3AttributeInfoBox(copyrightInfo);
        case MP3Attribute::Original:
            return MP3AttributeInfoBox(originalInfo);
        case MP3Attribute::Emphasis:
            return MP3AttributeInfoBox(emphasisInfo);
        }
        throw out_of_range("MP3Attribute out of range");
    }
    
    MP3AttributeInfo<BinaryAttributeStatus> &
        MP3AttributeSet::copyright_()
    {
        return copyrightInfo;
    }
    
    const MP3AttributeInfo<BinaryAttributeStatus> &
        MP3AttributeSet::copyright_() const
    {
        return copyrightInfo;
    }

    MP3AttributeInfo<EmphasisAttributeStatus> &
        MP3AttributeSet::emphasis_()
    {
        return emphasisInfo;
    }

    const MP3AttributeInfo<EmphasisAttributeStatus> &
        MP3AttributeSet::emphasis_() const
    {
        return emphasisInfo;
    }
    
    MP3AttributeSet MP3AttributeSet::getSubset(bool wholeFile) const
    {
        uint32_t data = this->data;
        for (int shift = 0; shift < 32; shift += 8)
            if (!(data & attributeWholeFileMask << shift) == wholeFile)
            {
                uint32_t mask =
                    (attributeStatusMask | attributeWholeFileMask) << shift;
                data &= ~mask;
            }
        return MP3AttributeSet(data);
    }

    MP3AttributeSet MP3AttributeSet::getUnspecified() const
    {
        return MP3AttributeSet(data & wholeFileMask);
    }

    int MP3AttributeSet::initAttributeStatus(MP3Attribute attribute, int status)
    {
        int shift = 8 * static_cast<int>(attribute);
        int currentStatus = data >> shift & attributeStatusMask;
        if (currentStatus == 0) data |= status << shift;
        return currentStatus;
    }

    bool MP3AttributeSet::isUnspecified() const
    {
        return !(data & statusMask);
    }

    bool MP3AttributeSet::isValid(MP3AttributeSet attributeSet)
    {
        for (
            MP3Attribute attribute = MP3Attribute::First;
            attribute <= MP3Attribute::Last;
            ++attribute)
        {
            MP3AttributeInfoBox attributeInfo = attributeSet[attribute];
            if (!attributeInfo.isValid()) return false;
        }
        return true;
    }

    bool MP3AttributeSet::isWholeFile() const
    {
        return (data & wholeFileMask) != 0;
    }

    bool MP3AttributeSet::matches(MP3AttributeSet attributeSet) const
    {
        uint32_t thatData = attributeSet.data;

        uint32_t xorData = data ^ thatData;

        uint32_t nimpData = xorData & thatData; // = ~data & thatData

        // The set matches if all of its attributes do.
        for (
            uint32_t
            shiftedAttributeStatusMask      = attributeStatusMask,
            shiftedAttributeWholeFileMask   = attributeWholeFileMask;
            shiftedAttributeStatusMask != 0;
            shiftedAttributeStatusMask      <<= 8,
            shiftedAttributeWholeFileMask   <<= 8)
        {
            // An unspecified attribute is always matched.
            if (!(thatData & shiftedAttributeStatusMask)) continue;

            // A specified attribute is never matched by an attribute with a
            // different status.
            if ((xorData & shiftedAttributeStatusMask) != 0) return false;

            // A specified whole-file attribute is never matched by a non-whole-
            // file attribute.
            if ((nimpData & shiftedAttributeWholeFileMask) != 0) return false;
        }
        return true;
    }
    
    MP3AttributeInfo<BinaryAttributeStatus> & MP3AttributeSet::original_()
    {
        return originalInfo;
    }
    
    const MP3AttributeInfo<BinaryAttributeStatus> &
        MP3AttributeSet::original_() const
    {
        return originalInfo;
    }
    
    MP3AttributeInfo<BinaryAttributeStatus> & MP3AttributeSet::private_()
    {
        return privateInfo;
    }
    
    const MP3AttributeInfo<BinaryAttributeStatus> &
        MP3AttributeSet::private_() const
    {
        return privateInfo;
    }

    void MP3AttributeSet::setWholeFile(bool wholeFile)
    {
        data = wholeFile ? data | wholeFileMask : data & ~wholeFileMask;
    }

    xstring
        MP3AttributeSet::toString(
        bool useCompactFormat,
        const IMP3AttributeSetFormatInfo & formatInfo)
        const
    {
        // Cache the current instance.

        MP3AttributeSet attributeSet = *this;

        // See if the current instance is invalid.

        if (!isValid(attributeSet)) return formatInfo.getInvalidSymbol();

        // Creation of the output string begins.

        xostringstream ostream;
        for (
            MP3Attribute attribute = MP3Attribute::First;
            attribute <= MP3Attribute::Last;
            ++attribute)
        {
            MP3AttributeInfoBox attributeInfo = attributeSet[attribute];
            int attributeStatus = attributeInfo.getStatus();

            if (!useCompactFormat)
            {
                xchar attributeChar = formatInfo.getAttributeChar(attribute);
                xchar wholeFileChar =
                    attributeInfo.isWholeFile() ?
                    formatInfo.getWholeFileChar() :
                    XSTR(' ');
                if (attribute != MP3Attribute::Emphasis)
                {
                    xchar sign;
                    switch (static_cast<BinaryAttributeStatus>(attributeStatus))
                    {
                    case BinaryAttributeStatus::Unspecified:
                        sign = XSTR('=');
                        break;
                    case BinaryAttributeStatus::NotSet:
                        sign = XSTR('-');
                        break;
                    case BinaryAttributeStatus::Set:
                        sign = XSTR('+');
                        break;
                    DEFAULT_UNREACHABLE;
                    }
                    ostream <<
                        sign << attributeChar << wholeFileChar << XSTR(' ');
                }
                else
                {
                    xchar predicate;
                    switch (
                        static_cast<EmphasisAttributeStatus>(attributeStatus))
                    {
                    case EmphasisAttributeStatus::Unspecified:
                        predicate = XSTR('.');
                        break;
                    case EmphasisAttributeStatus::None:
                        predicate = XSTR('0');
                        break;
                    case EmphasisAttributeStatus::e50_15_µs:
                        predicate = XSTR('1');
                        break;
                    case EmphasisAttributeStatus::CCITT_j_17:
                        predicate = XSTR('2');
                        break;
                    case EmphasisAttributeStatus::Invalid:
                        predicate = XSTR('x');
                        break;
                    DEFAULT_UNREACHABLE;
                    }
                    ostream << attributeChar << predicate << wholeFileChar;
                }
            }
            else
                ostream << formatInfo.getCompactFormatChar(attributeInfo);
        }
        return ostream.str();
    }

    void MP3AttributeSet::updateAttributeStatus(
        MP3Attribute attribute,
        int status,
        bool isKeyFrame)
    {
        int shift = 8 * static_cast<int>(attribute);
        int currentStatus = data >> shift & attributeStatusMask;
        if (currentStatus == 0)
            data |= status << shift;
        else if (currentStatus != status)
        {
            if (isKeyFrame)
            {
                uint32_t mask =
                    (attributeStatusMask | attributeWholeFileMask) << shift;
                data &= ~mask;
                data |= status << shift;
            }
            else
                data &= ~(attributeWholeFileMask << shift);
        }
    }
}
