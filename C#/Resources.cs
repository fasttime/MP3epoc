using System.Resources;

namespace MP3Attr
{
    internal class Resources
    {
        public static string GetString(string name)
        {
            return ResourceManager.GetString(name);
        }

        public static string QuotePath(string path)
        {
            return '"' + path + '"';
        }

        private static readonly ResourceManager ResourceManager =
            new ResourceManager(typeof(Resources));
    }
}
