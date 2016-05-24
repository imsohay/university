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


vcl generate_literals_vector(const int AMOUNT_VARS,
                             const int AMOUNT_PREDS,
                             const int DIMENSION_PREDS,
                             const int AMOUNT_PREDICAT_TYPES = 1)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(AMOUNT_VARS / 2, AMOUNT_VARS / 6);

    int tries_count = 0;
    bool bad_literal = false;
	Literal* l = NULL;
    vcl v(AMOUNT_PREDS);
	set<ui> s;
	forn(i, AMOUNT_PREDS)
    {
        bad_literal = false;
        l = new Literal(false, (1 + rand() % AMOUNT_PREDICAT_TYPES), DIMENSION_PREDS, new ui[DIMENSION_PREDS]);
		s.clear();
        while (s.size() < DIMENSION_PREDS)
        {
            s.insert(rand() % AMOUNT_VARS);
//            s.insert((int)std::round(d(gen)) % AMOUNT_VARS);
        }

		int k;
		vui v1(s.begin(), s.end());

		forn(j, DIMENSION_PREDS)
		{
			k = rand() % v1.size();
			l->vars[j] = v1[k];
			v1.erase(v1.begin() + k);
		}

        for(size_t q = 0; q < i; q++)
        {
            if (v[q] == NULL)
            {
                cout << v[q] << endl;
                cout << q << " " << i << endl;
                assert(NULL);
            }
            if (*(v[q]) == *l)
            {
//                cout << "check\n";
                delete l;
                l = NULL;
                tries_count++;
                i--;
                bad_literal = true;
                break;

                if (tries_count > 100000)
                    assert(NULL);
            }
        }

        if (!bad_literal)
        {
//            cout << i << "  " << *l << endl;
            v[i] = l;
        }
	}

//    cout << "===========================\n";
	return v;
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


void measure_correctness_equality_algo() {
    srand(time(NULL));
    const int amount_vars = 3, amount_preds = 3, predicat_dimension = 3;
    const int AMOUNT_TIMES = 1000000;
    vcl v1,v2;
    int amount_fails = 0, cur_amount_vars = 0, cur_amount_preds = 0, cur_predicat_dimension = 0;

    forn(i, AMOUNT_TIMES) {
        if (i % (AMOUNT_TIMES / 100) == 0)
            cout << i / (AMOUNT_TIMES / 100) << "%\n";

        cur_amount_vars = amount_vars + (rand() % 5);
        cur_amount_preds = amount_preds + (rand() % 8);
        cur_predicat_dimension = predicat_dimension + (rand() % 3);
        if (cur_predicat_dimension >= cur_amount_vars)
            cur_amount_vars = cur_predicat_dimension + 1;

        v1 = generate_literals_vector(cur_amount_vars, cur_amount_preds, cur_predicat_dimension);
        v2 = generate_literals_vector(cur_amount_vars, cur_amount_preds, cur_predicat_dimension);
        Formula_wrapper f1(v1);
        Formula_wrapper f2(v2);

        if (f1.beta_equal(f2) ^ (f1 == f2))
        {
            amount_fails++;
        }


        clear_vcl(v1);
        clear_vcl(v2);
    }

    cout << (double)amount_fails / AMOUNT_TIMES * 100 << "% fails\n";
}


void measure_algo(ALGORITHM_VERS vers)
{
    srand(time(NULL));
    const int amount_vars = 40, amount_preds = 100, predicat_dimension = 5, amount_predicat_types = 6;
    const int AMOUNT_TIMES = 50;
    double sum = 0.0, min = 1e12, max = -1.0, current = 0.0;
    vcl v1,v2;
    forn(i, AMOUNT_TIMES) {
//        if (i % (AMOUNT_TIMES / 100) == 0)
//            cout << i / (AMOUNT_TIMES / 100) << "%\n";
        cout << i << endl;

        v1 = generate_literals_vector(amount_vars, amount_preds, predicat_dimension, amount_predicat_types);
        v2 = generate_literals_vector(amount_vars, amount_preds, predicat_dimension, amount_predicat_types);
        Formula_wrapper f1(v1);
        Formula_wrapper f2(v2);

        auto start = std::chrono::system_clock::now();

        Variable_substitution s(&f1,&f2);
        Formula_wrapper* res = Calculator_formula::max_common_subformula(f1, f2, vers, &s);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        current = (double)elapsed.count() / 1000000.0;
        min = (min < current ? min : current);
        max = (max > current ? max : current);
        sum += current;

        clear_vcl(v1);
        clear_vcl(v2);
        check_and_clear(res);
    }

    cout << "min:  " << min << endl
         << "avrg:  " << sum / AMOUNT_TIMES << endl
         << "max:  " << max << endl;
}


void measure_mlcd()
{
    srand(time(NULL));
    const int amount_vars = 12, amount_preds = 30, predicat_dimension = 4, amount_predicat_types = 3;
    const int AMOUNT_TIMES = 50;
    const int AMOUNT_FORMULAS = 5;
    double sum = 0.0, min = 1e12, max = -1.0, current = 0.0;
    vcl temp;
    forward_list<Formula_wrapper> list;

    forn(i, AMOUNT_TIMES) {
        cout << i << endl;

        forn(j, AMOUNT_FORMULAS)
        {
            temp = generate_literals_vector(amount_vars, amount_preds, predicat_dimension, amount_predicat_types);
            list.push_front(Formula_wrapper(temp));
            clear_vcl(temp);
        }

        auto start = std::chrono::system_clock::now();

        MLCD mlcd(list);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        current = (double)elapsed.count() / 1000000.0;
        min = (min < current ? min : current);
        max = (max > current ? max : current);
        sum += current;

        list.clear();
    }

    cout << "min:  " << min << endl
         << "avrg:  " << sum / AMOUNT_TIMES << endl
         << "max:  " << max << endl;
}


void measure_inference()
{
    srand(time(NULL));
    const int amount_vars = 10, amount_preds = 25, predicat_dimension = 3, amount_predicat_types = 1;
    const int AMOUNT_TIMES = 50;
    const int AMOUNT_FORMULAS = 7;
    double sum = 0.0, min = 1e12, max = -1.0, current = 0.0;
    vcl temp;
    forward_list<Formula_wrapper> list;
    vector<Formula_wrapper> v;


    forn(j, AMOUNT_FORMULAS)
    {
        temp = generate_literals_vector(amount_vars, amount_preds, predicat_dimension, amount_predicat_types);
        list.push_front(Formula_wrapper(temp));
        v.push_back(Formula_wrapper(temp));
        clear_vcl(temp);
    }

//    MLCD mlcd(list);

    forn(i, AMOUNT_TIMES) {
        cout << i << endl;


//        mlcd.test();
        Formula_wrapper fw = v[rand() % v.size()];

        auto start = std::chrono::system_clock::now();

        int k = 0;
        while(fw != v[k])
        {
            assert(k < v.size());
            k++;
        }
//        mlcd.inference(fw, NULL);

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        current = (double)elapsed.count() / 1000000.0;
        min = (min < current ? min : current);
        max = (max > current ? max : current);
        sum += current;

    }

    cout << "min:  " << min << endl
         << "avrg:  " << sum / AMOUNT_TIMES << endl
         << "max:  " << max << endl;
}


void found_all_max_subformulas()
{
    vector<vcl *>* data = read_from_file();
    vector<Formula_wrapper> v;
    ofstream out("result.txt");

    forn(i, data->size())
    {
        v.push_back(Formula_wrapper(*data->at(i)));
    }

    out << "input formulas:\n";
    for(auto el : v)
    {
        out << el << "\n\n\n";
    }
    out.close();

    forn(i, v.size())
    {
        for(int j = i + 1; j < v.size(); j++)
        {
            stringstream ss;
            ss << "result_" << i << "_" << j << ".txt";
            ofstream temp_stream(ss.str());
            Variable_substitution s(&v[i], &v[j]);
            auto start = std::chrono::system_clock::now();
            Formula_wrapper* res = Calculator_formula::max_common_subformula(v[i], v[j], SECOND, &s);
            auto end = std::chrono::system_clock::now();

            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            temp_stream << "time:  " << elapsed.count() / 1000.0 << " s\n\n";

            temp_stream << *res << "\n\n"
                   << s << "\n\n\n";

            temp_stream.close();
            check_and_clear(res);
        }

    }


    for(auto el : *data)
    {
        clear_vcl(*el);
        check_and_clear(el);
    }
    check_and_clear(data);

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
    description.test();
    description.test_inference();

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


























