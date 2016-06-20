#include "CFrelement.h"

using namespace std;
static bool debug = false; 

bool fremenSort(SFrelement i,SFrelement j) 
{ 
	return (i.amplitude>j.amplitude); 
}

CFrelement::CFrelement()
{
	outlierSet = NULL;
	frelements = NULL;
	signalLength = gain = outliers = order = 0;
}

CFrelement::~CFrelement()
{
	free(frelements);
	free(outlierSet);
}

void CFrelement::build(unsigned char* signal,int signalLengthi,CFFTPlan *plan,int printStuff)
{
	signalLength = signalLengthi;
	int fftLength = signalLength/2+1;
	if (order > fftLength-1)order = fftLength-1;
	fftw_complex *coeffs;
	double *fftSignal;
	CTimer timer;
	timer.start();
	int tim = 0;
	unsigned char *reconstructed = (unsigned char*)malloc(signalLength*sizeof(unsigned char));

	if (order > 0){
		fftSignal = plan->signal;
		coeffs = plan->coeffs;
		gain = 0;
		for (int i = 0;i<signalLength;i++) gain+=signal[i];
		gain = gain/signalLength;

		for (int i = 0;i<signalLength;i++) fftSignal[i] = (signal[i]-gain);
		timer.reset();
		//cout << "FFT preparation time " << timer.getTime() << endl;

		/*calculation of the spectral model*/
		fftw_execute_dft_r2c(plan->direct,fftSignal,coeffs);
		if (debug) cout << "FFT calculation time " << timer.getTime() << endl;
		timer.reset();
		SFrelement *tmpFrelements = (SFrelement *)malloc(fftLength*sizeof(SFrelement));
		for(int i=0;i<fftLength;i++)
		{
			tmpFrelements[i].amplitude = (coeffs[i][0]*coeffs[i][0]+coeffs[i][1]*coeffs[i][1]);
			tmpFrelements[i].frequency = i;
		}
		//gain = sqrt(tmpFrelements[0].amplitude)/signalLength;

		if (printStuff >0)
		{
			printf("Spectrum: 0 %f\n",gain);
			for (int i = 1;i<printStuff;i++) printf("Spectrum: %i %lf\n",i,sqrt(tmpFrelements[i].amplitude)/signalLength); 
		}

		partial_sort(tmpFrelements+1,tmpFrelements+order+1,tmpFrelements+fftLength,fremenSort);
		//partial_sort(tmpFrelements+1,tmpFrelements+order+1,tmpFrelements+100,fremenSort);
		tim = timer.getTime();

		for(int i=1;i<order+1;i++){
			frelements[i-1].amplitude = sqrt(tmpFrelements[i].amplitude)/signalLength;
			frelements[i-1].phase = atan2(coeffs[tmpFrelements[i].frequency][1],coeffs[tmpFrelements[i].frequency][0]);
			frelements[i-1].frequency = tmpFrelements[i].frequency;
		}
		free(tmpFrelements);
		tim = timer.getTime();
		if (debug) cout << "Spectrum recovery time " << tim << endl;

		reconstruct(reconstructed,plan);
	}else{
		int sum = 0;
		for (int i = 0;i<signalLength;i++) sum += signal[i];
		gain = ((float) sum)/signalLength;
		if (gain < 0.5){ 
			memset(reconstructed,0,signalLength*sizeof(unsigned char)); 
		}else{
			memset(reconstructed,1,signalLength*sizeof(unsigned char));
		} 
	}
	/*calculation of the outlier set*/
	unsigned char flip = 0;
	for (int i = 0;i<signalLength;i++)
	{
		if (signal[i] != (reconstructed[i]^flip)){
			 flip = 1-flip;
			 unsigned int* outlierSetTmp = (unsigned int*)realloc(outlierSet,(outliers+1)*(sizeof(int)));
			 if (outlierSetTmp == NULL) fprintf(stderr,"Failed to reallocate the outlier set!\n"); 
			 outlierSet = outlierSetTmp;
			 outlierSet[outliers++] = i;
		}
	}
	free(reconstructed);

	return;
}

void CFrelement::update(int modelOrder,CFFTPlan *plan,int printStuff)
{
	if (order == 0 && outliers == 0){
		printf("nothing to do!\n");
	}else{
		unsigned char *reconstructed = (unsigned char*)malloc(signalLength*sizeof(unsigned char));
		reconstruct(reconstructed,plan);
		order = modelOrder;
		outliers = 0;
		free(frelements);
		frelements = (SFrelement*) malloc(order*sizeof(SFrelement));
		if (frelements == NULL) fprintf(stderr,"Failed to reallocate spectral components!\n");
		build(reconstructed,signalLength,plan,printStuff);
		free(reconstructed);
	}
}

float CFrelement::estimate(float* signal,CFFTPlan *plan,float anomalyThreshold,int signalLengthi)
{
	CTimer timer;
	timer.start();
	int fftLength = signalLengthi/2+1;

	fftw_complex *coeffs;
	double *probability;
	int ratio = signalLengthi/signalLength;
	probability = plan->probability; 
	coeffs = plan->coeffs;
	
	/*reconstructing the frequency spectrum*/
	if (order > 0){	
		memset(coeffs,0,fftLength*sizeof(fftw_complex));
		coeffs[0][0] = gain;
		for (int i=0;i<order;i++){
			coeffs[frelements[i].frequency*ratio][0] = frelements[i].amplitude*cos(frelements[i].phase);
			coeffs[frelements[i].frequency*ratio][1] = frelements[i].amplitude*sin(frelements[i].phase);
		}
		//cout << "IFFT preparation " << timer.getTime() << endl;
		fftw_execute_dft_c2r(plan->inverse,coeffs,probability);
		//for (int i = 0;i<signalLength;i++) cout << "Pro " << probability[i] << " " << estimate(i) << endl;
	}else{
		for (int i = 0;i<signalLengthi;i++) probability[i] = gain;
	}
	if (debug) cout << "IFFT calculation " << timer.getTime() << endl;
	for (int i = 0;i<signalLengthi;i++){
		signal[i] = probability[i];
		//if (signal[i]  > 1.0) signal[i] = 1.0;
		//if (signal[i]  < 0.0) signal[i] = 0.0;
	}
	return 0;
}

float CFrelement::reconstruct(unsigned char* signal,CFFTPlan *plan,bool evaluate)
{
	float evaluation = -1; 
	CTimer timer;
	timer.start();

	int fftLength = signalLength/2+1;

	fftw_complex *coeffs;
	double *probability;

	probability = plan->probability; 
	coeffs = plan->coeffs;
	
	/*reconstructing the frequency spectrum*/
	if (order > 0){	
		memset(coeffs,0,fftLength*sizeof(fftw_complex));
		coeffs[0][0] = gain;
		for (int i=0;i<order;i++){
			coeffs[frelements[i].frequency][0] = frelements[i].amplitude*cos(frelements[i].phase);
			coeffs[frelements[i].frequency][1] = frelements[i].amplitude*sin(frelements[i].phase);
		}
		//cout << "IFFT preparation " << timer.getTime() << endl;
		fftw_execute_dft_c2r(plan->inverse,coeffs,probability);
		//for (int i = 0;i<signalLength;i++) cout << "Pro " << probability[i] << " " << estimate(i) << endl;
	}else{
		for (int i = 0;i<signalLength;i++) probability[i] = gain;
	}
	if (debug) cout << "IFFT calculation " << timer.getTime() << endl;

	/*application of the outlier set*/
	int j=0;
	unsigned char flip = 0;
	timer.reset();
	if (outliers > 0){
		int flipPos = outlierSet[j];
		for (int i = 0;i<signalLength;i++)
		{
			if (flipPos == i){
				flip = 1-flip;
				j++;
				if (j >= outliers) j = outliers-1; 
				flipPos = outlierSet[j];
			}
			signal[i] = ((probability[i]>=0.5)^flip);
		}
	}else{
		for (int i = 0;i<signalLength;i++) signal[i] = probability[i]>0.5;
	}
	if (evaluate){
		 evaluation = 0;
		 for (int i = 0;i<signalLength;i++) evaluation+=fabs(signal[i]-(probability[i]>0.5));
		 evaluation/=signalLength;
	}
	if (debug) cout << "Signal reconstruction time " << timer.getTime() << endl;

	return evaluation;
}

/*gets length in terms of values measured*/
unsigned int CFrelement::getLength()
{
	return signalLength;
}

float CFrelement::evaluate()
{
	int errs = 0;
	for (int i=0;i<outliers/2;i++) errs+=(outlierSet[2*i+1]-outlierSet[2*i]);
	if (outliers%2 == 1) errs+=signalLength-outlierSet[outliers-1];
	return ((float)errs)/signalLength;
}

float CFrelement::evaluate(unsigned char* signal,int start,int length,CFFTPlan *plan)
{
	unsigned char *reconstruction=(unsigned char*)calloc(1000000,sizeof(unsigned char));
	unsigned char *recPointer;
	int numOutliers = outliers;
	outliers = 0;
	reconstruct(reconstruction,plan,false);
	outliers = numOutliers;
	recPointer = &reconstruction[start];
	int errs = 0;
	for (int i = 0;i<length;i++) errs+=abs(recPointer[i]-signal[i]);
	free(reconstruction);
	return ((float)errs)/length;
}

void CFrelement::add(unsigned char value)
{
	if (((estimate(signalLength) > 0.5)^((outliers%2)==1))!=value)
	{
		unsigned int* outlierSetTmp = (unsigned int*)realloc(outlierSet,(outliers+1)*(sizeof(unsigned int)));
		if (outlierSetTmp==NULL) fprintf(stderr,"Failed to reallocate the outlier set!\n"); 
		outlierSet = outlierSetTmp;
		outlierSet[outliers++] = signalLength;
	}
	signalLength++;
	return; 
}

/*text representation of the fremen model*/
void CFrelement::print(bool verbose)
{
	int errs = 0;
	for (int i=0;i<outliers/2;i++) errs+=(outlierSet[2*i+1]-outlierSet[2*i]);
	if (outliers%2 == 1) errs+=signalLength-outlierSet[outliers-1];
	std::cout << "model order " << (int)order << " prior: " << gain << " error: " << ((float)errs/signalLength) << " size: " << signalLength << " ";
	if (order > 0) std::cout  << endl;
	if (verbose||true){
		for (int i = 0;i<order;i++){
			std::cout << "frelement " << i << " " << frelements[i].amplitude << " " << frelements[i].phase << " " << frelements[i].frequency << " " << endl;
		}
	}
	if (verbose){
	std::cout << "outlier set size " << outliers << ":";
		for (int i = 0;i<outliers;i++) std::cout << " " << outlierSet[i];
	}
	std::cout << endl; 
}


/*retrieves a boolean*/
unsigned char CFrelement::retrieve(int timeStamp)
{
	int i = 0;
	for (i= 0;i<outliers;i++){
		if (timeStamp < outlierSet[i]) break;
	}
	return (estimate(timeStamp) >= 0.5)^(i%2);
}
 
int CFrelement::save(char* name,bool lossy)
{
	FILE* file = fopen(name,"w");
	int size = fwrite(&signalLength,sizeof(unsigned int),1,file)*sizeof(unsigned int);
	size += save(file,lossy);
	fclose(file);
	return size;
}

int CFrelement::load(char* name)
{
	FILE* file = fopen(name,"r");
	if (fread(&signalLength,sizeof(unsigned int),1,file)!=1) return -1;
	load(file);
	fclose(file);
	return 0;
}

int CFrelement::save(FILE* file,bool lossy)
{
	unsigned int outlierNum = outliers;
	unsigned char code = 255;
	int size = 0;
	if (order == 0 && outliers == 0)
	{
		if (gain == 1) code=254;
		size+=fwrite(&code,sizeof(unsigned char),1,file)*sizeof(unsigned char);
	}else{ 
		if (lossy) outlierNum = 0; 
		size+=fwrite(&order,sizeof(unsigned char),1,file)*sizeof(unsigned char);
		size+=fwrite(&outlierNum,sizeof(unsigned int),1,file)*sizeof(unsigned int);
		size+=fwrite(&gain,sizeof(float),1,file)*sizeof(float);
		size+=fwrite(frelements,sizeof(SFrelement),order,file)*sizeof(SFrelement);
		size+=fwrite(outlierSet,sizeof(unsigned int),outlierNum,file)*sizeof(unsigned int);;
	}
	return size;
}

int CFrelement::load(FILE* file)
{
	int ret =0;
	ret+=fread(&order,sizeof(unsigned char),1,file);
	if (order>250){
		gain = 0;
		if (order == 254) gain = 1;
		outliers = 0;
		order = 0;
		if (ret != 1) ret = -1; else ret = 0;
	}else{
		ret+=fread(&outliers,sizeof(unsigned int),1,file);
		ret+=fread(&gain,sizeof(float),1,file);
		free(outlierSet);
		free(frelements);
		frelements = (SFrelement*) malloc(order*sizeof(SFrelement));
		outlierSet = (unsigned int*)malloc(outliers*(sizeof(unsigned int)));
		ret+=fread(frelements,sizeof(SFrelement),order,file);
		ret+=fread(outlierSet,sizeof(unsigned int),outliers,file);
		if (ret != 4+outliers+order) ret = -1; else ret = 0;
	}
	return ret;
}

float CFrelement::estimate(int timeStamp)
{
	float time = (float)timeStamp/signalLength;
	float estimate = gain-0.01;
	for (int i = 0;i<order;i++){
		estimate+=2*frelements[i].amplitude*cos(time*frelements[i].frequency*2*M_PI+frelements[i].phase);
	}
	if (estimate > 1.0) return 1.0;
	if (estimate < 0.0) return 0.0;
	return estimate;
}

float CFrelement::fineEstimate(float timeStamp)
{
	float time = (float)timeStamp/signalLength;
	float estimate = gain;
	for (int i = 0;i<order;i++){
		estimate+=2*frelements[i].amplitude*cos(time*frelements[i].frequency*2*M_PI+frelements[i].phase);
	}
	if (estimate > 1.0) return 1.0;
	if (estimate < 0.0) return 0.0;
	return estimate;
}
