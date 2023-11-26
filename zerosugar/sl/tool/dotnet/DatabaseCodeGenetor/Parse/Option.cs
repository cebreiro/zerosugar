using System.Text;
using ClangSharp.Interop;

namespace DatabaseCodeGenerator.Parse
{
    internal class Option
    {
        public string Spelling { get; }
        public uint Line { get; }
        public bool PrimaryKey { get; }
        public bool ForeignKey { get; }
        public string ForeignKeyTargetTable { get; }
        public string ForeignKeyTargetField { get; }
        public bool Index { get; }
        public uint? MaxLength { get; }
        public bool AutoIncrement { get; }
        public bool Unique { get; }
        public string? DefaultValue { get; }

        public Option(CXTranslationUnit tu, CXCursor cursor)
        {
            Spelling = tu.Tokenize(cursor.SourceRange).ToArray()
                .Select(token => token.GetSpelling(tu).ToString())
                .Aggregate((str, token) => str + token);
            Line = cursor.GetLine();

            string spelling = cursor.Spelling.ToString();
            if (spelling != "ZEROSUGAR_DATABASE_OPTION")
            {
                throw new NotImplementedException($"not implementation macro {spelling}");
            }

            StringBuilder builder = new StringBuilder();
            foreach (var token in tu.Tokenize(cursor.SourceRange).ToArray().Skip(1))
            {
                builder.Append(token.GetSpelling(tu).ToString());
            }

            string sources = builder.ToString();
            sources = sources.Replace(" ", "");

            foreach (var option in sources.Split('(', ')', ','))
            {
                if (string.IsNullOrEmpty(option))
                {
                    continue;
                }

                if (option == "PK")
                {
                    PrimaryKey = true;
                }
                else if (option == "INDEX")
                {
                    Index = true;
                }
                else if (option == "AI")
                {
                    AutoIncrement = true;
                }
                else if (option == "UNIQUE")
                {
                    Unique = true;
                }
                else if (option.StartsWith("FK"))
                {
                    var split = option.Split('=')[1].Split('.');

                    ForeignKey = true;
                    ForeignKeyTargetTable = split[0];
                    ForeignKeyTargetField = split[1];
                }
                else if (option.StartsWith("MAX_LENGTH"))
                {
                    var split = option.Split('=');
                    var target = split[1];

                    if (uint.TryParse(target, out uint result))
                    {
                        MaxLength = result;
                    }
                    else
                    {
                        throw new Exception($"invalid MAX_LENGTH value. file: {tu.Spelling}, line: {cursor.GetLine()}, value: {target}");
                    }
                }
                else if (option.StartsWith("DEFAULT"))
                {
                    var split = option.Split('=');
                    DefaultValue = split[1];
                }
                else
                {
                    throw new NotImplementedException($"not implemented db option: {option}");
                }
            }
        }
    }
}
