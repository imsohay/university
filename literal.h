#ifndef LITERAL_H
#define LITERAL_H

#include "term.h"



class Literal
{
    friend class CalculatorFormula;
    friend class Hash_set_literal;

public:
    Literal()
        :sign(0), predicat_id(-1), amount_vars(-1), vars(NULL) {}

    Literal(bool _sign, ui _predicat_id, ui _amount_vars, ui* _vars)
        :sign(_sign),
         predicat_id(_predicat_id),
         amount_vars(_amount_vars),
         vars(_vars) {}

    Literal(const Literal& rhs);

    bool operator ==(const Literal& other) const;
    bool operator !=(const Literal& other) const { return !operator ==(other); }
    ~Literal() { check_and_clear_array(vars); }
    void show() const;
    ui hash() const;
    ui hash(vui *eval) const;

    inline static bool equal_literals(const Literal* l1, const Literal* l2)
    { return l1->sign == l2->sign && l1->predicat_id == l2->predicat_id; }
    bool equal_on_eval(const Literal* other, const vi& eval) const;

    friend std::ostream& operator<<(std::ostream& os, Literal& literal);

//private:
    bool sign;
    ui predicat_id;
    ui amount_vars;
    ui* vars;

};


#endif // LITERAL_H
