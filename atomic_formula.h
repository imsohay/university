#ifndef ATOMIC_FORMULA_H
#define ATOMIC_FORMULA_H

#include "term.h"

enum type_atomic_formula { F, T, PROP_VAR, PRED };

class Atomic_formula
{
public:
    friend class Formula_wrapper;
    friend class Interpretation;

    Atomic_formula(type_atomic_formula _type,
                   ui _id,
                   ui _amount_sub_terms,
                   Term* _sub_terms)
        :type (_type),
         id (_id),
         amount_sub_terms (_amount_sub_terms),
         sub_terms (_sub_terms) {}
    ~Atomic_formula();

    type_atomic_formula get_type() const { return type; }
    ui get_id() const { return id; }
    Term* get_terms() const { return sub_terms; }

    friend std::ostream& operator<<(std::ostream& os, Atomic_formula& af);


private:
    type_atomic_formula type;
    ui id;
    ui amount_sub_terms;
    Term* sub_terms;
};

#endif // ATOMIC_FORMULA_H
