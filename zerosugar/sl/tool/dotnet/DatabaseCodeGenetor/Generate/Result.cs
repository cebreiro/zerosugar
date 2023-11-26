using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DatabaseCodeGenerator.Generate
{
    internal struct Result
    {
        public string FileName;
        public string Header;
        public string Cxx;

        public Result(string fileName, string header, string cxx)
        {
            FileName = fileName;
            Header = header;
            Cxx = cxx;
        }
    }
}
