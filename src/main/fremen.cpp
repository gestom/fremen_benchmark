#include <iostream>
#include <fstream>	
#include <cstdlib>	
#include "CFrelement.h"
#include "CTimer.h"
#define MAX_SIGNAL_LENGTH 1000000

CFrelement frelement;

unsigned char signal[MAX_SIGNAL_LENGTH];
unsigned char testSignal[MAX_SIGNAL_LENGTH];
int testSignalLength = 0;
float probs[MAX_SIGNAL_LENGTH*100];
int signalLength = 0; 
int dummy = 0;

int maina(int argc,char *argv[])
{
	FILE* file=fopen(argv[1],"r");
	while (feof(file)==0)
	{
		fscanf(file,"%i\n",&dummy);
		signal[signalLength++]=dummy;
	}
	fclose(file);

	file=fopen(argv[2],"r");
	while (feof(file)==0)
	{
		fscanf(file,"%i\n",&dummy);
		testSignal[testSignalLength++]=dummy;
	}
	fclose(file);

	CFFTPlan plan;
	plan.prepare(signalLength);
	CFrelement frelement;
	frelement.build(signal,signalLength,&plan);
	for (int i =0;i<50;i++){
		frelement.update(i,&plan);
		float err = frelement.evaluate(testSignal,atoi(argv[3])*24*3600,testSignalLength,&plan);
		fprintf(stdout,"ERR: %i %.3f\n",i,err);
	}
	return 0;
}

//compression, estimation error
int mainb(int argc,char *argv[])
{
	FILE* file=fopen(argv[1],"r");
	while (feof(file)==0)
	{
		fscanf(file,"%i\n",&dummy);
		signal[signalLength++]=dummy;
	}
	fclose(file);

	CFFTPlan plan;
	printf("Building a plan\n");
	plan.prepare(signalLength);
	CFrelement frelement;
	printf("Building a model\n");
	frelement.build(signal,signalLength,&plan);
	char nam[20];
	float comprLossy,comprLossless,eval;
	for (int i = 0;i<50;i++){
		frelement.update(i,&plan);
		sprintf(nam,"fremen_%02i.lossless.fre",i);
		comprLossless = frelement.save(nam,true);
		sprintf(nam,"fremen_%02i.lossy.fre",i);
		comprLossy = frelement.save(nam,false);
		eval = frelement.evaluate();
		printf("ESTIMATION: %i %.3f %.f %.3f\n",i,eval,signalLength/8.0/comprLossless*2,signalLength/8.0/comprLossy*2);
		frelement.estimate(&probs[MAX_SIGNAL_LENGTH*i],&plan,0.5,signalLength);
	}
	frelement.print();

	file=fopen(argv[2],"w");
	for (int i = 0;i<signalLength;i+=1)
	{
		fprintf(file,"%i ",signal[i]);
		for (int o = 0;o<15;o++) fprintf(file,"%.3f ",probs[o*MAX_SIGNAL_LENGTH+i]);
		fprintf(file,"\n");
	}
	fclose(file);
	return 0;
}

//demo
int main(int argc,char *argv[])
{
	FILE* file=fopen(argv[1],"r");
	while (feof(file)==0)
	{
		fscanf(file,"%i\n",&dummy);
		signal[signalLength++]=dummy;
	}
	fclose(file);
	int maxOrder = 15;
	CFFTPlan plan;
	plan.prepare(signalLength);
	CFrelement frelement;
	frelement.build(signal,signalLength,&plan,maxOrder);
	frelement.update(atoi(argv[3]),&plan,maxOrder);
	printf("Selected: 0 %f\n",frelement.gain);
	for (int i = 0;i<atoi(argv[3]);i++) printf("Selected: %i %lf\n",frelement.frelements[i].frequency,frelement.frelements[i].amplitude); 
	frelement.estimate(probs,&plan,0.5,signalLength);

	file=fopen(argv[2],"w");
	for (int i = 0;i<signalLength;i+=1)
	{
		fprintf(file,"%.3f %i\n",probs[i],probs[i]>0.5);
	}
	fclose(file);
	return 0;
}
