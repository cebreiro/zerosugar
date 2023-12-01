using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;

namespace DatabaseCodeGenetor.Generate.Table
{
    internal static class StructExtension
    {
        public static string GetGeneratedClassName(this Struct udt)
        {
            return $"{udt.Name}Table";
        }

        public static string GetDbTableName(this Struct udt)
        {
            return udt.Name;
        }
    }
}
