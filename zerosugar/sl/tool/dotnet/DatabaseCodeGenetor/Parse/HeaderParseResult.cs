using System.Runtime.CompilerServices;
using ClangSharp.Interop;

namespace DatabaseCodeGenerator.Parse
{
    internal class HeaderParseResult
    {
        public string FileName { get; }
        public List<Option> Options = new();
        public List<Struct> Structs = new();

        public HeaderParseResult(CXTranslationUnit tu, CXCursor cursor)
        {
            FileName = tu.Spelling.ToString();

            unsafe
            {
                cursor.VisitChildren((c, p, _) =>
                {
                    if (c.Location.IsInSystemHeader)
                    {
                        return CXChildVisitResult.CXChildVisit_Continue;
                    }

                    if (c.kind == CXCursorKind.CXCursor_MacroExpansion)
                    {
                        Options.Add(new Option(tu, c));
                    }

                    return CXChildVisitResult.CXChildVisit_Continue;

                }, new CXClientData(IntPtr.Zero));

                cursor.VisitChildren((c, p, _) =>
                {
                    if (c.Location.IsInSystemHeader)
                    {
                        return CXChildVisitResult.CXChildVisit_Continue;
                    }

                    if (c.kind == CXCursorKind.CXCursor_Namespace)
                    {
                        return CXChildVisitResult.CXChildVisit_Recurse;
                    }

                    if (c.kind == CXCursorKind.CXCursor_StructDecl)
                    {
                        Structs.Add(new Struct(tu, c));
                    }

                    return CXChildVisitResult.CXChildVisit_Continue;

                }, new CXClientData(IntPtr.Zero));
            }

            BindOption();
        }

        private void BindOption()
        {
            foreach (var udt in Structs)
            {
                foreach (var column in udt.Columns)
                {
                    Option? option = Options.SingleOrDefault(option => option.Line == column.Line - 1);
                    if (option != null)
                    {
                        column.Option = option;
                    }
                }
            }
        }
    }
}
