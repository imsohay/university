#include "interpretation.h"

using namespace std;

std::vector<int *> Interpretation::satisfy()
{
    vector<int* > v;
    while(next_evaluation())
    {
        if (satisfy(f))
        {
            int* a = new int[amount_vars];
            forn(i, amount_vars) a[i] = evaluation[i];
            v.push_back(a);
        }
    }

    return v;
}




bool Interpretation::satisfy(Formula* f)
{
    switch(f->_label)
    {
    case NONE:
        return satisfy(f->m);
        break;
    case NOT:
        return !satisfy(f->l);
        break;
    case AND:
        return satisfy(f->l) && satisfy(f->r);
        break;
    case OR:
        return satisfy(f->l) || satisfy(f->r);
        break;
    case IMP:
        return !satisfy(f->l) && satisfy(f->r);
        break;
    default:
        assert(NULL);
    }
}

bool Interpretation::satisfy(Atomic_formula *f)
{
    bool res = 0;
    Term* terms;

    switch(f->type)
    {
    case F:
        res = false;
        break;
    case T:
        res = true;
        break;
    case PROP_VAR:
        res = ps[f->id](NULL);
        break;
    case PRED:
//        int args[f->amount_sub_terms];
//        terms = f->sub_terms;
//        for(ui i = 0; i < f->amount_sub_terms; i++)
//        {
//            args[i] = satisfy(&terms[i]);
//        }

//        res = ps[f->id](args);
        break;

    }

    return res;
}

int Interpretation::satisfy(Term *t)
{
    int res = 0;
    Term* terms;

    switch(t->type)
    {
    case CONST:
        res = fs[t->id](NULL);
        break;
    case VAR:
        res = domain[evaluation[t->id]];
        break;
    case FUNC:
        //int args[t->amount_sub_terms];
        //terms = t->sub_terms;
        //for(ui i = 0; i < t->amount_sub_terms; i++)
        //{
        //    args[i] = satisfy(&terms[i]);
        //}

        //res = fs[t->id](args);
//        break;
    default:
        cerr << "Not correct label in formula!\n";
        exit(EXIT_FAILURE);
//        throw exception("Not correct label in formula!\n");
    }

    return res;
}

inline int Interpretation::next_element(int i)
{
    return domain[i + 1];
}

bool Interpretation::next_evaluation()
{
    if (evaluation == NULL)
    {
        evaluation = new int[amount_vars];
        forn(i, amount_vars) evaluation[i] = 0;
        return true;
    }

    bool flag = false;
    forn(i, amount_vars)
    {
        if (evaluation[i] < domain_size - 1)
        {
            flag = true;
            break;
        }
    }

    if (!flag) return false;

    for(int i = amount_vars - 1; i >= 0; i--)
    {
        if (evaluation[i] == domain_size - 1)
        {
            evaluation[i] = 0;
        }
        else
        {
            evaluation[i]++;
            break;
        }
    }

    return true;
}


