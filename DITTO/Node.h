#ifndef NODE_H
#define NODE_H

#include "stdafx.h"
#include "Event.h"
#include "DittoPattern.h"

class Node
{
	public:
		Node(int alphabetSize, int* alphabetSizes, int timestep) : g_alphabetSize(alphabetSize), g_alphabetSizes(alphabetSizes), g_timestep(timestep)
		{
			g_tree_id = 0;	//ROOT
			init(-1);		
		}
		Node(int alphabetSize, int* alphabetSizes, int timestep, int id, int aid) : g_alphabetSize(alphabetSize), g_alphabetSizes(alphabetSizes), g_timestep(timestep), g_tree_id(id)
		{
			init(aid);
		}		
		~Node(void);

		void init(int aid)
		{
			g_next_timestep = 0; 
			g_infrequent = false;
			g_pos_correction = 0;
			for(int i = 0; i <= aid; ++i)
				g_pos_correction += g_alphabetSizes[i];				//symbols of lower or similar attributes can't become children
			g_nr_children = g_alphabetSize - g_pos_correction;		//g_nr_children = g_alphabetSize - g_tree_id;//for item set data
			g_children = new Node*[g_nr_children];
			for(int i = 0; i < g_nr_children; ++i)
				g_children[i] = 0;		
		}


		bool		findDittoPattern(DittoPattern *p, int startPos, eventSet::iterator it, eventSet::iterator end)		//True when eventSet found and thus not frequent, False otherwise
		{	
			if(g_infrequent)
				return true;

			if(it == end)		//check if there is a next node to go to
			{
				if(g_timestep == p->getLength()-1-startPos)
					return false;
				else
				{
					if(g_next_timestep == 0)
						return false;
					else
						return g_next_timestep->findDittoPattern(p, startPos, p->getSymbols(startPos+g_timestep+1)->begin(), p->getSymbols(startPos+g_timestep+1)->end());
				}
			}
			//evs is SORTED on tree_id, i.e. first ascending on attribute-level and then ascending alphabetically
			int pos = (*it)->tree_id - g_pos_correction - 1;
			if(g_children[pos] == 0)
				return false;
			else
				return g_children[pos]->findDittoPattern(p, startPos, ++it, end);
		}

		void addInfrequentDittoPattern(DittoPattern *p, eventSet::iterator it, eventSet::iterator end)	//evs is SORTED: first ascending on attribute-level and then ascending alphabetically
		{
			if(it == end)
			{
				if(g_timestep == p->getLength()-1) 
					g_infrequent = true;
				else
				{
					if(g_next_timestep == 0)
						g_next_timestep = new Node(g_alphabetSize, g_alphabetSizes, g_timestep+1);//Root
					g_next_timestep->addInfrequentDittoPattern(p, p->getSymbols(g_timestep+1)->begin(), p->getSymbols(g_timestep+1)->end());
				}
			}
			else
			{
				int tree_id = (*it)->tree_id;
				int pos = tree_id - g_pos_correction - 1;
				if(g_children[pos] == 0)
					g_children[pos] = new Node(g_alphabetSize, g_alphabetSizes, g_timestep, tree_id, (*it)->attribute);
				g_children[pos]->addInfrequentDittoPattern(p, ++it, end);
			}
		}

		void print(string tab)
		{
			cout << tab << "NODE id: " << g_tree_id << "  infreq=" << g_infrequent << "  #Children: " << g_nr_children << "  nextTimestep: " << g_next_timestep << endl; 
			for(int i = 0; i < g_nr_children; ++i)
				if(g_children[i] != 0)
					g_children[i]->print(tab + "\t");
		}

	private:
		bool g_infrequent;	//whether the pattern represented by this node is known to be infrequent
		int g_alphabetSize;
		int *g_alphabetSizes;
		int g_tree_id;		//ROOT = 0, rest ranges from 1 to g_alphabetSize
		int g_nr_children;	//g_alphabetSize - g_tree_id
		int g_pos_correction;

		Node **g_children;

		Node *g_next_timestep;
		int g_timestep;		//range 0 to ?

};

#endif