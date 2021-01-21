#ifndef PARAMS_H
#define PARAMS_H

#include <limits>
#include <stdint.h>

#include "defines.h"


struct params {
	params() : sparse(false), thresh(1) {};

	bool sparse;
	int thresh;
};

#endif
