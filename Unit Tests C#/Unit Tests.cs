using Microsoft.VisualStudio.TestTools.UnitTesting;
using MP3Attr.UI;
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

namespace MP3Attr.UnitTests
{
    [TestClass]
    public class UnitTests
    {
        #region Setup

        private static readonly Assembly assembly;
        private static readonly PrivateType consoleApplicationType;
        private static readonly PrivateType resourcesType;
        private static readonly string tempDir;

        static UnitTests()
        {
            Type type;
            assembly = typeof(MP3Attribute).Assembly;
            type = assembly.GetType("MP3Attr.UI.ConsoleApplication");
            consoleApplicationType = new PrivateType(type);
            type = assembly.GetType("MP3Attr.Resources");
            resourcesType = new PrivateType(type);
            string tempPath = Path.GetTempPath();
            tempDir =
                Path.Combine(
                tempPath,
                Path.GetRandomFileName() + Path.DirectorySeparatorChar
                );
        }

        private static string
            getExpectedMessage(string name, params object[] args)
        {
            Regex finder = new Regex(@"\{\d+\}", RegexOptions.CultureInvariant);
            string text =
                resourcesType.InvokeStatic("GetString", name) as string;
            int usedCount = 0;
            MatchEvaluator me =
                (match) =>
                {
                    if (args.Length <= usedCount)
                        throw new ArgumentException("not enough replacements");
                    return args[usedCount++].ToString();
                };
            text = finder.Replace(text, me);
            if (usedCount < args.Length)
                throw
                    new ArgumentException("no more tokens to replace");
            return text;
        }

        #endregion

        #region ShrinkTextWidth

        [TestMethod]
        public void ShrinkTextWidth()
        {
            TestShrinkTextWidth("", 80, "");
            TestShrinkTextWidth("Hello World", 1, "Hello\r\nWorld\r\n");
            TestShrinkTextWidth("Hello    World", 1, "Hello\r\nWorld\r\n");
            TestShrinkTextWidth("Hello World", 11, "Hello World\r\n");
            TestShrinkTextWidth(
                "List\r\n  1\r\n  2\r\n",
                12,
                "List\r\n  1\r\n  2\r\n"
                );
            TestShrinkTextWidth(
                "  ABC ABC DEF DEF",
                10,
                "  ABC ABC\r\n  DEF DEF\r\n"
                );
            TestShrinkTextWidth(
                "1234567890 ABC DEF GHI",
                10,
                "1234567890\r\nABC DEF\r\nGHI\r\n"
                );
            TestShrinkTextWidth("ABC‐DEF", 4, "ABC‐\r\nDEF\r\n");
            TestShrinkTextWidth("ä ö ü", 3, "ä ö\r\nü\r\n");
        }

        private static void TestShrinkTextWidth(
            string text,
            int width,
            string expected)
        {
            string actual = Commons.ShrinkTextWidth(text, width);
            Assert.AreEqual(expected, actual);
        }

        #endregion

        #region IsDirectory

        [TestMethod]
        public void IsDirectory()
        {
            Assert.IsTrue(Commons.IsDirectory("."));
            Assert.IsFalse(Commons.IsDirectory(">"));
        }

        #endregion

        #region FindFilePaths

        [DllImport("kernel32.dll")]
        static extern bool CreateSymbolicLink(
            string lpSymlinkFileName,
            string lpTargetFileName,
            int dwFlags
            );

        [DllImport("shell32.dll")]
        public static extern bool IsUserAnAdmin();

        const int SYMBOLIC_LINK_FLAG_DIRECTORY = 1;

        [TestMethod]
        public void FindFilePaths()
        {
            if (!IsUserAnAdmin())
            {
                Assert.Inconclusive(
                    "Elevated privileges are required for this test."
                    );
                return;
            }
            Directory.CreateDirectory(tempDir);
            Directory.CreateDirectory(tempDir + "a");
            Assert.IsInstanceOfType(
                GetException(() =>
                {
                    bool wildcardsUsed;
                    Commons.FindFilePaths(tempDir + "*", out wildcardsUsed);
                }),
                typeof(FileNotFoundException)
                );
            File.Create(tempDir + "b");
            {
                bool wildcardsUsed;
                Assert.AreEqual(
                    1,
                    Commons.FindFilePaths(tempDir + "*", out wildcardsUsed)
                    .Length
                    );
                Assert.IsTrue(wildcardsUsed);
            }
            CreateSymbolicLink(tempDir + "B1", tempDir + "b", 0);
            {
                bool wildcardsUsed;
                string[] result =
                    Commons.FindFilePaths(tempDir + "*", out wildcardsUsed);
                Assert.AreEqual(2, result.Length);
                Assert.AreEqual(tempDir + "B1", result[0]);
                Assert.AreEqual(tempDir + "b", result[1]);
                Assert.IsTrue(wildcardsUsed);
            }
            CreateSymbolicLink(
                tempDir + "c",
                tempDir + "a",
                SYMBOLIC_LINK_FLAG_DIRECTORY
                );
            {
                bool wildcardsUsed;
                Assert.AreEqual(
                    2,
                    Commons.FindFilePaths(tempDir + "*", out wildcardsUsed)
                    .Length
                    );
                Assert.IsTrue(wildcardsUsed);
            }
            Assert.IsInstanceOfType(
                GetException(() =>
                {
                    bool wildcardsUsed;
                    Commons.FindFilePaths(
                        tempDir + "abc.xyz",
                        out wildcardsUsed
                        );
                }),
                typeof(FileNotFoundException)
                );
            Assert.IsInstanceOfType(
                GetException(() =>
                {
                    bool wildcardsUsed;
                    Commons.FindFilePaths(tempDir + "*.xyz", out wildcardsUsed);
                }),
                typeof(FileNotFoundException)
                );
            {
                bool wildcardsUsed;
                Assert.AreEqual(
                    1,
                    Commons.FindFilePaths(tempDir + "b", out wildcardsUsed)
                    .Length
                    );
                Assert.IsFalse(wildcardsUsed);
            }
            if (Path.VolumeSeparatorChar == ':')
            {
                Directory.SetCurrentDirectory(tempDir);
                {
                    bool wildcardsUsed;
                    Assert.AreEqual(
                        2,
                        Commons.FindFilePaths(
                        tempDir.Substring(0, 2) + "*",
                        out wildcardsUsed)
                        .Length
                        );
                    Assert.IsTrue(wildcardsUsed);
                }
            }
        }

        private delegate void TestProc();

        private static SystemException GetException(TestProc func)
        {
            try
            {
                func();
            }
            catch (SystemException exception)
            {
                return exception;
            }
            return null;
        }

        #endregion

        #region FindAllFilePaths

        [TestMethod]
        public void FindAllFilePaths()
        {
            List<string> loggedErrors = new List<string>();
            Action<string> logError =
                error =>
                {
                    loggedErrors.Add(error);
                };
            string dir = Path.GetTempPath();
            IList<string> paths = new List<string>() { dir };
            List<string> filePaths = new List<string>();

            int result =
                (int)
                consoleApplicationType.InvokeStatic(
                "FindAllFilePaths",
                logError,
                paths,
                filePaths
                );

            Assert.AreEqual(-1, result);
            Assert.AreEqual(0, filePaths.Count);
            Assert.AreEqual(1, loggedErrors.Count);
            string quotedPath =
                (string)resourcesType.InvokeStatic("QuotePath", dir);
            string expectedMessage =
                getExpectedMessage("PathIsDir", quotedPath);
            Assert.AreEqual(expectedMessage, loggedErrors[0]);
        }

        #endregion

        #region ProcessFile

        [TestMethod]
        public void ProcessFile()
        {
            TextWriter newWriter = new StringWriter();
            TextWriter oldWriter = Console.Out;
            Console.SetOut(newWriter);
            object result = null;
            try
            {
                result =
                    consoleApplicationType.InvokeStatic(
                    "ProcessFile",
                    "",
                    new MP3GearWheel(),
                    new MP3AttributeSet(),
                    '\0'
                    );
            }
            finally
            {
                Console.SetOut(oldWriter);
            }
            string resultText = result.ToString();
            Assert.AreEqual("Unprocessed", resultText);

            string actual = newWriter.ToString();
            string expected = 
                    getExpectedMessage("ERROR") +
                    ": " +
                    getExpectedMessage("MP3GenericException", "\"\"") +
                    Environment.NewLine;
            Assert.AreEqual(expected, actual);
        }

        #endregion

        #region MP3FormatException

        [TestMethod]
        public void MP3FileInvalidException()
        {
            MP3FileInvalidException e =
                new MP3FileInvalidException("FILEPATH", 1234);
            Assert.AreEqual("FILEPATH", e.FilePath);
            Assert.AreEqual(1234, e.Offset);
            string actual = e.Message;
            string expected =
                getExpectedMessage("MP3FileInvalidException", "\"FILEPATH\"");
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void MP3FirstFrameNotFoundException()
        {
            MP3FirstFrameNotFoundException e =
                new MP3FirstFrameNotFoundException("FILEPATH", 1234);
            Assert.AreEqual("FILEPATH", e.FilePath);
            Assert.AreEqual(1234, e.Offset);
            string actual = e.Message;
            string expected =
                getExpectedMessage(
                "MP3FirstFrameNotFoundException",
                "\"FILEPATH\""
                );
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void MP3FormatException()
        {
            MP3FormatException e = new MP3FormatException("FILEPATH", 1234);
            Assert.AreEqual("FILEPATH", e.FilePath);
            Assert.AreEqual(1234, e.Offset);
            string actual = e.Message;
            string expected =
                getExpectedMessage(
                "MP3FormatException",
                "\"FILEPATH\"",
                "1234 (0x4D2)"
                );
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void MP3FrameCRCTestException()
        {
            MP3FrameCRCTestException e =
                new MP3FrameCRCTestException("FILEPATH", 1234, 10);
            Assert.AreEqual("FILEPATH", e.FilePath);
            Assert.AreEqual(1234, e.Offset);
            Assert.AreEqual(10, e.FrameNumber);
            string actual = e.Message;
            string expected =
                getExpectedMessage(
                "MP3FrameCRCTestException",
                10,
                "\"FILEPATH\"",
                "1234 (0x4D2)"
                );
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void MP3FrameCRCUnknownException()
        {
            MP3FrameCRCUnknownException e =
                new MP3FrameCRCUnknownException("FILEPATH", 1234, 10);
            Assert.AreEqual("FILEPATH", e.FilePath);
            Assert.AreEqual(1234, e.Offset);
            Assert.AreEqual(10, e.FrameNumber);
            string actual = e.Message;
            string expected =
                getExpectedMessage(
                "MP3FrameCRCUnknownException",
                10,
                "\"FILEPATH\"",
                "1234 (0x4D2)"
                );
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void MP3FrameException()
        {
            MP3FrameException e = new MP3FrameException("FILEPATH", 1234, 10);
            Assert.AreEqual("FILEPATH", e.FilePath);
            Assert.AreEqual(1234, e.Offset);
            Assert.AreEqual(10, e.FrameNumber);
            string actual = e.Message;
            string expected =
                getExpectedMessage(
                "MP3FrameException",
                10,
                "\"FILEPATH\"",
                "1234 (0x4D2)"
                );
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void MP3FrameSizeUnknownException()
        {
            MP3FrameSizeUnknownException e =
                new MP3FrameSizeUnknownException("FILEPATH", 1234, 10);
            Assert.AreEqual("FILEPATH", e.FilePath);
            Assert.AreEqual(1234, e.Offset);
            Assert.AreEqual(10, e.FrameNumber);
            string actual = e.Message;
            string expected =
                getExpectedMessage(
                "MP3FrameSizeUnknownException",
                10,
                "\"FILEPATH\"",
                "1234 (0x4D2)"
                );
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void MP3GenericException()
        {
            MP3GenericException e = new MP3GenericException("FILEPATH");
            Assert.AreEqual("FILEPATH", e.FilePath);
            string actual = e.Message;
            string expected =
                getExpectedMessage("MP3GenericException", "\"FILEPATH\"");
            Assert.AreEqual(expected, actual);
        }

        [TestMethod]
        public void MP3KeyFrameNotFoundException()
        {
            MP3KeyFrameNotFoundException e =
                new MP3KeyFrameNotFoundException("FILEPATH");
            Assert.AreEqual("FILEPATH", e.FilePath);
            string actual = e.Message;
            string expected =
                getExpectedMessage(
                "MP3KeyFrameNotFoundException",
                "\"FILEPATH\""
                );
            Assert.AreEqual(expected, actual);
        }

        #endregion
    }
}
