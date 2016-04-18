#include "stdafx.h"
#include "LLkParser.h"
#include <assert.h> 

LLkParser::LLkParser(std::string filename)
{
    std::string value;
    std::ifstream f(filename);
    f>>m_K;
    while(!f.eof())
    {
        char state;
        f>>state>>value;
        m_States[state].push_back(value);
    }
}

std::vector<std::string> LLkParser::First(const std::string& val)
{
	std::vector<std::string> result;
	if(val[0] == '~')
	{
		assert(val.size() == 1);
		result.push_back(val);
		return result;
	}
	for(char c : val)
	{
		if(IsNonTerminal(c))
		{
			std::string temp;
			temp = c;
			AppendToAll(result, First(temp));
		}
		else
		{
			std::string temp;
			temp = c;
			AppendToAll(result, temp);
		}
	
	}
	if(val.size() == 1 && IsNonTerminal(val[0]))
	{
		m_FirstCache[val[0]] = result;
	}
	return result;
}

bool LLkParser::IsNonTerminal(char c)
{
	return c >= 'A' && c <= 'Z';
}

void LLkParser::AppendToAll(std::vector<std::string>& appendTo, std::string appended)
{
	for(auto& str : appendTo)
	{
		str += appended;
	}
}

void LLkParser::AppendToAll(std::vector<std::string>& appendTo, const std::vector<std::string>& appended)
{
	std::vector<std::string>& old(appendTo);
	appendTo.clear();
	for(auto& str1 : old)
	{
		for(auto& str2 : appended)
		{
			appendTo.push_back(str1 + str2);
		}
	}
}