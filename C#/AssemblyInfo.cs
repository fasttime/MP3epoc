using MP3Attr.App;
using System;
using System.Reflection;
using System.Runtime.CompilerServices;

[assembly: AssemblyTitle(AssemblyInfo.Name)]
[assembly: AssemblyCompany(AssemblyInfo.Company)]
[assembly: AssemblyProduct(AssemblyInfo.Name)]
[assembly: AssemblyCopyright(AssemblyInfo.Copyright)]
[assembly: AssemblyVersion(AssemblyInfo.Version)]
[assembly: AssemblyInformationalVersion(AssemblyInfo.Version)]
[assembly: CLSCompliant(true)]

namespace MP3Attr.App
{
    internal static class AssemblyInfo
    {
        internal const string Name = "MP3epoc";
        internal const string Company = "fasttime.org";
        internal const string Copyright =
            "Copyright 2005-2013 Francesco Trotta";
        internal const string Version = "1.3";
    }
}
