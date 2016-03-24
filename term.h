#ifndef TERM_H
#define TERM_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <assert.h>
#include <vector>

#define forn(i,n) for(size_t i = 0; i < n; i++)
#define check_and_clear(ptr) { if (ptr != NULL) { delete ptr; ptr = NULL; } }
#define check_and_clear_array(ptr) { if (ptr != NULL) { delete[] ptr; ptr = NULL; } }


typedef int (*func_symb)(int*);
typedef bool (*pred_symb)(int*);
typedef std::vector<unsigned int> vui;
typedef std::vector<vui > vvui;
typedef std::vector<int> vi;


enum type_term { CONST, VAR, FUNC, NOT_DEF };
typedef unsigned int ui;

class Term
{
public:
    friend class Formula_wrapper;
    friend class Interpretation;

    Term()
        : type(NOT_DEF), id(-1), amount_sub_terms(-1), sub_terms(NULL) {}

    Term(type_term _type, ui _id, ui _amount_sub_terms, Term* _sub_terms)
        :type(_type), id(_id), amount_sub_terms(_amount_sub_terms), sub_terms(_sub_terms) {}
    ~Term();


    type_term get_type() const { return type; }
    ui get_id() const { return id; }

    friend std::ostream& operator<<(std::ostream& os, Term& term);

private:
    type_term type;
    ui id;
    ui amount_sub_terms;
    Term* sub_terms;
};

#endif // TERM_H
