#ifndef FORMULA_H
#define FORMULA_H

#include "atomic_formula.h"


enum label { NONE, NOT, AND, OR, IMP, ALL, EXIST };

class Formula
{
public:
    friend class Interpretation;
    friend class Formula_wrapper;

    Formula(const Formula& other) { *this = other; }
    friend std::ostream& operator<<(std::ostream& os, Formula& formula);
    Formula & operator=(const Formula &other);

private:
    label _label;
    ui id;
    Formula* l;
    Formula* r;
    Atomic_formula* m;


    Formula()
        :_label(NONE), id(-1), l(NULL), r(NULL), m(NULL) {}
    Formula(label __label, Formula* _l, Formula* _r, Atomic_formula* _m)
        :_label(__label), l(_l), r(_r), m(_m) {}
    ~Formula();

    bool empty() const;


};

#endif // FORMULA_H
