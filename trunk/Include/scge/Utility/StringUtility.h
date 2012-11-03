#ifndef __StringUtility_h__
#define __StringUtility_h__

#include <boost/lexical_cast.hpp>
#include <vector>

namespace StringUtility
{
	template <typename VariableType>
	inline VariableType toVariable(const std::string &text)
	{
		try
		{
			return boost::lexical_cast<VariableType>(text);
		}
		catch(boost::bad_lexical_cast &)
		{
			return VariableType();
		}
	}

	template <>
	inline bool toVariable(const std::string &text)
	{
		if(text == "true")
			return true;
		else if(text == "false")
			return false;

		try
		{
			return boost::lexical_cast<bool>(text);
		}
		catch(boost::bad_lexical_cast &)
		{
			return false;
		}
	}

	template <>
	inline std::string toVariable(const std::string &text)
	{
		return text;
	}

	template <typename VariableType>
	inline std::string toString(VariableType variable)
	{
		try
		{
			return boost::lexical_cast<std::string>(variable);
		}
		catch(boost::bad_lexical_cast &)
		{
			return std::string();
		}
	}

	template <>
	inline std::string toString(bool variable)
	{
		return variable ? "true" : "false";
	}

	template <>
	inline std::string toString(std::string variable)
	{
		return variable;
	}

#if defined(_MSC_VER)
static_assert(_MSC_VER == 1700, "Enable this");
#endif
#if defined(SCGE_CPP11)
	inline void formatRecurse(const char *s, std::ostream &ss)
	{
		while(*s)
			ss.put(*s++);
	}

	template<typename T, typename... Args>
	inline void formatRecurse(const char *s, std::ostream &ss, T value, Args... args)
	{
		while(*s)
		{
			if(*s == '%')
			{
				ss << toString(value1);
				formatRecurse(++s, ss, args...);
				return;
			}

			ss.put(*s++);
		}
	}

	template<typename... Args>
	std::string format(const char *s, Args... args)
	{
		std::stringstream ss;
		formatRecurse(s, ss, args...);
		return ss.str();
	}

	template<typename... Args>
	std::string format(const std::string &str, Args... args)
	{
		return format(str.c_str(), args...);
	}
#else
	inline void formatRecurse(const char *s, std::ostream &ss)
	{
		while(*s) ss.put(*s++);
	}

	template<typename T1> inline void formatRecurse(const char *s, std::ostream &ss, T1 value1)
	{
		while(*s)
		{
			if(*s == '%') { ss << toString(value1); formatRecurse(++s, ss); return; }
			ss.put(*s++);
		}
	}
	template<typename T1, typename T2> inline void formatRecurse(const char *s, std::ostream &ss, T1 value1, T2 value2)
	{
		while(*s)
		{
			if(*s == '%') { ss << toString(value1); formatRecurse(++s, ss, value2); return; }
			ss.put(*s++);
		}
	}
	template<typename T1, typename T2, typename T3> inline void formatRecurse(const char *s, std::ostream &ss, T1 value1, T2 value2, T3 value3)
	{
		while(*s)
		{
			if(*s == '%') { ss << toString(value1); formatRecurse(++s, ss, value2, value3); return; }
			ss.put(*s++);
		}
	}
	template<typename T1, typename T2, typename T3, typename T4> inline void formatRecurse(const char *s, std::ostream &ss, T1 value1, T2 value2, T3 value3, T4 value4)
	{
		while(*s)
		{
			if(*s == '%') { ss << toString(value1); formatRecurse(++s, ss, value2, value3, value4); return; }
			ss.put(*s++);
		}
	}
	template<typename T1, typename T2, typename T3, typename T4, typename T5> inline void formatRecurse(const char *s, std::ostream &ss, T1 value1, T2 value2, T3 value3, T4 value4, T5 value5)
	{
		while(*s)
		{
			if(*s == '%') { ss << toString(value1); formatRecurse(++s, ss, value2, value3, value4, value5); return; }
			ss.put(*s++);
		}
	}

	template<typename T1> std::string format(const char *s, T1 value1)
	{
		std::stringstream ss; formatRecurse(s, ss, value1); return ss.str();
	}
	template<typename T1, typename T2> std::string format(const char *s, T1 value1, T2 value2)
	{
		std::stringstream ss; formatRecurse(s, ss, value1, value2); return ss.str();
	}
	template<typename T1, typename T2, typename T3> std::string format(const char *s, T1 value1, T2 value2, T3 value3)
	{
		std::stringstream ss; formatRecurse(s, ss, value1, value2, value3); return ss.str();
	}
	template<typename T1, typename T2, typename T3, typename T4> std::string format(const char *s, T1 value1, T2 value2, T3 value3, T4 value4)
	{
		std::stringstream ss; formatRecurse(s, ss, value1, value2, value3, value4); return ss.str();
	}
	template<typename T1, typename T2, typename T3, typename T4, typename T5> std::string format(const char *s, T1 value1, T2 value2, T3 value3, T4 value4, T5 value5)
	{
		std::stringstream ss; formatRecurse(s, ss, value1, value2, value3, value4, value5); return ss.str();
	}

	template<typename T1> std::string format(const std::string &str, T1 value1)
	{
		return format(str.c_str(), value1);
	}
	template<typename T1, typename T2> std::string format(const std::string &str, T1 value1, T2 value2)
	{
		return format(str.c_str(), value1, value2);
	}
	template<typename T1, typename T2, typename T3> std::string format(const std::string &str, T1 value1, T2 value2, T3 value3)
	{
		return format(str.c_str(), value1, value2, value3);
	}
	template<typename T1, typename T2, typename T3, typename T4> std::string format(const std::string &str, T1 value1, T2 value2, T3 value3, T4 value4)
	{
		return format(str.c_str(), value1, value2, value3, value4);
	}
	template<typename T1, typename T2, typename T3, typename T4, typename T5> std::string format(const std::string &str, T1 value1, T2 value2, T3 value3, T4 value4, T5 value5)
	{
		return format(str.c_str(), value1, value2, value3, value4, value5);
	}
#endif

	std::vector<std::string> getTokens(const std::string &strText, const std::string &delimiters = " ");

	std::pair<std::string, std::string> splitSection(const std::string &str, std::string::value_type startCharacter, std::string::value_type endCharacter);

	template<typename T>
	void foreachTokenSection(const std::string &text, T function)
	{
		std::stringstream currentSection;
		std::stringstream currentToken;
		std::vector<std::string> currentTokens;
		bool foundComment = false;
		char groupChar = ' ';
		for(char character : text)
		{
			if(currentToken.str().empty())
			{
				if(character == '#')
					break;

				if(foundComment)
				{
					if(character == '\\')
						break;
					else
					{
						foundComment = false;
						currentToken << '\\';
					}
				}
				else if(character == '\\')
				{
					foundComment = true;
					continue;
				}
			}

			if(character != ';' || groupChar != ' ')
				currentSection << character;

			if((groupChar != ' ' && character != groupChar) || (character != '\'' && character != '"' && character != ' ' && character != '=' && character != ';'))
				currentToken << character;

			if(groupChar != ' ')
			{
				if(character != groupChar)
					continue;
				else
					groupChar = ' ';
			}
			else if(character == '"' || character == '\'')
			{
				groupChar = character;
				continue;
			}

			if(character == ' ' || character == '=')
			{
				if(!currentToken.str().empty())
				{
					currentTokens.push_back(currentToken.str());
					currentToken.str("");
				}
			}
			else if(character == ';')
			{
				if(!currentToken.str().empty())
				{
					currentTokens.push_back(currentToken.str());
					currentToken.str("");
				}

				if(!currentTokens.empty())
					function(currentTokens, currentSection.str());
				currentTokens.clear();
				currentSection.str("");
			}
		}

		if(!currentToken.str().empty())
		{
			currentTokens.push_back(currentToken.str());
			currentToken.str("");
		}

		if(!currentTokens.empty())
			function(currentTokens, currentSection.str());
	}
}

#endif // __StringUtility_h__
