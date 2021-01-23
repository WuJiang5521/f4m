#include "stdafx.h"
#include "Window.h"
#include "DittoPattern.h"

Window::Window(const Multi_event **mevp, DittoPattern *p) : mev_positions(mevp),
                                                            pat(p),
                                                            next(nullptr),
                                                            nextDisjoint(nullptr),
                                                            prevDisjoint(nullptr) {
    first = mev_positions[0];
    last = mev_positions[pat->getLength() - 1];

    active = false;

    gaps = new set<int>();
    //start with all positions within the window
    for (int ts = first->id; ts <= last->id; ++ts)
        gaps->insert(ts);
    set<int>::iterator it, end = gaps->end();

    //remove all positions where the pattern is present
    for (int ts = 0; ts < pat->getLength(); ++ts)
        if ((it = gaps->find(mev_positions[ts]->id)) != end)
            gaps->erase(it);

    gapLength = gaps->size();
}

Window::~Window() {
    delete[]mev_positions;
    gaps->clear();
}

void Window::print() const {
    cout << "Window:  first=" << first->id << " last=" << last->id;
    if (next != nullptr)
        cout << " next=(" << next->first->id << "-" << next->last->id << ")";
    else
        cout << " next=0";
    if (nextDisjoint != nullptr)
        cout << " nextDisjoint=(" << nextDisjoint->first->id << "-" << nextDisjoint->last->id << ")";
    else
        cout << " nextDisjoint=0";
    if (prevDisjoint != nullptr)
        cout << " prevDisjoint=(" << prevDisjoint->first->id << "-" << prevDisjoint->last->id << ")" << endl;
    else
        cout << " prevDisjoint=0" << endl;
}

bool Window::equal(Window *rhs) const {
    return (first->id == rhs->first->id && last->id == rhs->last->id);
}

