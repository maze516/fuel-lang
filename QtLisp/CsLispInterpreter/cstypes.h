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

#ifndef _CSTYPES_H
#define _CSTYPES_H

#include <list>
#include <vector>
#include <map>

#include "csstring.h"
#include "Exception.h"

#define var auto

#define null 0

namespace CsLisp
{
	class LispToken;
	class LispScope;
	class LispVariant;
	class object;

	typedef std::function<void()> Action;
	typedef std::function<std::shared_ptr<LispVariant>(std::vector<std::shared_ptr<object>>, std::shared_ptr<LispScope>)> FuncX;

	template <class T>
	class IEnumerable : public std::list<T>
	{
	public:
		size_t Count() const
		{
			return std::list<T>::size();
		}

		std::vector<T> ToArray() const
		{
			return std::vector<T>(std::list<T>::begin(), std::list<T>::end());
		}

		std::list<T> ToList() const
		{
			return std::list<T>(std::list<T>::begin(), std::list<T>::end());
		}

		const T & First() const
		{
			return std::list<T>::front();
		}

		const T & Last() const
		{
			return std::list<T>::back();
		}

		const T & ElementAt(int index) const
		{
			std::vector<T> aCopy(begin(), end());
			return aCopy[index];
		}

		IEnumerable<T> Skip(int skipNoOfElements)
		{
			IEnumerable<T> temp(*this);
			for (int i = 0; i < skipNoOfElements; i++)
			{
				temp.pop_front();
			}
			return temp;
		}

		bool SequenceEqual(const IEnumerable & other) const
		{
            if (this->size() == other.size())
			{
				var iter2 = other.begin();
                for (var iter = this->begin(); iter != this->end(); ++iter)
				{
					if (*iter != *iter2)
					{
						return false;
					}
					++iter2;
				}
			}
			else
			{
				return false;
			}
			return true;
		}

		void Add(const T & elem)
		{
			std::list<T>::push_back(elem);
		}

		void AddRange(const IEnumerable<T> & other)
		{
            std::list<T>::insert(this->end(), other.begin(), other.end());
		}
	};

	//template <class T>
	//class IList : public IEnumerable<T>
	//{
	//public:
	//};

	//template <class T>
	//class List : public IEnumerable<T>
	//{
	//public:
	//};

	template <class K, class V>
	class Dictionary : public std::map<K,V>
	{
	public:
		IEnumerable<K> GetKeys() const
		{
			IEnumerable<K> keys;
			for (auto & kvp : *this)
			{
				keys.push_back(kvp.first);
			}
			return keys;
		}

		bool ContainsKey(const K & key) const
		{
            return std::map<K,V>::count(key) == 1;
		}
	};

	template <class T1, class T2>
	class Tuple : std::pair<T1, T2>
	{
	public:
		Tuple(const T1 & val1, const T2 & val2)
			: std::pair<T1, T2>(val1, val2)
		{
		}
		T1 Item1() const
		{
			return std::pair<T1, T2>::first;
		}
		T2 Item2() const
		{
			return std::pair<T1, T2>::second;
		}
	};

	class TextWriter
	{
	public:
		void Write(const string & txt);
		void WriteLine(const string & txt);
		void WriteLine(const string & txt, const string & txt1);
		void WriteLine(const string & txt, const string & txt1, const string & txt2);
		void WriteLine(const string & txt, const string & txt1, const string & txt2, const string & txt3);
		void WriteLine(const string & txt, const string & txt1, const string & txt2, const string & txt3, const string & txt4);
	};

	class TextReader
	{
	};

	template <class K, class V>
	class KeyValuePair
	{
	public:
		KeyValuePair(K key, V value)
			: Key(key), Value(value)
		{
		}

		K Key;
		V Value;
	};

	struct LispFunctionWrapper
	{
	private:
		bool m_bIsSpecialForm;
		bool m_bIsBuiltin;

	public:
		LispFunctionWrapper()
			: Signature(""), 
			  m_bIsSpecialForm(false), 
			  m_bIsBuiltin(false)
		{
		}

		/*public*/ /*Func<object[], LispScope, LispVariant>*/FuncX Function; // { get; private set; }

		/*public*/ string Signature; // { get; private set; }

		/*public*/ string Documentation; // { get; private set; }

		bool IsBuiltin() const
		{
			return m_bIsBuiltin;
		}

		string ModuleName;

		/*public*/ string GetFormatedDoc() const
		{
			return "<TODO>";
		}

		/*public*/ string GetHtmlFormatedDoc() const
		{
			return "<TODO>";
		}

		bool IsSpecialForm() const
		{
			return m_bIsSpecialForm;
		}
		void SetSpecialForm(bool value)
		{
			m_bIsSpecialForm = value;
		}
	};

	//class Exception
	//{
	//};

	template <class T>
	int CompareToType(T d1, T d2)
	{
		if (d1 < d2)
		{
			return -1;
		}
		else if (d1 > d2)
		{
			return 1;
		}
		return 0;
	}
}

#endif
