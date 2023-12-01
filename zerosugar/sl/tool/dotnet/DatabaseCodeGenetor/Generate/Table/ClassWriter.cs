using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Generate.Table.Method;
using DatabaseCodeGenerator.Parse;
using DatabaseCodeGenetor.Generate.Table;

namespace DatabaseCodeGenerator.Generate.Table
{
    internal class ClassWriter
    {
        private readonly List<List<IClassMethodWriter>> _methodWriters = new()
        {
            new List<IClassMethodWriter>
            {
                new CtorWriter(),
            },
            new List<IClassMethodWriter>
            {
                new CreateTableWriter(),
                new DropTableWriter(),
            },
            new List<IClassMethodWriter>
            {
                new AddWriter(),
                new RemoveWriter(),
            },
            new List<IClassMethodWriter>()
            {
                new AssignWriter(),
                new UpdateWriter(),
            },
            new List<IClassMethodWriter>()
            {
                new FindWriter(),
            },
        };

        public List<Result> GenerateSourceFile(List<HeaderParseResult> headerParseResults)
        {
            return headerParseResults
                .Where(results => results.Structs.Count > 0)
                .Select(GenerateSource)
                .ToList();
        }

        private Result GenerateSource(HeaderParseResult result)
        {
            Text header = new Text();
            Text cxx = new Text();

            header.WriteLine("#pragma once");
            header.WriteLine("#include <boost/mysql.hpp>");
            header.WriteLine("#include <boost/describe.hpp>");
            header.WriteLine($"#include \"{GetIncludePath(result)}\"");
            header.BreakLine();
            header.WriteLine("namespace zerosugar::sl::db");
            header.WriteLine("{");

            cxx.WriteLine($"#include \"{GetFileName(result)}.h\"");
            cxx.BreakLine();
            cxx.WriteLine("#include <vector>");
            cxx.BreakLine();
            cxx.WriteLine("namespace zerosugar::sl::db");
            cxx.WriteLine("{");

            foreach (var udt in result.Structs)
            {
                string temp = udt.Columns
                    .Select(column => $"{column.FieldName}")
                    .Aggregate((s1, s2) => $"{s1}, {s2}");
                header.WriteLine($"    BOOST_DESCRIBE_STRUCT({udt.GetFullName()}, (), ({temp}))");
                header.BreakLine();

                string className = udt.GetGeneratedClassName();

                header.WriteLine($"    class {className}");
                header.WriteLine("    {");
                header.WriteLine("    public:");

                foreach (var writers in _methodWriters)
                {
                    foreach (var writer in writers)
                    {
                        writer.Write(udt, header, cxx);
                        cxx.BreakLine();
                    }

                    header.BreakLine();
                }

                header.WriteLine("    private:");
                header.WriteLine("        boost::mysql::tcp_ssl_connection& _connection;");
                header.WriteLine("    };");
            }

            header.WriteLine("}"); // namespace
            cxx.WriteLine("}");  // namespace

            return new Result(GetFileName(result), header.Get(), cxx.Get());
        }

        private string GetFileName(HeaderParseResult result)
        {
            return Path.GetFileNameWithoutExtension(result.FileName).ToLower() + "_table";
        }

        public static string GetIncludePath(HeaderParseResult result)
        {
            return $"{result.FileName.Substring(result.FileName.IndexOf("zerosugar/sl/"))}";
        }
    }
}
