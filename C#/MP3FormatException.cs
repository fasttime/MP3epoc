using System;

namespace MP3Attr
{
    [Serializable] public class MP3DataUnknownException: MP3FormatException
    {
        #region Public Constructors

        public MP3DataUnknownException(string filePath, long offset)
            : base(
            Resources.GetString("MP3DataUnknownException"),
            filePath,
            offset)
        { }

        public MP3DataUnknownException(
            string filePath,
            long offset,
            string message)
            : base(filePath, offset, message)
        { }

        #endregion
    }

    [Serializable] public class MP3FileInvalidException: MP3FormatException
    {
        #region Public Constructors

        public MP3FileInvalidException(string filePath, long offset)
            : base(
            Resources.GetString("MP3FileInvalidException"),
            filePath,
            offset)
        { }

        public MP3FileInvalidException(
            string filePath,
            long offset,
            string message)
            : base(filePath, offset, message)
        { }

        #endregion
    }

    [Serializable]
    public class MP3FirstFrameNotFoundException: MP3FormatException
    {
        #region Public Constructors

        public MP3FirstFrameNotFoundException(string filePath, long offset)
            : base(
            Resources.GetString("MP3FirstFrameNotFoundException"),
            filePath,
            offset)
        { }

        public MP3FirstFrameNotFoundException(
            string filePath,
            long offset,
            string message)
            : base(filePath, offset, message)
        { }

        #endregion
    }

    [Serializable] public class MP3FormatException: MP3GenericException
    {
        #region Public Constructors

        public MP3FormatException(string filePath, long offset)
            : this(
            Resources.GetString("MP3FormatException"),
            filePath,
            offset)
        { }

        public MP3FormatException(string filePath, long offset, string message)
            : base(filePath, message)
        { _offset = offset; }

        #endregion

        #region Public Properties

        public virtual long Offset
        {
            get
            { return _offset; }
        }

        #endregion

        #region Protected Internal Constructors

        protected internal MP3FormatException(
            string format,
            string filePath,
            long offset,
            params object[] moreData)
            : this(
            filePath,
            offset,
            MakeMessage(format, filePath, offset, moreData))
        { }

        #endregion

        #region Private Fields

        private readonly long _offset;

        #endregion

        #region Private Methods

        private static string MakeMessage(
            string format,
            string filePath,
            long offset,
            object[] moreData)
        {
            string offsetString = String.Format("{0} (0x{0:X})", offset);
            object[] formatArgs =
                CreateFormatArgs(moreData, filePath, offsetString);
            return String.Format(format, formatArgs);
        }

        #endregion
    }

    [Serializable] public class MP3FrameCRCTestException : MP3FrameException
    {
        #region Public Constructors

        public MP3FrameCRCTestException(
            string filePath,
            long offset,
            long frameNumber)
            : base(
            Resources.GetString("MP3FrameCRCTestException"),
            filePath,
            offset,
            frameNumber)
        { }

        public MP3FrameCRCTestException(
            string filePath,
            long offset,
            long frameNumber,
            string message)
            : base(filePath, offset, frameNumber, message)
        { }

        #endregion
    }

    [Serializable] public class MP3FrameException : MP3FormatException
    {
        #region Public Constructors

        public MP3FrameException(
            string filePath,
            long offset,
            long frameNumber)
            : base(
            Resources.GetString("MP3FrameException"),
            filePath,
            offset,
            frameNumber)
        { _frameNumber = frameNumber; }

        public MP3FrameException(
            string filePath,
            long offset,
            long frameNumber,
            string message)
            : base(filePath, offset, message)
        { _frameNumber = frameNumber; }

        #endregion

        #region Public Properties

        public virtual long FrameNumber
        {
            get
            { return _frameNumber; }
        }

        #endregion

        #region Protected Internal Constructors

        protected internal MP3FrameException(
            string format,
            string filePath,
            long offset,
            long frameNumber,
            params object[] moreData)
            : base(
            format,
            filePath,
            offset,
            PrependFrameNumber(frameNumber, moreData))
        { _frameNumber = frameNumber; }

        #endregion

        #region Private Fields

        private readonly long _frameNumber;

        #endregion

        #region Private Methods

        private static object[] PrependFrameNumber(
            long frameNumber,
            object[] moreData)
        {
            object[] allData = new object[moreData.Length + 1];
            allData[0] = frameNumber;
            moreData.CopyTo(allData, 1);
            return allData;
        }

        #endregion
    }

    [Serializable] public class MP3FrameCRCUnknownException : MP3FrameException
    {
        #region Public Constructors

        public MP3FrameCRCUnknownException(
            string filePath,
            long offset,
            long frameNumber)
            : base(
            Resources.GetString("MP3FrameCRCUnknownException"),
            filePath,
            offset,
            frameNumber)
        { }

        public MP3FrameCRCUnknownException(
            string filePath,
            long offset,
            long frameNumber,
            string message)
            : base(filePath, offset, frameNumber, message)
        { }

        #endregion
    }

    [Serializable] public class MP3FrameSizeUnknownException : MP3FrameException
    {
        #region Public Constructors

        public MP3FrameSizeUnknownException(
            string filePath,
            long offset,
            long frameNumber)
            : base(
            Resources.GetString("MP3FrameSizeUnknownException"),
            filePath,
            offset,
            frameNumber)
        { }

        public MP3FrameSizeUnknownException(
            string filePath,
            long offset,
            long frameNumber,
            string message)
            : base(filePath, offset, frameNumber, message)
        { }

        #endregion
    }

    [Serializable] public class MP3GenericException : ApplicationException
    {
        #region Public Constructors

        public MP3GenericException(string filePath)
            : this(
            Resources.GetString("MP3GenericException"),
            filePath,
            new object[] { })
        { }

        public MP3GenericException(string filePath, string message)
            : base(message)
        { _filePath = filePath; }

        #endregion

        #region Public Properties

        public virtual string FilePath
        {
            get
            { return _filePath; }
        }

        #endregion

        #region Protected Internal Constructors

        protected internal MP3GenericException(
            string format,
            string filePath,
            params object[] moreData)
            : this(
            filePath,
            MakeMessage(format, filePath, moreData))
        { }

        #endregion

        #region Protected Internal Methods

        protected internal static object[] CreateFormatArgs(
            object[] moreData,
            string filePath,
            params object[] preMoreData)
        {
            int leadingDataLength = 1 + preMoreData.Length;
            object[] formatArgs =
                new object[leadingDataLength + moreData.Length];
            formatArgs[0] = Resources.QuotePath(filePath);
            preMoreData.CopyTo(formatArgs, 1);
            moreData.CopyTo(formatArgs, leadingDataLength);
            return formatArgs;
        }

        #endregion

        #region Private Fields

        private readonly string _filePath;

        #endregion

        #region Private Methods

        private static string MakeMessage(
            string format,
            string filePath,
            object[] moreData)
        {
            object[] formatArgs = CreateFormatArgs(moreData, filePath);
            return String.Format(format, formatArgs);
        }

        #endregion
    }

    [Serializable]
    public class MP3KeyFrameNotFoundException : MP3GenericException
    {
        #region Public Constructors

        public MP3KeyFrameNotFoundException(string filePath)
            : base(
            Resources.GetString("MP3KeyFrameNotFoundException"),
            filePath,
            new object[] { })
        { }

        public MP3KeyFrameNotFoundException(string filePath, string message)
            : base(filePath, message)
        { }

        #endregion
    }
}
