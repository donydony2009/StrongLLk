#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

typedef unsigned int u32;
class LLkParser
{
    u32 m_K;
    std::unordered_map<char, std::vector<std::string>> m_States;
public:
    LLkParser(std::string filename);

};