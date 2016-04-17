#include "term.h"




Term::~Term()
{
    check_and_clear_array(sub_terms);
}

Term &Term::operator=(const Term &other)
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
                sub_terms[i] = other.sub_terms[i];
        }
        else
        {
            sub_terms = NULL;
        }
    }

    return *this;
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
