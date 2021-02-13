#ifndef NODE_H
#define NODE_H

#include "stdafx.h"
#include "Event.h"
#include "Pattern.h"

class Node {
public:
    Node(int alphabet_size, int *alphabet_sizes, int timestep) : g_alphabetSize(alphabet_size),
                                                                 g_alphabetSizes(alphabet_sizes), g_timestep(timestep) {
        g_tree_id = 0;    //ROOT
        init(-1);
    }

    Node(int alphabet_size, int *alphabet_sizes, int timestep, int id, int aid) : g_alphabetSize(alphabet_size),
                                                                                  g_alphabetSizes(alphabet_sizes),
                                                                                  g_timestep(timestep), g_tree_id(id) {
        init(aid);
    }

    ~Node();

    void init(int aid) {
        g_next_timestep = nullptr;
        g_infrequent = false;
        g_pos_correction = 0;
        for (int i = 0; i <= aid; ++i)
            g_pos_correction += g_alphabetSizes[i];                //symbols of lower or similar attributes can't become children
        g_nr_children = g_alphabetSize -
                        g_pos_correction;        //g_nr_children = g_alphabetSize - g_tree_id;//for item set data
        g_children = new Node *[g_nr_children];
        for (int i = 0; i < g_nr_children; ++i)
            g_children[i] = nullptr;
    }


    bool find_pattern(Pattern *p, int start_pos, event_set::iterator it,
                      event_set::iterator end)        //True when event_set found and thus not frequent, False otherwise
    {
        if (g_infrequent)
            return true;

        if (it == end)        //check if there is a next node to go to
        {
            if (g_timestep == p->get_length() - 1 - start_pos)
                return false;
            else {
                if (g_next_timestep == nullptr)
                    return false;
                else
                    return g_next_timestep->find_pattern(p, start_pos,
                                                         p->get_symbols(start_pos + g_timestep + 1)->begin(),
                                                         p->get_symbols(start_pos + g_timestep + 1)->end());
            }
        }
        //evs is SORTED on tree_id, i.e. first ascending on attribute-level and then ascending alphabetically
        int pos = (*it)->tree_id - g_pos_correction - 1;
        if (g_children[pos] == nullptr)
            return false;
        else
            return g_children[pos]->find_pattern(p, start_pos, ++it, end);
    }

    void add_infrequent_pattern(Pattern *p, event_set::iterator it,
                                event_set::iterator end)    //evs is SORTED: first ascending on attribute-level and then ascending alphabetically
    {
        if (it == end) {
            if (g_timestep == p->get_length() - 1)
                g_infrequent = true;
            else {
                if (g_next_timestep == nullptr)
                    g_next_timestep = new Node(g_alphabetSize, g_alphabetSizes, g_timestep + 1);//Root
                g_next_timestep->add_infrequent_pattern(p, p->get_symbols(g_timestep + 1)->begin(),
                                                        p->get_symbols(g_timestep + 1)->end());
            }
        } else {
            int tree_id = (*it)->tree_id;
            int pos = tree_id - g_pos_correction - 1;
            if (g_children[pos] == nullptr)
                g_children[pos] = new Node(g_alphabetSize, g_alphabetSizes, g_timestep, tree_id, (*it)->attribute);
            g_children[pos]->add_infrequent_pattern(p, ++it, end);
        }
    }

    void print(const string& tab) {
        cout << tab << "NODE id: " << g_tree_id << "  infreq=" << g_infrequent << "  #Children: " << g_nr_children
             << "  nextTimestep: " << g_next_timestep << endl;
        for (int i = 0; i < g_nr_children; ++i)
            if (g_children[i] != nullptr)
                g_children[i]->print(tab + "\t");
    }

private:
    bool g_infrequent;    //whether the pattern represented by this node is known to be infrequent
    int g_alphabetSize;
    int *g_alphabetSizes;
    int g_tree_id;        //ROOT = 0, rest ranges from 1 to g_alphabetSize
    int g_nr_children;    //g_alphabetSize - g_tree_id
    int g_pos_correction;

    Node **g_children;

    Node *g_next_timestep;
    int g_timestep;        //range 0 to ?

};

#endif