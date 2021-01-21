#ifndef DITTO_H
#define DITTO_H

#include "stdafx.h"
#include "DittoSequence.h"
#include "DittoPattern.h"
#include "CodeTable.h"
#include "Cover.h"
#include "Node.h"
#include "getopt.h"

class Ditto
{
	public:
		Ditto(Parameters *parameters);
		~Ditto();

		time_t			tijd();

		DittoPattern*		buildDittoPattern(DittoPattern *a, DittoPattern*b, int offset);
		DittoPattern*		buildInterleavedDittoPattern(DittoPattern *singleton, DittoPattern *p, int pos_singleton);
			
		bool			checkDittoPatternAttributeOverlap(DittoPattern *a, DittoPattern*b, int offset);
		bool			checkEventsetAttributeOverlap(eventSet *a, eventSet *b);
		bool			checkSubset(DittoPattern *a, DittoPattern *b);
		bool			checkSubset(eventSet *esa, eventSet *esb);
		bool			checkIfDummy(DittoPattern *p);

		bool			combineDittoPatterns(DittoPattern *a, DittoPattern *b, int totalUsage, patternSet* result);
		eventSet*		joinEventsets(eventSet* a, eventSet* b);

		void			insertCandidates(patternSet *list);
		usgSz*			postprune(DittoPattern *accepted, int totalUsg, double new_size);
		
		patternSet::iterator findDittoPatternInSet(patternSet *pset, DittoPattern *p);
		candpatternSet::iterator findDittoPatternInSet(candpatternSet *pset, DittoPattern *p);
		prunepatternSet::iterator findDittoPatternInSet(prunepatternSet *pset, DittoPattern *p);

		void			breakDown(DittoPattern *p);
		void			generateCandidates(usagepatternSet::iterator *pt_ct_1, usagepatternSet::iterator *pt_ct_2, usagepatternSet::iterator *pt_begin_ct, usagepatternSet::iterator *pt_end_ct, usgSz *current_usgSz);
		usgSz*			tryVariations(DittoPattern *accepted, usgSz* current_usgSz);
		void			loadOrBuildMinWindows(DittoPattern *p);

		CodeTable*		get_codeTable() { return g_ct; }
		Cover*			get_cover() { return g_cover; }


	private:
		CodeTable		*g_ct;

		candpatternSet	*g_cand;			//ordered on estimated gain
		usagepatternSet	*g_ct_on_usg;		//ordered on usage

		Cover			*g_cover; 
		
		patternSet		*g_whiteList;			//contains all considered candidates so we don't need to compute their support and minWindows again
		patternSet		*g_blackList;			//contains all considered candidates with sup = 0

		ostringstream	g_outputStream;

		Node			*g_root;	

		Parameters		*par;

		
	
};

int dittoEnter(int argc, char* argv[]);

#endif