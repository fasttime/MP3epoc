using System;

namespace MP3Attr
{
    [Serializable] public enum BinaryAttributeStatus
    {
        Unspecified = 0,
        NotSet      = 0x00 + 1,
        Set         = 0x01 + 1,
    }

    [Serializable] public enum EmphasisAttributeStatus
    {
        Unspecified = 0,
        None        = 0x00 + 1,
        e50_15_µs   = 0x01 + 1,
        CCITT_j_17  = 0x03 + 1,
        Invalid     = 0x02 + 1,
    }

    public interface IMP3AttributeInfo
    {
        Type    GetStatusType();
        bool    IsUnspecified   { get; }
        int     Status          { get; }
        bool    WholeFile       { get; }
    }

    [Serializable] public enum MP3Attribute
    {
        Private,
        Copyright,
        Original,
        Emphasis,
    }
}
