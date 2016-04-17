#include <iostream>
#include <ctime>
#include <fstream>
#include "calcformula.h"
#include "mlcd.h"
#include <bitset>
#include <string.h>
#include <sstream>
#include <chrono>


using namespace std;

#define DEFAULT_FILENAME "data.txt"


vcl generate_literals_vector(const int AMOUNT_VARS, const int AMOUNT_PREDS, const int DIMENSION_PREDS)
{
    int tries_count = 0;
    bool bad_literal = false;
	Literal* l = NULL;
    vcl v(AMOUNT_PREDS);
	set<ui> s;
	forn(i, AMOUNT_PREDS)
    {
        bad_literal = false;
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

//        Formula_wrapper* res = Calculator_formula::max_common_subformula(wrap_1, wrap_2, vers, max_subst);

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


bool main_test_correct_subst(const Formula_wrapper& from, const Formula_wrapper& to, Variable_substitution* s)
{
    vcl* v_from = from.get_literals();
    vcl* v_to = to.get_literals();
    bool flag = false;

    for(const Literal* lit : *v_to)
    {
        forn(i, lit->amount_vars)
        {
            lit->vars[i] = s->at(lit->vars[i]);
        }

        flag = false;
        for(const Literal* lit_from : *v_from)
        {
            if (*lit_from == *lit)
            {
                flag = true;
                break;
            }
        }

        if (!flag)
        {
            return false;
        }
    }

    clear_vcl(*v_from);
    clear_vcl(*v_to);
    check_and_clear(v_from);
    check_and_clear(v_to);
    return true;
}



int main()
{
    srand(time(0));
    const int amount_vars = 5, amount_preds = 15, predicat_dimension = 3, amount_formulas = 3;

    forward_list<Formula_wrapper> list;
    const int AMOUNT_TIMES = 1;
    vcl temp;


//    forn(i, AMOUNT_TIMES)
//    {
//        if (i % (AMOUNT_TIMES / 100) == 0)
//        {
//            cout << i / (AMOUNT_TIMES / 100) << "% done\n";
//        }

//        forn(i, amount_formulas)
//        {
//            temp = generate_literals_vector(amount_vars, amount_preds, predicat_dimension);
//            list.push_front(Formula_wrapper(temp));
//            clear_vcl(temp);
//        }

//        MLCD mlcd(list);
//        cerr << mlcd << endl;
//        mlcd.debug_info(cerr);
//        mlcd.test();
//        mlcd.clear();
//        list.clear();
//    }

        vector<vcl *>* data = read_from_file();
        cerr << data->size() << endl;
        forn(i, data->size() - 1)
        {
            list.push_front(Formula_wrapper(*data->at(i)));
        }
        MLCD mlcd(list);
        cerr << mlcd << endl;
        mlcd.test();
        Formula_wrapper goal(*data->at(data->size() - 1));
        cerr << "\n\ngoal formula:\n";
        cerr << goal << endl;
//        Variable_substitution s(&goal, &goal);
        Formula_wrapper res = mlcd.inference(goal, NULL);
        cerr << "result of inference:\n";
        cerr << res << endl;

//        Formula_wrapper f1(*(data->at(0)));
//        Formula_wrapper f2(*(data->at(1)));

//        cout << f1 << endl << f2 << endl;
//        vector<Variable_properties> v1, v2;
//        f1.analyse_variable_properties(v1);
//        f2.analyse_variable_properties(v2);
//        Variable_substitution s(&f1, &f2);
//        Formula_wrapper* res = Calculator_formula::max_common_subformula(f1, f2, FIRST, &s);
//        cout << *res << endl;

//        for(auto el : v1)
//        {
//            cout << el;
//        }
//        cout << endl;
//        for(auto el : v2)
//        {
//            cout << el;
//        }
//        cout << endl;


//        forn(i, data->size() )
//        {
//            auto fw = Formula_wrapper (*(data->at(i)));
//    //        cout << fw << endl;
//            list.push_front(fw);
//        }

    return 0;
}


























