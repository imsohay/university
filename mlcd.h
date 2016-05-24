#ifndef MLCD_H
#define MLCD_H


#include "calcformula.h"
#include <forward_list>

class Element;
typedef std::forward_list<Element> level;



class Element
{
    friend class MLCD;
public:

    Element() {}
    Element(const Element& other);
    Element(Formula_wrapper _data)
        :data(_data) {}

    Element(Formula_wrapper _data, Element* _parent_left, Element* _parent_right);
    Element(Formula_wrapper _data, Element *_parent_left, Element *_parent_right, Variable_substitution* _left, Variable_substitution* _right);
    Element(Formula_wrapper _data, std::vector<Element*> _parents, std::vector<Variable_substitution*> _substitutions)
        :data(_data), parents(_parents), substitutions(_substitutions) {}

    Element & operator=(const Element& other);
    ~Element() { clear(); }


private:
    Formula_wrapper data;
    std::vector<Element*> parents;
    std::vector<Variable_substitution*> substitutions;

    void add_parent(Element* parent, Variable_substitution* subst);
    bool operator ==(const Element& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, Element& el);
    void clear();
};



/********          Multi-level class description    ********/


class MLCD
{
public:
    MLCD(const std::forward_list<Formula_wrapper > &descriptions);

    friend std::ostream& operator<<(std::ostream& os, MLCD& mlcd);
    Formula_wrapper inference(Formula_wrapper& fw, Variable_substitution *result_subst);
    void debug_info(std::ostream &out);
    void test();
    void test_inference();
    inline void clear() { data.clear(); }
    void rebuild(Formula_wrapper& fw);

private:

    std::vector<level> data;
    int remove_duplicates(level &l);
    void remove_empties(level& l);
    Element *inference_search(Element* current, Formula_wrapper& goal, Variable_substitution *partial_subst);
    bool is_true(Element* current, Formula_wrapper& goal, Variable_substitution* partial_subst);
    void make_new_element(Element &el1, Element &el2, Element& result);
    void test(Element &el);
    bool test_correct_subst(Formula_wrapper &from, Formula_wrapper &to, Variable_substitution *s);
};

#endif // MLCD_H
