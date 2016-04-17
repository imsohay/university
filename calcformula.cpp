#include "calcformula.h"
#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;


#define debug(x) {cout << "debug search:\t"; x; }


int find_at(vi v, int el)
{
    forn(i, v.size())
        if (v[i] == el) return i;
    return -1;
}


void reverse_map(map<int,int>& s)
{
    map<int,int> temp;
    for(auto el : s)
        temp[el.second] = el.first;
    s.clear();
    s = temp;
}


int reverse_find(const map<int,int>& s, int value)
{
    for(const auto & el : s)
    {
        if (el.second == value)
            return el.first;
    }

    __throw_out_of_range("not found key by value");
}


template <typename T>
void Calculator_formula::do_substitude(vcl literals, T s)
{
    for(auto& el : literals)
    {
        forn(i, el->amount_vars)
        {
            el->vars[i] = s.at(el->vars[i]);
        }
    }
}


void Calculator_formula::do_reverse_substitude(vcl literals, const std::map<int,int> &s)
{
    for(auto& el : literals)
    {
        forn(i, el->amount_vars)
        {
            el->vars[i] = reverse_find(s, el->vars[i]);
        }
    }
}



int Calculator_formula::have(const vcl& v, const Literal* x)
{
    int i = 0;
    for(auto& el : v)
    {
        if (el->predicat_id == x->predicat_id && el->sign == x->sign) return i;
        i++;
    }
    return -1;

}


int Calculator_formula::have(const vui& v, ui x)
{
    int i = 0;
    for(auto& el : v)
    {
        if (el == x) return i;
        i++;
    }
    return -1;
}


void Calculator_formula::cut_off_literals(std::vector<const Literal *> &v1, const std::vector<const Literal *> &v2)
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


ui Calculator_formula::calc_amount_vars(const vcl& v, std::map<int, int>& subst)
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


void debug_show(map<int,int> s)
{
    for (auto el : s)
        printf("%3i ", el.first);
    cout << endl;
    for (auto el : s)
        printf("%3i ", el.second);
    cout << endl;
}


void debug_show(vi v)
{
    string s;
    forn(i, v.size())
        printf("%3i", i);
    cout << endl;
    for(auto x : v)
        printf("%3i", x);
    cout << endl;
}


void debug_show(vcl v)
{
    for(const Literal* el : v)
    {
       cout << "\t";
        forn(i, el->amount_vars)
            cout << el->vars[i] << " ";
       cout << endl;
    }
}


void Calculator_formula::transform_substitution(const Formula_wrapper& wrap_1,
                                                const Formula_wrapper& wrap_2,
                                                std::map<int,int> subst_1,
                                                std::map<int,int> subst_2,
                                                Variable_substitution* subst)
{
    assert((*(subst->getFrom()) == wrap_1) && (*(subst->getTo()) == wrap_2) ||
           (*(subst->getTo()) == wrap_1) && (*(subst->getFrom()) == wrap_2));
    if (subst->getFrom() == &wrap_2)
    {
        std::swap(subst_1, subst_2);
    }


    map<int,int> transformed_subst;
    for(const auto& el : subst->getValue())
    {
        transformed_subst.insert(std::pair<int,int>(subst_2.at(el.first),
                                                    (el.second == -1 ? -1 : subst_1.at(el.second))));
    }

//    cout << "first subst:\n";
//    debug_show(subst_1);
//    cout << "second subst:\n";
//    debug_show(subst_2);

//    cout << "transformed:\n";
//    debug_show(transformed_subst);
    subst->clear();
    for(auto el : transformed_subst)
        subst->insert(el.first, el.second);
}


//    BOTH FORMULA MUST BE IN CNF
Formula_wrapper* Calculator_formula::max_common_subformula(const Formula_wrapper& wrap_1,
                                                         const Formula_wrapper& wrap_2,
                                                         ALGORITHM_VERS vers,
                                                         Variable_substitution* subst)
{
    bool swaped = false;
    vector<const Literal* >* p_v1 = wrap_1.get_literals();
    vector<const Literal* >* p_v2 = wrap_2.get_literals();
    vector<const Literal* > v1 = *p_v1;
    vector<const Literal* > v2 = *p_v2;
    check_and_clear(p_v1);
    check_and_clear(p_v2);

    cut_off_literals(v1, v2);
    cut_off_literals(v2, v1);
    map<int, int> subst_1, subst_2;
    ui amount_vars_1 = calc_amount_vars(v1, subst_1);
    ui amount_vars_2 = calc_amount_vars(v2, subst_2);
    transform_substitution(wrap_1, wrap_2, subst_1, subst_2, subst);
    //cerr << "transformed subst:\n";
    //cerr << *subst << endl;

    vector<const Literal*> common_literals = max_common_subformula(v1, amount_vars_1, v2,
                                                                   amount_vars_2, vers, subst, swaped);

//    cout << "common subst:\n";
//    cout << *subst << endl;
    if (vers == ALGORITHM_VERS::SECOND)
    {
        if (swaped)
        {
            std::swap(subst_1, subst_2);
            subst->swap_formulas();
        }

        reverse_map(subst_1);
        map<int,int> new_s;
        int temp_val = -1;
        for(auto el : subst_2)
        {
            temp_val = subst->at(el.second);
            if (temp_val == -1)
            {
                new_s[el.first] = temp_val;
            }
            else
            {
                new_s[el.first] = subst_1.at(temp_val);
            }
        }

        subst->clear();
        for(auto el : new_s)
            subst->insert(el.first, el.second);

        clear_vcl(v1);
        clear_vcl(v2);
        do_reverse_substitude(common_literals, subst_2);
    }
        auto res = new Formula_wrapper(common_literals);
        clear_vcl(common_literals);

//    subst->flush();
    return res;
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


bool Calculator_formula::have_literal(const std::vector<const Literal*>& vl, const Literal* l, vui& eval)
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


vui Calculator_formula::partial_inference(const vector<const Literal*>& from, const ui amount_vars_from,
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


bool Calculator_formula::correct_on_subst(const Literal* possible_subst, const Literal* remain_lit, const vi& subst)
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


void Calculator_formula::add_pos_subst_to_subst(vi& subst, const Literal* possible_subst, const Literal* remain_lit)
{
    forn(i, remain_lit->amount_vars)
    {
        subst[remain_lit->vars[i]] = possible_subst->vars[i];
    }
}


void Calculator_formula::throw_fail_lits(const vi &subst, const vcl &from, const vcl &to, vui &remain_lits, const vui& possible_subst)
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


void Calculator_formula::add_success_lits(const vi &subst, const vcl &from, const vcl &to, vui &remain_lits, vui &current_subform, vui possible_subst)
{

    vui copy_remain_lits;
    bool flag = false;
    forn(i, remain_lits.size())
    {
        flag = false;
        for (ui j : possible_subst)
        {
            if (from[j]->equal_on_eval(to[remain_lits[i]], subst))
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


void Calculator_formula::throw_fail_substs(vi &subst, vui &possible_subst, vui &remain_lits, const vcl &from,  const vcl &to)
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



void Calculator_formula::search(const vcl &from,
                                const ui& amount_vars_from,
                                const vcl &to,
                                const ui& amount_vars_to,
                                vui& max_subform,
                                vui& current_subform,
                                vui& remain_lits,
                                vui& possible_subst,
                                vi& cur_subst,
                                Variable_substitution* max_subst)
{
    throw_fail_lits(cur_subst, from, to, remain_lits, possible_subst);
    add_success_lits(cur_subst, from, to, remain_lits, current_subform, possible_subst);
    throw_fail_substs(cur_subst, possible_subst, remain_lits, from, to);
    if (current_subform.size() > max_subform.size())
    {
        max_subform.clear();
        max_subst->clear();
        forn(i, cur_subst.size())
            (*max_subst)[i] = cur_subst[i];
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
            if (correct_on_subst(from[j], to[i], cur_subst)) // ??? возможно, следует выкинуть проверку
            {
                copy_current_subform = current_subform;
                copy_remain_lits = remain_lits;
                copy_possible_subst = possible_subst;
                copy_subst = cur_subst;

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


vui Calculator_formula::partial_inference_2(const vcl &from,
                                            const ui amount_vars_from,
                                            const vcl &to,
                                            const ui amount_vars_to,
                                            Variable_substitution* subst)
{
    vui max_subform, possible_subst, current_subform, remain_lits;
    vi current_subst;

    if (subst->empty())
    {
        current_subst = vi(amount_vars_to, -1);
    }
    else
    {
        current_subst = vi(subst->size(), -1);
        for(const auto& el : subst->getValue())
        {
            assert(el.first < (int)amount_vars_to);
            assert(el.first >= 0);
            assert(el.second < (int)amount_vars_from);
            current_subst[el.first] = el.second;
        }
        //cerr << current_subst.size() << endl;
        //cerr << subst->size() << endl;
//        debug_show(current_subst);
        subst->clear();
    }

    forn(i, from.size()) possible_subst.push_back(i);
    forn(i, to.size()) remain_lits.push_back(i);

    search(from, amount_vars_from, to, amount_vars_to, max_subform, current_subform, remain_lits, possible_subst, current_subst, subst);

    return max_subform;
}


std::vector<const Literal*> Calculator_formula::max_common_subformula(const std::vector<const Literal *> &v1,
                                              ui                                 amount_vars_v1,
                                              const std::vector<const Literal *> &v2,
                                              ui                                 amount_vars_v2,
                                              ALGORITHM_VERS                     vers,
                                              Variable_substitution*             subst,
                                              bool &swaped)
{
    vui vv2;
    const vector<const Literal* >* pv;
    if (vers == FIRST)
    {
        if (amount_vars_v1 > amount_vars_v2)
        {
            vv2 = partial_inference(v1, amount_vars_v1, v2, amount_vars_v2);
            pv = &v2;
            swaped = false;
        }
        else
        {
            vv2 = partial_inference(v2, amount_vars_v2, v1, amount_vars_v1);
            pv = &v1;
            swaped = true;
        }
    }
    else
    {
        if (v1.size() >= v2.size())
        {
            vv2 = partial_inference_2(v1, amount_vars_v1, v2, amount_vars_v2, subst);
            pv = &v2;
            swaped = false;
        }
        else
        {
            vv2 = partial_inference_2(v2, amount_vars_v2, v1, amount_vars_v1, subst);
            pv = &v1;
            swaped = true;
        }
    }

    vector<const Literal*> res(vv2.size());
    forn(i, vv2.size())
    {
        res[i] = new Literal(*(pv->at(vv2[i])));

    }
    return res;
}




