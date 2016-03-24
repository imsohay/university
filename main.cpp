#include <iostream>
#include <ctime>
#include <fstream>
#include "calcformula.h"
#include <bitset>
#include <string.h>
#include <sstream>
#include <chrono>


using namespace std;

#define DEFAULT_FILENAME "data.txt"


vcl generate_literals_vector(int AMOUNT_VARS, int AMOUNT_PREDS, int DIMENSION_PREDS)
{
	Literal* l = NULL;
    vcl v(AMOUNT_PREDS);
	set<ui> s;
	forn(i, AMOUNT_PREDS)
	{
		l = new Literal(false, 1, DIMENSION_PREDS, new ui[DIMENSION_PREDS]);
		s.clear();
		while (s.size() < DIMENSION_PREDS) s.insert(rand() % AMOUNT_VARS);
		int k;
		vui v1(s.begin(), s.end());

		forn(j, DIMENSION_PREDS)
		{
			k = rand() % v1.size();
			l->vars[j] = v1[k];
			v1.erase(v1.begin() + k);
		}

		v[i] = l;
	}

	return v;
}

void measure_perfomance()
{
	srand(time(NULL));
	double sum = 0.0, min = 1e6, max = -1.0, time_after = 0.0, time = 0.0;

	vi max_subst;
	const int TIMES = 25;
	for (int i = 0; i < TIMES; i++)
	{
        vcl vl1 = generate_literals_vector(8, 22, 3);
        vcl vl2 = generate_literals_vector(8, 22, 3);
		//        for(auto x : vl1) x->show();
		//        for(auto x : vl2) x->show();

		ALGORITHM_VERS vers = SECOND;
		Formula_wrapper wrap_1(vl1);
		Formula_wrapper wrap_2(vl2);
		//Formula* f2 = CalculatorFormula::build_formula(vl2);


		time = clock(), time_after = 0.0;

		Formula_wrapper* res = CalculatorFormula::max_common_subformula(wrap_1, wrap_2, vers, max_subst);

		time_after = (clock() - time) / CLOCKS_PER_SEC;
		if (time_after > max) max = time_after;
		if (time_after < min) min = time_after;
		sum += time_after;
	}

	cout << "avrg: " << sum / TIMES << endl
		<< "min: " << min << endl
		<< "max: " << max << endl;
	cout << "Time: " << time_after << endl;
}


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


void output_debug(Formula_wrapper* res)
{

}


int main()
{

    srand(time(NULL) + clock());
//    system("pwd");
//    vector<vcl *>* data = read_from_file();

//	Formula_wrapper wrap_1(*(data->at(0)));
//	Formula_wrapper wrap_2(*(data->at(1)));
    int res = 0;
    const int amount_preds = 20;
    const int amount_vars = 7;
    const int size_of_result = 10;
    const int predicat_dimension = 3;
    int results[size_of_result] = {0};

    int amount_try = 1000;
    int sum = 0;
    int index = 0;
    for(int i = 0; i < amount_try; i++)
    {
//        system("clear");
//        cout << i << endl;

        vcl vl1 = generate_literals_vector(amount_vars, amount_preds, predicat_dimension);
        vcl vl2 = generate_literals_vector(amount_vars, amount_preds, predicat_dimension);
        Formula_wrapper wrap_1(vl1);
        Formula_wrapper wrap_2(vl2);

//        ALGORITHM_VERS version = SECOND;
//        vi max_subst;

//        auto start = chrono::system_clock::now();
//        Formula_wrapper* common = CalculatorFormula::max_common_subformula(wrap_1, wrap_2, version, max_subst);

//        auto end= chrono::system_clock::now();

//        cout << *common << endl;

//        cout << "\n\n";
        vector<vector<int>>* t1 = CalculatorFormula::analyse_variable_characteristic(wrap_1);
//        cout << "\n\n";
        vector<vector<int>>* t2 = CalculatorFormula::analyse_variable_characteristic(wrap_2);
//        cout << "\n\n";
        res = CalculatorFormula::amount_mathing(t1,t2);

        sum += res;
        if (res)
        {
            index = 10.0 * res / amount_vars;
            if (index >= amount_vars)
                index = amount_vars - 1;
            results[index]++;
        }

        delete t1;
        delete t2;
//        delete common;
        clear_vcl(vl1);
        clear_vcl(vl2);

//        auto time = chrono::duration_cast<chrono::milliseconds>(end-start);
//        cout << "\n\nTime: " << time.count() << endl;
    }


    cout << "total try: " << amount_try << endl;
    cout << "average: " << (double)sum / amount_try << endl;
    forn(i, size_of_result) cout << results[i] << " ";
    cout << endl;
    return 0;
}

























