using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;
using DatabaseCodeGenetor.Generate.Table;

namespace DatabaseCodeGenerator.Generate.Table.Method
{
    internal class CtorWriter : IClassMethodWriter
    {
        public void Write(Struct udt, Text header, Text cxx)
        {
            string className = udt.GetClassName();

            header.WriteLine($"        ~{className}() = default;");
            header.WriteLine($"        explicit {className}(boost::mysql::tcp_ssl_connection& connection);");

            cxx.WriteLine($"    {className}::{className}(boost::mysql::tcp_ssl_connection& connection)");
            cxx.WriteLine("        : _connection(connection)");
            cxx.WriteLine("    {");
            cxx.WriteLine("    }");
        }
    }
}
