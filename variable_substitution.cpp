#include "variable_substitution.h"
#include <string.h>

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

void Variable_substitution::initialize_value(std::map<int, int> s)
{
    value.clear();
    value = s;
}

Variable_substitution *Variable_substitution::compose(Variable_substitution *other) const
{
    Variable_substitution copy = *this;
//    cerr << "\n\nbefore(this,other):\n";
//    cerr << copy << std::endl;
//    cerr << (*other) << std::endl;
    //assert(this->size() == other->size());
//    cerr << *from << endl;
//    cerr << *to << endl;
//    cerr << *other->from << endl;
//    cerr << *other->to << endl;

    Variable_substitution* result = new Variable_substitution(this->from, other->to);
    for(auto el : other->value)
    {
        result->insert(el.first, (el.second == -1 ? -1 : this->at(el.second)));
    }

//    cerr << "result:\n";
//    cerr << (*result) << endl;
    return result;
}


void Variable_substitution::apply_to(Formula_wrapper &fw) const
{
    map<int,int> id = fw.create_id_substitution();
    map<int,int> new_subst = value;
    set<int> _set;
    for(auto el : new_subst)
    {
        _set.insert(el.second);
    }
    int i = 0;
    for(auto el : id)
    {
        try
        {
            new_subst.at(el.first);
        }
        catch(std::out_of_range e)
        {
            while(true)
            {
                if (_set.find(i) == _set.end())
                    break;

                i++;
            }
            new_subst[el.first] = i;
            _set.insert(i);
            i++;
        }
    }

    vcl* literals = fw.get_literals();
    for(const Literal * lit : *literals)
    {

        forn(j, lit->amount_vars)
        {
            lit->vars[j] = new_subst[lit->vars[j]];
        }
    }
    fw = *literals;
    clear_vcl(*literals);
    check_and_clear(literals);
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
//    os << "from: " << *subst.from << std::endl << "to: " << *subst.to << std::endl;
    return os;
}

