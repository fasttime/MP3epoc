#define IMPL_MP3Attribute

#include "MP3Attribute.h"

using namespace std;

namespace MP3epoc
{
    // MP3Attribute

    MP3Attribute & operator ++ (MP3Attribute & attribute)
    {
        return
            attribute =
            static_cast<MP3Attribute>(static_cast<int>(attribute) + 1);
    }

    // MP3AttributeInfo

    template <>
    bool
        MP3AttributeInfo<BinaryAttributeStatus>::isValid(
        MP3AttributeInfo attributeInfo)
    {
        BinaryAttributeStatus attributeStatus = attributeInfo.getStatus();
        return
            attributeStatus >= BinaryAttributeStatus::Unspecified &&
            attributeStatus <= BinaryAttributeStatus::Set;
    }

    template <>
    bool
        MP3AttributeInfo<EmphasisAttributeStatus>::isValid(
        MP3AttributeInfo attributeInfo)
    {
        EmphasisAttributeStatus attributeStatus = attributeInfo.getStatus();
        return
            attributeStatus >= EmphasisAttributeStatus::Unspecified &&
            attributeStatus <= EmphasisAttributeStatus::Invalid;
    }

    template class MP3AttributeInfo<BinaryAttributeStatus>;
    template class MP3AttributeInfo<EmphasisAttributeStatus>;

    // MP3AttributeInfoBase

    int MP3AttributeInfoBase::getStatus() const
    {
        return data & attributeStatusMask;
    }

    bool MP3AttributeInfoBase::isWholeFile() const
    {
        return (data & attributeWholeFileMask) != 0;
    }

    void MP3AttributeInfoBase::setWholeFile(bool wholeFile)
    {
        if (wholeFile == !(data & attributeWholeFileMask))
            data ^= attributeWholeFileMask;
    }

    // MP3AttributeInfoBox

    bool MP3AttributeInfoBox::isValid() const
    {
        return descriptor.isValid(attributeInfoBase);
    }

    int MP3AttributeInfoBox::getStatus() const
    {
        return attributeInfoBase.getStatus();
    }

    const type_info & MP3AttributeInfoBox::getStatusType() const
    {
        return descriptor.typeInfo;
    }

    bool MP3AttributeInfoBox::isWholeFile() const
    {
        return attributeInfoBase.isWholeFile();
    }

    // MP3AttributeStatusTypeDescriptor

    MP3AttributeStatusTypeDescriptor::MP3AttributeStatusTypeDescriptor(
        decltype(typeInfo) typeInfo,
        decltype(isValid) isValid):
    typeInfo(typeInfo), isValid(isValid)
    { }
}
