// StrongLLK.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LLkParser.h"
#include <iostream>
using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
    LLkParser parser("Intrare.txt");
	if(parser.IsValid())
	{
		while(true)
		{
			std::string toParse;
			cout<<"Parser: ";
			cin >> toParse;
			std::vector<std::pair<char, u32>> road;
			bool result = parser.Parse(toParse, road);
			if(result)
			{
				cout<<"True\n";
				std::string start = "S$";
				cout << start << "\n";
				for(auto& pair : road)
				{
					parser.Rewrite(start, pair.first, pair.second);
					cout << start << "\n";
				}
			}
			else
			{
				cout<<"False\n";
			}
		}
	}
	else
	{
		cout << parser.GetError() << "\n";
	}
	return 0;
}

