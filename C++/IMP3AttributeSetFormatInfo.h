#pragma once

#include "MP3Attribute.h"
#include "xsys.h"

namespace MP3epoc
{
    class IMP3AttributeSetFormatInfo
    {
    public:
        virtual ~IMP3AttributeSetFormatInfo() = default;
        virtual const xchar * getInvalidSymbol() const = 0;
        virtual xchar getWholeFileChar() const = 0;
        virtual xchar getAttributeChar(MP3Attribute attribute) const = 0;
        virtual xchar
            getCompactFormatChar(MP3AttributeInfoBox attributeInfo) const = 0;
    };

    class DefaultMP3AttributeSetFormatInfo: public IMP3AttributeSetFormatInfo
    {
    public:
        virtual const xchar * getInvalidSymbol() const override;
        virtual xchar getWholeFileChar() const override;
        virtual xchar getAttributeChar(MP3Attribute attribute) const override;
        virtual xchar
            getCompactFormatChar(MP3AttributeInfoBox attributeInfo) const
            override;
    };
}
