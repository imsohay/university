#ifndef FORMULA_WRAPPER_H
#define FORMULA_WRAPPER_H

#include "formula.h"
#include "literal.h"
#include <set>
#include <vector>


typedef const Literal* const_p_lit;
typedef std::vector<const_p_lit> vcl;


class Formula_wrapper
{
public:
    Formula_wrapper()
        :f(NULL) {}

    Formula_wrapper(vcl vl);
    ~Formula_wrapper() { check_and_clear(f); }
    std::vector<const Literal *> *get_literals() const;

    friend std::ostream& operator<<(std::ostream& os, Formula_wrapper& fw);

private:
    Formula* f;


    Formula *build_formula(const Literal *pl);
    Literal *create_literal(Atomic_formula *p_af, bool sign) const;
    void create_set_literal(const Formula *p_f, std::set<const Literal *> *s) const;
    void get_id_vars(const Term * const term, vui &v) const;
};

#endif // FORMULA_WRAPPER_H
