#include "formula_wrapper.h"
#include <map>
#include "calcformula.h"
#include <cmath>



using namespace std;


Variable_properties::Variable_properties(vector<int> dimensions)
{
    for (auto d : dimensions)
    {
        data.push_back(vector<int>(d, 0));
    }
}


void Variable_properties::increment_value(int literal_num, int position_num)
{
    data[literal_num][position_num]++;
}


bool Variable_properties::operator ==(const Variable_properties &other) const
{
    if (data.size() != other.data.size())
    {
        return false;
    }

    forn(i, data.size())
    {
        if (data[i].size() != other.data[i].size())
        {
            return false;
        }
    }

    forn(i, data.size())
    {
        forn(j, data[i].size())
        {
            if (data[i][j] != other.data[i][j])
                return false;
        }
    }

    return true;
}


ull my_pow(ull x, ull p)
{
  if (p == 0) return 1;
  if (p == 1) return x;

  ull tmp = my_pow(x, p / 2);
  if (p % 2 == 0)
      return tmp * tmp;
  else
      return x * tmp * tmp;
}


ostream &operator<<(ostream &os, Variable_properties &vp)
{
    for(auto v : vp.data)
    {
        for (int x : v)
        {
            os << x;
        }
        os << endl;
    }
    return os;
}


Formula* Formula_wrapper::build_formula(const Literal* pl)
{
    Formula *pf, **temp;
    if (pl->sign)
    {
        pf = new Formula(NOT, NULL, NULL, NULL);
        temp = &pf->l;
    }
    else
    {
        temp = &pf;
    }

    Term* terms = new Term[pl->amount_vars];
    forn(j, pl->amount_vars)
    {
        terms[j] = Term(VAR, pl->vars[j], 0, NULL);
    }

    (*temp) = new Formula(NONE, NULL, NULL, new Atomic_formula(PRED, pl->predicat_id,
            pl->amount_vars, terms));

    return pf;
}


//   FORMULA IN CNF
Formula_wrapper::Formula_wrapper(std::vector<const Literal*> vl)
{
    if (vl.size() == 1)
    {
        f = build_formula(vl[0]);
        return;
    }

    f = new Formula();
    Formula* current = f;
    forn(i, vl.size())
    {
        current->_label = AND;
        current->l = build_formula(vl[i]);
        if (i == vl.size() - 2)
        {
            current->r = build_formula(vl[i + 1]);
            break;
        }
        current->r = new Formula();
        current = current->r;
    }
}


vector<const Literal* >* Formula_wrapper::get_literals() const
{
    vcl* p_v = new vcl();
    if (!empty())
        create_literals_list(f, p_v);

    return p_v;
}

std::map<int, int> Formula_wrapper::create_id_substitution() const
{
    map<int,int> subst;
    std::set<int> s;
    vcl* v = new vcl();
    create_literals_list(f, v);
    for(const Literal* lit : *v)
    {
        forn(i, lit->amount_vars)
        {
            s.insert(lit->vars[i]);
        }
    }

    for(auto x : s)
        subst.insert(std::pair<int,int>(x,x));

    clear_vcl(*v);
    check_and_clear(v);
    return subst;
}


bool Formula_wrapper::operator ==(const Formula_wrapper &other) const
{
    return equal(other);
}

bool Formula_wrapper::operator !=(const Formula_wrapper &other) const
{
    return !(equal(other));
}


Formula_wrapper::Formula_wrapper(const Formula_wrapper &other)
{
    *this = other;
}


Formula_wrapper &Formula_wrapper::operator=(const Formula_wrapper &other)
{
    if (this != &other)
    {
        if (other.f != NULL)
        {
            f = new Formula;
            *f = *(other.f);
        }
        else
        {
            f = NULL;
        }
    }
    return *this;
}

ostream &operator<<(ostream &os, Formula_wrapper &fw)
{
    if (!fw.empty())
        os << *fw.f;
    else
        os << "<empty>";
    return os;
}

//    FORMULA MUST BE IN CNF
void Formula_wrapper::create_literals_list(const Formula* p_f, vcl *v) const
{
    assert(p_f);

    switch (p_f->_label)
    {
    case NOT:
        v->push_back(create_literal(p_f->l->m, true));
        break;
    case NONE:
        v->push_back(create_literal(p_f->m, false));
        break;
    case AND:
        create_literals_list(p_f->l, v);
        create_literals_list(p_f->r, v);
        break;
    default:
        assert(0);
    }
}


// AGRUMENTS MUST BE ARE DISTINCT
Literal* Formula_wrapper::create_literal(Atomic_formula* p_af, bool sign) const
{
    assert(p_af);
    vui v;
    forn(i, p_af->amount_sub_terms)
        get_id_vars(&(p_af->sub_terms[i]), v);

    Literal* l = new Literal();
    l->sign = sign;
    l->predicat_id = p_af->id;
    l->amount_vars = v.size();
    l->vars = new ui[l->amount_vars];
    ui i = 0;
    for(ui var_id : v)
    {
        l->vars[i] = var_id;
        i++;
    }

    v.clear();
    return l;
}


void Formula_wrapper::get_id_vars(const Term* const term, vui& v) const
{
    switch(term->type)
    {
    case VAR:
        v.push_back(term->id);
        break;
    case FUNC:
        forn(i, term->amount_sub_terms)
            get_id_vars(&(term->sub_terms[i]), v);
        break;
    case CONST:
        break;
    case NOT_DEF:
        assert(0);
    }
}


size_t Formula_wrapper::just_calc_amount_vars(vcl * literals) const
{
    set<ui> set;
    for(auto l : *literals)
    {
        forn(i, l->amount_vars)
        {
            set.insert(l->vars[i]);
        }
    }

    return set.size();
}


bool Formula_wrapper::equal(const Formula_wrapper& other) const
{
    vcl * literals_1 = get_literals();
    vcl * literals_2 = other.get_literals();
    if (literals_1->size() != literals_2->size())
    {
        clear_vcl(*literals_1);
        clear_vcl(*literals_2);
        check_and_clear(literals_1);
        check_and_clear(literals_2);
        return false;
    }

    Variable_substitution s(this, &other);
    auto res = Calculator_formula::max_common_subformula(*this, other, ALGORITHM_VERS::SECOND, &s);
    vcl * res_literals = res->get_literals();
    bool eq = (res_literals->size() == literals_1->size());
    clear_vcl(*literals_1);
    clear_vcl(*literals_2);
    clear_vcl(*res_literals);
    check_and_clear(literals_1);
    check_and_clear(literals_2);
    check_and_clear(res_literals);
    check_and_clear(res);

    return eq;
}

bool Formula_wrapper::beta_equal(const Formula_wrapper& other) const
{
    vector<Variable_properties> vprop_1, vprop_2;
    analyse_variable_properties(vprop_1);
    other.analyse_variable_properties(vprop_2);

    if (vprop_1.size() != vprop_2.size())
    {
        return false;
    }

    set<int> found;
    bool flag = false;
    forn(i, vprop_1.size())
    {
        flag = false;
        forn(j, vprop_2.size())
        {
            if (vprop_1[i] == vprop_2[j] && found.find(j) == found.end())
            {
                found.insert(j);
                flag = true;
                break;
            }
        }

        if (!flag)
            return false;
    }

    return true;
}


void Formula_wrapper::calc_literals_dimensions(vcl* literals, map<int, int> &v) const
{
    for (auto lit : *literals)
    {
        if (v.find(lit->predicat_id) == v.end())
        {
            v.insert(std::pair<int,int>(lit->predicat_id, lit->amount_vars));
        }
    }
}


void Formula_wrapper::count_variables(vcl *literals, std::map<int, int> &mapping) const
{
    int count = 0;
    for(const Literal* lit : *literals)
    {
        forn(i, lit->amount_vars)
        {
            if (mapping.find(lit->vars[i]) == mapping.end())
            {
                mapping.insert(std::pair<int,int>(lit->vars[i], count));
                count++;
            }
        }
    }
}


void Formula_wrapper::analyse_variable_properties(std::vector<Variable_properties> &v) const
{
    vcl *literals = get_literals();
    map<int,int> dimensions, variables_mapping;
    vector<int> v_dimensions;
    int i = 0;
    int amount_vars = just_calc_amount_vars(literals);
    calc_literals_dimensions(literals, dimensions);
    count_variables(literals, variables_mapping);
    for(auto el : dimensions)
    {
        v_dimensions.push_back(el.second);
    }
    map<int,int> literals_id_to_id;
    for(const auto& pair : dimensions)
    {
        literals_id_to_id.insert(std::pair<int,int>(pair.first, i));
        i++;
    }

    forn(i, amount_vars)
        v.push_back(Variable_properties(v_dimensions));

    int variable_id = -1;
    forn(i, literals->size())
    {
        const Literal* l = literals->at(i);
        forn(j, l->amount_vars)
        {
            variable_id = variables_mapping[l->vars[j]];
            assert(variable_id >= 0 && variable_id < v.size());
            v[variable_id].increment_value(literals_id_to_id[l->predicat_id], j);
        }
    }

    clear_vcl(*literals);
    check_and_clear(literals);
}








