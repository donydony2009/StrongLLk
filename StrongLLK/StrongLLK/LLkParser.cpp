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
        f>>state;
		getline(f, value);
        m_States[state].push_back(value.substr(1));
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
	BuildPreditctTable();
}

bool LLkParser::Parse(std::string toParse, std::vector<std::pair<char, u32>>& road)
{
	std::string state = "S$";
	for(int i = 0; i < toParse.size(); i++)
	{
		while(IsNonTerminal(state[0]))
		{
			std::string lookahead;
			if(i + m_K > toParse.size())
			{
				lookahead = toParse.substr(i);
				if(lookahead.size() != m_K)
				{
					lookahead += '$';
				}
			}
			else
			{
				lookahead = toParse.substr(i, m_K);
			}

			auto& it = m_PredictTable.find(state[0] + lookahead);
			if(it != m_PredictTable.end())
			{
				road.push_back(std::make_pair(state[0], it->second));
				Rewrite(state, it->second);
			}
			else
			{
				return false;
			}
		}

		if(toParse[i] != state[0])
		{
			return false;
		}
		state = state.substr(1);
	}
	return true;
}

void LLkParser::Rewrite(std::string& toRewrite, u32 idx)
{
	char nonTerminal = toRewrite[0];
	std::string state = m_States[nonTerminal][idx];
	if(state == "~")
	{
		state = "";
	}
	toRewrite = state + toRewrite.substr(1);
}

void LLkParser::Rewrite(std::string& toRewrite, char nonTerminal, u32 idx)
{
	u32 pos = toRewrite.find(nonTerminal);
	if(pos == std::string::npos)
	{
		return;
	}
	std::string state = m_States[nonTerminal][idx];
	if(state == "~")
	{
		state = "";
	}
	toRewrite = toRewrite.substr(0, pos) + state + toRewrite.substr(pos + 1);
}

std::vector<std::string> LLkParser::First(const std::string& val)
{
	if(!m_Error.empty())
		return std::vector<std::string>();
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
		if(val.size() >= 1 && val[0] == '~')
		{
			assert(val.size() == 1);
			result.push_back("");
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
	if(!m_Error.empty())
		return std::vector<std::string>();
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
			for(u32 pos = state.find(val); pos != std::string::npos; pos = state.find(val, pos + 1))
			{
				std::vector<std::string> intermediateResult;
				intermediateResult.push_back("");

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

				if(intermediateResult.size() != 0)
				{
					auto& follow = Follow(it.first);
					AppendToAll(intermediateResult, follow);
					MoveIntermediateResults(intermediateResult, result);
				}

			
				result.insert(result.end(), intermediateResult.begin(), intermediateResult.end());
			}
		}
	}
	if(val == 'S')
	{
		result.push_back("$");
	}
	m_FollowCache[val] = result;
	return result;
}

void LLkParser::BuildPreditctTable()
{
	for(auto& stateVector : m_States)
	{
		for(int i = 0; i < stateVector.second.size(); i++)
		{
			auto& state = stateVector.second[i];
			auto& keys = First(state);
			AppendToAll(keys, Follow(stateVector.first));
			for(auto& key : keys)
			{
				if(m_PredictTable.find(stateVector.first + key) == m_PredictTable.end())
				{
					m_PredictTable[stateVector.first + key] = i;
				}
				else
				{
					m_Error = std::string("There is a conflict between 2 transitions at the non terminal ") + stateVector.first;
					return;
				}
			}
		}
	}
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
		size_t strSize = intermediateResult[i].size();
		if(strSize >= m_K || (strSize && intermediateResult[i][strSize - 1] == '$'))
		{
			if(strSize > m_K)
			{
				intermediateResult[i].resize(m_K);
			}

			if(std::find(result.begin(), result.end(), intermediateResult[i]) == result.end())
			{
				result.push_back(intermediateResult[i]);
			}
			if(size != i + 1)
			{
				std::move(intermediateResult.begin() + i + 1, intermediateResult.end(), intermediateResult.begin() + i);
			}
			i--;
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
		if(str.size() > m_K)
		{
			str.resize(m_K);
		}
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
			if(toAdd.size() > m_K)
			{
				toAdd.resize(m_K);
			}

			if(std::find(appendTo.begin(), appendTo.end(), toAdd) == appendTo.end())
			{
				appendTo.push_back(toAdd);
			}
		}
	}
}