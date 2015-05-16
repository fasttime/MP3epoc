#pragma once

#include <typeinfo>

namespace
{
    const unsigned attributeStatusMask      = 0x0f;
    const unsigned attributeWholeFileMask   = 0x10;
}

namespace MP3epoc
{
    enum class BinaryAttributeStatus: int
    {
        Unspecified = 0,
        NotSet      = 0x00 + 1,
        Set         = 0x01 + 1,
    };

    enum class EmphasisAttributeStatus: int
    {
        Unspecified = 0,
        None        = 0x00 + 1,
        e50_15_µs   = 0x01 + 1,
        CCITT_j_17  = 0x03 + 1,
        Invalid     = 0x02 + 1,
    };

    class MP3AttributeInfoBase
    {
    public:
        int getStatus() const;
        bool isWholeFile() const;
        void setWholeFile(bool wholeFile);
    private:
        unsigned char data;
    };

    template <typename T>
    class MP3AttributeInfo: public MP3AttributeInfoBase
    {
    public:
        T getStatus() const;
        static bool isValid(MP3AttributeInfo attributeInfo);
    };

    template <typename T>
    T MP3AttributeInfo<T>::getStatus() const
    {
        return static_cast<T>(MP3AttributeInfoBase::getStatus());
    }

#ifndef IMPL_MP3Attribute

    extern template class MP3AttributeInfo<BinaryAttributeStatus>;
    extern template class MP3AttributeInfo<EmphasisAttributeStatus>;

#endif // #ifndef IMPL_MP3Attribute

    class MP3AttributeStatusTypeDescriptor
    {
    public:
        const std::type_info & typeInfo;
        bool (* const isValid)(MP3AttributeInfoBase attributeInfoBase);
        MP3AttributeStatusTypeDescriptor(
            decltype(typeInfo) typeInfo,
            decltype(isValid) isValid
            );
        template <typename T>
        static const MP3AttributeStatusTypeDescriptor & getInstance();
    private:
        MP3AttributeStatusTypeDescriptor &
            operator = (const MP3AttributeStatusTypeDescriptor &);
    };

    template <typename T>
    const MP3AttributeStatusTypeDescriptor &
        MP3AttributeStatusTypeDescriptor::getInstance()
    {
        static const MP3AttributeStatusTypeDescriptor instance(
            typeid(T),
            [] (MP3AttributeInfoBase attributeInfoBase) -> bool
        {
            return
                MP3AttributeInfo<T>::isValid(
                static_cast<MP3AttributeInfo<T> &>(attributeInfoBase)
                );
        }
            );
        return instance;
    }

    class MP3AttributeInfoBox
    {
    public:
        template <typename T>
        MP3AttributeInfoBox(const MP3AttributeInfo<T> & attributeInfo);
        int getStatus() const;
        const std::type_info & getStatusType() const;
        bool isValid() const;
        bool isWholeFile() const;
    private:
        const MP3AttributeStatusTypeDescriptor & descriptor;
        const MP3AttributeInfoBase & attributeInfoBase;
        MP3AttributeInfoBox & operator = (const MP3AttributeInfoBox &);
    };

    template <typename T>
    MP3AttributeInfoBox::MP3AttributeInfoBox(
        const MP3AttributeInfo<T> & attributeInfo):
        descriptor(MP3AttributeStatusTypeDescriptor::getInstance<T>()),
        attributeInfoBase(attributeInfo)
    { }

    enum class MP3Attribute: int
    {
        Private,
        Copyright,
        Original,
        Emphasis,

        First   = Private,
        Last    = Emphasis
    };

    MP3Attribute & operator ++ (MP3Attribute & attribute);
}
