using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ClangSharp.Interop;

namespace DatabaseCodeGenerator.Parse
{
    internal static class CXCursorExtension
    {
        public static uint GetLine(this CXCursor cursor)
        {
            cursor.Location.GetFileLocation(out CXFile file, out uint line, out uint column, out uint offset);
            return line;
        }
    }
}
