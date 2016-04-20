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

	for(auto& state : m_States)
	{
		std::string tmp;
		tmp = state.first;
		First(tmp);
	}

	Follow('S');
	for(auto& state : m_States)
	{
		Follow(state.first);
	}
	int i = 0;
	i++;

	i--;
}

std::vector<std::string> LLkParser::First(const std::string& val)
{
	std::vector<std::string> result;
	
	if(val.size() == 1 && IsNonTerminal(val[0]))
	{
		auto& it = m_FirstCache.find(val[0]);
		if(it != m_FirstCache.end())
		{
			return it->second;
		}

		for(auto& state : m_States[val[0]])
		{
			auto& stateFirsts = First(state);
			for(auto& stateFirst : stateFirsts)
			{
				if(std::find(result.begin(), result.end(), stateFirst) == result.end())
				{
					result.push_back(stateFirst);
				}
			}
		}
		m_FirstCache[val[0]] = result;
	}
	else
	{
		if(val[0] == '~')
		{
			assert(val.size() == 1);
			return result;
		}

		std::vector<std::string> intermediateResult;
		intermediateResult.push_back("");
		for(char c : val)
		{
			if(IsNonTerminal(c))
			{
				std::string temp;
				temp = c;
				AppendToAll(intermediateResult, First(temp));
			}
			else
			{
				std::string temp;
				temp = c;
				AppendToAll(intermediateResult, temp);
			}
			MoveIntermediateResults(intermediateResult, result);
			if(intermediateResult.size() == 0)
				break;
		}
		result.insert(result.end(), intermediateResult.begin(), intermediateResult.end());
	}
	return result;
}

std::vector<std::string> LLkParser::Follow(char val)
{
	std::vector<std::string> result;
	
	auto& found = m_FollowCache.find(val);
	if(found != m_FollowCache.end())
	{
		return found->second;
	}

	for(auto& it : m_States)
	{
		for(auto& state : it.second)
		{
			std::vector<std::string> intermediateResult;
			intermediateResult.push_back("");
			u32 pos = state.find(val);
			if(pos == std::string::npos)
			{
				continue;
			}

			for(int i = pos + 1; i < state.size(); i++)
			{
				if(IsNonTerminal(state[i]))
				{
					AppendToAll(intermediateResult, m_FirstCache[state[i]]);
				}
				else
				{
					AppendToAll(intermediateResult, state[i]);
				}
				MoveIntermediateResults(intermediateResult, result);
				if(intermediateResult.size() == 0)
					break;
			}
			
			result.insert(result.end(), intermediateResult.begin(), intermediateResult.end());
		}
	}
	if(val == 'S')
	{
		result.push_back("$");
	}
	m_FollowCache[val] = result;
	return result;
}

bool LLkParser::IsNonTerminal(char c)
{
	return c >= 'A' && c <= 'Z';
}

void LLkParser::MoveIntermediateResults(std::vector<std::string>& intermediateResult, std::vector<std::string>& result)
{
	int size = intermediateResult.size();
	for(int i = 0; i < size; i++)
	{
		if(intermediateResult[i].size() >= m_K)
		{
			intermediateResult[i].resize(m_K);
			if(std::find(result.begin(), result.end(), intermediateResult[i]) == result.end())
			{
				result.push_back(intermediateResult[i]);
			}
			if(size != i + 1)
			{
				std::move(intermediateResult.begin() + i + 1, intermediateResult.end(), intermediateResult.begin() + i);
			}
			size--;
		}
	}
	intermediateResult.resize(size);
}

void LLkParser::AppendToAll(std::vector<std::string>& appendTo, char appended)
{
	for(auto& str : appendTo)
	{
		str += appended;
	}
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
	std::vector<std::string> old;
	old.insert(old.end(), appendTo.begin(), appendTo.end());
	appendTo.clear();
	for(auto& str1 : old)
	{
		for(auto& str2 : appended)
		{
			std::string toAdd = str1 + str2;
			if(std::find(appendTo.begin(), appendTo.end(), toAdd) == appendTo.end())
			{
				appendTo.push_back(str1 + str2);
			}
		}
	}
}