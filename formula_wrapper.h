#ifndef FORMULA_WRAPPER_H
#define FORMULA_WRAPPER_H

#include "formula.h"
#include "literal.h"
#include <set>
#include <vector>
#include <map>





class Variable_properties
{
public:
    Variable_properties(std::vector<int> dimensions);
    void increment_value(int literal_num, int position_num);

    bool operator ==(const Variable_properties& other) const;
    friend std::ostream& operator<<(std::ostream& os, Variable_properties& vp);


private:
    std::vector<std::vector<int>> data;
};




class Formula_wrapper
{
public:
    Formula_wrapper()
        :f(NULL) {}

    Formula_wrapper(vcl vl);
    Formula_wrapper(const Formula_wrapper& other);
    ~Formula_wrapper() { check_and_clear(f); }
    std::vector<const Literal *> *get_literals() const;
    std::map<int,int> create_id_substitution() const;
    void analyse_variable_properties(std::vector<Variable_properties> &) const;


    bool empty() const { return (f == NULL) || f->empty(); }
    void clear() { check_and_clear(f); }


    friend std::ostream& operator<<(std::ostream& os, Formula_wrapper& fw);
    bool operator ==(const Formula_wrapper& other) const;
    bool operator !=(const Formula_wrapper& other) const;
    Formula_wrapper & operator=(const Formula_wrapper& o);

    bool beta_equal(const Formula_wrapper &other) const;

private:
    Formula* f;


    Formula *build_formula(const Literal *pl);
    Literal *create_literal(Atomic_formula *p_af, bool sign) const;
    void create_literals_list(const Formula *p_f, vcl *v) const;
    void get_id_vars(const Term * const term, vui &v) const;
    std::vector<std::vector<int>>* analyse_variable_properties();
    size_t just_calc_amount_vars(vcl *literals) const;
    bool equal(const Formula_wrapper &other) const;
    void calc_literals_dimensions(vcl* literals, std::map<int, int> &v) const;
    void count_variables(vcl* literals, std::map<int,int>& mapping) const;
};

#endif // FORMULA_WRAPPER_H
