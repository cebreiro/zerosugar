using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;
using DatabaseCodeGenetor.Generate.Table;

namespace DatabaseCodeGenerator.Generate.Table.Method
{
    internal class AddWriter : IClassMethodWriter
    {
        public void Write(Struct udt, Text header, Text cxx)
        {
            string className = udt.GetClassName();
            string methodName = "Add";
            string paramType = $"const {udt.GetFullName()}&";
            string paramName = "item";

            header.WriteLine($"        void {methodName}({paramType} {paramName});");

            cxx.WriteLine($"    void {className}::{methodName}({paramType} {paramName})");
            cxx.WriteLine("    {");
            cxx.WriteLine("        constexpr const char* queryString = R\"delimiter(");
            cxx.WriteLine($"INSERT INTO `{udt.GetDbTableName()}`");

            string temp1 = udt.Columns
                .Select(column => $"`{column.FieldName}`")
                .Aggregate((s1, s2) => $"{s1}, {s2}");
            cxx.WriteLine($"    ({temp1})");
            cxx.WriteLine("VALUES");

            string temp2 = udt.Columns
                .Select(_ => "?")
                .Aggregate((s1, s2) => $"{s1}, {s2}");
            cxx.WriteLine($"    ({temp2});");

            cxx.WriteLine(")delimiter\";");

            cxx.WriteLine("        auto stmt = _connection.prepare_statement(queryString);");

            string temp3 = udt.Columns
                .Select(column => $"{paramName}.{column.FieldName}")
                .Aggregate((s1, s2) => $"{s1}, {s2}");
            cxx.WriteLine($"        auto bound = stmt.bind({temp3});");
            cxx.BreakLine();
            cxx.WriteLine("        boost::mysql::results result;");
            cxx.WriteLine("        _connection.execute(bound, result);");
            cxx.WriteLine("    }");
        }
    }
}
