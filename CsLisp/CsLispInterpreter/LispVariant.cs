﻿using System;
using System.Collections.Generic;
using System.Globalization;

namespace CsLisp
{
    /// <summary>
    /// Lisp data types.
    /// </summary>
    public enum LispType
    {
        Undefined = 0,
        Nil = 1,
        Bool = 2,
        Int = 3,
        Double = 4,
        String = 5,
        List = 6,
        Function = 7,
        Symbol = 8,
        NativeObject = 9,
        //Array = 10,
        Error = 999
    }

    /// <summary>
    /// Enumeration for unqoute modus
    /// </summary>
    public enum LispUnQuoteModus
    {
        None = 0,
        UnQuote = 1,
        UnQuoteSplicing = 2
    }

    /// <summary>
    /// Generic data container for lisp data types.
    /// </summary>
    public class LispVariant
    {
        #region constants

        private const string CanNotConvertTo = "can not convert to ";
        private const string NoOperatorForTypes = "no {0} operator for types {1} and {2}";

        #endregion

        #region properties

        private static double Tolerance { get; set; }

        public LispUnQuoteModus IsUnQuoted { get; private set; }

        public object Value { get; set; }

        public LispType Type { get; set; }

        public LispToken Token { get; private set; }

        public bool IsNil
        {
            get { return Type == LispType.Nil; }
        }

        public bool IsError
        {
            get { return Type == LispType.Error; }
        }

        public bool IsUndefined
        {
            get { return Type == LispType.Undefined; }
        }

        public bool IsString
        {
            get { return Type == LispType.String; }
        }

        public bool IsDouble
        {
            get { return Type == LispType.Double; }
        }

        public bool IsInt
        {
            get { return Type == LispType.Int; }
        }

        public bool IsBool
        {
            get { return Type == LispType.Bool; }
        }

        public bool IsList
        {
            get { return Type == LispType.List || Type == LispType.Nil; }
        }

        public bool IsFunction
        {
            get { return Type == LispType.Function; }
        }

        public bool IsSymbol
        {
            get { return Type == LispType.Symbol; }
        }

        public bool IsNativeObject
        {
            get { return Type == LispType.NativeObject; }
        }

        #endregion

        #region Constructer

        static LispVariant()
        {
            Tolerance = 1e-8;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="LispVariant"/> class.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <param name="value">The value.</param>
        /// <param name="unQuoted">The unquoted modus.</param>
        public LispVariant(LispType type = LispType.Undefined, object value = null, LispUnQuoteModus unQuoted = LispUnQuoteModus.None)
        {
            Type = type;
            Value = value;
            IsUnQuoted = unQuoted;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="LispVariant"/> class.
        /// </summary>
        /// <param name="val">The value.</param>
        /// <param name="unQuoted">The unquoted modus.</param>
        public LispVariant(object val, LispUnQuoteModus unQuoted = LispUnQuoteModus.None)
            : this(TypeOf(val), val, unQuoted)
        {
            var value = val as LispVariant;
            if (value != null)
            {                
                Type = value.Type;
                Value = value.Value;
                IsUnQuoted = value.IsUnQuoted;
            }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="LispVariant"/> class.
        /// </summary>
        /// <param name="token">The token.</param>
        /// <param name="unQuoted">The unquoted modus.</param>
        internal LispVariant(LispToken token, LispUnQuoteModus unQuoted = LispUnQuoteModus.None)
            : this(TypeOf(token.Value), token.Value, unQuoted)
        {
            Token = token;
            if (token.Type == LispTokenType.Nil)
            {
                Type = LispType.Nil;
            }
            if (token.Type == LispTokenType.Symbol)
            {
                Type = LispType.Symbol;
            }
        }

        /// <summary>
        /// Creates a new value representing an error.
        /// </summary>
        /// <param name="errorMessage">The error message.</param>
        /// <returns>The value</returns>
        public static LispVariant CreateErrorValue(string errorMessage)
        {
            return new LispVariant(LispType.Error, errorMessage);
        }

        #endregion

        #region Casts

        public LispFunctionWrapper FunctionValue
        {
            get
            {
                if (Type != LispType.Function)
                {
                    throw CreateInvalidCastException("function");
                }
                return (LispFunctionWrapper)Value;
            }
        }

        public IEnumerable<object> ListValue
        {
            get
            {
                // Nil is an empty list () !
                if (Type == LispType.Nil)
                {
                    return new List<object>();
                }
                if (Type != LispType.List)
                {
                    throw CreateInvalidCastException("list");
                }
                return (IEnumerable<object>)Value;
            }
        }

        public double DoubleValue
        {
            get
            {
                if (Type != LispType.Double)
                {
                    throw CreateInvalidCastException("double");
                }
                return (double)Value;
            }
        }

        public int IntValue
        {
            get
            {
                if (Type != LispType.Int)
                {
                    throw CreateInvalidCastException("int");
                }
                return (int)Value;
            }
        }

        public bool BoolValue
        {
            get
            {
                if (Type != LispType.Bool)
                {
                    throw CreateInvalidCastException("bool");
                }
                return (bool)Value;
            }
        }

        public object NativeObjectValue
        {
            get
            {
                if (Type != LispType.NativeObject)
                {
                    throw CreateInvalidCastException("native object");
                }
                return Value;
            }
        }

        public bool ToBool()
        {
            if (IsBool)
            {
                return BoolValue;
            }
            if (IsInt)
            {
                return IntValue != 0;
            }
            if (IsDouble)
            {
                return Math.Abs(DoubleValue) > Tolerance;
            }
            throw CreateInvalidCastException("bool", CanNotConvertTo);
        }

        public int ToInt()
        {
            if (IsBool)
            {
                return BoolValue ? 1 : 0;
            }
            if (IsInt)
            {
                return IntValue;
            }
            if (IsDouble)
            {
                return (int) DoubleValue;
            }
            if (IsString)
            {
                return Convert.ToInt32(StringValue);
            }
            throw CreateInvalidCastException("int", CanNotConvertTo);
        }

        public double ToDouble()
        {
            if (IsBool)
            {
                return BoolValue ? 1.0 : 0.0;
            }
            if (IsInt)
            {
                return IntValue;
            }
            if (IsDouble)
            {
                return DoubleValue;
            }
            if (IsString)
            {
                return Convert.ToDouble(StringValue);
            }
            throw CreateInvalidCastException("double", CanNotConvertTo);
        }

        public string StringValue
        {
            get
            {
                return Value.ToString();
            }
        }

        /// <summary>
        /// Comverts this value into a string representation used by the compiler module.
        /// </summary>
        /// <returns>The string representation</returns>
        public string ToStringCompiler()
        {
            if (IsBool)
            {
                return BoolValue ? "true" : "false";
            }
            if (IsDouble)
            {
                return DoubleValue.ToString("F", CultureInfo.InvariantCulture);
            }
            if (IsString)
            {
                return "\"" + StringValue + "\"";
            }
            return ToString();
        }

        public override string ToString()
        {
            if (IsString)
            {
                return StringValue;
            }
            if (IsInt)
            {
                return IntValue.ToString(CultureInfo.InvariantCulture);
            }
            if (IsDouble)
            {
                return DoubleValue.ToString(CultureInfo.InvariantCulture);
            }
            if (IsBool)
            {
                return BoolValue ? "#t" : "#f";
            }
            if (IsNil)
            {
                return "NIL";
            }
            if (IsList)
            {
                /*
                string ret = string.Empty;
                foreach (var elem in ListValue)
                {
                    if (ret.Length > 0)
                    {
                        ret += ", ";
                    }
                    ret += elem.ToString();
                }
                return "("+ret+")";
                */
                return ExpandContainerToString(ListValue);
            }
            if (IsFunction)
            {
                return "function " + (FunctionValue.Signature != null ? FunctionValue.Signature : "<unknown>");
            }
            if (IsSymbol)
            {
                return Value.ToString();
            }
            if (IsNativeObject)
            {
                return "NativeObject<"+Value.GetType()+">";
            }
            if (IsUndefined)
            {
                return "<undefined>";
            }
            if (IsError)
            {
                return "Error: " + Value;
            }
            return "?";
        }

        private static string ExpandContainerToString(object maybeContainer)
        {
            string ret = string.Empty;

            if (maybeContainer is IEnumerable<object>)
            {
                var container = (IEnumerable<object>)maybeContainer;
                foreach (var item in container)
                {
                    if (ret.Length > 0)
                    {
                        ret += " ";
                    }
                    ret += ExpandContainerToString(item);
                }
                ret = "(" + ret + ")";
            }
            else
            {
                ret += maybeContainer.ToString();
            }

            return ret;
        }

        #endregion

        public bool SymbolCompare(object other)
        {
            if (other is LispVariant)
            {
                return Value.Equals(((LispVariant)other).Value);
            }
            return false;
        }

        #region Operations
        /*
        public override bool Equals(object other)
        {
            if (other is LispVariant)
            {
                return Value.Equals(((LispVariant)other).Value);                
            }
            return false;
        }
        */
        /*
        protected bool Equals(LispVariant other)
        {
            return IsUnQuoted == other.IsUnQuoted && Equals(Value, other.Value) && Type == other.Type && Equals(Token, other.Token);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((LispVariant)obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                int hashCode = (int)IsUnQuoted;
                hashCode = (hashCode * 397) ^ (Value != null ? Value.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (int)Type;
                hashCode = (hashCode * 397) ^ (Token != null ? Token.GetHashCode() : 0);
                return hashCode;
            }
        }
        */
        public void Add(object value)
        {
            if (Type != LispType.List)
            {
                throw CreateInvalidCastException("list");
            }
            var list = (List<object>)Value;
            list.Add(value);
        }

        public static LispVariant operator +(LispVariant l, LispVariant r)
        {
            if (l.IsString || r.IsString)
            {
                return new LispVariant(l.StringValue + r.StringValue);
            }
            if (l.IsDouble || r.IsDouble)
            {
                return new LispVariant(l.ToDouble() + r.ToDouble());
            }
            if (l.IsInt || r.IsInt)
            {
                return new LispVariant(l.ToInt() + r.ToInt());
            }
            if (l.IsList && r.IsList)
            {
                var newList = new List<object>();
                newList.AddRange(l.ListValue);
                newList.AddRange(r.ListValue);
                return new LispVariant(newList);
            }
            throw CreateInvalidOperationException("+", l, r);
        }

        public static LispVariant operator -(LispVariant l, LispVariant r)
        {
            if (l.IsDouble || r.IsDouble)
            {
                return new LispVariant(l.ToDouble() - r.ToDouble());
            }
            if (l.IsInt || r.IsInt)
            {
                return new LispVariant(l.ToInt() - r.ToInt());
            }
            throw CreateInvalidOperationException("-", l, r);
        }

        public static LispVariant operator *(LispVariant l, LispVariant r)
        {
            if (l.IsDouble || r.IsDouble)
            {
                return new LispVariant(l.ToDouble() * r.ToDouble());
            }
            if (l.IsInt || r.IsInt)
            {
                return new LispVariant(l.ToInt() * r.ToInt());
            }
            throw CreateInvalidOperationException("*", l, r);
        }

        public static LispVariant operator /(LispVariant l, LispVariant r)
        {
            if (l.IsDouble || r.IsDouble)
            {
                return new LispVariant(l.ToDouble() / r.ToDouble());
            }
            if (l.IsInt || r.IsInt)
            {
                return new LispVariant(l.ToInt() / r.ToInt());
            }
            throw CreateInvalidOperationException("/", l, r);
        }

        public static LispVariant operator <(LispVariant l, LispVariant r)
        {
            if (l.IsDouble || r.IsDouble)
            {
                return new LispVariant(l.ToDouble() < r.ToDouble());
            }
            if (l.IsInt || r.IsInt)
            {
                return new LispVariant(l.ToInt() < r.ToInt());
            }
            throw CreateInvalidOperationException("< or >", l, r);
        }

        public static LispVariant operator >(LispVariant l, LispVariant r)
        {
            return r < l;
        }

        public static LispVariant operator <=(LispVariant l, LispVariant r)
        {
            if (l.IsDouble || r.IsDouble)
            {
                return new LispVariant(l.ToDouble() <= r.ToDouble());
            }
            if (l.IsInt || r.IsInt)
            {
                return new LispVariant(l.ToInt() <= r.ToInt());
            }
            throw CreateInvalidOperationException("<= or >=", l, r);
        }

        public static LispVariant operator >=(LispVariant l, LispVariant r)
        {
            return r <= l;
        }

        public static bool EqualOp(LispVariant l, LispVariant r)
        {
            if (l.IsDouble || r.IsDouble)
            {
                return Math.Abs(l.ToDouble() - r.ToDouble()) < Tolerance;
            }
            if (l.IsInt || r.IsInt)
            {
                return l.ToInt() == r.ToInt();
            }
            if (l.IsBool && r.IsBool)
            {
                return l.BoolValue == r.BoolValue;
            }
            if (l.IsString || r.IsString)
            {
                return l.ToString() == r.ToString();
            }
            throw CreateInvalidOperationException("==", l, r);
        }

        public static explicit operator Func<object[], LispScope, LispVariant>(LispVariant variant)
        {
            return variant.FunctionValue.Function;
        }

        public static explicit operator Func<LispVariant, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, scope) => variant.FunctionValue.Function(new object[] { arg1 }, scope);
        }

        public static explicit operator Func<LispVariant, LispVariant, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, arg2, scope) => variant.FunctionValue.Function(new object[] { arg1, arg2 }, scope);
        }

        public static explicit operator Func<LispVariant, LispVariant, LispVariant, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, arg2, arg3, scope) => variant.FunctionValue.Function(new object[] { arg1, arg2, arg3 }, scope);
        }

        public static explicit operator Func<LispVariant, LispVariant, LispVariant, LispVariant, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, arg2, arg3, arg4, scope) => variant.FunctionValue.Function(new object[] { arg1, arg2, arg3, arg4 }, scope);
        }

        public static explicit operator Func<LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, arg2, arg3, arg4, arg5, scope) => variant.FunctionValue.Function(new object[] { arg1, arg2, arg3, arg4, arg5 }, scope);
        }

        public static explicit operator Func<LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispScope, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, arg2, arg3, arg4, arg5, arg6, scope) => variant.FunctionValue.Function(new object[] { arg1, arg2, arg3, arg4, arg5, arg6 }, scope);
        }

        public static explicit operator Func<LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispScope, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, arg2, arg3, arg4, arg5, arg6, arg7, scope) => variant.FunctionValue.Function(new object[] { arg1, arg2, arg3, arg4, arg5, arg6, arg7 }, scope);
        }

        public static explicit operator Func<LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispScope, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, scope) => variant.FunctionValue.Function(new object[] { arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 }, scope);
        }

        public static explicit operator Func<LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispVariant, LispScope, LispScope, LispVariant>(LispVariant variant)
        {
            return (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, scope) => variant.FunctionValue.Function(new object[] { arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 }, scope);
        }

        #endregion

        #region private methods

        private static LispType TypeOf(object obj)
        {
            if (obj is int)
            {
                return LispType.Int;
            }
            if (obj is double)
            {
                return LispType.Double;
            }
            if (obj is bool)
            {
                return LispType.Bool;
            }
            if (obj is string)
            {
                return LispType.String;
            }
            if (obj is IEnumerable<object>)
            {
                return LispType.List;
            }
            if (obj is LispFunctionWrapper)
            {
                return LispType.Function;
            }
            if (obj is LispVariant)
            {
                return ((LispVariant)obj).Type;
            }
            if (obj is LispToken)
            {
                return LispType.Symbol;
            }
            return LispType.Undefined;
        }

        private Exception CreateInvalidCastException(string name, string msg = "no")
        {
            var exception = new InvalidCastException(string.Format("{0} {1}", msg, name));
            exception.Data[LispUtils.LineNo] = Token != null ? Token.LineNo : -1;
            return exception;
        }

        static private Exception CreateInvalidOperationException(string operation, LispVariant l, LispVariant r)
        {
            var exception = new InvalidCastException(String.Format(NoOperatorForTypes, operation, l.Type, r.Type));
            exception.Data[LispUtils.LineNo] = (l.Token != null ? l.Token.LineNo : (int?)null);
            return exception;
        }

        #endregion
    }
}
