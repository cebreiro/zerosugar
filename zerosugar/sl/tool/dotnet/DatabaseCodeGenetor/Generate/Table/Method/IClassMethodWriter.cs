using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DatabaseCodeGenerator.Parse;

namespace DatabaseCodeGenerator.Generate.Table.Method
{
    internal interface IClassMethodWriter
    {
        void Write(Struct udt, Text header, Text cxx);
    }
}
