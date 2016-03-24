#include "literal.h"
#include <bitset>
using namespace std;

Literal::Literal(const Literal &rhs)
{
    sign = rhs.sign;
    predicat_id = rhs.predicat_id;
    amount_vars = rhs.amount_vars;
    vars = new ui[amount_vars];
    forn(i, amount_vars) vars[i] = rhs.vars[i];
}

bool Literal::operator ==(const Literal &other) const
{
    if (!(predicat_id == other.predicat_id && sign == other.sign)) return false;
    forn(i, other.amount_vars)
            if (vars[i] != other.vars[i]) return false;

    return true;
}



void Literal::show() const
{
}

ui Literal::hash() const
{
    ui res = 0;
    ui bit_per_var = 10 / amount_vars;
    if (!bit_per_var) bit_per_var++;
    ui mod = 0;
    ui mods[] = { 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023 };
    mod = mods[bit_per_var - 1];
    forn(i, amount_vars)
    {
        res <<= bit_per_var;
        res += vars[i] & mod;
    }
    res <<= 3;
    res += amount_vars & 7;
    res <<= 1;
    res += sign & 1;
    return res;
}

ui Literal::hash(vui* eval) const
{
    ui res = 0;
    ui bit_per_var = 10 / amount_vars;
    if (!bit_per_var) bit_per_var++;
    ui mod = 0;
    ui mods[] = { 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023 };
    mod = mods[bit_per_var - 1];
    forn(i, amount_vars)
    {
        res <<= bit_per_var;
        res += eval->at(vars[i]) & mod;
    }
    res <<= 3;
    res += amount_vars & 7;
    res <<= 1;
    res += sign & 1;
    return res;
}

bool Literal::equal_on_eval(const Literal *other, const vi &eval) const
{
    if (!Literal::equal_literals(this, other)) return false;

    forn(i, amount_vars)
    {
		//cout << amount_vars << "  " << other->amount_vars << endl;
        assert(amount_vars == other->amount_vars);
		//cout << other->vars[i] << " eval: " << eval.size() << endl;
        if (other->vars[i] >= eval.size() || vars[i] != eval[other->vars[i]]) return false;
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

























