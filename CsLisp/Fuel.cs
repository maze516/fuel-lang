﻿using System;
using System.IO;
using System.Linq;
using System.Reflection;

namespace CsLisp
{
    /// <summary>
    /// Fast Usable Embeddable Lisp Interpreter and Compiler (FUEL).
    /// </summary>
    public class Fuel
    {
        public static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Usage();
                return;
            }

            var doNotLoadFiles = false;
            var trace = false;
            var compile = false;
            var showCompileOutput = false;
            var measureTime = false;
            var lengthyErrorOutput = false;
            var result = new LispVariant();
            var startTickCount = Environment.TickCount;
            var debugger = TryGetDebugger();

            if (args.Contains("-m"))
            {
                measureTime = true;
            }
            if (args.Contains("-v"))
            {
                Console.WriteLine(Lisp.Version);
                return;
            }
            if (args.Contains("-h"))
            {
                Usage();
                return;
            }
            if (args.Contains("-l"))
            {
                lengthyErrorOutput = true;
            }
            if (args.Contains("-e"))
            {
                var script = LispUtils.GetScriptFilesFromProgramArgs(args).FirstOrDefault();
                result = Lisp.Eval(script);
                doNotLoadFiles = true;
            }
            if (args.Contains("-t"))
            {
                trace = true;
            }
            if (debugger != null)
            {
                if (args.Contains("-i"))
                {
                    InteractiveLoopHeader();
                    debugger.InteractiveLoop(startedFromMain:true, tracing: trace);
                    doNotLoadFiles = true;
                }
                if (args.Contains("-d"))
                {
                    InteractiveLoopHeader();
                    result = debugger.DebuggerLoop(args, tracing: trace);
                    doNotLoadFiles = true;
                }                
            }
            if (args.Contains("-c"))
            {
                compile = true;
            }
            if (args.Contains("-s"))
            {
                showCompileOutput = true;
            }

            if (!doNotLoadFiles)
            {
                var scriptFiles = LispUtils.GetScriptFilesFromProgramArgs(args);

                foreach (var fileName in scriptFiles)
                {
                    var script = LispUtils.ReadFile(fileName);
                    ILispCompiler compiler = TryGetCompiler();
                    if (compile && compiler != null)
                    {
                        result = compiler.CompileToExe(script, fileName + ".exe");
                    }
                    else if (showCompileOutput && compiler != null)
                    {
                        result = compiler.CompileToCsCode(script);
                        Console.WriteLine(result.StringValue);
                    }
                    else
                    {
                        result = Lisp.SaveEval(script, moduleName: fileName, verboseErrorOutput: lengthyErrorOutput, tracing: trace);
                    }
                }
            }

            if (trace)
            {
                Console.WriteLine("Result=" + result);
            }
            if (measureTime)
            {
                Console.WriteLine("Execution time = {0} s", (Environment.TickCount - startTickCount) * 0.001);
            }
        }

        #region private methods

        private static void Usage()
        {
            LispUtils.ShowAbout();
            Console.WriteLine("usage:");
            Console.WriteLine(">" + Lisp.ProgramName + " [options] [script_file_name]");
            Console.WriteLine();
            Console.WriteLine("options:");
            Console.WriteLine("  -v          : show version");
            Console.WriteLine("  -h          : show help");
            Console.WriteLine("  -e \"script\" : execute given script");
            Console.WriteLine("  -m          : measure execution time");
            Console.WriteLine("  -t          : enable tracing");
            Console.WriteLine("  -l          : lengthy error output");
            if (TryGetDebugger() != null)
            {
                Console.WriteLine("  -i          : interactive shell");
                Console.WriteLine("  -d          : start debugger");
            }
            else
            {
                Console.WriteLine();
                Console.WriteLine("Info: no debugger support installed !");
            }
            if (TryGetCompiler() != null)
            {
                Console.WriteLine("  -c          : compile program");
                Console.WriteLine("  -s          : show C# compiler output");
            }
            else
            {
                Console.WriteLine();
                Console.WriteLine("Info: no compiler support installed !");                
            }
            Console.WriteLine();
        }

        private static void InteractiveLoopHeader()
        {
            LispUtils.ShowVersion();
            Console.WriteLine("Type \"help\" for informations.");
            Console.WriteLine();
        }

        private static ILispCompiler TryGetCompiler()
        {
            return TryGetClassFromDll<ILispCompiler>("CsLispCompiler.dll", "CsLisp.LispCompiler");
        }

        private static ILispDebugger TryGetDebugger()
        {
            return TryGetClassFromDll<ILispDebugger>("CsLispDebugger.dll", "CsLisp.LispDebugger");
        }
        
        private static T TryGetClassFromDll<T>(string dllName, string className)
        {
            var applicationPath = GetApplicationPath();
            var dllPath = Path.Combine(applicationPath, dllName);

            if (File.Exists(dllPath))
            {
                var dll = Assembly.LoadFile(dllPath);
                if (dll != null)
                {
                    var classType = dll.GetType(className);
                    var instance = Activator.CreateInstance(classType);
                    return (T)instance;
                }
            }

            return default(T);
        }

        private static string GetApplicationPath()
        {
            var application = Assembly.GetExecutingAssembly().Location;
            var applicationPath = Path.GetDirectoryName(application);
            return applicationPath;
        }

        #endregion
    }


    // TODO:
    // (- debuggen: run funktioniert nicht in errorinmodule.fuel
    // ((- debuggen: anzeige module und line no in stack
    // (- debuggen: anzeige des korrekten codes, falls module geladen ist
    // - debuggen: set breakpoints in andren modulen realisieren
    // (- debuggen: up/down sollte auch den --> Zeiger anpassen
    // - ist LispScope.Finished und LispScope.SourceCode noch notwendig? 
    // - debuggen: set next statement realisieren?
    // - bug: step out funktioniert anscheinend bei modulen nicht ganz korrekt
    // (- ggf. module als eigenen Scope implementieren --> ###modules###
    // - debuggen: show loaded module names 
    // - debzggen: funcs befehl um module erweitern
    // - stdlib um list<object> erweitern, damit man immer mit listen arbeiten kann
    // - debugger: v (step over) funktioniert nicht so wie erwartet --> haengt bei quote
}
