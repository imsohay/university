#ifndef CALCFORMULA_H
#define CALCFORMULA_H

#include <assert.h>
#include <set>
#include <vector>
#include <algorithm>
#include "formula_wrapper.h"
#include "literal.h"
#include "formula.h"
#include <map>

enum ALGORITHM_VERS { FIRST = 1, SECOND = 2 };
typedef std::vector<std::vector<int>> vvi;


void clear_vcl(vcl v);


class CalculatorFormula
{

public:
    CalculatorFormula() {}

    static std::vector<int*> satisfy();
    static Formula_wrapper *max_common_subformula(const Formula_wrapper& wrap_1,
                                           const Formula_wrapper& wrap_2, ALGORITHM_VERS vers, vi &max_subst);
    static Formula* build_formula(std::vector<const Literal *> vl);

    static std::vector<std::vector<int>>* analyse_variable_characteristic(const Formula_wrapper& fw);
    static size_t amount_mathing(vvi* t1, vvi* t2);
    ~CalculatorFormula();

private:
    static vui partial_inference(const std::vector<const Literal *> &from,
                                  const ui amount_vars_from,
                                  const std::vector<const Literal *> &to,
                                  const ui amount_vars_to);

    static vui partial_inference_2(const vcl &from,
                                  const ui amount_vars_from,
                                  const vcl &to,
                                  const ui amount_vars_to, vi &max_subst);

    static std::vector<const Literal *> max_common_subformula(const std::vector<const Literal *> &v1,
                                  ui amount_vars_v1,
                                  const std::vector<const Literal *> &v2,
                                  ui amount_vars_v2, ALGORITHM_VERS vers, vi &max_subst, ui &num_of_from_formula);
    static Formula *build_formula(const Literal *pl);

    static void cut_off_literals(std::vector<const Literal *> &v1, const std::vector<const Literal *> &v2);
    static int have(const vcl &v, const Literal *x);
    static int have(const vui &v, ui x);
    static ui calc_amount_vars(const vcl &v, std::map<int, int> &subst);
    static bool have_literal(const std::vector<const Literal *> &vl, const Literal *l, vui &eval);
    static void search(const vcl &from, const ui &amount_vars_from, const vcl &to, const ui &amount_vars_to, vui &max_subform, vui &current_subform, vui &remain_lits, vui &possible_subst, vi &subst, vi &max_subst);
    static bool check_for_subst(ui* subst, ui len, Literal* l, const vcl &from);
    static bool correct_on_subst(const Literal *possible_subst, const Literal *remain_lit, const vi &subst);
    static void add_pos_subst_to_subst(vi &subst, const Literal *possible_subst, const Literal *remain_lit);
    static void throw_fail_lits(const vi &subst, const vcl &from, const vcl &to, vui& remain_lits, const vui &possible_subst);
    static void add_success_lits(const vi& subst, const vcl &from, const vcl &to, vui& remain_lits, vui &current_subform, vui possible_subst);
    static void throw_fail_substs(vi& subst, vui &possible_subst, vui &remain_lits, const vcl &from, const vcl &to);
};



#endif // CALCFORMULA_H
