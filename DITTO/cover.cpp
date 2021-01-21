#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Cover.h"

/*	Changes the usage of the DittoPatterns in the CodeTable as a result of the covering process
	bool otherData: false = loop through the block's occurences, true = try to fit block on all positions in data */
Cover::Cover(DittoSequence *s, CodeTable *codeTable, bool otherData) : g_sequence(s), g_codeTable(codeTable), otherData(otherData)
{	
	g_sequence->getParameters()->cntCovers++;

	g_totalUsage = 0;
	g_szSequenceAndCT = 0;
	g_sequence->resetCover();
			
	bool coverComplete = false;
	codeTableSet *ct = g_codeTable->getCT();
	DittoPattern* candidate = 0;
	
	par = g_sequence->getParameters();

	//make an array to quickly get pointers to singletons based on their attribute and symbol
	int nrAttr = par->nrOfAttributes;
	int *alpSzs = par->alphabetSizes;

	DittoPattern*** singletons = new DittoPattern**[nrAttr];
	for(int atr = 0; atr < nrAttr; ++atr)
		singletons[atr] = new DittoPattern*[alpSzs[atr]];

	//Standard Cover Order
	codeTableSet::iterator it_blockList = ct->begin(), it_end = ct->end();
	for(it_blockList; it_blockList != it_end ; ++it_blockList)	
	{
		candidate = (*it_blockList);
	
		if(candidate->getSize() == 1)									//optimisation -> when arrived at singletons just loop once over data and cover all uncovered symbols with singletons
		{
			coverComplete = true;										//to make sure the usages of all singletons are reset
			singletons[(*candidate->getSymbols(0)->begin())->attribute][(*candidate->getSymbols(0)->begin())->symbol] = candidate;
		}

		candidate->resetUsage();										//reset the usage for the DittoPattern in the codeTable before covering
		if(coverComplete)
			continue;													//do not break, because we still need to reset all usages

		coverComplete = coverWithDittoPatternMinWindows(candidate);
	}
	
	//cover the rest with singletons
	g_sequence->coverSingletons(singletons);
	for(int atr = 0; atr < nrAttr; ++atr)
		delete []singletons[atr];
	delete []singletons;

	computeTotalUsage(ct);												//excluding laplace
	updateDittoPatternCodelengths(ct);										//only changes CL when we otherData = false
	g_szSequenceAndCT = g_codeTable->compute_sz(g_sequence);
}

void Cover::computeTotalUsage(codeTableSet *ct)
{
	g_totalUsage = 0;
	codeTableSet::iterator it_blockList = ct->begin(), it_end = ct->end();
	for (it_blockList; it_blockList != it_end; ++it_blockList)
		g_totalUsage += (*it_blockList)->getUsage();		
}


//cover all minimal windows of the pattern (when not covered yet)
//RETURN true if the cover is complete afterwards
bool Cover::coverWithDittoPatternMinWindows(DittoPattern* p)
{
	bool coverComplete = false;

	//loop through all this block's minimal windows 
	list<Window*> *lst = p->getMinWindows(g_sequence, otherData);
	list<Window*>::iterator it = lst->begin(), end = lst->end();
	while(it != end)
	{
		Window *w = *it;			//defines a minimal window
		
		if(coverWindowWithDittoPattern(w, p))		//cover this window with pattern p (NOTE: we do this only AFTER having checked tryCover
		{
			//TODO FUTURE: try if alternative of other patterns is cheaper
			w->active = true;
			p->updateUsages(w->get_GapLength());
			coverComplete = g_sequence->cover(p, w);
		}
		
		if(coverComplete)
			return true;
		++it;
	}	
	return false;
}

bool Cover::coverWindowWithDittoPattern(Window *w, DittoPattern *p)
{
	//check if gap is not bigger than patternlength-1
	if(w->get_GapLength() > p->getLength()-1)
		return false;

	//for each timestep in the pattern test whether the data can still be covered for this window
	for(int ts = 0; ts < p->getLength(); ++ts)
	{
		if(!g_sequence->tryCover(p->getSymbols(ts), w->get_mev_position(ts)->id))
			return false;
	}
	return true;
}


void Cover::updateDittoPatternCodelengths(codeTableSet *ct)
{
	if(!otherData)
	{
		double sum = (double)g_totalUsage + g_codeTable->getCTLength() * laplace;		//including laplace

		//loop through all DittoPatterns in the codeTable to update their optimal codelength
		codeTableSet::iterator it_blockList = ct->begin(), it_end = ct->end();
		while(it_blockList != it_end)									
		{
			(*it_blockList)->updateCodelength(sum);
			++it_blockList;				
		}
	}
}


Cover::~Cover(void)
{
}
