#ifndef VARIABLE_SUBSTITUTION_H
#define VARIABLE_SUBSTITUTION_H

#include "formula_wrapper.h"


class Variable_substitution
{
public:

    Variable_substitution(const Formula_wrapper * _from, const Formula_wrapper * _to)
        :from(_from), to(_to) {}

    Variable_substitution(const Variable_substitution& other);

    inline int& operator[] (int key) { return value[key]; }
    inline std::size_t size() const { return value.size(); }
    inline bool empty() { return value.empty(); }
    inline void insert(int key, int val) { value.insert(std::pair<int,int>(key,val)); }
    inline void clear() { value.clear(); }
    inline int at(int key) const { return value.at(key); }

    const std::map<int,int>& getValue() const { return value; }
    const Formula_wrapper* getFrom() const { return from; }
    const Formula_wrapper* getTo() const { return to; }
    void swap_formulas();
    void set_identical_substitution(std::size_t size);
    void flush();
    void initialize_value(std::map<int,int> s);
    Variable_substitution* compose(Variable_substitution *other) const;
    void apply_to(Formula_wrapper& fw) const;

    friend std::ostream& operator<<(std::ostream& os, Variable_substitution& subst);
    Variable_substitution & operator=(const Variable_substitution& other);

private:
    std::map<int,int> value;
    const Formula_wrapper* from;
    const Formula_wrapper* to;
};

#endif // VARIABLE_SUBSTITUTION_H

