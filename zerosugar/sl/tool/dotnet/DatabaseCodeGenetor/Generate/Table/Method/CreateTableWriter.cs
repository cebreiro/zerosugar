using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;
using DatabaseCodeGenetor.Generate.Table;

namespace DatabaseCodeGenerator.Generate.Table.Method
{
    internal class CreateTableWriter : IClassMethodWriter
    {
        public void Write(Struct udt, Text header, Text cxx)
        {
            string methodName = "CreateTable";

            header.WriteLine($"        void {methodName}();");

            cxx.WriteLine($"    void {udt.GetGeneratedClassName()}::{methodName}()");
            cxx.WriteLine("    {");
            cxx.WriteLine("        constexpr const char* queryString = R\"delimiter(");
            cxx.WriteLine($"CREATE TABLE IF NOT EXISTS `{udt.GetDbTableName()}` (");

            for (var i = 0; i < udt.Columns.Count; i++)
            {
                cxx.Write("    ");

                if (i > 0)
                {
                    cxx.Write(", ");
                }

                cxx.WriteLine(GetCreateTableColumnText(udt.Columns[i]));
            }

            bool autoIncrement = false;

            foreach (var column in udt.Columns.Where(column => column.Option != null))
            {
                Option option = column.Option;

                if (option.PrimaryKey)
                {
                    cxx.WriteLine($"    , PRIMARY KEY(`{column.FieldName}`)");

                    autoIncrement = option.AutoIncrement;
                }

                if (option.Index)
                {
                    cxx.WriteLine($"    , KEY `{udt.Name}_idx_{column.FieldName}` (`{column.FieldName}`)");
                }

                if (option.Unique)
                {
                    cxx.WriteLine($"    , UNIQUE KEY `{udt.Name}_unique_{column.FieldName}` (`{column.FieldName}`)");
                }

                if (option.ForeignKey)
                {
                    cxx.WriteLine($"    , CONSTRAINT `{udt.Name}_fk_{column.FieldName}` FOREIGN KEY (`{column.FieldName}`)");
                    cxx.WriteLine($"        REFERENCES `{option.ForeignKeyTargetTable}` (`{option.ForeignKeyTargetField}`)");
                    cxx.WriteLine($"        ON DELETE CASCADE ON UPDATE CASCADE");
                }
            }

            cxx.Write(") ENGINE=InnoDB");

            if (autoIncrement)
            {
                cxx.Write(" AUTO_INCREMENT=1");
            }
            cxx.WriteLine(" DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;");

            cxx.WriteLine(")delimiter\";");
            cxx.BreakLine();
            cxx.WriteLine("        boost::mysql::results result;");
            cxx.WriteLine("        _connection.execute(queryString, result);");
            cxx.WriteLine("    }");
        }

        private string GetCreateTableColumnText(Struct.Column column)
        {
            StringBuilder builder = new StringBuilder();
            builder.Append($"`{column.FieldName}`");

            Option option = column.Option;
            bool isNullable = column.TypeName.StartsWith("std::optional");

            string typeName = column.TypeName;
            if (isNullable)
            {
                typeName = column.TypeName.Split('<', '>')[1];
            }

            switch (typeName)
            {
                case "int8_t":
                case "uint8_t":
                    builder.Append(" TINYINT");
                    break;
                case "int16_t":
                case "uint16_t":
                    builder.Append(" SMALLINT");
                    break;
                case "int32_t":
                case "uint32_t":
                    builder.Append(" INT");
                    break;
                case "int64_t":
                    builder.Append(" BIGINT");
                    break;
                case "float":
                    builder.Append(" FLOAT");
                    break;
                case "double":
                    builder.Append(" DOUBLE");
                    break;
                case "std::string":
                    builder.Append($" VARCHAR({option?.MaxLength ?? 4096})");
                    break;
                default:
                    throw new NotImplementedException($"invalid typename: {typeName}");
            }

            if (!isNullable)
            {
                builder.Append(" NOT NULL");
            }

            if (option != null)
            {
                if (option.DefaultValue != null)
                {
                    builder.Append($" DEFAULT '{option.DefaultValue}' ");
                }

                if (option.AutoIncrement)
                {
                    builder.Append(" AUTO_INCREMENT");
                }
            }

            return builder.ToString();
        }
    }
}
