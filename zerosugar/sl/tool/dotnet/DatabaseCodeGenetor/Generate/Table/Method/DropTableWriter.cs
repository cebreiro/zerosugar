using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;
using DatabaseCodeGenetor.Generate.Table;

namespace DatabaseCodeGenerator.Generate.Table.Method
{
    internal class DropTableWriter : IClassMethodWriter
    {
        public void Write(Struct udt, Text header, Text cxx)
        {
            string methodName = "DropTable";

            header.WriteLine($"        void {methodName}();");

            cxx.WriteLine($"    void {udt.GetClassName()}::{methodName}()");
            cxx.WriteLine("    {");
            cxx.WriteLine("        constexpr const char* queryString = R\"delimiter(");
            cxx.WriteLine($"DROP TABLE IF EXISTS `{udt.GetDbTableName()}`;");
            cxx.WriteLine(")delimiter\";");
            cxx.BreakLine();
            cxx.WriteLine("        boost::mysql::results result;");
            cxx.WriteLine("        _connection.execute(queryString, result);");
            cxx.WriteLine("    }");
        }
    }
}
