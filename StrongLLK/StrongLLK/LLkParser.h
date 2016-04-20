#include <string>
#include <vector>
#include <unordered_map>

#include <fstream>

typedef unsigned int u32;
class LLkParser
{
public:
    LLkParser(std::string filename);
	std::vector<std::string> First(const std::string& val);
	std::vector<std::string> Follow(char val);
private:
	bool IsNonTerminal(char c);
	void MoveIntermediateResults(std::vector<std::string>& intermediateResult, std::vector<std::string>& result);
	void AppendToAll(std::vector<std::string>& appendTo, char appended);
	void AppendToAll(std::vector<std::string>& appendTo, std::string appended);
	void AppendToAll(std::vector<std::string>& appendTo, const std::vector<std::string>& appended);
	bool CheckLengthRequirement(std::vector<std::string> toCheck);

	u32 m_K;
	std::string m_Error;
	std::unordered_map<char, std::vector<std::string>> m_States;
	std::unordered_map<char, std::vector<std::string>> m_FirstCache;
	std::unordered_map<char, std::vector<std::string>> m_FollowCache;
};