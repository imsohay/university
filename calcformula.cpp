#include "calcformula.h"
#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;


int find_at(vi v, int el)
{
    forn(i, v.size())
        if (v[i] == el) return i;
    return -1;
}

int find_at(vcl v, const Literal* el)
{
    forn(i, v.size())
        if (*(v[i]) == *el) return i;
    return -1;
}


int find_at(vector<Literal> v, Literal el)
{
    forn(i, v.size())
        if (v[i] == el) return i;
    return -1;
}


int CalculatorFormula::have(const vcl& v, const Literal* x)
{
    int i = 0;
    for(auto& el : v)
    {
        if (el->predicat_id == x->predicat_id && el->sign == x->sign) return i;
        i++;
    }
    return -1;

}


int CalculatorFormula::have(const vui& v, ui x)
{
    int i = 0;
    for(auto& el : v)
    {
        if (el == x) return i;
        i++;
    }
    return -1;
}


void CalculatorFormula::cut_off_literals(std::vector<const Literal *> &v1, const std::vector<const Literal *> &v2)
{
    forn(i, v1.size())
    {
        if (have(v2, v1[i]) < 0)
        {
            delete v1[i];
            v1.erase(v1.begin() + i);
            i--;
        }
    }
}


ui CalculatorFormula::calc_amount_vars(const vcl& v, std::map<int, int>& subst)
{
    vui vars;
    ui current;
    int id = -1;
    const Literal* pl;

    forn(j, v.size())
    {
        pl = v[j];
        forn(i, pl->amount_vars)
        {
            current = pl->vars[i];
            id = have(vars, current);
            if (id < 0)
            {
                vars.push_back(current);
                subst.insert(pair<int,int>(current, vars.size() - 1));
                pl->vars[i] = vars.size() - 1;
            }
            else
            {
                pl->vars[i] = id;
            }
        }
    }

    return vars.size();
}


inline void show(vector<const Literal* > v)
{
    for (const Literal* x : v)
    {
        Literal l = *x;
        cout << l;
    }
}

inline void show(vector<Literal> v)
{
    for (auto x : v)
    {
        x.show();
    }
}


inline void show_with_subst(const vector<const Literal*> v, const vi subst)
{
    int n;
    for(auto x : v)
    {
        cout << x->predicat_id << "  ";
        forn(i, x->amount_vars)
        {
            n = subst[x->vars[i]];
            cout << n << " ";
        }

        cout << endl;
    }
}


inline void substitude_vector(vector<Literal> &v, vi subst)
{
    forn(j, v.size())
    {
        forn(i, v[j].amount_vars)
        {
            v[j].vars[i] = subst[v[j].vars[i]];
        }

    }
}

inline void substitude_literal(vector<const Literal*>& v, vi subst)
{
    for(auto& x : v)
    {
        forn(i, x->amount_vars)
        {
            x->vars[i] = subst[x->vars[i]];
        }
    }
}


void show_literals_with_common(vcl major,
                               vcl common,
                               set<ui> vars,
                               vi subst_major,
                               vi subst_common,
                               string name_formula,
                               string name_var,
                               map<int,int> begin_subst,
                               ofstream& fout)
{
	
    fout << name_formula << " =";
    for(auto el : major)
    {
        bool have_literal = 0;
        for(auto common_el : common)
        {
            if (el->predicat_id == common_el->predicat_id && el->sign == common_el->sign)
            {
                bool vars_not_equal = 0;
                forn(i, el->amount_vars)
                {
                    if (subst_major[el->vars[i]] != subst_common[common_el->vars[i]])
                    {
                        vars_not_equal = 1;
//                        cout << endl;
//                        forn(j, el->amount_vars) cout << subst_major[el->vars[j]] << " ";
//                        cout << "   [" << i << "]\n";
//                        forn(j, common_el->amount_vars) cout << subst_common[common_el->vars[j]] << " ";
//                        cout << endl;
                        break;
                    }
                }
                if (!vars_not_equal)
                {
                    have_literal = 1;
                    break;
                }
            }
        }

        if (!have_literal)
        {
            fout << " p" << el->predicat_id << "(";
            forn(i, el->amount_vars)
            {
                int x = subst_major[el->vars[i]];
                if (vars.find(x) != vars.end())
                {
                    fout << "u" << x;
                }
                else
                {
                    int n = -2;
                    for(auto p : begin_subst)
                    {
                        if (p.second == el->vars[i])
                        {
                            n = p.first;
                            break;
                        }
                    }
                    assert(n != -2);

                    fout << name_var << n;
                }
                if (i + 1 != el->amount_vars) fout << " ";
            }
            fout << ") &";
        }
    }
    fout << " C\n";
}

//    BOTH FORMULA MUST BE IN CNF
Formula_wrapper* CalculatorFormula::max_common_subformula(const Formula_wrapper& wrap_1,
                                                         const Formula_wrapper& wrap_2,
                                                         ALGORITHM_VERS vers, vi& max_subst)
{
    ui num_of_from_formula = 0;
    vector<const Literal* >* p_v1 = wrap_1.get_literals();
    vector<const Literal* >* p_v2 = wrap_2.get_literals();

    vector<const Literal* > v1 = *p_v1;
    vector<const Literal* > v2 = *p_v2;
    check_and_clear(p_v1);
    check_and_clear(p_v2);

    cut_off_literals(v1, v2);
    cut_off_literals(v2, v1);
    map<int, int> subst_1, subst_2, subst;
    ui amount_vars_1 = calc_amount_vars(v1, subst_1);
    ui amount_vars_2 = calc_amount_vars(v2, subst_2);

//    cout << "before searching:\n";
//    cout << "first:\n";
//    show(v1);
//    cout << "second:\n";
//    show(v2);
//    cout << "\n\n\n";

    vector<const Literal*> common_literals = max_common_subformula(v1, amount_vars_1, v2,
                                                                   amount_vars_2, vers, max_subst, num_of_from_formula);

    clear_vcl(v1);
    clear_vcl(v2);
    auto res = new Formula_wrapper(common_literals);
    clear_vcl(common_literals);
    return res;
}


size_t just_calc_amount_vars(vcl * literals)
{
    set<ui> set;
    for(auto l : *literals)
    {
        forn(i, l->amount_vars)
        {
            set.insert(l->vars[i]);
        }
    }

    return set.size();
}

vector<vector<int>>* CalculatorFormula::analyse_variable_characteristic(const Formula_wrapper &fw)
{
    vcl *literals = fw.get_literals();

    map<int,int> subst;
    size_t amount_vars = calc_amount_vars(*literals, subst);
    size_t dimension = literals->at(0)->amount_vars;
    vector<vector<int>>* table = new vector<vector<int>>();
    forn(i, amount_vars)
    {
        table->push_back(vector<int>(dimension, 0));
    }

    forn(i, literals->size())
    {
        const Literal* l = literals->at(i);
        forn(j, dimension)
        {
            (*table)[l->vars[j]][j]++;
        }
    }

//    forn(i, amount_vars)
//    {
//        forn(j, dimension)
//        {
//            cout << (*table)[i][j] << " ";
//        }
//        cout << "   ";
//    }

    clear_vcl(*literals);
    check_and_clear(literals);
    return table;
}


bool equal(vector<int> v1, vector<int> v2)
{
    if (v1.size() != v2.size()) return false;

    forn(i, v1.size())
    {
        if (v1[i] != v2[i])
            return false;
    }

    return true;
}

size_t CalculatorFormula::amount_mathing(vvi *t1, vvi *t2)
{
    size_t sum = 0;
    set<size_t> matched;
    forn(i, t1->size())
    {
        forn(j, t2->size())
        {
            if (equal(t1->at(i), t2->at(j)) && matched.count(j) == 0)
            {
                sum++;
                matched.insert(j);
                break;
            }
        }
    }

    return sum;
}


bool go_next(vui& v, ui limit)
{
    ui size = v.size();
    for(int i = size - 1; i >= 0; i--)
    {
        if (v[i] < limit - 1 -(size - 1 - i))
        {
            ++v[i];
            for(ui j = i + 1; j < size; ++j) v[j] = v[j - 1] + 1;
            return true;
        }
    }

    return false;
}


bool CalculatorFormula::have_literal(const std::vector<const Literal*>& vl, const Literal* l, vui& eval)
{
    bool equal = true;
    for(const Literal* curr : vl)
    {
        if (curr->predicat_id == l->predicat_id &&
            curr->sign == l->sign &&
            curr->amount_vars == l->amount_vars)
        {
            equal = true;
            forn(i, curr->amount_vars)
            {
                if (curr->vars[i] != eval[l->vars[i]])
                {
                    equal = false;
                    break;
                }
            }

            if (equal) return true;
        }
    }

    return false;
}


vui CalculatorFormula::partial_inference(const vector<const Literal*>& from, const ui amount_vars_from,
                                         const vector<const Literal*>& to, const  ui amount_vars_to)
{
    ui begin_size = to.size();
    vui combination(to.size());
    vui eval(amount_vars_to, 0);
    forn(i, eval.size()) eval[i] = i;
    bool has_all_literals = true;

    for(ui size_subset = to.size(); size_subset > 0; --size_subset)
    {
        forn(i, combination.size()) combination[i] = i;

        do
        {
            forn(i, eval.size()) eval[i] = i;

            do
            {
                has_all_literals = true;
                for(ui i : combination)
                {
                    if (!have_literal(from, to[i], eval))
                    {
                        has_all_literals = false;
                        break;
                    }

                }

                if (has_all_literals)
                {
                    return combination;
                }
            } while(next_permutation(eval.begin(), eval.end()) || go_next(eval, amount_vars_from));
        } while(go_next(combination, begin_size));

        combination.pop_back();
    }

    combination.clear();
    return combination;
}


bool CalculatorFormula::correct_on_subst(const Literal* possible_subst, const Literal* remain_lit, const vi& subst)
{
    if (!Literal::equal_literals(possible_subst, remain_lit)) return false;
    int x, x2, x3;
    forn(i, remain_lit->amount_vars)
    {
        x = subst[remain_lit->vars[i]];
        x2 = possible_subst->vars[i];
        x3 = find_at(subst, x2);
        if ((x != -1 && x != x2) || (x3 != -1 && x3 != remain_lit->vars[i]))
            return false;
    }
    return true;
}


void CalculatorFormula::add_pos_subst_to_subst(vi& subst, const Literal* possible_subst, const Literal* remain_lit)
{
    forn(i, remain_lit->amount_vars)
    {
        subst[remain_lit->vars[i]] = possible_subst->vars[i];
    }
}


void CalculatorFormula::throw_fail_lits(const vi &subst, const vcl &from, const vcl &to, vui &remain_lits, const vui& possible_subst)
{
    vui copy_remain_lits;
    bool correct;
    forn(i, remain_lits.size())
    {
        correct = false;
        for (ui j : possible_subst)
        {
            if (correct_on_subst(from[j], to[remain_lits[i]], subst))
            {
                correct = true;
                break;
            }
        }

        if (correct)
        {
            copy_remain_lits.push_back(remain_lits[i]);
        }
    }

    remain_lits = copy_remain_lits;
}


void CalculatorFormula::add_success_lits(const vi &subst, const vcl &from, const vcl &to, vui &remain_lits, vui &current_subform, vui possible_subst)
{
    vui copy_remain_lits;

    bool flag = false;
    forn(i, remain_lits.size())
    {
        flag = false;
        for (ui j : possible_subst)
        {
            if (to[remain_lits[i]]->equal_on_eval(from[j], subst))
            {
                current_subform.push_back(remain_lits[i]);
                flag = true;
                break;
            }
        }

        if (!flag)
            copy_remain_lits.push_back(remain_lits[i]);
    }

    remain_lits = copy_remain_lits;
}


void CalculatorFormula::throw_fail_substs(vi &subst, vui &possible_subst, vui &remain_lits, const vcl &from,  const vcl &to)
{
    vui copy_possible_subst;

    bool correct = false;
    forn(i, possible_subst.size())
    {
        correct = false;
        for(ui j : remain_lits)
        {

            if (correct_on_subst(from[possible_subst[i]], to[j], subst))
            {
                correct = true;
                break;
            }
        }

        if (correct)
            copy_possible_subst.push_back(possible_subst[i]);
    }

    possible_subst = copy_possible_subst;
    return;
}


void CalculatorFormula::search(const vcl &from, const ui& amount_vars_from, const vcl &to, const ui& amount_vars_to,
            vui& max_subform, vui& current_subform, vui& remain_lits, vui& possible_subst, vi& subst, vi& max_subst)
{
    throw_fail_lits(subst, from, to, remain_lits, possible_subst);
    add_success_lits(subst, from, to, remain_lits, current_subform, possible_subst);
    throw_fail_substs(subst, possible_subst, remain_lits, from, to);
    if (current_subform.size() > max_subform.size())
    {
        max_subform.clear();

        max_subst.clear();
        max_subst = vi(subst.begin(), subst.end());
        for(auto x : current_subform) max_subform.push_back(x);
    }

    if (possible_subst.empty() || remain_lits.empty())
        return;

    if (max_subform.size() >= possible_subst.size() + current_subform.size())
        return;
    ui i, j, r, q;
    vui copy_current_subform, copy_remain_lits, copy_possible_subst;
    vi copy_subst;

    for(r = 0; r < remain_lits.size(); r++)
    {
        i = remain_lits[r];
        for(q = 0; q < possible_subst.size(); q++)
        {
            j = possible_subst[q];
            if (correct_on_subst(from[j], to[i], subst)) // ??? возможно, следует выкинуть проверку
            {
                copy_current_subform = current_subform;
                copy_remain_lits = remain_lits;
                copy_possible_subst = possible_subst;
                copy_subst = subst;

                add_pos_subst_to_subst(copy_subst, from[j], to[i]);

                if (from[j]->equal_on_eval(to[i], copy_subst))
                {
                    copy_current_subform.push_back(i);
                    copy_remain_lits.erase(copy_remain_lits.begin() + r);
                }

                copy_possible_subst.erase(copy_possible_subst.begin() + q);

                search(from, amount_vars_from, to, amount_vars_to, max_subform, copy_current_subform,
                       copy_remain_lits, copy_possible_subst, copy_subst, max_subst);

                if (max_subform.size() >= possible_subst.size() + current_subform.size())
                    return;
            }
        }

    }

}


vui CalculatorFormula::partial_inference_2(const vcl &from, const ui amount_vars_from,
                                           const vcl &to, const ui amount_vars_to, vi& max_subst)
{
    vui max_subform, possible_subst, current_subform, remain_lits;
    vi subst(amount_vars_to, -1);
    forn(i, from.size()) possible_subst.push_back(i);
    forn(i, to.size()) remain_lits.push_back(i);

    search(from, amount_vars_from, to, amount_vars_to, max_subform, current_subform, remain_lits, possible_subst, subst, max_subst);

    return max_subform;
}


std::vector<const Literal*> CalculatorFormula::max_common_subformula(
                                              const std::vector<const Literal *> &v1,
                                              ui amount_vars_v1,
                                              const std::vector<const Literal *> &v2,
                                              ui amount_vars_v2, ALGORITHM_VERS vers,
                                              vi& max_subst, ui& num_of_from_formula)
{
    vui vv2;
    const vector<const Literal* >* pv;
    if (vers == FIRST)
    {
        if (amount_vars_v1 > amount_vars_v2)
        {
            vv2 = partial_inference(v1, amount_vars_v1, v2, amount_vars_v2);
            pv = &v2;
            num_of_from_formula = 1;
        }
        else
        {
            vv2 = partial_inference(v2, amount_vars_v2, v1, amount_vars_v1);
            pv = &v1;
            num_of_from_formula = 2;
        }
    }
    else
    {
        if (v1.size() >= v2.size())
        {
            vv2 = partial_inference_2(v1, amount_vars_v1, v2, amount_vars_v2, max_subst);
            pv = &v2;
            num_of_from_formula = 1;
        }
        else
        {
            vv2 = partial_inference_2(v2, amount_vars_v2, v1, amount_vars_v1, max_subst);
            pv = &v1;
            num_of_from_formula = 2;
        }
    }

    vector<const Literal*> res(vv2.size());
    forn(i, vv2.size())
    {
        res[i] = new Literal(*(pv->at(vv2[i])));

    }
    return res;
}


CalculatorFormula::~CalculatorFormula()
{
}



void clear_vcl(vcl v)
{
    forn(i, v.size())
    {
        check_and_clear(v[i]);
    }
}
