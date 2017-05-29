#ifndef CFREPOISSON_H
#define CFREPOISSON_H

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "CTimer.h"
#include "CTemporal.h"
#include "CFrelement.h"

/**
@author Ferdian Jovan 
*/
typedef struct{
	float interval;
	float shape;
	float scale;
	float rate;
}PoissonRate;

using namespace std;

class CFrePoisson: public CTemporal
{
	public:
		CFrePoisson(const char* id);
		~CFrePoisson();

		//adds a serie of measurements to the data
		int add(uint32_t time,float state);
		void init(int iMaxPeriod,int elements,int numActivities);

		//estimates the probability for the given times 
		float estimate(uint32_t time);
		float predict(uint32_t time);

		void update(int modelOrder);
		void print(bool verbose=true);

		int save(FILE* file,bool lossy = false);
		int load(FILE* file);
		int save(char* name,bool lossy = false);
		int load(char* name);

		PoissonRate *rates;
		char id[MAX_ID_LENGTH];
		CFrelement *fremen;
		SFrelement *predictFrelements;
		int64_t firstTime;
		int64_t  lastTime;
        int max_addition;
        int max_iteration;
        int order;
};

#endif
