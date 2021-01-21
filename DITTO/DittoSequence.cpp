#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <iterator>
#include "DittoSequence.h"

DittoSequence::DittoSequence(FILE *f, Parameters *par) : par(par)
{
	g_error = false;
	if(read(f))
	{
		g_error = true;
		return;
	}
	if(par->dummy_file != "")
	{
		if(load_dummies())
		{
			g_error = true;
			return;
		}
	}
	else
		par->nrOfDittoPatterns = 0;
	init();
}

int DittoSequence::load_dummies()
{	
	string line;
	ifstream myfile (par->dummy_file.c_str());
	if (myfile.is_open())
	{		
		int cnt = -1;
		while ( getline(myfile, line) )
		{	
			if(cnt == -1)
			{
				istringstream buf(line);
				istream_iterator<string> beg(buf), end;
				vector<string> tokens(beg, end); 
				vector<string>::iterator it_vec = tokens.begin(), end_vec = tokens.end();
				par->input_type = atoi((*it_vec).c_str()); 
				it_vec++;
				par->nrOfDittoPatterns = atoi((*it_vec).c_str());
				par->dummies = new dummy*[par->nrOfDittoPatterns];
				it_vec++;
				par->nrOfAttributes = atoi((*it_vec).c_str()); 
				it_vec++;
				par->tempAlphabetSize = atoi((*it_vec).c_str()); 
				par->alphabetSizes = new int[par->nrOfAttributes];
				for(int i = 0; i < par->nrOfAttributes; ++i)
					par->alphabetSizes[i] = par->tempAlphabetSize;
				par->alphabetSize = par->nrOfAttributes * par->tempAlphabetSize;
				
				cnt++;
				continue;
			}
	
			int size, length, support;
			float gapChance;
			attrSymSet** events;
			istringstream buf(line);
			istream_iterator<string> beg(buf), end;
			vector<string> tokens(beg, end); 
			vector<string>::iterator it_vec = tokens.begin(), end_vec = tokens.end();
			size = atoi((*it_vec).c_str()); 
			it_vec++;
			length = atoi((*it_vec).c_str()); 
			it_vec++;
			support = atoi((*it_vec).c_str()); 
			it_vec++;
			gapChance = atof((*it_vec).c_str()); 
			it_vec++;
			events = new attrSymSet*[length];
			for(int l = 0; l < length; ++l)
			{
				int height = atoi((*it_vec).c_str());		
				it_vec++;
				events[l] = new attrSymSet;
				for(int h = 0; h < height; ++h)
				{
					int aid = atoi((*it_vec).c_str());	
					it_vec++;
					int sym = atoi((*it_vec).c_str());	
					it_vec++;
					events[l]->insert(new attrSym(aid, sym));
				}
			}		
			par->dummies[cnt] = new dummy(size, length, events, support, gapChance);
			cnt++;
			if(cnt == par->nrOfDittoPatterns)
				break;
		}
		myfile.close();
	}
	else
	{
		cout << "ERROR opening pattern file: " << par->dummy_file << endl;
		par->dummy_file = "";
		return 1;
	}
	return 0;	
}


void DittoSequence::pre_init()
{
	//compute all tree-id's once and save them based on sym and attr
	tree_ids = new int*[par->nrOfAttributes];
	int tree_id = 1;
	for(int a = 0; a < par->nrOfAttributes; ++a)
	{
		tree_ids[a] = new int[par->alphabetSizes[a]];
		for(int s = 0; s < par->alphabetSizes[a]; ++s)
			tree_ids[a][s] = tree_id++;		
	}
}

void DittoSequence::init()
{
	//count the nr of multi_events with each possible size
	int *cnt = new int[par->nrOfAttributes+1];
	for(int sz = 0; sz <= par->nrOfAttributes; ++sz)
		cnt[sz] = 0;

	for(int mev = 0; mev < par->nrMulti_events; ++mev)
        cnt[g_mev_time[mev]->getSize()]++;

	g_mevTerm = new float[par->nrMulti_events];
	for(int mev = 0; mev < par->nrMulti_events; ++mev)		
		g_mevTerm[mev] = -lg2( cnt[g_mev_time[mev]->getSize()] / (float)par->nrMulti_events );

	mu = new mathutil(g_nrEvents, par->alphabetSize);

	computeSTCodelengths();

	isCovered = new int[par->nrMulti_events];
	for (int i = 0; i < par->nrMulti_events; ++i)
	{	
		isCovered[i] = 0;								//for each Multi_event we keep track whether it is covered
		g_mev_time[i]->finished();						//initialize the isCovered variable 
		if (i < (par->nrMulti_events - 1))					//set NEXT pointers
			g_mev_time[i]->setNext(g_mev_time[i + 1]);
	}
}

//Return 1 in case of error
int DittoSequence::read(FILE *f)
{
	int ecnt = 0;		//event count
	int mcnt = 0;		//multi_event count
	int scnt = 1;		//sequence count		NOTE: no -1 after last sequence
	int acnt = 1;		//attribute count		NOTE: no -2 after last attribute
	int a;				//symbol

	/*
	HEADER
		Contains the number of attributes followed by the alphabetsize per attribute
	CASE CATEGORICAL
		Every line contains one attribute and ends with -2, all sequences are separated by -1
		For each attribute i its values range from 0 to max_i
	CASE ITEM SET
		For every time step all its events are listed subsequently and time steps are separated with -2, all sequences are separated by -1
		Each value ranges from 0 to max, where the value also indicates the id of the attribute
	*/

	//read header info
	int cnt = -1;
	par->alphabetSize = 0;	
	while (fscanf(f, "%d", &a) == 1) 
	{		
		if(cnt == -1)
		{
			par->nrOfAttributes = a;
			par->alphabetSizes = new int[a];						
			cnt++;
			continue;
		}
		if(cnt == par->nrOfAttributes)
			break;
		par->alphabetSizes[cnt++] = a;
		par->alphabetSize += a;
	}

	//count sequences and events
	rewind(f);
	cnt = -1;
	if (par->input_type == CATEGORICAL) 
	{		
		while (fscanf(f, "%d", &a) == 1) 
		{
			if(cnt++ < par->nrOfAttributes)	//skip header line
				continue;
			if (a == -2)	acnt++;			//end of attribute
			if (acnt < 2)					//all attributes have same number of sequences and events, so we only count the events and sequences in the first attribute
				if (a >= 0) 
					mcnt++;					//new multi_event
				else 
					scnt++;					//new sequence
		}
		ecnt = mcnt*acnt;
	}
	else //ITEM SET
	{		
		while (fscanf(f, "%d", &a) == 1) 
		{
			if(cnt++ < par->nrOfAttributes)	//skip header line
				continue;
			if (a == -2)
			{
				if(mcnt == 0)	
					mcnt = 2;	//the first and last multi_events
				mcnt++;			//new multi_event			
			}
			if (a == -1)	scnt++;			//new sequence			
			if (a >= 0)		
			{
				ecnt++;						//new event
				if(a > acnt) 
					acnt = a;				//the max value is also the max nr of events in a multi_event, i.e. max nr of attributes
			}
		}
		acnt++;								//acnt contained the highest aid, but we also have aid=0, thus aid++
		
	}
	rewind(f);
	par->nrMulti_events = mcnt;
	g_nrSequences = scnt;
	g_nrEvents = ecnt;		
	
	if(acnt != par->nrOfAttributes)
	{
		cout << "WARNING: header line differs from data! Must contain number of attributes followed by alphabetsize per attribute.\n";
		return 0;	//Does not have to be an error
	}

	g_occ = new list<Multi_event*>*[par->nrOfAttributes];					//for each aid 
	for (int aid = 0; aid < par->nrOfAttributes; ++aid)
	{
		g_occ[aid] = new list<Multi_event*>[par->alphabetSizes[aid]];		//for each symbol 
		for (int s = 0; s < par->alphabetSizes[aid]; ++s)
			g_occ[aid][s] = list<Multi_event*>();						//a list of Multi_event* where it occurs
			
	}

	g_mev_time = new Multi_event*[par->nrMulti_events];					//number of multi-events
	for (int i = 0; i < par->nrMulti_events; ++i)
		g_mev_time[i] = 0;
	g_sequence_sizes = new int[g_nrSequences];							//number of sequences	
	for (int i = 0; i < g_nrSequences; ++i)
		g_sequence_sizes[i] = 0;

	pre_init();

	cnt = -1;
	if(par->input_type == CATEGORICAL)
	{
		int sym;
		int i = 0;				//event id
		int sid = 0;			//sequence id
		int aid = 0;			//attribute id
		while (fscanf(f, "%d", &sym) == 1) 
		{
			if(cnt++ < par->nrOfAttributes)	//skip header line
				continue;
			if (sym == -2) 
			{
				aid++;
				i = 0;
				sid = 0;
				continue;
			}
			if (sym == -1) 
			{
				sid++;
				continue;
			}			
			if (aid == 0)
			{			
				g_mev_time[i] = new Multi_event(par->nrOfAttributes, i, sid);
				g_sequence_sizes[sid]++;					
			}
			Multi_event *me = g_mev_time[i];
			g_occ[aid][sym].push_back(me);
			me->addEvent(new Event(sym, aid, me->getSize(), tree_ids[aid][sym]));
			i++;
		}
	}
	else //ITEM SET
	{
		int aid;
		int i = 0;				//event id
		int sid = 0;			//sequence id
		if(par->nrMulti_events > 0)
			g_mev_time[i] = new Multi_event(par->nrOfAttributes, i, sid);	//first multi_event
		while (fscanf(f, "%d", &aid) == 1) 
		{
			if(cnt++ < par->nrOfAttributes)	//skip header line
				continue;
			if (aid == -1) 
				sid++;
			if(aid < 0)
			{
				i++;
				g_mev_time[i] = new Multi_event(par->nrOfAttributes, i, sid);	
				continue;
			}
				
			g_sequence_sizes[sid]++;				
			Multi_event *me = g_mev_time[i];
			g_occ[aid][0].push_back(me);						//in item set case the symbol is always 0
			me->addEvent(new Event(0, aid, me->getSize(), tree_ids[aid][0]));
		}
	}
	return 0;
}

//we compute the ST code lengths over the entire data and not per attribute. This makes it more suitable for item set data
void DittoSequence::computeSTCodelengths()
{
	g_STcodelengths = new double*[par->nrOfAttributes];
	for (int aid = 0; aid < par->nrOfAttributes; ++aid)
	{
		g_STcodelengths[aid] = new double[par->alphabetSizes[aid]];
		for (int s = 0; s < par->alphabetSizes[aid]; ++s)
			g_STcodelengths[aid][s] = -lg2((g_occ[aid][s].size() + laplace) / (g_nrEvents + par->alphabetSize*laplace));
	}
	
}

//Find all Multi_events that contain this set of events
const list<Multi_event*>* DittoSequence::findOccurrences(eventSet *events) const
{
	list<Multi_event*> *occ = new list<Multi_event*>();
	int nrEvents = events->size();

	list<Multi_event*>::iterator* occPerEvent = new list<Multi_event*>::iterator[nrEvents];		//for each event an iterator to the list of Multi_event where it occurs
	list<Multi_event*>::iterator* endPerEvent = new list<Multi_event*>::iterator[nrEvents];		//for each event an iterator to the end of the list of Multi_event where it occurs
	eventSet::iterator it = events->begin(), end = events->end();
	int i = 0;
	while (it != end)
	{
		occPerEvent[i] = g_occ[(*it)->attribute][(*it)->symbol].begin();
		endPerEvent[i] = g_occ[(*it)->attribute][(*it)->symbol].end();
		++it;
		++i;
	}

	int smallestID, smallest;
	bool aligned;
	while (1)
	{	
		if(occPerEvent[0] == endPerEvent[0])
			break;
		
		smallestID = (*occPerEvent[0])->id, smallest = 0;
		aligned = true;
		for (int i = 1; i < nrEvents; ++i)
		{
			if(occPerEvent[i] == endPerEvent[i])
				break;

			if ((*occPerEvent[i])->id < smallestID)					//keep track of the event with the earliest occurence
			{
				smallestID = (*occPerEvent[i])->id;
				smallest = i;
			}

			if ((*occPerEvent[i])->id != (*occPerEvent[i-1])->id)	//check if all point at same Multi_event
				aligned = false;
		}

		if (aligned)												//add new occurence
			occ->push_back((*occPerEvent[0]));
		
		occPerEvent[smallest]++;									//go to next occurence for the event with the earliest occurence
		if (occPerEvent[smallest] == endPerEvent[smallest])			//if there is no next occurence we stop
			break;		
	}

	return occ;
}

//RETURN true when whole cover is complete
bool DittoSequence::cover(DittoPattern *p, Window *w)
{ 
	bool result = false;

	eventSet* events;
	for(int ts = 0; ts < p->getLength(); ++ts)
	{
		events = p->getSymbols(ts);
		eventSet::iterator it = events->begin(), end = events->end(); 
		while (it != end)
		{ 		
			if(result)
			{
				g_outputStream << "\n\n\n\n\n";
				g_outputStream << "ERROR: we checked that pattern p could cover window w, but everything is covered before this method is finished.\n";
				g_outputStream << "The next event is: "; (*it)->print();
				g_outputStream << "The pattern p: "; p->print();
				g_outputStream << "The window w: "; w->print();
				g_outputStream << "\n\n\n\n\n";
			}
			if(cover(*it, w->get_mev_position(ts)->id, p))
				result = true;
			++it; 
		} 		
	}
	return result; 
}
//RETURN true when whole cover is complete
bool DittoSequence::cover(Event *e, int pos, DittoPattern *p)
{ 
	if(!isCovered[pos])
	{
		if(g_mev_time[pos]->cover(e, p)) 
		{
			isCovered[pos] = 1;
			mevCovered++;
		}
	}
	return (mevCovered == par->nrMulti_events); 
}	

//RETURN true when cover is possible
bool DittoSequence::tryCover(eventSet *events, int pos)
{ 
	eventSet::iterator it = events->begin(), end = events->end(); 
	while (it != end)
	{ 		
		if(!tryCover(*it, pos))
			return false;
		++it; 
	} 
	return true; 
}
//RETURN true when cover is possible
bool DittoSequence::tryCover(Event *e, int pos)
{ 
	if(isCovered[pos])
		return false;
	
	return g_mev_time[pos]->tryCover(e);
}	

//cover the rest of the data with singletons. Loop over all data
void DittoSequence::coverSingletons(DittoPattern*** singletons)
{
	for(int i = 0; i < par->nrMulti_events; ++i)
	{
		if(isCovered[i])	//this multi-event is already covered
			continue;

		eventSet::iterator it = g_mev_time[i]->getEvents()->begin(), end = g_mev_time[i]->getEvents()->end();
		while(it != end)
		{	
			if(!g_mev_time[i]->testCovered((*it)->id))		//if not yet covered
			{
				g_mev_time[i]->cover((*it), singletons[(*it)->attribute][(*it)->symbol]);		//fill the isCovered array in Multi_event
				singletons[(*it)->attribute][(*it)->symbol]->updateUsages(0);				
			}
			++it;
		}
	}
}

string DittoSequence::print_sequence(bool allValues)
{
	stringstream g_outputStream;
	g_outputStream << "SEQUENCE. Alphabet size: " << par->alphabetSize << " #Attributes: " << par->nrOfAttributes << " alphabet sizes: {";	
	for (int i = 0; i < par->nrOfAttributes; ++i)
	{
		g_outputStream << par->alphabetSizes[i];
		if(i+1 < par->nrOfAttributes)
			g_outputStream << ", ";
	}
	g_outputStream << "}" << endl;
	g_outputStream << "#Multi_events: " << par->nrMulti_events << " total #events: " << g_nrEvents << endl;
	g_outputStream << "#Sequences: " << g_nrSequences << endl;

	if(allValues) //print the entire sequence
	{
		for(int i =0; i < par->nrMulti_events; ++i)
		{			
			eventSet::iterator it = g_mev_time[i]->getEvents()->begin(), end = g_mev_time[i]->getEvents()->end();
			while(it != end)
			{
				g_outputStream << i << ":  ";
				(*it)->print();
				++it;
			}
			g_outputStream << endl;
		}
	}

	return g_outputStream.str();
}

DittoSequence::~DittoSequence()
{
	for (int i = 0; i < par->nrMulti_events; ++i)
		delete g_mev_time[i];
	delete[]g_mev_time;
	
	for (int aid = 0; aid < par->nrOfAttributes; ++aid)
	{
		delete []g_STcodelengths[aid];
		delete[]g_occ[aid];
	}
	delete []g_STcodelengths;
	delete []g_occ;

	delete []g_sequence_sizes;						
	delete []par->alphabetSizes;

	delete []isCovered;
	
}