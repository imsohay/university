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



Formula &Formula::operator=(const Formula &other)
{
    if (this != &other)
    {
        if (other.l != NULL)
        {
            l = new Formula;
            *l = *other.l;
        }
        else
            l = NULL;

        if (other.r != NULL)
        {
            r = new Formula;
            *r = *other.r;
        }
        else
            r = NULL;

        if (other.m != NULL)
        {
            m = new Atomic_formula;
            *m = *other.m;
        }
        else
            m = NULL;

        _label = other._label;
        id = other.id;

    }
    return *this;
}

Formula::~Formula()
{
    check_and_clear(m);
    check_and_clear(l);
    check_and_clear(r);
}

bool Formula::empty() const
{
    return (m == NULL && r == NULL && l == NULL && _label == NONE);
}





