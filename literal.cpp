#include "literal.h"
#include <bitset>
using namespace std;

Literal::Literal(const Literal &rhs)
{
    *this = rhs;
}

bool Literal::operator ==(const Literal &other) const
{
    if (!(predicat_id == other.predicat_id && sign == other.sign)) return false;
    forn(i, other.amount_vars)
            if (vars[i] != other.vars[i]) return false;

    return true;
}

Literal &Literal::operator=(const Literal &other)
{
    if (this != &other)
    {
        sign = other.sign;
        predicat_id = other.predicat_id;
        amount_vars = other.amount_vars;
        vars = new ui[amount_vars];
        forn(i, amount_vars)
                vars[i] = other.vars[i];
    }

    return *this;
}



void Literal::show() const
{
}



bool Literal::equal_on_eval(const Literal *other, const vi &eval) const
{
    if (!Literal::equal_literals(this, other)) return false;
    int cur_var = -1;
    assert(amount_vars == other->amount_vars);
    forn(i, amount_vars)
    {
        cur_var = other->vars[i];
        if (cur_var >= eval.size() || vars[i] != eval[cur_var])
        {
            return false;
        }
    }
    return true;
}

ostream &operator<<(ostream &os, Literal &literal)
{
    os << "sign: " << literal.sign
              << "  id: " << literal.predicat_id
              << "  amount_vars: " << literal.amount_vars
              << "  vars:";

    forn(i, literal.amount_vars)
        os << " " << literal.vars[i];

    os << std::endl;
    return os;
}
























