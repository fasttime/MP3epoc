#pragma once

#include "IMP3AttributeSetFormatInfo.h"
#include "xsys.h"

#include <cstdint>
#include <string>

namespace MP3epoc
{
    class MP3AttributeSet
    {
    public:
        MP3AttributeSet();
        MP3AttributeInfoBox operator [] (MP3Attribute attribute) const;
        MP3AttributeInfo<BinaryAttributeStatus> & copyright_();
        const MP3AttributeInfo<BinaryAttributeStatus> & copyright_() const;
        MP3AttributeInfo<EmphasisAttributeStatus> & emphasis_();
        const MP3AttributeInfo<EmphasisAttributeStatus> & emphasis_() const;
        MP3AttributeInfo<BinaryAttributeStatus> & original_();
        const MP3AttributeInfo<BinaryAttributeStatus> & original_() const;
        MP3AttributeInfo<BinaryAttributeStatus> & private_();
        const MP3AttributeInfo<BinaryAttributeStatus> & private_() const;
        MP3AttributeSet getSubset(bool wholeFile) const;
        MP3AttributeSet getUnspecified() const;
        int initAttributeStatus(MP3Attribute attribute, int status);
        bool isUnspecified() const;
        static bool isValid(MP3AttributeSet attributeSet);
        bool isWholeFile() const;
        bool matches(MP3AttributeSet attributeSet) const;
        void setWholeFile(bool wholeFile);
        std::xstring
            toString(
            bool useCompactFormat,
            const IMP3AttributeSetFormatInfo & formatInfo =
            DefaultMP3AttributeSetFormatInfo())
            const;
        void updateAttributeStatus(
            MP3Attribute attribute,
            int status,
            bool isKeyFrame
            );
    private:

#pragma warning (push)
#pragma warning (disable: 4201)

        union
        {
            struct
            {
                MP3AttributeInfo<BinaryAttributeStatus>     privateInfo;
                MP3AttributeInfo<BinaryAttributeStatus>     copyrightInfo;
                MP3AttributeInfo<BinaryAttributeStatus>     originalInfo;
                MP3AttributeInfo<EmphasisAttributeStatus>   emphasisInfo;
            };
            uint32_t data;
        };

#pragma warning (pop)

        explicit MP3AttributeSet(uint32_t data);
    };
}
