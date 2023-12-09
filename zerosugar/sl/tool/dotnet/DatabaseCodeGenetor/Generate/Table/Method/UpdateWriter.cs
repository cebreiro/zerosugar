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

            WriteUpdateDifferenceMethod(udt, header, cxx, column);
        }

        private void WriteUpdateMethod(Struct udt, Text header, Text cxx,
            Column column, string methodName)
        {
            string className = udt.GetGeneratedClassName();
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

        private void WriteUpdateDifferenceMethod(Struct udt, Text header, Text cxx,
            Column column)
        {
            Column? pk = udt.GetPrimaryKeyColumn();
            if (pk == null)
            {
                return;
            }

            string className = udt.GetGeneratedClassName();
            string methodName = "UpdateDifference";
            string paramType = $"const {udt.GetFullName()}&";
            string param1Name = "oldOne";
            string param2Name = "newOne";

            header.WriteLine($"        void {methodName}({paramType} {param1Name}, {paramType} {param2Name});");

            cxx.WriteLine($"    void {className}::{methodName}({paramType} {param1Name}, {paramType} {param2Name})");
            cxx.WriteLine("    {");
            cxx.WriteLine($"        if ({param1Name}.{pk.FieldName} != {param2Name}.{pk.FieldName})");
            cxx.WriteLine("        {");
            cxx.WriteLine("            assert(false);");
            cxx.WriteLine("            return;");
            cxx.WriteLine("        }");
            cxx.BreakLine();
            
            cxx.WriteLine("        using field_views_t = boost::container::static_vector<");
            cxx.WriteLine("            boost::mysql::field_view,");
            cxx.WriteLine($"            boost::mp11::mp_size<boost::describe::describe_members<");
            cxx.WriteLine($"                {udt.GetFullName()},");
            cxx.WriteLine("                boost::describe::mod_any_access>>::value>;");
            cxx.WriteLine("        std::ostringstream oss;");
            cxx.WriteLine("        field_views_t views;");
            cxx.BreakLine();
            cxx.WriteLine("        if (!FillDifference(oldOne, newOne, oss, views))");
            cxx.WriteLine("        {");
            cxx.WriteLine("            return;");
            cxx.WriteLine("        }");
            cxx.WriteLine("        ");
            cxx.WriteLine($"        views.emplace_back({param2Name}.{pk.FieldName});");
            cxx.WriteLine("        ");
            cxx.WriteLine("        std::string difference = oss.str();");
            cxx.WriteLine("        assert(!difference.empty());");
            cxx.WriteLine("        ");
            cxx.WriteLine("        difference.pop_back();");
            cxx.WriteLine("        constexpr const char* queryString = R\"delimiter(");
            cxx.WriteLine($"UPDATE `{udt.GetDbTableName()}`");
            cxx.WriteLine($"SET");
            cxx.WriteLine("    {}");
            cxx.WriteLine("WHERE");
            cxx.WriteLine($"    {column.FieldName} = ?;");
            cxx.WriteLine(")delimiter\";");
            cxx.BreakLine();
            cxx.WriteLine("        const std::string dynamicQuery = std::format(queryString, difference);");
            cxx.WriteLine("        auto stmt = _connection.prepare_statement(dynamicQuery);");
            cxx.WriteLine("        auto bound = stmt.bind(views.begin(), views.end());");
            cxx.BreakLine();
            cxx.WriteLine("        boost::mysql::results result;");
            cxx.WriteLine("        _connection.execute(bound, result);");
            cxx.WriteLine("    }");
        }
    }
}
