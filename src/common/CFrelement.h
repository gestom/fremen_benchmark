#ifndef CFRELEMENT_H
#define CFRELEMENT_H

#include <iostream>
#include <vector>
#include <complex>	
#include <algorithm> 
#include <iterator> 
#include <complex>	// goes along with fftw
#include <fftw3.h>
#include <string.h>
#include "CTimer.h"
#include "CFFTPlan.h"
	
/**
@author Tom Krajnik
*/

using namespace std;

typedef struct
{
	float amplitude;
	float phase;
	unsigned int frequency;
}SFrelement;

class CFrelement
{
public:
  CFrelement();
  ~CFrelement();

  float estimate(float* signal,CFFTPlan *plan,float anomalyThreshold = 1.0,int signalLengthi = -1);
  float reconstruct(unsigned char* signal,CFFTPlan *plan,bool evaluate = false);

  /*state estimation: retrieves the state*/
  float estimate(int timeStamp);
  float fineEstimate(float timeStamp);

  /*state estimation: retrieves the state*/
  unsigned char retrieve(int timeStamp);

  /*adds a single measurement*/
  void add(unsigned char value);

  /*gets length of the stored signal*/
 unsigned int getLength();

  /*calculates estimation error*/
 float evaluate();

 float evaluate(unsigned char* signal,int start,int end,CFFTPlan *plan);

  void build(unsigned char* signal,int signalLength,CFFTPlan *plan,int printStuff = 0);

  void print(bool verbose=true);

  int save(FILE* file,bool lossy = false);
  int load(FILE* file);
  int save(char* name,bool lossy = false);
  int load(char* name);

  /*changes the model order*/
  void update(int modelOrder,CFFTPlan *plan,int printStuff = 0);

  double *signal;

//private:
	SFrelement *frelements;
	unsigned int *outlierSet;
	unsigned int outliers;
	unsigned char order;
	float gain;
	unsigned int signalLength;
};

#endif
