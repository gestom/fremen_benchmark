#include <iostream>
#include <fstream>	
#include <cstdlib>	
#include "CFrelement.h"
#include "CTimer.h"
#define MAX_SIGNAL_LENGTH 1000000

CFrelement frelement

int trainingTimes[MAX_SIGNAL_LENGTH];
unsigned char trainingStates[MAX_SIGNAL_LENGTH];
int testingTime;
float predictions[MAX_SIGNAL_LENGTH];

int testingLength = 0;
int trainingLength = 0;
int dummyState = 0;
int dummyTime = 0;

int main(int argc,char *argv[])
{
	/*read training data*/
	FILE* file=fopen(argv[1],"r");
	while (feof(file)==0)
	{
		fscanf(file,"%i %i\n",&dummyTime,&dummyState);
		trainingTimes[trainingLength] = dummyTime;
		trainingStates[trainingLength] = dummyState;
		trainingLength++;
	}
	fclose(file);

	/*traning model*/
	CFFTPlan plan;
	plan.prepare(trainingLength);
	CFrelement frelement;
	frelement.build(trainingStates,trainingLength,&plan);
	frelement.update(atoi(argv[3]),&plan);

	/*read testing timestamps and make predictions*/
	file=fopen(argv[2],"r");
	while (feof(file)==0)
	{
		fscanf(file,"%i\n",&testingTime);
		predictions[testingLength++] = frelement.estimate(testingTime);
	}
	fclose(file);

	file=fopen("tmp/predictions.txt","w");
	for (int i =0;i<testingLength;i++) fprintf(file,"%.3f\n",predictions[i]);
	fclose(file);
	return 0;
}
