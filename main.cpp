#include <iostream>
#include <ctime>
#include <fstream>
#include "calcformula.h"
#include "mlcd.h"
#include <bitset>
#include <string.h>
#include <sstream>
#include <chrono>
#include <random>
#include <cmath>
#include <iomanip>


using namespace std;

#define DEFAULT_FILENAME "data.txt"


Literal* read_literal(string s)
{
	bool sign;
	ui id;
	ui arity;
	ui* array_arguments;
	string prefix = "literal ";

    size_t i = s.find(prefix);
	s = s.substr(i + prefix.size());
	stringstream ss;
	ss << s;

	ss >> sign;
	ss >> id;
	ss >> arity;
	array_arguments = new ui[arity];
	forn(j, arity)
	{
		ss >> array_arguments[j];
	}

	return new Literal(sign, id, arity, array_arguments);
}


vector<vcl *>* read_from_file()
{
	ifstream is;
	string line;
	vector<vcl *>* res = new vector<vcl *>();
	vcl *temp = NULL;

	is.open(DEFAULT_FILENAME);
	if (!is.is_open())
	{
        cout << "cannot open file!\n";
        system("pwd");
        exit(EXIT_FAILURE);
	}

	while (true)
	{
		getline(is, line, '\n');
		if (line.find("end data") != string::npos)
		{
			break;
		}
		else if (line.find("new formula") != string::npos)
		{
			temp = new vcl();
		}
		else if (line.find("end") != string::npos)
		{
			if (temp)
			{
				res->push_back(temp);
				temp = NULL;
			}
		}
		else if (line.find("literal") != string::npos)
		{
			temp->push_back(read_literal(line));
		}
		else if (!line.empty())
		{
            cout << "not correct input file!\n";
            exit(EXIT_FAILURE);
		}
	}

	is.close();

	return res;
}



int main()
{
    vector<vcl*> * data = read_from_file();

    forward_list<Formula_wrapper> list;
    for(vcl * ptr : *data)
    {
        list.push_front(*ptr);
    }

    MLCD description(list);

    ofstream out("result.txt");
    out << description << endl;

    for(vcl * ptr : *data)
    {
        clear_vcl(*ptr);
        check_and_clear(ptr);
    }
    check_and_clear(data);
    return 0;
}


























