#ifndef INTERPRETATION_H
#define INTERPRETATION_H

#include "formula.h"

class Interpretation
{
public:
    Interpretation(Formula* _f,
                func_symb* _fs,
                pred_symb* _ps,
                int _domain_size,
                int* _domain,
                int _amount_vars)
        :f(_f), fs(_fs), ps(_ps), domain_size(_domain_size), domain(_domain), amount_vars(_amount_vars), evaluation(NULL) {}
    std::vector<int* > satisfy();

private:
    Formula* f;
    func_symb* fs;
    pred_symb* ps;
    int domain_size;
    int* domain;
    int amount_vars;
    int* evaluation;

    bool satisfy(Formula* f);
    bool satisfy(Atomic_formula* f);
    int satisfy(Term* t);

    bool next_evaluation();
    int next_element(int i);
};

#endif // INTERPRETATION_H
