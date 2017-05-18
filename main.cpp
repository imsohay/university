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


int measure_correctness_equality_algo(const int amount_vars, const int amount_preds, const int predicat_dimension)
{
     srand(time(NULL));
//     const int amount_vars = 3, amount_preds = 3, predicat_dimension = 2;
     const int AMOUNT_TIMES = 10000;
     vcl v1,v2;
     int amount_fails = 0, cur_amount_vars = 0, cur_amount_preds = 0, cur_predicat_dimension = 0;
     int percent = 0;

     forn(i, AMOUNT_TIMES) {

//         percent = AMOUNT_TIMES / 100;
//         if (i % percent == 0)
//             cout << i / percent << "%\n";

         //         cur_amount_vars = amount_vars + (rand() % 5);
         //         cur_amount_preds = amount_preds + (rand() % 8);
         //         cur_predicat_dimension = predicat_dimension + (rand() % 3);
              cur_amount_vars = amount_vars;
              cur_amount_preds = amount_preds;
              cur_predicat_dimension = predicat_dimension;
              if (cur_predicat_dimension >= cur_amount_vars)
                  cur_amount_vars = cur_predicat_dimension + 1;

         v1 = generate_literals_vector(cur_amount_vars, cur_amount_preds, cur_predicat_dimension);
         v2 = generate_literals_vector(cur_amount_vars, cur_amount_preds, cur_predicat_dimension);
         Formula_wrapper f1(v1);
         Formula_wrapper f2(v2);

         if (f1.beta_equal(f2) ^ (f1 == f2))
         {
//             cout << f1 << endl << f2 << endl;
//             break;
             amount_fails++;
         }


         clear_vcl(v1);
         clear_vcl(v2);
     }

//     cout << ((double)amount_fails/ AMOUNT_TIMES) * 100 << "% fails\n";
     return amount_fails;
}



void test()
{
    auto v = generate_literals_vector(4, 3, 3);
    auto v2 = generate_literals_vector(4, 3, 3);
    Formula_wrapper fw(v);
    Formula_wrapper fw2(v2);

    cout << fw << endl;

    auto incidences_list = fw.get_incidences_list();
    for(auto el : incidences_list)
    {
        cout << el.first << "  " << el.second << endl;
    }
    cout << "##############\n";
    auto incidences_list2 = fw2.get_incidences_list();
    for(auto el : incidences_list2)
    {
        cout << el.first << "  " << el.second << endl;
    }
//    cout << endl << fw << endl;
    cout << "#############################################";
    cout << (fw.equal_by_incidences_list(fw2) ? "equal" : "not equal") << endl;
}


int main()
{

    test();
//    int n_vars = 7, n_preds = 4, pred_dim = 4;

//    cout << measure_correctness_equality_algo(n_vars, n_preds, pred_dim) << endl;



//    for(pred_dim = 2; pred_dim < 5; pred_dim++)
//    {
//        for(n_preds = 2; n_preds <= 5; ++n_preds)
//        {
//            for(n_vars = 2; n_vars <= 8; ++n_vars)
//            {
//                if (n_vars < pred_dim)
//                    continue;

//                printf("for %d vars and %d predicats with predicat dimension %d  ---  %d neq\n", n_vars, n_preds, pred_dim,
//                       measure_correctness_equality_algo(n_vars, n_preds, pred_dim));
//            }
//        }
//    }
//    vector<vcl*> * data = read_from_file();

//    forward_list<Formula_wrapper> list;
//    for(vcl * ptr : *data)
//    {
//        list.push_front(*ptr);
//    }

//    MLCD description(list);

//    ofstream out("result.txt");
//    out << description << endl;

//    for(vcl * ptr : *data)
//    {
//        clear_vcl(*ptr);
//        check_and_clear(ptr);
//    }
//    check_and_clear(data)
    return 0;
}
























