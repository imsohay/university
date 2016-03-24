#include "term.h"



Term::~Term()
{
    check_and_clear_array(sub_terms);
}


std::ostream& operator<<(std::ostream &os, Term &term)
{
    switch(term.type)
    {
        case CONST:
            os << "f_0()";
            break;
        case VAR:
            os << "u";
            os << term.id;
            break;
        case FUNC:
            os << "f";
            os << term.id;
            os << "(";
            for(ui i = 0; i < term.amount_sub_terms; i++)
            {
                os << term.sub_terms[i];
                if (i + 1 != term.amount_sub_terms) os << " ";
            }
            os << ")";
            break;
        default:
            assert(0);
    }

    return os;
}
