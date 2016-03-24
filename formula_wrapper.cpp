#include "formula_wrapper.h"

using namespace std;



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
    set<const Literal*>* p_s = new set<const Literal*>();
    create_set_literal(f, p_s);
    vector<const Literal* >* p_v = new vector<const Literal*> (p_s->begin(), p_s->end());

    check_and_clear(p_s);
    return p_v;
}

ostream &operator<<(ostream &os, Formula_wrapper &fw)
{
    os << *fw.f;
    return os;
}

//    FORMULA MUST BE IN CNF
void Formula_wrapper::create_set_literal(const Formula* p_f, set<const Literal* > *s) const
{
    assert(p_f);

    switch (p_f->_label)
    {
    case NOT:
        s->insert(create_literal(p_f->l->m, true));
        break;
    case NONE:
        s->insert(create_literal(p_f->m, false));
        break;
    case AND:
        create_set_literal(p_f->l, s);
        create_set_literal(p_f->r, s);
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








