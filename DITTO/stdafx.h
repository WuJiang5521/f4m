#ifndef STDAFX_H
#define STDAFX_H

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"


#include <stdio.h>
#include <tchar.h>

#include <list>
#include <algorithm>

#include <limits>
#include <map>
#include <set>
#include <math.h>

#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>      
#include <string>
#include <string.h>

static const int CATEGORICAL = 1, ITEMSET = 2;
static const double laplace = 0; 
static const double logbase = log(2);
#define lg2(x) (log(x) / logbase)

#include <iomanip>     
#include <ctime>
#include <random>


#endif