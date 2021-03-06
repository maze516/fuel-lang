/*
* FUEL(isp) is a fast usable embeddable lisp interpreter.
*
* Copyright (c) 2016 Michael Neuroth
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* */

#include "Debugger.h"
#include "Lisp.h"
#include "Exception.h"

#include "cstypes.h"
#include "csstring.h"

using namespace CppLisp;

const string LispDebugger::Prompt = /*Lisp::Name*/string("FUEL(isp)") + "> ";

const string LispDebugger::DbgPrompt = /*Lisp::Name*/string("FUEL(isp)") + "-DBG> ";

extern "C" CppLisp::ILispDebugger * create_debugger()
{
	return new CppLisp::LispDebugger();
}

namespace CppLisp
{
	LispDebugger::LispDebugger()
	{
		Breakpoints = std::vector<LispBreakpointInfo>();
		Output = /*Console.Out*/std::make_shared<TextWriter>();
		Input = /*Console.In*/std::make_shared<TextReader>();
		CommandLineScript = string::Empty;
		Reset();
	}

	LispDebugger::~LispDebugger()
	{
	}

	bool LispDebugger::InteractiveLoop(LispDebugger * debugger, std::shared_ptr<LispScope> initialTopScope, bool startedFromMain, bool tracing, std::shared_ptr<TextWriter> outp, std::shared_ptr<TextReader> inp)
	{
		string interactiveScript;

		startedFromMain = startedFromMain || debugger == null;
		if (debugger == null)
		{
			debugger = new LispDebugger();
		}
		var globalScope = initialTopScope != null ? initialTopScope->GlobalScope : LispEnvironment::CreateDefaultScope();
		// do not switch off tracing if already enabled
		if (!globalScope->Tracing)
		{
			globalScope->Tracing = tracing;
		}
		globalScope->Output = outp != null ? outp : (initialTopScope != null ? initialTopScope->Output : std::make_shared<TextWriter>());
		globalScope->Input = inp != null ? inp : (initialTopScope != null ? initialTopScope->Input : std::make_shared<TextReader>());
		var topScope = initialTopScope != null ? initialTopScope : globalScope;
		var currentScope = topScope;
		var bContinueWithNextStatement = false;
		var bRestart = false;
		do
		{
			debugger->Output->Write(debugger != null ? DbgPrompt : Prompt);

			// Warning:
			// QProcess and .NET >3.5 does not work correclty reading from input !!!
			// see: http://www.qtcentre.org/threads/62415-QProcess-not-communicating-with-net-framework-gt-3-5
			// ==> CsLisp is now using .NET 3.5 !
			var cmd = debugger->Input->ReadLine();
			cmd = /*cmd != null ?*/ cmd.Trim() /*: null*/;

			if (/*cmd == null ||*/string::IsNullOrEmpty(cmd) || cmd.Equals("exit") || cmd.Equals("quit") || cmd.Equals("q"))
			{
				bContinueWithNextStatement = true;
				bRestart = false;
				if (!startedFromMain)
				{
					throw LispStopDebuggerException();
				}
			}
			else if (cmd.Equals("help") || cmd.Equals("h"))
			{
				ShowInteractiveCmds(debugger->Output);
			}
			else if (cmd.Equals("about"))
			{
				/*LispUtils.*/ShowAbout(debugger->Output);
			}
#ifndef _DISABLE_DEBUGGER
			else if (cmd.Equals("funcs"))
			{
				globalScope->DumpFunctions();
			}
			else if (cmd.Equals("macros"))
			{
				globalScope->DumpMacros();
			}
			else if (cmd.Equals("builtins"))
			{
				globalScope->DumpBuiltinFunctions();
			}
			else if (cmd.StartsWith("doc"))
			{
				var items = cmd.Split(' ');
				if (items.size() > 1)
				{
					string docCmd = "(doc '" + items[1] + ")";
					std::shared_ptr<LispVariant> result = Lisp::Eval(docCmd, currentScope, currentScope->ModuleName);
					debugger->Output->WriteLine(string("{0}"), result->ToString());
				}
				else
				{
					globalScope->DumpBuiltinFunctionsHelp();
				}
			}
			else if (cmd.StartsWith("searchdoc"))
			{
				var items = cmd.Split(' ');
				if (items.size() > 1)
				{
					string docCmd = "(searchdoc '" + items[1] + ")";
					std::shared_ptr<LispVariant> result = Lisp::Eval(docCmd, currentScope, currentScope->ModuleName);
					debugger->Output->WriteLine(string("{0}"), result->ToString());
				}
				else
				{
					globalScope->DumpBuiltinFunctionsHelp();
				}
			}
			else if (cmd.Equals("modules"))
			{
				globalScope->DumpModules();
			}
			else if (cmd.StartsWith("clear"))
			{
				ClearBreakpoints(debugger, cmd);
			}
			else if (cmd.Equals("stack") || cmd.StartsWith("k"))
			{
				topScope->DumpStack(currentScope->GetCallStackSize());
			}
			else if (cmd.Equals("code") || cmd.StartsWith("c"))
			{
				var script = string::Empty;
				var moduleName = currentScope->ModuleName;
				if (string::IsNullOrEmpty(moduleName))
				{
					script = interactiveScript;
				}
				else
				{
					script = moduleName.StartsWith(LispEnvironment::EvalStrTag) ? moduleName.Substring(LispEnvironment::EvalStrTag.size() + moduleName.IndexOf(":", LispEnvironment::EvalStrTag.size())) : /*LispUtils*/ReadFileOrEmptyString(moduleName);
				}
				// use the script given on command line if no valid module name was set
				if (string::IsNullOrEmpty(script))
				{
					script = debugger->CommandLineScript;
				}
				ShowSourceCode(debugger, script, currentScope->ModuleName, currentScope->CurrentLineNo());
			}
			else if (cmd.StartsWith("list") || cmd.StartsWith("t"))
			{
				debugger->ShowBreakpoints();
			}
			else if (cmd.StartsWith("break ") || cmd.StartsWith("b "))
			{
				AddBreakpoint(debugger, cmd, currentScope->ModuleName);
			}
			else if (cmd.Equals("up") || cmd.StartsWith("u"))
			{
				if (currentScope->Next != null)
				{
					currentScope = currentScope->Next;
				}
			}
			else if (cmd.Equals("down") || cmd.StartsWith("d"))
			{
				if (currentScope->Previous != null)
				{
					currentScope = currentScope->Previous;
				}
			}
			else if (cmd.Equals("step") || cmd.Equals("s"))
			{
				debugger->DoStep(currentScope);
				bContinueWithNextStatement = true;
			}
			else if (cmd.Equals("over") || cmd.Equals("v"))
			{
				debugger->DoStepOver(currentScope);
				bContinueWithNextStatement = true;
			}
			else if (cmd.Equals("out") || cmd.Equals("o"))
			{
				debugger->DoStepOut(currentScope);
				bContinueWithNextStatement = true;
			}
			else if (cmd.Equals("run") || cmd.Equals("r"))
			{
				debugger->DoRun();
				bContinueWithNextStatement = true;
			}
			else if (cmd.Equals("locals") || cmd.StartsWith("l"))
			{
				currentScope->DumpVars();
			}
			else if (cmd.Equals("globals") || cmd.StartsWith("g"))
			{
				globalScope->DumpVars();
			}
			else if (cmd.Equals("restart"))
			{
				bContinueWithNextStatement = true;
				bRestart = true;
			}
#endif
			else if (cmd.Equals("version") || cmd.Equals("ver"))
			{
				/*LispUtils::*/ShowVersion(debugger->Output);
			}
			else
			{
				try
				{
					std::shared_ptr<LispVariant> result = Lisp::Eval(cmd, currentScope, currentScope->ModuleName, /*tracing:*/false, outp, inp);
					debugger->Output->WriteLine("result={0}", result->ToString());
					interactiveScript += cmd + "\n";
				}
				catch (LispExceptionBase & ex)
				{
					debugger->Output->WriteLine("Exception: " + ex.Message);
				}
				catch (...)
				{
					debugger->Output->WriteLine("Native Exception");
				}
			}
		} while (!bContinueWithNextStatement);

		return bRestart;
	}

	void LispDebugger::InteractiveLoop(std::shared_ptr<LispScope> initialTopScope, std::shared_ptr<IEnumerable<std::shared_ptr<object>>> currentAst, bool startedFromMain, bool tracing, std::shared_ptr<TextWriter> outp, std::shared_ptr<TextReader> inp)
	{
		if (currentAst != null)
		{
			var lineNumber = initialTopScope != null ? initialTopScope->CurrentLineNo() : 0;
			var startPos = initialTopScope != null ? initialTopScope->CurrentToken->StartPos : 0;
			var stopPos = initialTopScope != null ? initialTopScope->CurrentToken->StopPos : 0;
			var moduleName = initialTopScope != null ? initialTopScope->ModuleName : "?";
			var tokenTxt = initialTopScope != null ? initialTopScope->CurrentToken->ToString() : "?";
			Output->WriteLine("--> " + (*((*currentAst).begin()))->ToString()/*[0]*/ + " line=" + std::to_string((int)lineNumber) + " start=" + std::to_string((int)startPos) + " stop=" + std::to_string((int)stopPos) + " module=" + moduleName + " token=" + tokenTxt);
		}
		InteractiveLoop(this, initialTopScope, startedFromMain, tracing, outp, inp);
	}

	bool LispDebugger::NeedsBreak(std::shared_ptr<LispScope> scope, LispBreakpointPosition posInfosOfCurrentAstItem)
	{
		if ((IsProgramStop && IsStopStepFcn(scope)) || HitsBreakpoint(posInfosOfCurrentAstItem.Item3(), scope->ModuleName, scope))
		{
			IsProgramStop = false;
			return true;
		}
		return false;
	}

	std::shared_ptr<LispVariant> LispDebugger::DebuggerLoop(const string & script, const string & moduleName, bool tracing)
	{
		std::shared_ptr<LispVariant> result = null;
		var bRestart = true;
		while (bRestart)
		{
			// save the source code if the script is transfered via command line
			if (moduleName == /*LispUtils::CommandLineModule*/"command-line")
			{
				CommandLineScript = script;
			}

			var globalScope = LispEnvironment::CreateDefaultScope();
			globalScope->Input = Input;
			globalScope->Output = Output;
			globalScope->Debugger = this;

			try
			{
				result = Lisp::Eval(script, globalScope, moduleName, /*tracing:*/ tracing, Output, Input);
				Reset();
			}
			catch (LispStopDebuggerException & /*exc*/)
			{
				bRestart = false;
			}
			catch (LispExceptionBase & exception)
			{
				Output->WriteLine("\nException: {0}", exception.ToString());
				// TODO --> implement...
				//string stackInfo = exception.Data.Contains(/*LispUtils.StackInfo*/"StackInfo") ? (string)exception.Data[/*LispUtils.StackInfo*/"StackInfo"] : string::Empty;
				string stackInfo = "<TODO>";
				Output->WriteLine("\nStack:\n{0}", stackInfo);
				bRestart = InteractiveLoop(this, globalScope, /*startedFromMain:*/ true);
			}
			catch (...)
			{
				Output->WriteLine("\nNative Exception...");
			}

			if (bRestart)
			{
				Output->WriteLine("restart program");

				// process empty script --> just start interactive loop
				if (result == null)
				{
					bRestart = InteractiveLoop(this, globalScope, /*startedFromMain:*/ true);
				}
			}

			globalScope->Debugger = null;
		}

		return result;
	}

	void LispDebugger::Reset()
	{
		IsProgramStop = true;
		IsStopStepFcn = [](std::shared_ptr<LispScope> /*scope*/) -> bool { return true; };
	}

	bool LispDebugger::HitsBreakpoint(size_t lineNo, const string & moduleName, std::shared_ptr<LispScope> scope)
	{
		for (var breakpoint : Breakpoints)
		{
			bool isSameModule = IsSameModule(breakpoint.ModuleName, scope != null ? scope->ModuleName : moduleName);
			if (isSameModule && (lineNo == breakpoint.LineNo))
			{
				if (breakpoint.Condition.size() > 0 && scope != null)
				{
					try
					{
						std::shared_ptr<LispVariant> result = Lisp::Eval(breakpoint.Condition, scope, scope->ModuleName);
						return result->BoolValue();
					}
					catch (...)
					{
						Output->WriteLine("Error: bad condition for line {0}: {1}", std::to_string(breakpoint.LineNo), breakpoint.Condition);
						return false;
					}
				}
				return true;
			}
		}
		return false;
	}

	bool LispDebugger::HasBreakpointAt(size_t lineNo, const string & moduleName)
	{
		return HitsBreakpoint(lineNo, moduleName, null);
	}

	void LispDebugger::AddBreakpoint(size_t lineNo, const string & moduleName, const string & condition)
	{
		var newItem = /*new*/ LispBreakpointInfo(lineNo, moduleName, condition);
		//var index = Breakpoints.FindIndex(elem => (elem.LineNo == lineNo) && (elem.ModuleName == moduleName));
		var indexIter = std::find_if(Breakpoints.begin(), Breakpoints.end(), [lineNo, moduleName](const LispBreakpointInfo & elem) -> bool { return (elem.LineNo == lineNo) && (elem.ModuleName == moduleName); });
		size_t index = indexIter != Breakpoints.end() ? indexIter - Breakpoints.begin() : -1;
		if (/*index >= 0 &&*/ index != (size_t)-1)
		{
			// replace existing item for this line
			Breakpoints[index] = newItem;
		}
		else
		{
			Breakpoints./*Add*/push_back(newItem);
		}
	}

	bool LispDebugger::ClearBreakpoint(int no)
	{
		size_t index = no - 1;
		if (/*index >= 0 &&*/ index < Breakpoints.size())
		{
			Breakpoints./*RemoveAt*/erase(Breakpoints.begin() + index);
			return true;
		}
		return false;
	}

	void LispDebugger::ClearAllBreakpoints()
	{
		Breakpoints.clear();
	}

	void LispDebugger::DoStep(std::shared_ptr<LispScope> /*currentScope*/)
	{
		IsStopStepFcn = [](std::shared_ptr<LispScope> /*scope*/) -> bool { return true; };
		IsProgramStop = true;
	}

	void LispDebugger::DoStepOver(std::shared_ptr<LispScope> currentScope)
	{
		var currentCallStackSize = currentScope->GetCallStackSize();
		IsStopStepFcn = [currentCallStackSize](std::shared_ptr<LispScope> scope) -> bool { return currentCallStackSize >= scope->GetCallStackSize(); };
		IsProgramStop = true;
	}

	void LispDebugger::DoStepOut(std::shared_ptr<LispScope> currentScope)
	{
		var currentCallStackSize = currentScope->GetCallStackSize();
		IsStopStepFcn = [currentCallStackSize](std::shared_ptr<LispScope> scope) -> bool { return currentCallStackSize - 1 >= scope->GetCallStackSize(); };
		IsProgramStop = true;
	}

	void LispDebugger::DoRun()
	{
		IsProgramStop = false;
	}
	
	void LispDebugger::ShowBreakpoints()
	{
		Output->WriteLine("Breakpoints:");
		var no = 1;
		for (var breakpoint : Breakpoints)
		{
			Output->WriteLine("#{0,-3} line={1,-5} module={2,-25} condition={3}", std::to_string(no), std::to_string(breakpoint.LineNo), breakpoint.ModuleName, breakpoint.Condition);
			no++;
		}
	}

	bool LispDebugger::IsSameModule(const string & moduleName1, const string & moduleName2)
	{
		// if one module name is not set --> handle as same module
		if (string::IsNullOrEmpty(moduleName1) || string::IsNullOrEmpty(moduleName2))
		{
			return true;
		}

		// compare only with file name, ignore the path
		//            var module1 = new FileInfo(moduleName1);
		//            var module2 = new FileInfo(moduleName2);
		//            return module1.Name.Equals(module2.Name);
		return moduleName1 == moduleName2;
	}

	void LispDebugger::ShowSourceCode(LispDebugger * debugger, const string & sourceCode, const string & moduleName, size_t/*?*/ currentLineNo)
	{
		if (debugger != null)
		{
			var sourceCodeLines = sourceCode.Split('\n');
			for (size_t i = 0; i < sourceCodeLines.size(); i++)
			{
				string breakMark = debugger->HasBreakpointAt(i + 1, moduleName) ? "B " : "  ";
				string mark = /*currentLineNo != null &&*/ currentLineNo/*.Value*/ == i + 1 ? string("-->") : string::Empty;
				debugger->Output->WriteLine("{0,3} {1,2} {2,3} {3}", std::to_string(i + 1), breakMark, mark, sourceCodeLines[i]);
			}
		}
	}

	void LispDebugger::ClearBreakpoints(LispDebugger * debugger, const string & cmd)
	{
		if (debugger != null)
		{
			string rest = cmd.Substring(5).Trim();
			if (rest.size() > 0)
			{
				Tuple<bool, int> val = ConvertToInt(rest);
				if (!val.Item1() || !debugger->ClearBreakpoint(val.Item2()))
				{
					debugger->Output->WriteLine("Warning: no breakpoint cleared");
				}
			}
			else
			{
				debugger->Output->WriteLine("Really delete all breakpoints? (y/n)");
				string answer;
				do
				{
					answer = debugger->Input->ReadLine();
					//if (answer != null)
					{
						answer = answer.ToUpper();
					}
				} while (!(answer == "Y" || answer == "N" || answer == "YES" || answer == "NO"));
				if (answer == "Y" || answer == "YES")
				{
					debugger->ClearAllBreakpoints();
				}
			}
		}
	}

	void LispDebugger::AddBreakpoint(LispDebugger * debugger, const string & cmd, const string & currentModuleName)
	{
		bool added = false;
		if (debugger != null)
		{
			std::vector<string> cmdArgs/* = new string[0]*/;
			string moduleName = currentModuleName;
			string rest = cmd.Substring(cmd.IndexOf(" ", "StringComparison.Ordinal")).Trim();
			if (rest.StartsWith("\""))
			{
				// process: filename:linenumber
				size_t iStopPos;
				moduleName = GetStringLiteral(rest.Substring(1), /*out*/ iStopPos);
				rest = rest.Substring(iStopPos + 2); // adjust for the two "
				if (rest.StartsWith(":"))
				{
					cmdArgs = rest.Substring(1).Split(' ');
				}
			}
			else
			{
				// process: linennumber
				cmdArgs = rest.Split(' ');
			}
			size_t indexRest = rest.IndexOf(" ", "StringComparison.Ordinal");
			rest = indexRest != string::npos ? rest.Substring(indexRest).Trim() : string::Empty;
			if (cmdArgs.size() > 0)
			{
				string lineNumberString = cmdArgs[0];
				size_t posModuleSeparator = cmdArgs[0].LastIndexOf(":", "StringComparison.Ordinal");
				if (/*posModuleSeparator >= 0 &&*/ posModuleSeparator != (size_t)-1)
				{
					lineNumberString = cmdArgs[0].Substring(posModuleSeparator + 1);
					moduleName = cmdArgs[0].Substring(0, posModuleSeparator);
				}
				Tuple<bool, int> val = ConvertToInt(lineNumberString);
				if (val.Item1())
				{
					debugger->AddBreakpoint(val.Item2(), moduleName, rest.size() > 0 ? rest : string::Empty);
					added = true;
				}
			}
		}
		if (!added && debugger != null)
		{
			debugger->Output->WriteLine("Warning: no breakpoint set or modified");
		}
	}

	string LispDebugger::GetStringLiteral(const string & text, /*out*/ size_t & stopPos)
	{
		string result = string::Empty;
		stopPos = text.size();

		size_t i = 0;
		while (i < text.size())
		{
			char ch = text[i];
			if (ch == '"')
			{
				// string literal is finished, stop loop
				stopPos = i;
				break;
			}
			result += ch;
			i++;
		}

		return result;
	}

	void LispDebugger::ShowInteractiveCmds(std::shared_ptr<TextWriter> output)
	{
		output->WriteLine();
		output->WriteLine("help for interactive loop:");
		output->WriteLine();
		output->WriteLine("  (h)elp                       : show this help");
		output->WriteLine("  version                      : show of this interpreter");
		output->WriteLine("  about                        : show informations about this interpreter");
		output->WriteLine("  (c)ode                       : show the program code");
		output->WriteLine("  stac(k)                      : show the current call stack");
		output->WriteLine("  (u)p                         : go one step up in call stack");
		output->WriteLine("  (d)own                       : go one step down in call stack");
		output->WriteLine("  (r)un                        : execute the program");
		output->WriteLine("  (s)tep                       : step into function");
		output->WriteLine("  o(v)er                       : step over function");
		output->WriteLine("  (o)ut                        : step out of function");
		output->WriteLine("  (b)reak [module:]line [cond] : set a breakpoint in line no with condition cond");
		output->WriteLine("  clear [no]                   : clears a breakpoint with number no or clears all");
		output->WriteLine("  lis(t)                       : shows all breakpoints");
		output->WriteLine("  restart                      : restart program");
		output->WriteLine("  (l)ocals                     : show all local variables of current scope");
		output->WriteLine("  (g)lobals                    : show all global variables");
		output->WriteLine("  modules                      : show all available modules");
		output->WriteLine("  builtins                     : show all builtin functions");
		output->WriteLine("  funcs                        : show all available functions");
		output->WriteLine("  macros                       : show all available macros");
		output->WriteLine("  doc [function]               : show documentation for all or only given function(s)");
		output->WriteLine("  searchdoc name               : show documentation for function(s) containing name");
		output->WriteLine("  exit                         : exit the interactive loop");
		output->WriteLine();
	}

	Tuple<bool, int> LispDebugger::ConvertToInt(const string & value)
	{
		var result = 0;
		var ok = true;
		try
		{
			result = /*Convert.ToInt32*/atoi(value.c_str());
		}
		catch (/*FormatException*/...)
		{
			ok = false;
		}
		return /*new*/ Tuple<bool, int>(ok, result);
	}
}
