#include "atomic_formula.h"


Atomic_formula::~Atomic_formula()
{
    check_and_clear_array(sub_terms);
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




