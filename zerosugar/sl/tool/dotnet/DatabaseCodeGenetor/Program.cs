using System;
using System.Runtime.CompilerServices;
using ClangSharp.Interop;
using DatabaseCodeGenerator.Generate.Table;
using DatabaseCodeGenerator.Parse;

try
{
    var commandLines = Environment.GetCommandLineArgs();
    if (commandLines.Length < 3)
    {
        Console.WriteLine("usage - this.exe <target_directory> <output_directory> <libclang_args>...");
        return;
    }

    string targetDirectory = commandLines[1];
    string outputDirectory = commandLines[2];
    Console.WriteLine($"target directory: {targetDirectory}");
    Console.WriteLine($"output directory: {outputDirectory}");

    List<string> options = new()
    {
        "-x",
        "c++",
        "--std=c++20"
    };

    for (int i = 3; i < commandLines.Length; i++)
    {
        options.Add(commandLines[i]);

        Console.WriteLine($"libclang commandline option {commandLines[i]} added");
    }

    Console.WriteLine("Start...");
    Console.WriteLine("----------------------------------------------");

    List<HeaderParseResult> parseResults = new List<HeaderParseResult>();

    var files = Directory.GetFiles(targetDirectory, "*.h", SearchOption.TopDirectoryOnly);
    foreach (var file in files)
    {
        var index = CXIndex.Create();
        var tu = CXTranslationUnit.Parse(
            index,
            file,
            options.ToArray(),
            Array.Empty<CXUnsavedFile>(),
            CXTranslationUnit_Flags.CXTranslationUnit_DetailedPreprocessingRecord);
        if (tu == null)
        {
            throw new Exception("Translate unit is null");
        }

        HeaderParseResult result = new HeaderParseResult(tu, tu.Cursor);
        parseResults.Add(result);
    }

    List<string> generatedFilePaths = new();

    foreach (var result in new ClassWriter().GenerateSourceFile(parseResults))
    {
        string headerPath = Path.Combine(outputDirectory, $"{result.FileName}.h");
        string cxxPath = Path.Combine(outputDirectory, $"{result.FileName}.cpp");

        File.WriteAllText(headerPath, result.Header);
        File.WriteAllText(cxxPath, result.Cxx);

        generatedFilePaths.Add(headerPath);
        generatedFilePaths.Add(cxxPath);
    }

    Console.WriteLine("----------------------------------------------");
    foreach (var path in generatedFilePaths)
    {
        Console.WriteLine($"generated: {path}");
    }

    Console.WriteLine("----------------------------------------------");
    Console.WriteLine("Done");
}
catch (Exception e)
{
    Console.WriteLine($"Exception: {e.Message}\n{e.StackTrace}");
}
