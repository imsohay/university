#include "variable_substitution.h"
#include <string.h>
#include "calcformula.h"

using namespace std;


Variable_substitution::Variable_substitution(const Variable_substitution &other)
{
    *this = other;
}

void Variable_substitution::swap_formulas()
{
    std::swap(from, to);
}

void Variable_substitution::set_identical_substitution(std::size_t size)
{
    value.clear();
    forn(i, size)
    {
        value.insert(std::pair<int,int>(i, i));
    }
}


//   remove transition kind of "x -> -1"
void Variable_substitution::flush()
{
    map<int,int> copy;
    for(auto el : value)
    {
        if (el.second != -1) copy.insert(pair<int,int>(el.first, el.second));
    }
    value = copy;
}

void Variable_substitution::initialize_value(const std::map<int, int> s)
{
    value.clear();
    value = s;
}

Variable_substitution *Variable_substitution::compose(Variable_substitution *other) const
{
    Variable_substitution copy = *this;

    Variable_substitution* result = new Variable_substitution(this->from, other->to);
    for(auto el : other->value)
    {
        result->insert(el.first, (el.second == -1 ? -1 : this->at(el.second)));
    }

    return result;
}


void Variable_substitution::apply_to(Formula_wrapper &fw) const
{
    map<int,int> id = fw.create_id_substitution();
    map<int,int> new_subst = value;
    new_subst = Calculator_formula::complete_substitution(new_subst, id);


    vcl* literals = fw.get_literals();
    for(const Literal * lit : *literals)
    {

        forn(j, lit->amount_vars)
        {
            lit->vars[j] = new_subst.at(lit->vars[j]);
        }
    }
    fw.~Formula_wrapper();
    fw = *literals;
    clear_vcl(*literals);
    check_and_clear(literals);
}

void Variable_substitution::reverse_value()
{
    map<int,int> temp;
    for(auto el : value)
        temp[el.second] = el.first;
    value.clear();
    value = temp;
}


Variable_substitution &Variable_substitution::operator=(const Variable_substitution &other)
{
    if (this != &other)
    {
        from = other.from;
        to = other.to;
        for(auto el : other.value)
            value.insert(std::pair<int,int>(el.first, el.second));
    }
    return *this;
}

std::ostream &operator<<(std::ostream &os, Variable_substitution &subst)
{
    std::string s = "";
    char buf[100] = { 0 };
    for(const auto& el : subst.getValue())
    {
        snprintf(buf, sizeof(buf), "%3i ", el.first);
        s += buf;
    }
    s += "\n";
    for(const auto& el : subst.getValue())
    {
        snprintf(buf, sizeof(buf), "%3i ", el.second);
        s += buf;
    }
    s += "\n";
    os << s;
    return os;
}

