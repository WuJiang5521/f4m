#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Window.h"
#include "DittoPattern.h"

Window::Window(const Multi_event **mevp, DittoPattern *p) : mev_positions(mevp), pat(p), next(0), nextDisjoint(0), prevDisjoint(0)
{ 
	first = mev_positions[0];
	last = mev_positions[pat->getLength()-1]; 	

	active = false;
	
	gaps = new set<int>();
	//start with all positions within the window
	for(int ts = first->id; ts <= last->id; ++ts)
		gaps->insert(ts);
	set<int>::iterator it, end = gaps->end();

	//remove all positions where the pattern is present
	for(int ts = 0; ts < pat->getLength(); ++ts)
		if((it = gaps->find(mev_positions[ts]->id)) != end)
			gaps->erase(it);
		
	int length = last->id - first->id + 1;
	gapLength = gaps->size();
}

Window::~Window() 
{ 
	delete []mev_positions; 
	gaps->clear();
}

void Window::print ()
{
	cout << "Window:  first=" << first->id << " last=" << last->id;
	if(next != 0)
		cout << " next=(" << next->first->id << "-" << next->last->id << ")";
	else
		cout << " next=0";
	if(nextDisjoint != 0)
		cout << " nextDisjoint=(" << nextDisjoint->first->id << "-" << nextDisjoint->last->id << ")";
	else
		cout << " nextDisjoint=0";
	if(prevDisjoint != 0)
		cout << " prevDisjoint=(" << prevDisjoint->first->id << "-" << prevDisjoint->last->id << ")" << endl;
	else
		cout << " prevDisjoint=0" << endl;
}

bool Window::equal(Window* rhs)
{ 
	return (first->id == rhs->first->id && last->id == rhs->last->id); 
}

