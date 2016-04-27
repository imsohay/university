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


void measure_correctness_equality_algo() {
    srand(time(NULL));
    const int amount_vars = 4, amount_preds = 5, predicat_dimension = 3;
    const int AMOUNT_TIMES = 1000000;
    vcl v1,v2;
    int amount_fails = 0, cur_amount_vars = 0, cur_amount_preds = 0, cur_predicat_dimension = 0;

    forn(i, AMOUNT_TIMES) {
        cur_amount_vars = amount_vars + (rand() % 5);
        cur_amount_preds = amount_preds + (rand() % 8);
        cur_predicat_dimension = predicat_dimension + (rand() % 3);

        v1 = generate_literals_vector(amount_vars, amount_preds, predicat_dimension);
        v2 = generate_literals_vector(amount_vars, amount_preds, predicat_dimension);
        Formula_wrapper f1(v1);
        Formula_wrapper f2(v2);

//        if ((v1 == v2) ^ f1.beta_equal(f2))
        if (!f1.beta_equal(f2) && (f1 == f2))
            amount_fails++;

        clear_vcl(v1);
        clear_vcl(v2);
    }

    cout << (double)amount_fails / AMOUNT_TIMES * 100 << "% fails\n";
}



int main()
{
    auto start = std::chrono::system_clock::now();

    measure_correctness_equality_algo();

    auto end = std::chrono::system_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "time:  " << elapsed.count() << " ms\n\n";

    return 0;


//    srand(time(0));
//    const int amount_vars = 5, amount_preds = 7, predicat_dimension = 3, amount_formulas = 6;

    forward_list<Formula_wrapper> list;
//    const int AMOUNT_TIMES = 1000;
//    vcl temp;


    //    forn(i, AMOUNT_TIMES)
    //    {
    //        if (i % (AMOUNT_TIMES / 100) == 0)
    //        {
    //            cout << i / (AMOUNT_TIMES / 100) << "% done\n";
    //        }

    //        forn(j, amount_formulas)
    //        {
    //            bool fail = false;
    //            temp = generate_literals_vector(amount_vars, amount_preds, predicat_dimension);
    //            forn(k, temp.size())
    //            {
    //                forn(q, temp.size())
    //                {
    //                    if (k != q && *temp[k] == *temp[q])
    //                    {
    //                        fail = true;
    //                        break;
    //                    }
    //                }

    //                if (fail)
    //                    break;
    //            }
    //            if (fail)
    //            {
    //                assert(NULL);
    //                j--;
    //            }
    //            else
    //            {
    //                list.push_front(Formula_wrapper(temp));
    //            }


    //            clear_vcl(temp);
    //        }

    //        MLCD mlcd(list);
    ////        cerr << mlcd << endl;
    ////        mlcd.debug_info(cerr);
    //        mlcd.test();
    //        mlcd.test_inference();
    //        mlcd.clear();
    //        list.clear();
    //    }


    vector<vcl *>* data = read_from_file();
    ofstream out("result.txt");

//    cerr << data->size() << endl;
    vector<Formula_wrapper> v;

    forn(i, data->size())
    {
        list.push_front(Formula_wrapper(*data->at(i)));
//        v.push_back(Formula_wrapper(*data->at(i)));
    }

    for(auto el : list)
        cout << el << "\n\n";

//    Variable_substitution subst(&v[0], &v[1]);

//    auto start = std::chrono::system_clock::now();

//    auto res = Calculator_formula::max_common_subformula(v[0], v[1], SECOND, &subst);

//    auto end = std::chrono::system_clock::now();
//    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//    std::cout << "time:  " << elapsed.count() << " ms\n\n";

//    cout << *res << endl;


    MLCD mlcd(list);
    mlcd.test();

    out << mlcd << endl;
    out.close();

//    mlcd.test_inference();


//    Formula_wrapper goal(*data->at(3));
//    cerr << "goal formula:\n";
//    cerr << goal << endl;
//    mlcd.rebuild(goal);
//    mlcd.test();
//    mlcd.test_inference();
//    cerr << "\n====================================================\nafter rebuild:\n";
//    cerr << mlcd << endl;

    for(vcl* el : *data)
    {
        clear_vcl(*el);
        check_and_clear(el);
    }
    check_and_clear(data);
    return 0;
}


























