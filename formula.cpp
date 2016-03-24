#include "formula.h"
#include <assert.h>

std::ostream &operator<<(std::ostream &os, Formula &formula)
{
    switch (formula._label)
    {
    case NONE:
        os << *formula.m;
        break;
    case NOT:
        os << "! " << *formula.l;
        break;
    case AND:
        os << *formula.l << " & " << *formula.r;
        break;
    case OR:
        os << *formula.l << " | " << *formula.r;
        break;
    case IMP:
        os << *formula.l << " => " << *formula.r;
        break;
    case ALL:
        os << "FOR ALL x" << formula.id;
        os << " " << *formula.l;
        break;
    case EXIST:
        os << "EXIST x" << formula.id;
        os << " " << *formula.l;
        break;
    }

    return os;
}


Formula::~Formula()
{
    check_and_clear(m);
    check_and_clear(l);
    check_and_clear(r);
}





