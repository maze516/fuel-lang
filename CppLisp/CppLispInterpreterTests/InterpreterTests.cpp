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

#include "stdafx.h"

#include "CppUnitTest.h"

#include "../CppLispInterpreter/Lisp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace CppLisp;

#include <math.h>

double Math_Round(double val)
{
	return round(val);
}

namespace QtLispUnitTests
{
	TEST_CLASS(UnitTestLispInterpreter)
	{
	public:

		TEST_METHOD(Test_Comments)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (println \"hello\") ; a comment\n(println; separate lists with comments\n\"world\"));comment in last line");
			Assert::AreEqual("world", result->ToString().c_str());
		}

		TEST_METHOD(Test_DoAndPrint)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (println \"hello\")\n (println \"world\"))");
			Assert::AreEqual("world", result->ToString().c_str());
		}

		TEST_METHOD(Test_PrintLnMultilines)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (println \"hello\nworld\"))");
			Assert::AreEqual("hello\nworld", result->ToString().c_str());
		}

		TEST_METHOD(Test_PrintTrace)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (trace #t) (println \"hello world\") (println (+ 9 8)) (gettrace))");
			Assert::AreEqual("hello world17", result->ToString().c_str());
		}

		TEST_METHOD(Test_If1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(if #t (+ 1 2) (- 3 5))");
			Assert::AreEqual(3, result->ToInt());
		}

		TEST_METHOD(Test_If2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(if #f (* 1 0) (/ 6 3))");
			Assert::AreEqual(2, result->ToInt());
		}

		TEST_METHOD(Test_If3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(if true 1 0)");
			Assert::AreEqual(1, result->ToInt());
		}

		TEST_METHOD(Test_If4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(if false 1 0)");
			Assert::AreEqual(0, result->ToInt());
		}

		TEST_METHOD(Test_If5)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(if true 1)");
			Assert::AreEqual(1, result->ToInt());
			result = Lisp::Eval("(if false 1)");
			Assert::IsNull(result.get());
		}

		TEST_METHOD(Test_Setf1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 1) (def b 1) (setf (first (list 'a 'b 'c)) 9))");
			Assert::AreEqual(9, result->ToInt());
		}

		TEST_METHOD(Test_SetfWithNth)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c d)) (setf (nth 2 l) 9) (print l))");
			Assert::AreEqual("(a b 9 d)", result->ToString().c_str());
		}

		TEST_METHOD(Test_SetfWithFirst)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c d)) (setf (first l) 21) (print l))");
			Assert::AreEqual("(21 b c d)", result->ToString().c_str());
		}

		TEST_METHOD(Test_SetfWithLast)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c d)) (setf (last l) \"xyz\") (print l))");
			Assert::AreEqual("(a b c \"xyz\")", result->ToString().c_str());
		}

		TEST_METHOD(Test_QuoteList)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 1) (def l '(a b c)))");
			Assert::AreEqual("(a b c)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Def1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 1) (def b 1) (def (nth 2 (list 'a 'b 'c)) 9) (+ c 2))");
			Assert::AreEqual(11, result->ToInt());
		}

		TEST_METHOD(Test_DefWithNil)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a nil) (println a))");
			Assert::AreEqual(LispToken::NilConst.c_str()	, result->ToString().c_str());
		}

		TEST_METHOD(Test_Fn)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def f (fn (x) (+ x x 1))) (println (f 8)))");
			Assert::AreEqual(17, result->ToInt());
		}

		TEST_METHOD(Test_Gdef)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f (x) (gdef z (+ x x))) (f 8) (println z))");
			Assert::AreEqual(16, result->ToInt());
		}

		TEST_METHOD(Test_Gdefn)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f (x) (gdefn g (x) (+ x x))) (f 2) (g 8))");
			Assert::AreEqual(16, result->ToInt());
		}

		TEST_METHOD(Test_Eval1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(eval (list 'def 'x 43))");
			Assert::AreEqual(43, result->ToInt());
		}

		TEST_METHOD(Test_Eval2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(eval '(def x 456))");
			Assert::AreEqual(456, result->ToInt());
		}

		TEST_METHOD(Test_Eval3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(eval #t)");
			Assert::AreEqual(true, result->ToBool());
			result = Lisp::Eval("(eval 42)");
			Assert::AreEqual(42, result->ToInt());
		}

		TEST_METHOD(Test_EvalStr)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(evalstr \"(def x 456)\")");
			Assert::AreEqual(456, result->ToInt());
		}

		TEST_METHOD(Test_Doc)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(doc 'if)");
			var s = result->ToString();
			Assert::IsTrue(s.Contains("-------------------------------------------------"));
			Assert::IsTrue(s.Contains("if  [special form]"));
			Assert::IsTrue(s.Contains("Syntax: (if cond then-block [else-block])"));
			Assert::IsTrue(s.Contains("The if statement."));
		}

		TEST_METHOD(Test_DocForDefn)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("; this is a fcn documenataion\n(defn blub (x) (+ x 1))\n(doc 'blub)");
			var s = result->ToString();
			Assert::IsTrue(s.Contains("-------------------------------------------------"));
			Assert::IsTrue(s.Contains("blub"));
			Assert::IsTrue(s.Contains("Syntax: (blub x)"));
			Assert::IsTrue(s.Contains("; this is a fcn documenataion"));
		}

		TEST_METHOD(Test_NoAutoDocForDefn)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(defn blub (x y ) (+ x y 1))\n(doc 'blub)");
			var s = result->ToString();
			Assert::IsTrue(s.Contains("-------------------------------------------------"));
			Assert::IsTrue(s.Contains("blub"));
			Assert::IsTrue(s.Contains("Syntax: (blub x y)"));
		}

		TEST_METHOD(Test_TickCount)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(tickcount)");
			Assert::IsTrue(result->IsInt());
		}

		TEST_METHOD(Test_While1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 1) (def b 1) (while (< a 10) (do (setf a (+ a 1)) (setf b (+ b 1)))))");
			Assert::AreEqual(10, result->ToInt());
		}

		TEST_METHOD(Test_Defn1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def g_prn \"START:\") (defn prn (x) (setf g_prn (add g_prn x))) (prn \"34\") (println g_prn))");
			Assert::AreEqual("START:34", result->ToString().c_str());
		}

		TEST_METHOD(Test_AddString)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(+ \"abc\" \"def() ; blub\" \"xxx\")");
			Assert::AreEqual("abcdef() ; blubxxx", result->ToString().c_str());
		}

		TEST_METHOD(Test_AddLists)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(+ '(1 2 3) '(\"hello world\" 2.3 42))");
			string s = result->ToString();
			Assert::AreEqual("(1 2 3 \"hello world\" 2.300000 42)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListFirst)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(first '(1 2 3))");
			Assert::AreEqual(1, result->ToInt());
		}

		TEST_METHOD(Test_ListFirstSymbol)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(first '(a b c))");
			Assert::AreEqual("a", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListLast)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(last '(1 2 3))");
			Assert::AreEqual(3, result->ToInt());
		}

		TEST_METHOD(Test_ListLastSymbol)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(last '(abc def xyz))");
			Assert::AreEqual("xyz", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListCar)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(car '(\"abc\" 2 3))");
			Assert::AreEqual("abc", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListRest)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(rest '(1 2 3))");
			Assert::AreEqual("(2 3)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListCdr)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(cdr '(\"nix\" 1 2 3))");
			Assert::AreEqual("(1 2 3)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListLength)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(len '(1 2 3))");
			Assert::AreEqual(3, result->ToInt());
		}

		TEST_METHOD(Test_ListAppend)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(append (list 4 54 3) (list 7 9))");
			Assert::AreEqual("(4 54 3 7 9)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPush1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c)) (push z l))");
			Assert::AreEqual("(z a b c)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPush2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c)) (push z l 2))");
			Assert::AreEqual("(a b z c)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPush3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c)) (push z l 2) (print l))");
			Assert::AreEqual("(a b z c)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPushError)
		{
			try
			{
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l 42) (push z l 2))");
				Assert::Fail();
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_ListPop1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c)) (def a (pop l)) (print a l))");
			Assert::AreEqual("a (b c)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPop2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '()) (def a (pop l)) (print a l))");
			Assert::AreEqual("NIL ()", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPop3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '()) (def a (pop l)) (print a l))");
			Assert::AreEqual("NIL ()", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPop4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c)) (def a (pop l 5)) (print a l))");
			Assert::AreEqual("NIL (a b c)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPop5)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b c)) (def a (pop l 1)) (print a l))");
			Assert::AreEqual("b (a c)", result->ToString().c_str());
		}

		TEST_METHOD(Test_ListPopError)
		{
			try
			{
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l 5) (def a (pop l)) (print a l))");
				Assert::Fail();
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_LogicalOperators)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(list (and #t #f) (and #t #t) (or #t #f) (or #f #f) (or #t #f #t))");
			Assert::AreEqual("(#f #t #t #f #t)", result->ToString().c_str());
		}

		TEST_METHOD(Test_CompareOperators1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(list (= 1 2) (= 4 4) (== \"blub\" \"blub\") (== #t #f) (equal 3 4))");
			Assert::AreEqual("(#f #t #t #f #f)", result->ToString().c_str());
			result = Lisp::Eval("(do (def a ()) (== a ()))");
			Assert::AreEqual(true, result->BoolValue());
			result = Lisp::Eval("(do (def a 42) (== a ()))");
			Assert::AreEqual(false, result->BoolValue());
			result = Lisp::Eval("(do (def a blub) (def b nix) (== a b))");
			Assert::AreEqual(false, result->BoolValue());
			result = Lisp::Eval("(do (def a blub) (def b blub) (== a b))");
			Assert::AreEqual(true, result->BoolValue());
			result = Lisp::Eval("(do (def a blub) (def b blub) (== a b))");
			Assert::AreEqual(true, result->BoolValue());
			result = Lisp::Eval("(do (def a (list 1 2 3)) (def b (list 2 3 4)) (== a b))");
			Assert::AreEqual(false, result->BoolValue());
			result = Lisp::Eval("(do (def a (list 1 2 3)) (def b (list 1 2 3)) (== a b))");
			Assert::AreEqual(true, result->BoolValue());
			result = Lisp::Eval("(do (def a (list 1 2 3)) (def b (list 1 (sym 2) 3)) (== a b))");
			Assert::AreEqual(false, result->BoolValue());
			result = Lisp::Eval("(do (def a blub) (def b nix) (!= a b))");
			Assert::AreEqual(true, result->BoolValue());
			result = Lisp::Eval("(do (def a 7) (def b 7) (!= a b))");
			Assert::AreEqual(false, result->BoolValue());
		}

		TEST_METHOD(Test_CompareOperators2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(list (< 1 2) (< 4 1) (> 5 2) (> 1 3) (> 4.0 4.0))");
			Assert::AreEqual("(#t #f #t #f #f)", result->ToString().c_str());
			result = Lisp::Eval("(do (def a \"abc\") (def b \"def\") (< a b))");
			Assert::AreEqual(true, result->BoolValue());
			result = Lisp::Eval("(do (def a \"abc\") (def b \"abc\") (< a b))");
			Assert::AreEqual(false, result->BoolValue());
			result = Lisp::Eval("(do (def a \"abc\") (def b \"def\") (<= a b))");
			Assert::AreEqual(true, result->BoolValue());
			result = Lisp::Eval("(do (def a \"abc\") (def b \"abc\") (<= a b))");
			Assert::AreEqual(true, result->BoolValue());
		}

		TEST_METHOD(Test_CompareOperators3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(list (<= 1 2) (<= 4 1) (>= 5 2) (>= 1 3) (>= 4.0 4.0) (<= 42 42))");
			Assert::AreEqual("(#t #f #t #f #t #t)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Not)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(list (! #t) (not #t) (not #f) (! #f))");
			Assert::AreEqual("(#f #f #t #t)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Arithmetric1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(+ 1 2 3 4)");
			Assert::AreEqual(10, result->ToInt());
		}

		TEST_METHOD(Test_Arithmetric2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(+ 1.1 2.2 3.3 4.3)");
			int res = (int)Math_Round(result->ToDouble() * 10.0);
			Assert::AreEqual(109, res);
		}

		TEST_METHOD(Test_Arithmetric3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(* 3 8 2)");
			Assert::AreEqual(48, result->ToInt());
		}

		TEST_METHOD(Test_Arithmetric4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(/ 1.0 2.0)");
			int res = (int)Math_Round(result->ToDouble() * 10.0);
			Assert::AreEqual(5, res);
		}

		TEST_METHOD(Test_Arithmetric5)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(/ 10 2)");
			Assert::AreEqual(5, result->ToInt());
		}

		TEST_METHOD(Test_Arithmetric6)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(- 42 12 6)");
			Assert::AreEqual(24, result->ToInt());
		}

		TEST_METHOD(Test_Arithmetric7)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(- 42.5 0.5)");
			int res = (int)Math_Round(result->ToDouble() * 10.0);
			Assert::AreEqual(420, res);
		}

		TEST_METHOD(Test_Arithmetric8)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(sub 42.5 1.5 2.0)");
			int res = (int)Math_Round(result->ToDouble() * 10.0);
			Assert::AreEqual(390, res);
		}

		TEST_METHOD(Test_Arithmetric9)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(div 12 3)");
			Assert::AreEqual(4, result->ToInt());
		}

		TEST_METHOD(Test_Arithmetric10)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(mul 2 3 4)");
			Assert::AreEqual(24, result->ToInt());
		}

		TEST_METHOD(Test_Arithmetric11)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(add 2 3 4)");
			Assert::AreEqual(9, result->ToInt());
		}

		TEST_METHOD(Test_MacrosEvaluateNested)
		{
			const string macroExpandScript = "(do\
			(define-macro-eval first-macro\
				(a b)\
				(do\
					(println first-macro)\
					(def i 1)\
					(+ a b i)\
				)\
			)\
\
			(define-macro-eval second-macro\
				(x y)\
			    (do\
					(println second-macro)\
					(* x y (first-macro (+ x 1) (+ y 2)))\
				)\
			)\
\
			(def m (second-macro 4 3))\
		)";

			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval(macroExpandScript, scope);
				Assert::AreEqual("132", result->ToString().c_str());

				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("first-macro"));
				Assert::IsTrue(s.Contains("second-macro"));
			}
		}

		TEST_METHOD(Test_MacrosEvaluateRecursive)
		{
			const string macroExpandScript = "(do\
			(define-macro-eval first-macro\
				(a b)\
				(do\
					(println first-macro)\
					(def i 1)\
					(+ a b i)\
				)\
			)\
\
			(define-macro-eval second-macro\
				(x y)\
				(do\
					(println second-macro)\
					(* x y (first-macro x (+ y 4)))\
				)\
			)\
\
			(def m (second-macro 4 3))\
		)";

			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval(macroExpandScript, scope);
				Assert::AreEqual("144", result->ToString().c_str());

				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("first-macro"));
				Assert::IsTrue(s.Contains("second-macro"));
			}
		}


		TEST_METHOD(Test_MacrosEvaluateDoubleMacroCall)
		{
			const string macroExpandScript = "(do\
			(define-macro-eval first-macro\
				(a b)\
				(do\
					(println first-macro)\
					(def i 1)\
					(+ a b i)\
				)\
			)\
\
			(define-macro-eval second-macro\
				(x y)\
				(do\
					(println second-macro)\
					(* x y)\
				)\
			)\
\
			(def m (second-macro 4 (first-macro 6 3)))\
		)";

			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval(macroExpandScript, scope);
				Assert::AreEqual("40", result->ToString().c_str());

				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("first-macro"));
				Assert::IsTrue(s.Contains("second-macro"));
			}
		}

		TEST_METHOD(Test_MacrosExpand1)
		{
#ifdef ENABLE_COMPILE_TIME_MACROS
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (define-macro-expand blub (x y) (println x y)) (println (quote (1 2 3))) (blub 3 4))");
			Assert::AreEqual("3 4", result->ToString().c_str());
#else
			Assert::IsTrue(true);
#endif
		}

		TEST_METHOD(Test_MacrosExpand2)
		{
			const string macroExpandScript = "(do\
				(define-macro-expand first-macro\
					(a b)\
					(do\
						(def i 1)\
						(+ a b i)\
					)\
				)\
\
				(define-macro-expand second-macro\
					(x y)\
					(do\
						(* x y (first-macro x y))\
					)\
				)\
\
				(def m (second-macro 4 3))\
			)";
#ifdef ENABLE_COMPILE_TIME_MACROS
			std::shared_ptr<LispVariant> result = Lisp::Eval(macroExpandScript);
			Assert::AreEqual("96", result->ToString().c_str());
#else
			Assert::IsTrue(true);
#endif
		}

              		TEST_METHOD(Test_MacrosExpandDefineStruct)
		{
			const string macroExpandScript = "(do \
  (define-macro-expand defstruct (name field1) \
        (do            \
 \
	       (defn (sym (+ (str make-) (str name))) ((sym field1) (nth 2 (quoted-macro-args))) \
              (list (arg 0) (arg 1)) \
	       ) \
 \
    	   (println 'a (nth 1 (quoted-macro-args)) (nth 2 (quoted-macro-args))) \
        ) \
  ) \
    \
  (defstruct point x y) \
  (def p (make-point 2 3)) \
)";
#ifdef ENABLE_COMPILE_TIME_MACROS
					{
						std::shared_ptr<LispVariant> result = Lisp::Eval(macroExpandScript);
						Assert::AreEqual("(2 3)", result->ToString().c_str());
					}
#else
					{
						Assert::IsTrue(true);
					}
#endif
		}

		TEST_METHOD(Test_MacrosExpandNested)
		{
			const string macroExpandScript = "(do\
				(define-macro-expand first-macro\
					(a b)\
					(do\
						(println first-macro)\
						(def i 1)\
						(+ a b i)\
					)\
				)\
\
				(define-macro-expand second-macro\
					(x y)\
					(do\
						(println second-macro)\
							(* x y (first-macro (+ x 1) (+ y 2)))\
						)\
					)\
\
				(def m (second-macro 4 3))\
			)";

#ifdef ENABLE_COMPILE_TIME_MACROS
			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval(macroExpandScript, scope);
				Assert::AreEqual("132", result->ToString().c_str());

				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("first-macro"));
				Assert::IsTrue(s.Contains("second-macro"));
			}
#else
			Assert::IsTrue(true);
#endif
		}

		TEST_METHOD(Test_MacrosExpandRecursive)
		{
			const string macroExpandScript = "(do\
				(define-macro-expand first-macro\
					(a b)\
					(do\
						(println first-macro)\
						(def i 1)\
						(+ a b i)\
					)\
				)\
\
				(define-macro-expand second-macro\
					(x y)\
					(do\
						(println second-macro)\
						(* x y (first-macro x (+ y 4)))\
					)\
				)\
\
				(def m (second-macro 4 3))\
			)";

#ifdef ENABLE_COMPILE_TIME_MACROS
				//using (ConsoleRedirector cr = new ConsoleRedirector())
				{
					var scope = LispEnvironment::CreateDefaultScope();
					scope->Output->EnableToString(true);
					std::shared_ptr<LispVariant> result = Lisp::Eval(macroExpandScript, scope);
					Assert::AreEqual("144", result->ToString().c_str());

					string s = scope->Output->GetContent().Trim();
					Assert::IsTrue(s.Contains("first-macro"));
					Assert::IsTrue(s.Contains("second-macro"));
				}
#else
				Assert::IsTrue(true);
#endif
		}


		TEST_METHOD(Test_MacrosExpandDoubleMacroCall)
		{
			const string macroExpandScript = "(do\
				(define-macro-expand first-macro\
					(a b)\
					(do\
						(println first-macro)\
						(def i 1)\
						(+ a b i)\
					)\
				)\
\
				(define-macro-expand second-macro\
					(x y)\
					(do\
						(println second-macro)\
						(* x y)\
					)\
				)\
\
				(def m (second-macro 4 (first-macro 6 3)))\
			)";

#ifdef ENABLE_COMPILE_TIME_MACROS
			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval(macroExpandScript, scope);
				Assert::AreEqual("40", result->ToString().c_str());

				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("first-macro"));
				Assert::IsTrue(s.Contains("second-macro"));
			}
#else
			Assert::IsTrue(true);
#endif
		}

		TEST_METHOD(Test_MacrosSetf1)
		{
#ifdef ENABLE_COMPILE_TIME_MACROS
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result =
					Lisp::Eval(
						"(do (def a 42) (define-macro-expand my-setf (x value) (setf x value)) (my-setf a (+ \"blub\" \"xyz\")) (println a))", scope);
				Assert::AreEqual("blubxyz", result->ToString().c_str());
			}
#else
			Assert::IsTrue(true);
#endif
		}

		TEST_METHOD(Test_MacrosSetf2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 42) (defn my-setf (x value) (setf x value)) (my-setf a (+ 8 9)) (println a))");
			Assert::AreEqual(42, result->ToInt());
		}

		TEST_METHOD(Test_MacrosSetf3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 42) (define-macro-eval my-setf (x value) (setf x value)) (my-setf a (+ \"blub\" \"xyz\")) (println a))");
			Assert::AreEqual("blubxyz", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a '(42 99 102 \"hello\")) (def b 55) (println (type a)) (println (nth 3 `(1 2 3 ,@a))))");
			Assert::AreEqual("42", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 42) (println `(1 2 3 ,a)))");
			Assert::AreEqual("(1 2 3 42)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 42) (def lst (list 6 8 12)) (println (quasiquote (1 2 3 ,a ,@lst))))");
			Assert::AreEqual("(1 2 3 42 6 8 12)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 42) (def lst (list 6 8 12)) (println (quasiquote (1 2 3 ,a ,lst))))");
			Assert::AreEqual("(1 2 3 42 (6 8 12))", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote5)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 42) (println (quasiquote (1 2 3 ,(+ 3 a)))))");
			Assert::AreEqual("(1 2 3 45)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote6)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 42) (println (quasiquote (1 2 3 ,@(list 9 8 7 a)))))");
			Assert::AreEqual("(1 2 3 9 8 7 42)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote7)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def args '(1 2 3)) `,(first args))");
			Assert::AreEqual("1", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote8)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def args '(1 2 3)) `(,(first args)))");
			Assert::AreEqual("(1)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote9)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do `(b))");
			Assert::AreEqual("(b)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quasiquote10)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do `a)");
			Assert::AreEqual("a", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quote1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def x 42) (println 'x))");
			Assert::AreEqual("x", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quote2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do 'x)");
			Assert::AreEqual("x", result->ToString().c_str());
		}

		TEST_METHOD(Test_Quote3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do '(a b 6 x))");
			Assert::AreEqual("(a b 6 x)", result->ToString().c_str());
		}

		TEST_METHOD(Test_EvalQuasiQuote1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 4) (def lst '(a 2 3)) (eval `,(first lst)))");
			Assert::AreEqual("4", result->ToString().c_str());
		}

		TEST_METHOD(Test_String1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(println \"hello \\\\ \\' ��� \n \\\"blub\\\"\")");
			Assert::AreEqual("hello \\ ' ��� \n \"blub\"", result->ToString().c_str());
		}

		TEST_METHOD(Test_String2)
		{
			std::shared_ptr<LispVariant>  result = Lisp::Eval("(string \"hello\" \"-\" \"world\")");
			Assert::AreEqual("hello-world", result->ToString().c_str());
		}

		TEST_METHOD(Test_Map)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(map (lambda (x) (+ x 1)) '(1 2 3))");
			Assert::AreEqual("(2 3 4)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Reduce1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(reduce (lambda (x y) (+ x y)) '(1 2 3) 0)");
			Assert::AreEqual(6, result->ToInt());
		}

		TEST_METHOD(Test_Reduce2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(reduce (lambda (x y) (* x y)) '(2 3 4 5) 2)");
			Assert::AreEqual(240, result->ToInt());
		}

		TEST_METHOD(Test_Closure1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn addx (delta) (lambda (x) (+ x delta))) (def addclosure (addx 41)) (println (addclosure 1)))");
			Assert::AreEqual(42, result->ToInt());
		}

		TEST_METHOD(Test_Closure2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn g (x) (do (+ x 2))) (defn f (x) (do (def i 7) (+ x 1 i (g 2)))) (println (f 1)))");
			Assert::AreEqual(13, result->	ToInt());
		}

		TEST_METHOD(Test_Closure3)
		{
			try
			{
				Lisp::Eval("(do (defn g (x) (do (+ x 2 i))) (defn f (x) (do (def i 7) (+ x 1 i (g 2)))) (println (f 1)))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_Closure4)
		{
// TODO working...
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn g (x) (do (+ x 2))) (defn f (x) (do (def i 7) (+ x 1 i (g 2)))) (println (f 1)))");
			Assert::AreEqual(13, result->ToInt());
		}

		TEST_METHOD(Test_RecursiveCall1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn addConst (x a) (+ x a)) (def add2 (lambda (x) (addConst x 2))) (println (addConst 8 2)) (println (add2 4)))");
			Assert::AreEqual(6, result->ToInt());
		}

		TEST_METHOD(Test_Return1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f (x) (return (+ x x))) (println (f 7)))");
			Assert::AreEqual(14, result->ToInt());
		}

		TEST_METHOD(Test_Return2)
		{
			// return statement was not implmented correctly until 25.7.2018...
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f (x) (do (return (+ x x)) (return 9))) (println (f 7)))");
			Assert::AreEqual(14, result->ToInt());
		}

		/* TODO --> not implemented yet for C++
		TEST_METHOD(Test_Call1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def obj 0) (setf obj (call \"CsLisp.DummyNative\")) (println obj (type obj)) (call obj \"Test\"))");
			Assert::AreEqual(42, result->ToInt());
		}

		TEST_METHOD(Test_CallStatic)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (call-static \"System.IO.File\" Exists \"dummy\"))");
			Assert::AreEqual(false, result->ToBool());
		}
*/

		TEST_METHOD(Test_CallStaticError)
		{
			try
			{
				Lisp::Eval("(do (call-static \"System.IO.File\" NotExistingFunction \"dummy\"))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_Apply1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(apply (lambda (x) (println \"hello\" x)) '(55))");
			Assert::AreEqual("hello 55", result->ToString().c_str());
		}

		TEST_METHOD(Test_Apply2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def f (lambda (x) (+ x x))) (apply f '(5)))");
			Assert::AreEqual(10, result->ToInt());
		}

		TEST_METHOD(Test_Apply3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def f '+) (apply f '(5 6 7)))");
			Assert::AreEqual(18, result->ToInt());
		}

		TEST_METHOD(Test_Argscount1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f (x) (do (println \"count=\" (argscount)) (+ x x))) (f 5 6 7))");
			Assert::AreEqual(10, result->ToInt());
		}

		TEST_METHOD(Test_Arg2)
		{
			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f (x) (do (println \"count=\" (argscount)) (println (arg 0)) (println (arg 1)) (println (arg 2)) (println \"additional=\" (nth 1 _additionalArgs)) (+ x x))) (f 5 6 7))", scope);
				Assert::AreEqual(10, result->ToInt());

				string s = scope->Output->GetContent().Trim();
				Assert::AreEqual(true, s.Contains("count= 3"));
				Assert::AreEqual(true, s.Contains("5"));
				Assert::AreEqual(true, s.Contains("6"));
				Assert::AreEqual(true, s.Contains("7"));
				Assert::AreEqual(true, s.Contains("additional= 7"));
			}
		}

		TEST_METHOD(Test_Arg3)
		{
			try
			{
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f (x) (do (arg 7) (+ x x))) (f 5 6 7))");
				Assert::AreEqual(10, result->ToInt());
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_Arg4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f () (do (def z (arg 2)) (+ z z))) (f 5 6 7))");
			Assert::AreEqual(14, result->ToInt());
		}

		TEST_METHOD(Test_Args1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f () (do (def z (args)))) (f 5 6 7))");
			Assert::AreEqual("(5 6 7)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Cons1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(cons 1 2)");
			Assert::AreEqual("(1 2)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Cons2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(cons 1 '(2 3 4))");
			Assert::AreEqual("(1 2 3 4)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Cons3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(cons 12)");
			Assert::AreEqual("(12)", result->ToString().c_str());
		}

		TEST_METHOD(Test_Cons4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(cons)");
			Assert::AreEqual("()", result->ToString().c_str());
		}

		TEST_METHOD(Test_Nop)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(nop)");
			Assert::IsTrue(result->IsUndefined());
		}

		TEST_METHOD(Test_Symbol)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(sym a)");
			Assert::IsTrue(result->IsSymbol());
			Assert::AreEqual("a", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Str)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(str abc)");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("abc", result->StringValue().c_str());
		}

		TEST_METHOD(Test_MapError1)
		{
			try
			{
				Lisp::Eval("(map 4 '(1 2 3))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_MapError2)
		{
			try
			{
				Lisp::Eval("(map (lambda (x) (+ x 1)) 4)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_ReduceError1)
		{
			try
			{
				Lisp::Eval("(reduce \"blub\" '(1 2 3) 0)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_ReduceError2)
		{
			try
			{
				Lisp::Eval("(reduce (lambda (x y) (+ x y))  \"test\" 0)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_SetfError)
		{
			try
			{
				Lisp::Eval("(setf a 2.0)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_Parser1)
		{
			try
			{
				Lisp::Eval("(println \"hello\"))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_Parser2)
		{
			try
			{
				Lisp::Eval("((println \"hello\")");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_Parser3)
		{
			try
			{
				Lisp::Eval("(blub 1 2 3)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_NotError)
		{
			try
			{
				Lisp::Eval("(not a 2.0)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_CompareError1)
		{
			try
			{
				Lisp::Eval("(> 2.0)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_CompareError2)
		{
			try
			{
				Lisp::Eval("(> 2.0 5 234)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_ScriptToLong)
		{
			try
			{
				Lisp::Eval("(setf a 2.0) asdf");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_DefError)
		{
			try
			{
				Lisp::Eval("(def 1 2)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_DoError)
		{
			try
			{
				Lisp::Eval("(do (def a 2) blub (setf a 5))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_IfError)
		{
			try
			{
				Lisp::Eval("(if #t 1 2 3)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_FunctionNotFound)
		{
			try
			{
				Lisp::Eval("(unknown-fcn 1 2 3)");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_BracketsOutOfBalance1)
		{
			try
			{
				Lisp::Eval("(do (println 2)))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_BracketsOutOfBalance2)
		{
			try
			{
				Lisp::Eval("(do ( (println 2))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_UnexpectedToken1)
		{
			try
			{
				Lisp::Eval("blub (do (println 2))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_UnexpectedTokenButIsBracketsOutOfBalance)
		{
			try
			{
				Lisp::Eval("(do (println 2)) asfd");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_LispVariantToString)
		{
			LispVariant result(std::make_shared<object>("hello"));
			string s = result.ToString();
			Assert::AreEqual("hello", s.c_str());
		}

		TEST_METHOD(Test_LispScope1)
		{
			LispScope scope;
			var result = scope.GetPreviousToken(std::make_shared<LispToken>("a", 0, 0, 1));
			Assert::IsNull(result.get());
		}

		TEST_METHOD(Test_LispTokenToString)
		{
			LispToken token("(", 0, 1, 1);
			string s = token.ToString();
			Assert::AreEqual("(", s.c_str());
		}

		TEST_METHOD(Test_Type1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(type 7)");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(3, result->IntValue());
		}

		TEST_METHOD(Test_Type2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(type #f)");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(2, result->IntValue());
		}

		TEST_METHOD(Test_Type3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(type '(2 3 1))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(6, result->IntValue());
		}

		TEST_METHOD(Test_Type4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(type aSymbol)");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(8, result->IntValue());
		}

		TEST_METHOD(Test_Type5)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(type \"a string\")");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(5, result->IntValue());
		}

		TEST_METHOD(Test_TypeStr)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(typestr 1.23)");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("Double", result->Value->ToString().c_str());
		}

		TEST_METHOD(Test_Vars)
		{
			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 4) (def b \"asdf\") (vars))", scope);
				Assert::IsTrue(result->IsUndefined());

				string s = scope->Output->GetContent().Trim();
				Assert::AreEqual(true, s.Contains("a --> 4"));
				Assert::AreEqual(true, s.Contains("b --> \"asdf\""));
			}
		}

		TEST_METHOD(Test_Fuel)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(fuel)");
			Assert::IsTrue(result->IsString());
			Assert::IsTrue(result->StringValue().Contains("fuel version"));
		}

		TEST_METHOD(Test_Copyright)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(copyright)");
			Assert::IsTrue(result->IsString());
			Assert::IsTrue(result->StringValue().Contains("Copyright: MIT-License"));
		}

		TEST_METHOD(Test_Help)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(help)");
			Assert::IsTrue(result->IsString());
			Assert::IsTrue(result->StringValue().Contains("available functions:"));
		}

		TEST_METHOD(Test_Import)
		//[DeploymentItem(@"..\..\..\Library\fuellib.fuel", "Library")]
		{
			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (import \"Library\\\\fuellib.fuel\") (foreach '(1 5 7) (lambda (x) (println x))))", scope);
				Assert::IsTrue(result->IsInt());
				Assert::AreEqual(3, result->IntValue());

				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("1"));
				Assert::IsTrue(s.Contains("7"));
				Assert::IsTrue(s.Contains("7"));
			}
		}

		TEST_METHOD(Test_Import2)
		//[DeploymentItem(@"..\..\..\Library\fuellib.fuel", "Library")]
		{
			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (import fuellib) (foreach '(1 4 6) (lambda (x) (println x))))", scope);
				Assert::IsTrue(result->IsInt());
				Assert::AreEqual(3, result->IntValue());    // is last value of internal loop variable in foreach

				// test results
				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("1"));
				Assert::IsTrue(s.Contains("4"));
				Assert::IsTrue(s.Contains("6"));
			}
		}

		TEST_METHOD(Test_BadForeach)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (import fuellib) (foreach))");
			Assert::IsTrue(result->IsUndefined());
		}

		TEST_METHOD(Test_Break)
		{
			//using (ConsoleRedirector cr = new ConsoleRedirector())
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				std::shared_ptr<LispVariant> result = Lisp::Eval("(break)", scope);
				Assert::IsTrue(result->IsUndefined());

				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("no debugger support"));
			}
		}

		TEST_METHOD(Test_ReadLine)
		{
			//using (ConsoleRedirector cr = new ConsoleRedirector("some input\nmore input\n"))
			{
				var scope = LispEnvironment::CreateDefaultScope();
				scope->Output->EnableToString(true);
				scope->Input->EnableFromString(true);
				scope->Input->SetContent("some input\nmore input\n");
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a (readline)) (println a) (def b (readline)) (println b))", scope);
				Assert::IsTrue(result->IsString());
				Assert::AreEqual("more input", result->ToString().c_str());

				string s = scope->Output->GetContent().Trim();
				Assert::IsTrue(s.Contains("some input"));
				Assert::IsTrue(s.Contains("more input"));
			}
		}

		TEST_METHOD(Test_ReadLineWithArgs)
		{
			//using (ConsoleRedirector cr = new ConsoleRedirector("some input\nmore input\n"))
			{
				try
				{
					var scope = LispEnvironment::CreateDefaultScope();
					scope->Output->EnableToString(true);
					std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a (readline 1)) (println a)", scope);
					Assert::IsTrue(false);
				}
				catch (const CppLisp::LispException &)
				{
					Assert::IsTrue(true);
				}
				catch (...)
				{
					Assert::IsTrue(false);
				}
			}
		}

		TEST_METHOD(Test_ParseInteger)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"42\") (def i (parse-integer s)))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(42, result->IntValue());
		}

		TEST_METHOD(Test_ParseIntegerError)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"nonumber\") (def i (parse-integer s)))");
			Assert::IsTrue(result->IsUndefined());
		}

		TEST_METHOD(Test_ParseFloat)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"42.789\") (def f (parse-float s)))");
			Assert::IsTrue(result->IsDouble());
			Assert::AreEqual(42.789, result->DoubleValue());
		}

		TEST_METHOD(Test_ParseFloatError)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"nonumber\") (def f (parse-float s)))");
			Assert::IsTrue(result->IsUndefined());
		}

		TEST_METHOD(Test_Slice1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (slice s 0 4))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("this", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Slice2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (slice s 8 -1))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("a string", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Slice3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (slice s 5 4))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("is a", result->StringValue().c_str());
		}

		TEST_METHOD(Test_SliceError)
		{
			try
			{
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (slice s))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_FirstForString)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (first s))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("t", result->StringValue().c_str());
		}

		TEST_METHOD(Test_LastForString)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (last s))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("g", result->StringValue().c_str());
		}

		TEST_METHOD(Test_RestForString)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (rest s))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("his is a string", result->StringValue().c_str());
		}

		TEST_METHOD(Test_NthForString)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (nth 5 s))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("i", result->StringValue().c_str());
		}

		TEST_METHOD(Test_LenForString)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a string\") (len s))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(16, result->IntValue());
		}

		TEST_METHOD(Test_Trim)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \" \t   this is a string  \") (trim s))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("this is a string", result->StringValue().c_str());
		}

		TEST_METHOD(Test_LowerCase)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"THIS is A stRing\") (lower-case s))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("this is a string", result->StringValue().c_str());
		}

		TEST_METHOD(Test_UpperCase)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"THIS is A stRing 8 ,.!?\") (upper-case s))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("THIS IS A STRING 8 ,.!?", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Find1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (import \"Library\\\\fuellib.fuel\") (def l '(1 5 7)) (find 5 l))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(1, result->IntValue());
		}

		TEST_METHOD(Test_Find2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (import \"Library\\\\fuellib.fuel\") (def l '(1 5 7)) (find 9 l))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(-1, result->IntValue());
		}

		TEST_METHOD(Test_DoTimes1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (import fuellib) (def l '()) (dotimes (ix 7) (setf l (cons ix l))) (println l))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("(6 5 4 3 2 1 0)", result->StringValue().c_str());
		}

		TEST_METHOD(Test_DoTimes2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (import fuellib) (def l '()) (dotimes (i 7) (setf l (cons i l))) (dotimes (i 9) (setf l (cons i l))) (println l))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("(8 7 6 5 4 3 2 1 0 6 5 4 3 2 1 0)", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Reverse)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l (list 1 2 b \"nix\" 4.5)) (print (reverse l)))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("(4.500000 \"nix\" b 2 1)", result->StringValue().c_str());
		}

		TEST_METHOD(Test_ReverseString)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is text\") (print (reverse s)))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("txet si siht", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Search1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is text\") (print (search \"tex\" s)))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("8", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Search2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is text\") (search \"tes\" s))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(-1, result->IntValue());
		}

		TEST_METHOD(Test_Search3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b 4 d 5.234 \"blub\")) (search b l))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(1, result->IntValue());
		}

		TEST_METHOD(Test_Search4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b 4 d 5.234 \"blub\")) (search 4 l))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(2, result->IntValue());
		}

		TEST_METHOD(Test_Search5)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b 4 d 5.234 \"blub\")) (search \"blub\" l))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(5, result->IntValue());
		}

		TEST_METHOD(Test_Search6)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l '(a b 4 d 5.234 \"blub\")) (search nix l))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(-1, result->IntValue());
		}

		TEST_METHOD(Test_Search7)
		{
			try
			{
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def l 5.234) (search nix l))");
				Assert::IsTrue(result->IsInt());
				Assert::AreEqual(-1, result->IntValue());
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_Replace1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a long text\") (replace s \"long\" \"short\"))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("this is a short text", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Replace2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a long long text\") (replace s \"long\" \"short\"))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("this is a short short text", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Replace3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a long text\") (replace s \"verylong\" \"short\"))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("this is a long text", result->StringValue().c_str());
		}

		TEST_METHOD(Test_Replace4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def s \"this is a long text with a lot of words a\") (replace s \"a \" \"an \"))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("this is an long text with an lot of words a", result->StringValue().c_str());
		}

		TEST_METHOD(Test_UnaryMinusInt)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 8) (- a))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(-8, result->IntValue());
		}

		TEST_METHOD(Test_UnaryMinusDouble)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a 1.234) (- a))");
			Assert::IsTrue(result->IsDouble());
			Assert::AreEqual(-1.234, result->DoubleValue());
		}

		TEST_METHOD(Test_UnaryMinusString)
		{
			try
			{
				std::shared_ptr<LispVariant> result = Lisp::Eval("(do (def a \"nix\") (- a))");
				Assert::IsTrue(false);
			}
			catch (const CppLisp::LispException &)
			{
				Assert::IsTrue(true);
			}
			catch (...)
			{
				Assert::IsTrue(false);
			}
		}

		TEST_METHOD(Test_ModuloInt)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (% 7 3))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(1, result->IntValue());
		}

		TEST_METHOD(Test_ModuloDouble)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (% 7.4 2.8))");
			Assert::IsTrue(result->IsDouble());
			Assert::AreEqual("1.800000", std::to_string(result->DoubleValue()).c_str());
		}

		TEST_METHOD(Test_NotEnoughFunctionArguments)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (defn f (x y z) (add (str x) (str y) (str z))) (f 3))");
			Assert::IsTrue(result->IsString());
			Assert::AreEqual("3NILNIL", result->StringValue().c_str());
		}

		TEST_METHOD(Test_IntConversion1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (int 7.4))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(7, result->IntValue());
		}

		TEST_METHOD(Test_IntConversion2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (int \"61.234\"))");
			Assert::IsTrue(result->IsUndefined());
		}

		TEST_METHOD(Test_IntConversion3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (int #t))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(1, result->IntValue());
		}

		TEST_METHOD(Test_IntConversion4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (int #f))");
			Assert::IsTrue(result->IsInt());
			Assert::AreEqual(0, result->IntValue());
		}

		TEST_METHOD(Test_IntConversion5)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (int \"a text\"))");
			Assert::IsTrue(result->IsUndefined());
		}

		TEST_METHOD(Test_FloatConversion1)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (float 7))");
			Assert::IsTrue(result->IsDouble());
			Assert::AreEqual(7.0, result->DoubleValue());
		}

		TEST_METHOD(Test_FloatConversion2)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (float \"61.234\"))");
			Assert::IsTrue(result->IsDouble());
			Assert::AreEqual(61.234, result->DoubleValue());
		}

		TEST_METHOD(Test_FloatConversion3)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (float #t))");
			Assert::IsTrue(result->IsDouble());
			Assert::AreEqual(1.0, result->DoubleValue());
		}

		TEST_METHOD(Test_FloatConversion4)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (float #f))");
			Assert::IsTrue(result->IsDouble());
			Assert::AreEqual(0.0, result->DoubleValue());
		}

		TEST_METHOD(Test_FloatConversion5)
		{
			std::shared_ptr<LispVariant> result = Lisp::Eval("(do (float \"a text\"))");
			Assert::IsTrue(result->IsUndefined());
		}
	
		// TODO / NOT IMPLEMENTED:
		// Test_CreateNative
		// Test_RegisterNativeObjects
		// Test_StdLibArray
		// Test_StdLibDictionary
		// Test_StdLibFile
		// Test_StdLibList
		// Test_StdLibListSort
		// Test_StdLibMath
		// Test_StdLibMath2

		// TODO --> open points:
		// compare functions in lisp available ? --> (== f g) where f and g are functions: (defn f (x) (+ x x))
	};
}