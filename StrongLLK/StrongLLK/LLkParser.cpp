#include "stdafx.h"
#include "LLkParser.h"

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