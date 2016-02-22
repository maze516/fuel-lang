﻿using System;

namespace CsLisp
{
    /// <summary>
    /// Exception for the FUEL lisp interpreter
    /// </summary>
    public class LispException : Exception
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="LispException"/> class.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <param name="scope">The scope.</param>
        public LispException(string text, LispScope scope = null)
            : base(text)
        {
            if (scope != null)
            {
                Data[LispUtils.StackInfo] = scope.DumpStackToString();
                Data[LispUtils.ModuleName] = scope.ModuleName;
                this.AddTokenInfos(scope.CurrentToken);
            }
        }
    }
}
