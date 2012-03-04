#include "scge\Utility\StringUtility.h"

std::pair<std::string, std::string> StringUtility::splitSection( const std::string &str, std::string::value_type startCharacter, std::string::value_type endCharacter )
{
	int currentSection = 0;
	std::string::size_type firstStartPosition = 0;
	std::string::size_type firstEndPosition = 0;

	std::string::size_type secondStartPosition = 0;
	std::string::size_type secondEndPosition = 0;

	std::string::size_type thirdStartPosition = 0;
	std::string::size_type thirdEndPosition = 0;

	for(std::string::size_type i = 0; i < str.size(); ++i)
	{
		const std::string::value_type currentChar = str[i];

		switch(currentSection)
		{
		case 0:
			if(currentChar == startCharacter)
				currentSection += 2;
			else if(currentChar != L' ')
			{
				firstStartPosition = i;
				firstEndPosition = i;
				++currentSection;
			}
			break;
		case 1:
			if(currentChar == startCharacter)
				++currentSection;
			else if(currentChar != L' ')
				firstEndPosition = i;
			break;
		case 2:
			if(currentChar == endCharacter)
				currentSection += 2;
			else if(currentChar != L' ')
			{
				secondStartPosition = i;
				secondEndPosition = i;
				++currentSection;
			}
			break;
		case 3:
			if(currentChar == endCharacter)
				++currentSection;
			else if(currentChar != L' ')
				secondEndPosition = i;
			break;
		case 4:
			if(currentChar != L' ')
			{
				thirdStartPosition = i;
				thirdEndPosition = i;
				++currentSection;
			}
			break;
		case 5:
			if(currentChar != L' ')
				thirdEndPosition = i;
			break;
		}
	}

	std::string firstSection = (firstStartPosition == 0 && firstEndPosition == 0) ? "" : str.substr(firstStartPosition, firstEndPosition-firstStartPosition+1);
	std::string secondSection = (secondStartPosition == 0 && secondEndPosition == 0) ? "" : str.substr(secondStartPosition, secondEndPosition-secondStartPosition+1);
	std::string thirdSection = (thirdStartPosition == 0 && thirdEndPosition == 0) ? "" : str.substr(thirdStartPosition, thirdEndPosition-thirdStartPosition+1);

	return std::make_pair(secondSection, firstSection+thirdSection);
}

std::vector<std::string> StringUtility::getTokens(const std::string &strText, const std::string &delimiters)
{
	std::string::size_type first = strText.find_first_not_of(delimiters, 0);
	std::string::size_type last = strText.find_first_of(delimiters, first);
	std::vector<std::string> tokens;

	while (std::string::npos != last || std::string::npos != first)
	{
		if(strText.substr(first, 1) == "\"")
		{
			last = strText.find_first_of("\"", first + 1);
			tokens.push_back(strText.substr(first + 1, last - first - 1));

			first = strText.find_first_not_of(delimiters, last + 1);
			last = strText.find_first_of(delimiters, first);
		}
		else
		{
			tokens.push_back(strText.substr(first, last - first));

			first = strText.find_first_not_of(delimiters, last);
			last = strText.find_first_of(delimiters, first);
		}
	}
	return tokens;
}