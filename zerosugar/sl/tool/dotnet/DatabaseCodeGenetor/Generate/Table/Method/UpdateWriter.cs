using DatabaseCodeGenerator.Generate.Table;
using DatabaseCodeGenerator.Generate.Table.Method;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;
using DatabaseCodeGenetor.Generate.Table;
using static DatabaseCodeGenerator.Parse.Struct;

namespace DatabaseCodeGenerator.Generate.Table.Method
{
    internal class UpdateWriter : IClassMethodWriter
    {
        public void Write(Struct udt, Text header, Text cxx)
        {
            Column? column = udt.GetPrimaryKeyColumn();
            if (column == null)
            {
                return;
            }

            string methodName = "Update";
            WriteUpdateMethod(udt, header, cxx, column, methodName);
        }

        private void WriteUpdateMethod(Struct udt, Text header, Text cxx,
            Column column, string methodName)
        {
            string className = udt.GetClassName();
            string paramType = $"const {udt.GetFullName()}&";
            string paramName = "item";

            header.WriteLine($"        void {methodName}({paramType} {paramName});");

            cxx.WriteLine($"    void {className}::{methodName}({paramType} {paramName})");
            cxx.WriteLine("    {");
            cxx.WriteLine("        constexpr const char* queryString = R\"delimiter(");
            cxx.WriteLine($"UPDATE `{udt.GetDbTableName()}`");
            cxx.WriteLine($"SET");

            string temp1 = udt.GetModifiableColumns()
                .Select(column => $"`{column.FieldName}` = ?")
                .Aggregate((s1, s2) => $"{s1}, {s2}");
            cxx.WriteLine($"    {temp1}");
            cxx.WriteLine("WHERE");
            cxx.WriteLine($"    {column.FieldName} = ?;");
            cxx.WriteLine(")delimiter\";");

            cxx.WriteLine("        auto stmt = _connection.prepare_statement(queryString);");

            string temp3 = udt.GetModifiableColumns()
                .Select(column => $"{paramName}.{column.FieldName}")
                .Aggregate((s1, s2) => $"{s1}, {s2}");

            temp3 += ($", {paramName}.{column.FieldName}");

            cxx.WriteLine($"        auto bound = stmt.bind({temp3});");
            cxx.BreakLine();
            cxx.WriteLine("        boost::mysql::results result;");
            cxx.WriteLine("        _connection.execute(bound, result);");
            cxx.WriteLine("    }");
        }
    }
}
