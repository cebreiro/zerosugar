using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;
using DatabaseCodeGenetor.Generate.Table;
using static DatabaseCodeGenerator.Parse.Struct;

namespace DatabaseCodeGenerator.Generate.Table.Method
{
    internal class RemoveWriter : IClassMethodWriter
    {
        public void Write(Struct udt, Text header, Text cxx)
        {
            Write_RemoveByPk(udt, header, cxx);
            Write_RemoveByFk(udt, header, cxx);
            Write_RemoveByIndex(udt, header, cxx);
        }

        private void Write_RemoveByPk(Struct udt, Text header, Text cxx)
        {
            Column? column = udt.GetPrimaryKeyColumn();
            if (column == null)
            {
                return;
            }

            string methodName = "Remove";
            WriteRemoveMethod(udt, header, cxx, column, methodName);
        }

        private void Write_RemoveByFk(Struct udt, Text header, Text cxx)
        {
            foreach (var column in udt.GetForeignKeyColumns())
            {
                string methodName = $"RemoveBy{column.FieldName.ToUpper()}";
                WriteRemoveMethod(udt, header, cxx, column, methodName);
            }
        }

        private void Write_RemoveByIndex(Struct udt, Text header, Text cxx)
        {
            foreach (var column in udt.GetIndexColumns())
            {
                string methodName = $"RemoveBy{column.FieldName.ToUpper()}";
                WriteRemoveMethod(udt, header, cxx, column, methodName);
            }
        }

        private void WriteRemoveMethod(Struct udt, Text header, Text cxx,
            Column column, string methodName)
        {
            string className = udt.GetClassName();
            string paramType = $"const {column.TypeName}&";
            string paramName = $"{column.FieldName}";

            header.WriteLine($"        void {methodName}({paramType} {paramName});");

            cxx.WriteLine($"    void {className}::{methodName}({paramType} {paramName})");
            cxx.WriteLine("    {");
            cxx.WriteLine("        constexpr const char* queryString = R\"delimiter(");
            cxx.WriteLine($"DELETE FROM `{udt.GetDbTableName()}` WHERE `{column.FieldName}` = ?;");
            cxx.WriteLine(")delimiter\";");
            cxx.WriteLine("        auto stmt = _connection.prepare_statement(queryString);");
            cxx.WriteLine($"        auto bound = stmt.bind({paramName});");
            cxx.BreakLine();
            cxx.WriteLine("        boost::mysql::results result;");
            cxx.WriteLine("        _connection.execute(bound, result);");
            cxx.WriteLine("    }");
        }
    }
}
