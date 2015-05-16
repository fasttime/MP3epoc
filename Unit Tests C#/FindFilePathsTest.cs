using System;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;

namespace MP3Attr.UI
{
    [TestClass]
    public class FindFilePathsTest
    {
        [TestMethod]
        public void TestFindFilePathsOk()
        {
            string dir = Path.GetTempPath();
            string tempDirectory = Path.Combine(dir, Path.GetRandomFileName());
            Directory.CreateDirectory(tempDirectory);
            Directory.CreateDirectory(tempDirectory + @"\LongLongName");
            List<FileStream> streams = new List<FileStream>();
            try
            {
                CreateTempFiles(
                    streams,
                    tempDirectory + @"\AAA.AAA",
                    tempDirectory + @"\ABB.AA",
                    tempDirectory + @"\LongLongName\LongLongFile.SSS"
                    );
                TestFindFilePathsMatch(
                    false,
                    tempDirectory.Replace(@"\", @"\\") + @"\AAA.AAA",
                    tempDirectory + @"\AAA.AAA"
                    );
                TestFindFilePathsMatch(
                    true,
                    InsertSpaces(tempDirectory) + @"/A*.A?A",
                    tempDirectory + @"\AAA.AAA"
                    );
                TestFindFilePathsMatch(
                    true,
                    tempDirectory.Replace('\\', '/') + @"\A??.A*A",
                    tempDirectory + @"\AAA.AAA",
                    tempDirectory + @"\ABB.AA"
                    );
                TestFindFilePathsOk(false, @".");
                TestFindFilePathsMatch(
                    false,
                    tempDirectory + @"\LONGLO~1\LONGLO~1.SSS",
                    tempDirectory + @"\LongLongName\LongLongFile.SSS"
                    );
            }
            finally
            {
                foreach (FileStream stream in streams)
                    if (stream as object != null) stream.Dispose();
            }
        }

        [TestMethod]
        public void TestFindFilePathsException()
        {
            TestFindFilePathsException(typeof(ArgumentNullException),
                null as string
                );
            TestFindFilePathsException(typeof(ArgumentException),
                @"",
                @"   ",
                "\"",
                @"<",
                @">",
                @"|",
                @"C:\Windows\ ..\*.*",
                @"C:\Windows\.. .\*.*",
                @":\*.*",
                @"::",
                @":",
                @" :",
                @"C :\",
                @" ."
                );
            for (char ch = '\x00'; ch < '\x20'; ++ch)
            {
                TestFindFilePathsException(typeof(ArgumentException),
                    @"C:\" + ch,
                    ch + @":\Windows\*.*",
                    @"\\Any" + ch,
                    @"C:\Any" + ch + @"\*.*"
                    );
            }
            TestFindFilePathsException(typeof(PathTooLongException),
                VeryLongPath,
                NotQuiteTooLongRelativePath + "*"
                );
            if (Directory.Exists(SystemDrive + @"Recycle.bin"))
            {
                TestFindFilePathsException(typeof(UnauthorizedAccessException),
                    SystemDrive + @"Recycle.bin\*.*"
                    );
            }
            TestFindFilePathsException(typeof(FileNotFoundException),
                @"\\",
                @"C:",
                @"C:\",
                Environment.SystemDirectory,
                Environment.SystemDirectory + '\\',
                @"123:\Windows",
                @"9:",
                NotQuiteTooLongRelativePath + ' ',
                NotQuiteTooLongRelativePath.Replace(@"\", @" \/  \/"),
                @"C:\nul.*",
                @"aux.txt",
                @"C:\..\..\con.!",
                @"\$Mft"
                );
        }

        private static string NotQuiteTooLongRelativePath
        {
            get
            {
                StringBuilder sb = new StringBuilder(259);
                sb.Append(@".\1234");
                for (int i = 0; i < 25; ++i) sb.Append(@"\ABCD-1234");
                return sb.Append(@"\..").ToString();
            }
        }

        private static string SystemDrive
        {
            get
            {
                return Path.GetPathRoot(Environment.SystemDirectory);
            }
        }

        private static string VeryLongPath
        {
            get
            {
                StringBuilder sb = new StringBuilder(300);
                sb.Append(@"C:\");
                for (int i = 0; i < 10; ++i)
                    sb.Append(@"\29_CHARS_INCLUDING_BACKSLASH");
                return sb.ToString();
            }
        }

        private static void CreateTempFiles(
            List<FileStream> streams,
            params string[] paths)
        {
            foreach (string path in paths) streams.Add(File.Create(path));
        }

        private static string FormatPath(string path)
        {
            return "path: " + path;
        }

        private static string InsertSpaces(string path)
        {
            return ' ' + path.Substring(0, 2) + ' ' + path.Substring(2);
        }

        private void TestFindFilePathsException(Type exceptionType, string path)
        {
            bool wildcardsUsed;
            try
            {
                Commons.FindFilePaths(path, out wildcardsUsed);
            }
            catch (Exception e)
            {
                Assert.AreEqual(exceptionType, e.GetType(), FormatPath(path));
                return;
            }
            Assert.Fail(
                exceptionType + " not thrown as expected, " + FormatPath(path)
                );
        }

        private void TestFindFilePathsException(
            Type exceptionType,
            params string[] paths)
        {
            foreach (string path in paths)
                TestFindFilePathsException(exceptionType, path);
        }

        private void TestFindFilePathsMatch(
            bool wildcardsUsed,
            string path,
            params string[] filePaths)
        {
            bool wildcardsUsedActual;
            string[] filePathsActual;
            try
            {
                filePathsActual =
                    Commons.FindFilePaths(path, out wildcardsUsedActual);
            }
            catch (Exception e)
            {
                Assert.Fail(e.GetType() + " thrown, " + FormatPath(path));
                return;
            }
            Assert.AreEqual(wildcardsUsed, wildcardsUsedActual);
            Assert.IsNotNull(filePathsActual, FormatPath(path));
            Array.Sort(filePaths);
            Array.Sort(filePathsActual);
            Assert.IsTrue(
                filePaths.SequenceEqual(filePathsActual),
                FormatPath(path)
                );
        }

        private void TestFindFilePathsOk(bool wildcardsUsed, string path)
        {
            bool wildcardsUsedActual;
            try
            {
                Commons.FindFilePaths(path, out wildcardsUsedActual);
            }
            catch (Exception e)
            {
                Assert.Fail(e.GetType() + " thrown, " + FormatPath(path));
                return;
            }
            Assert.AreEqual(wildcardsUsed, wildcardsUsedActual);
        }
    }
}
