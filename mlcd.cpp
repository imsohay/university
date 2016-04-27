#include "mlcd.h"
#include <algorithm>

using namespace std;


Element::Element(const Element& other)
{
    *this = other;
}



Element::Element(Formula_wrapper _data, Element *_parent_left, Element *_parent_right)
{
    data = _data;
    parents.push_back(_parent_left);
    parents.push_back(_parent_right);
}


ostream &operator<<(ostream &os, Element &el)
{
    os << el.data << endl;
    for(auto x : el.parents)
    {
        os << "parent: " << x->data << endl;
    }

    for(auto x : el.substitutions)
    {
        os << "subst:\n" << *x << endl;
    }

    return os;
}


bool Element::operator ==(const Element& rhs) const
{
    return (this->data == rhs.data);
}


void Element::add_parent(Element* parent, Variable_substitution* subst)
{
    parents.push_back(parent);
    substitutions.push_back(subst);
}



Element::Element(Formula_wrapper _data, Element *_parent_left, Element *_parent_right, Variable_substitution* _left, Variable_substitution* _right)
{
    data = _data;
    parents.push_back(_parent_left);
    parents.push_back(_parent_right);
    substitutions.push_back(_left);
    substitutions.push_back(_right);
}



void Element::clear()
{
    for(Variable_substitution* x : substitutions)
    {
        check_and_clear(x);
    }
}


Element& Element::operator=(const Element& other)
{
    if (this != &other)
    {
        data = other.data;
        for(auto el : other.parents)
            parents.push_back(el);
        for(Variable_substitution* x : other.substitutions)
        {
            substitutions.push_back(new Variable_substitution(*x));
        }
    }

    return *this;
}




MLCD::MLCD(const std::forward_list<Formula_wrapper> &descriptions)
{
    level temp;
    int current_count = -1;
    level * p_last;

    for(const Formula_wrapper& fw : descriptions)
    {
         temp.push_front(Element(fw));
    }
    data.push_back(temp);

    temp.clear();
    do
    {

        p_last = &(data[data.size() - 1]);
        int i = 0, j = 0;
        for(auto& el1 : *p_last)
        {
            j = 0;
            for(auto& el2 : *p_last)
            {
                if (i < j)
                {
                    Element new_el;
                    make_new_element(el1, el2, new_el);
                    temp.push_front(new_el);

                }
                j++;
            }
            i++;
        }

        remove_empties(temp);
        current_count = remove_duplicates(temp);
        data.push_back(temp);
//        cout << "count: " << current_count << endl;

        temp.clear();
    }
    while (current_count > 1);
}


void MLCD::make_new_element(Element& el1, Element& el2, Element &result)
{
    Variable_substitution* max_subst = new Variable_substitution(&(el1.data), &(el2.data));
    Formula_wrapper* ptr_fw = Calculator_formula::max_common_subformula(
                       el1.data, el2.data, SECOND, max_subst);

    Variable_substitution* id_subst = new Variable_substitution(&(el1.data), &(el2.data));

    id_subst->initialize_value(el2.data.create_id_substitution());
    result = Element(*ptr_fw, &el1, &el2, max_subst, id_subst);

    if (!test_correct_subst(result.parents[0]->data, result.data, result.substitutions[0]) ||
        !test_correct_subst(result.parents[1]->data, result.data, result.substitutions[1]))
    {
        __throw_logic_error("not correct subst in new element");
    }
    delete ptr_fw;
}


Formula_wrapper MLCD::inference(Formula_wrapper &fw, Variable_substitution* result_subst)
{
    level l = data[data.size() - 1];
    Element* p_res = NULL;
    for(Element el : l)
    {
        Variable_substitution partial_subst(&el.data, &fw);
        p_res = inference_search(&el, fw, &partial_subst);
        if (p_res)
        {
            return p_res->data;
        }
        p_res = NULL;
        if (result_subst)
            result_subst->clear();
    }

//    rebuild(fw);
    return Formula_wrapper();
}



//bool check_swapness(const Formula_wrapper& f1, const Formula_wrapper& f2)
//{
//    auto v1 = f1.get_literals();
//    auto v2 = f2.get_literals();
//    bool res = (v1->size() < v2->size());
//    clear_vcl(*v1);
//    clear_vcl(*v2);
//    check_and_clear(v1);
//    check_and_clear(v2);
//    return res;
//}


Element* MLCD::inference_search(Element* current, Formula_wrapper& goal, Variable_substitution* partial_subst)
{
    Element* result = NULL;
    Variable_substitution copy_subst(&current->data, &goal);
    copy_subst.initialize_value(partial_subst->getValue());
    if (is_true(current, goal, &copy_subst))
    {
        copy_subst.apply_to(goal);

        if (current->parents.empty())
            return current;

        forn(i, current->parents.size())
        {
            Variable_substitution copy(*current->substitutions[i]);
            result = inference_search(current->parents[i], goal, &copy);
            if (result)
            {
                return result;
            }
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}


bool MLCD::is_true(Element *current, Formula_wrapper &goal, Variable_substitution*partial_subst)
{
    auto res = Calculator_formula::max_common_subformula(current->data, goal, SECOND, partial_subst);
    bool eq = (*res == current->data);
    check_and_clear(res);
    return eq;
}



void MLCD::debug_info(ostream& out)
{

    for(level& l : data)
    {
        out << "##################################################################\n";
        out << "\nlevel:\n\n";
        out << "##################################################################\n";
        for(Element& x : l)
        {
            assert(x.parents.size() == x.substitutions.size());
            out << x.data << endl;

            forn(i, x.parents.size())
            {
                out << "parent_" << i << "  " << (x.parents[i]->data) << endl;
                out << "subst_" << i << ":\n" << *(x.substitutions[i]) << "\n\n";
            }
            out << "\n****************************\n";
        }
    }
}


void MLCD::test()
{
    for(auto& el : data[data.size() - 1])
    {
        test(el);
    }

}


vi extract_values_from_map(map<int,int>& s)
{
    set<int> _set;
    for(auto el : s) _set.insert(el.second);
    return vi(_set.begin(), _set.end());
}


void set_values_to_map(map<int,int>& s, vi& v)
{
    int i = 0;
    for(auto& el : s)
    {
        s[el.first] = v[i];
        i++;
    }
}


void MLCD::test_inference()
{
    int i = 0;
    for(auto& el : data.at(0))
    {
        Formula_wrapper f = el.data;
        Variable_substitution var_subst(&f, &f);
        map<int,int> subst = f.create_id_substitution();
        vi current_v = extract_values_from_map(subst);
        std::sort(current_v.begin(), current_v.end());

        i = 0;
        do
        {
//            if (i % 20 != 0)
//            {
//                i++;
//                continue;
//            }

            Formula_wrapper copy = f;
            set_values_to_map(subst, current_v);
            var_subst.initialize_value(subst);
            var_subst.apply_to(copy);
//            cerr << copy << endl;
            Formula_wrapper res = inference(copy, NULL);
            if (res.empty())
            {
                bool flag = false;
                int i = 0, j = 0;
                for(auto& el1 : data.at(0))
                {
                    for(auto& el2 : data.at(0))
                    {
                        if (i != j && el1.data == el2.data)
                        {
                            flag = true;
                            break;
                        }
                        j++;
                    }
                    i++;

                    if (flag)
                        break;
                }

                if (!flag)
                {
                    cerr << *this << endl;
                    cerr << copy << endl;
                    __throw_runtime_error("not correct inference");
                }
            }

            res.~Formula_wrapper();
            copy.~Formula_wrapper();
            i++;
        } while (std::next_permutation(current_v.begin(), current_v.end()));
        f.~Formula_wrapper();
    }
}


void MLCD::test(Element& el)
{
    assert(el.parents.size() == el.substitutions.size());
    forn(i, el.parents.size())
    {
        if (!test_correct_subst(el.parents[i]->data, el.data, el.substitutions[i]))
        {
            cerr << "FUUUCK UP!!!\n";
            cerr << "from: " << el.parents[i]->data << endl
                 << "to: " << el.data << endl
                 << *el.substitutions[i] << "\n\n";

            debug_info(cerr);

            __throw_logic_error("not correct subst");
        }
    }

    for(Element* x : el.parents)
    {
        test(*x);
    }
}


bool MLCD::test_correct_subst(Formula_wrapper& from, Formula_wrapper& to, Variable_substitution* s)
{
    vcl* v_from = from.get_literals();
    vcl* v_to = to.get_literals();
    bool flag = false;

    for(const Literal* lit : *v_to)
    {
        forn(i, lit->amount_vars)
        {
            lit->vars[i] = s->at(lit->vars[i]);
        }

        flag = false;
        for(const Literal* lit_from : *v_from)
        {
            if (*lit_from == *lit)
            {
                flag = true;
                break;
            }
        }

        if (!flag)
        {
            return false;
        }
    }

    clear_vcl(*v_from);
    clear_vcl(*v_to);
    check_and_clear(v_from);
    check_and_clear(v_to);
    return true;
}


std::ostream & operator<<(std::ostream &os, MLCD &mlcd)
{
    for(int i = mlcd.data.size() - 1; i >= 0; i--)
    {
        os << (mlcd.data.size() - i) << " level:\n";
        for(auto& el : mlcd.data[i])
        {
            os << el << std::endl;
        }
    }

    return os;
}


bool equal(const Formula_wrapper& f1, const Formula_wrapper& f2)
{
    vcl* literals_1 = f1.get_literals();
    vcl* literals_2 = f2.get_literals();
    bool eq = (literals_1->size() == literals_2->size());
    clear_vcl(*literals_1);
    clear_vcl(*literals_2);
    check_and_clear(literals_1);
    check_and_clear(literals_2);

    return eq;
}


int MLCD::remove_duplicates(level& l)
{
    int count = 0;
    level copy;
    while(!l.empty())
    {
        Element template_el = l.front();
        l.pop_front();

        l.remove_if([&template_el](Element& x){
            bool exist;
            int index;

            if (!equal(x.data, template_el.data))
            {
                return false;
            }

            Variable_substitution* subst = new Variable_substitution(&(x.data), &(template_el.data));
            Formula_wrapper* p_f = Calculator_formula::max_common_subformula(x.data,
                                                                             template_el.data,
                                                                             ALGORITHM_VERS::SECOND, subst);
            bool eq = equal(*p_f, *subst->getTo());
            check_and_clear(p_f);
            if (eq)
            {
                index = 0;
                for(Element* parent : x.parents)
                {
                    exist = false;
                    for(Element* existing_parent : template_el.parents)
                    {
                        if ((*existing_parent)==(*parent))
                        {
                            exist = true;
                            break;
                        }
                    }
                    if (!exist)
                    {
                        Variable_substitution* temp = (x.substitutions[index])->compose(subst);
                        template_el.add_parent(parent, temp);

//                        cerr << "\n############################\n";
//                        cerr << *p_f << "\n\n";
//                        cerr << "subst for: " << template_el.data << endl;
//                        cerr << "subst into:" << x.data << endl;
//                        cerr << "subst:\n" << *subst << endl;
//                        cerr << "\nsubst for: " << x.data << endl;
//                        cerr << "subst into: " << x.parents[index]->data << endl;
//                        cerr << "subst:\n" << *x.substitutions[index] << endl;
//                        cerr << "\nresult of composing:\n" << *temp << endl;
//                        cerr << "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
                    }

                    index++;
                }
            }
            check_and_clear(subst);
            return eq;
        });
        copy.push_front(Element(template_el));
        count++;
    }

    for(auto element : l)
        element.clear();
    l.clear();

    l = copy;
    return count;
}


void MLCD::remove_empties(level& l)
{
    l.remove_if([](Element& x){ return x.data.empty(); });
}


int size_of_list(forward_list<Element> l)
{
    int i = 0;
    for(auto& _ : l) i++;
    return i;
}

void MLCD::rebuild(Formula_wrapper &fw)
{
    int index_level = 0;
    level* current_level = &data.at(index_level);
    forward_list<Element> list, list_for_next_level;
    list.push_front(Element(fw));

    while (true)
    {
        cerr << index_level << " " << size_of_list(list) << "\n";
        cerr << "check\n";

        for(auto& old_el : *current_level)
        {
            for(auto& new_el : list)
            {
                Element el_for_next;
                make_new_element(old_el, new_el, el_for_next);
                list_for_next_level.push_front(el_for_next);
            }
        }

        for(auto& el : list) current_level->push_front(el);
        list.clear();

        remove_empties(list_for_next_level);
        remove_duplicates(list_for_next_level);
        if (list_for_next_level.empty())
            break;

        index_level++;
        if (index_level < data.size())
        {
            current_level = &data.at(index_level);
            list_for_next_level.remove_if([current_level](Element& x)
                {
                    for(auto& el : *current_level)
                        if (el.data == x.data)
                            return true;

                    return false;
                });

            if (list_for_next_level.empty())
                break;
        }
        else
        {
            data.push_back(forward_list<Element>());
            current_level = &data.at(index_level);
        }

        list = list_for_next_level;
        list_for_next_level.clear();
    }

}







