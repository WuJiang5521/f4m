#ifndef STDAFX_H
#define STDAFX_H


#include <cstdio>
//#include <tchar.h>
#include <cfloat>
#include <list>
#include <algorithm>

#include <limits>
#include <map>
#include <set>
#include <cmath>

#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include <queue>
#define LSH
#define MISS

static const int CATEGORICAL = 1, ITEMSET = 2;
static const double laplace = 0;
static const double logbase = log(2);
#define lg2(x) (log(x) / logbase)

#include <iomanip>
#include <ctime>
#include <random>

#ifdef MISS
extern bool miss_print_debug; // DEBUFG
extern std::ofstream outfile_miss; // DEBUG
#endif

#endif