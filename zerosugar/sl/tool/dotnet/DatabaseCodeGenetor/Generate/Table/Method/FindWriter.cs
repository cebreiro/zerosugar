using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;
using DatabaseCodeGenetor.Generate.Table;
using static DatabaseCodeGenerator.Parse.Struct;

namespace DatabaseCodeGenerator.Generate.Table.Method
{
    internal class FindWriter : IClassMethodWriter
    {
        public void Write(Struct udt, Text header, Text cxx)
        {
            Write_FindByPk(udt, header, cxx);
            Write_FindByUnique(udt, header, cxx);
            Write_FindByFk(udt, header, cxx);
            Write_FindRangeByFk(udt, header, cxx);
        }

        private void Write_FindByPk(Struct udt, Text header, Text cxx)
        {
            Column? column = udt.GetPrimaryKeyColumn();
            if (column == null)
            {
                return;
            }

            string methodName = "Find";

            WriteFindMethod(udt, header, cxx, column, methodName);
        }

        private void Write_FindByUnique(Struct udt, Text header, Text cxx)
        {
            foreach (var column in udt.GetUniqueColumns())
            {
                string methodName = $"FindBy{column.FieldName.ToUpper()}";

                WriteFindMethod(udt, header, cxx, column, methodName);
            }
        }

        private void Write_FindByFk(Struct udt, Text header, Text cxx)
        {
            foreach (var column in udt.GetForeignKeyColumns())
            {
                string methodName = $"FindBy{column.FieldName.ToUpper()}";
                WriteFindMethod( udt, header, cxx, column, methodName);
            }
        }

        private void Write_FindRangeByFk(Struct udt, Text header, Text cxx)
        {
            foreach (var column in udt.GetForeignKeyColumns())
            {
                string methodName = $"FindRangeBy{column.FieldName.ToUpper()}";
                WriteFindRangeMethod(udt, header, cxx, column, methodName);
            }
        }

        private void WriteFindMethod(Struct udt, Text header, Text cxx, Struct.Column column, string methodName)
        {
            string className = udt.GetGeneratedClassName();
            string resultType = $"std::optional<{udt.GetFullName()}>";
            string paramType = $"const {column.TypeName}&";
            string paramName = $"{column.FieldName}";

            header.WriteLine($"        auto {methodName}({paramType} {paramName}) -> {resultType};");

            cxx.WriteLine($"    auto {className}::{methodName}({paramType} {paramName}) -> {resultType}");
            cxx.WriteLine("    {");
            cxx.WriteLine("        constexpr const char* queryString = R\"delimiter(");
            cxx.WriteLine($"SELECT * FROM `{udt.GetDbTableName()}`");
            cxx.WriteLine($"WHERE");
            cxx.WriteLine($"    `{column.FieldName}` = ?;");
            cxx.WriteLine("        )delimiter\";");

            cxx.WriteLine("        auto stmt = _connection.prepare_statement(queryString);");
            cxx.WriteLine($"        auto bound = stmt.bind({column.FieldName});");
            cxx.BreakLine();
            cxx.WriteLine($"        boost::mysql::static_results<{udt.GetFullName()}> result;");
            cxx.WriteLine("        _connection.execute(bound, result);");
            cxx.BreakLine();
            cxx.WriteLine("        if (result.rows().empty())");
            cxx.WriteLine("        {");
            cxx.WriteLine("            return std::nullopt;");
            cxx.WriteLine("        }");
            cxx.BreakLine();
            cxx.WriteLine("        return *result.rows().begin();");
            cxx.WriteLine("    }");
        }


        private void WriteFindRangeMethod(Struct udt, Text header, Text cxx, Struct.Column column, string methodName)
        {
            string className = udt.GetGeneratedClassName();
            string resultType = $"std::vector<{udt.GetFullName()}>";
            string paramType = $"const {column.TypeName}&";
            string paramName = $"{column.FieldName}";

            header.WriteLine($"        auto {methodName}({paramType} {paramName}) -> {resultType};");

            cxx.WriteLine($"    auto {className}::{methodName}({paramType} {paramName}) -> {resultType}");
            cxx.WriteLine("    {");
            cxx.WriteLine("        constexpr const char* queryString = R\"delimiter(");
            cxx.WriteLine($"SELECT * FROM `{udt.GetDbTableName()}`");
            cxx.WriteLine($"WHERE");
            cxx.WriteLine($"    `{column.FieldName}` = ?;");
            cxx.WriteLine("        )delimiter\";");

            cxx.WriteLine("        auto stmt = _connection.prepare_statement(queryString);");
            cxx.WriteLine($"        auto bound = stmt.bind({column.FieldName});");
            cxx.BreakLine();
            cxx.WriteLine($"        boost::mysql::static_results<{udt.GetFullName()}> result;");
            cxx.WriteLine("        _connection.execute(bound, result);");
            cxx.BreakLine();
            cxx.WriteLine("        const auto& front = result.rows();");
            cxx.BreakLine();
            cxx.WriteLine($"        {resultType} results;");
            cxx.WriteLine("        results.reserve(front.size());");
            cxx.BreakLine();
            cxx.WriteLine("        for (const auto& element : front)");
            cxx.WriteLine("        {");
            cxx.WriteLine("            results.push_back(element);");
            cxx.WriteLine("        }");
            cxx.BreakLine();
            cxx.WriteLine("        return results;");
            cxx.WriteLine("    }");
        }
    }
}
