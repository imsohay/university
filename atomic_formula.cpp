#include "atomic_formula.h"


Atomic_formula::~Atomic_formula()
{
    check_and_clear_array(sub_terms);
}


Atomic_formula &Atomic_formula::operator=(const Atomic_formula &other)
{
    if (this != &other)
    {
        type = other.type;
        id = other.id;
        amount_sub_terms = other.amount_sub_terms;
        if (other.sub_terms != NULL)
        {
            sub_terms = new Term[amount_sub_terms];
            forn(i, amount_sub_terms)
                sub_terms[i] = (other.sub_terms[i]);
        }
        else
        {
            sub_terms = NULL;
        }
    }

    return *this;
}


std::ostream &operator<<(std::ostream &os, Atomic_formula &af)
{
    switch(af.type)
    {
    case F:
        os << "F";
        break;
    case T:
        os << "T";
        break;
    case PROP_VAR:
        os << "p_0()";
        break;
    case PRED:
        os << "p";
        os << af.id;
        os << "(";
        for(ui i = 0; i < af.amount_sub_terms; i++)
        {
            os << af.sub_terms[i];
            if (i + 1 != af.amount_sub_terms)
                os << " ";
        }
        os << ")";
        break;
    default:
        assert(0);
    }

    return os;

}




