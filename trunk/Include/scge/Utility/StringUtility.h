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

	inline void formatRecurse(const char *s, std::ostream &ss)
	{
		while(*s)
			ss.put(*s++);
	}

	template<typename T, typename... Args>
	inline void formatRecurse(const char *s, std::ostream &ss, const T &arg, const Args&... args)
	{
		while(*s)
		{
			if(*s == '%')
			{
				ss << toString(arg);
				formatRecurse(++s, ss, args...);
				return;
			}

			ss.put(*s++);
		}
	}

	template<typename... Args>
	std::string format(const char *s, const Args&... args)
	{
		std::stringstream ss;
		formatRecurse(s, ss, args...);
		return ss.str();
	}

	template<typename... Args>
	std::string format(const std::string &str, const Args&... args)
	{
		return format(str.c_str(), args...);
	}

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
