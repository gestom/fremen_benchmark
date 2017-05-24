#include "CFrePoisson.h"

using namespace std;

int calculateRate(float shape, float scale) 
{
	return (shape - 1.0) / scale;
}

CFrePoisson::CFrePoisson(const char* name)
{
	strcpy(id,name);
    printf("CFrePoisson");

	//initialization of the frequency set
	fremen = new CFrelement(name);
    order = 0;
    measurements = 0;
    max_addition = 10;
    max_iteration = 1000;
	firstTime = fremen->firstTime;
	lastTime = fremen->lastTime;
	type = TT_FREPOISSON;
}

void CFrePoisson::init(int iMaxPeriod,int elements,int numActivities)
{
    fremen->init(iMaxPeriod, elements, numActivities);
	maxPeriod = fremen->maxPeriod;
	numElements = fremen->numElements;
	rates = (PoissonRate*)malloc(sizeof(PoissonRate)*maxPeriod);
	for (int i=0;i<maxPeriod;i++) {
        rates[i].shape = 1.1;
        rates[i].scale = 1.0; 
        rates[i].rate = calculateRate(1.1, 1.0);
    }
	predictFrelements = (SFrelement*)malloc(sizeof(SFrelement)*numElements);
    for (int i=0;i<numElements;i++) predictFrelements[i].amplitude = predictFrelements[i].phase = 0; 
	for (int i=0;i<numElements;i++) predictFrelements[i].period = (maxPeriod)/(i+1);
}

CFrePoisson::~CFrePoisson()
{
	free(rates);
	free(fremen);
    free(predictFrelements);
}

// adds new state observations at given times
int CFrePoisson::add(uint32_t time,float state)
{
    if (firstTime == -1)
        firstTime = time;

    rates[measurements % maxPeriod].shape += state;
    rates[measurements % maxPeriod].scale += 1.0;
    rates[measurements % maxPeriod].rate = calculateRate(
        rates[measurements % maxPeriod].shape,  
        rates[measurements % maxPeriod].scale
    );

    lastTime = time;
	measurements++;
	return 0; 
}

/*not required in incremental version*/
void CFrePoisson::update(int modelOrder)
{
    // copy rates to temporary array for calculation
    float temp[maxPeriod];
    for (int i = 0; i < maxPeriod; i++)
        temp[i] = rates[i].rate;

    // get significant frequencies
    order = modelOrder;
    int exit_counter = max_iteration;
    int freq_counter = 0;
    int freq_indices [numElements][2]; // [period][numOfOccurrence]
    for (int i = 0; i < numElements; i++) {
            freq_indices[i][0] = -1;
            freq_indices[i][1] = 0;
    }
    while (freq_counter < (order*2)){
        // find the highest frequency
        for (int i = 0;i<maxPeriod;i++)
            fremen->add(i,temp[i]);
        fremen->update(1);
        // substract data with wave from the highest frequency
        for (int time = 0;time<maxPeriod;time++)
            temp[time] -= fremen->predictFrelements[0].amplitude*cos(time/fremen->predictFrelements[0].period*2*M_PI-fremen->predictFrelements[0].phase);
        // stack by amplitude the highest frequency
        for (int i = 0;i<numElements;i++) {
            if (predictFrelements[i].period == fremen->predictFrelements[0].period) {
                if (freq_counter > 0) {
                    int j = 0;
                    bool new_added_order = true;
                    while (j < freq_counter) {
                        if (freq_indices[j][0] == predictFrelements[i].period) {
                            if (freq_indices[j][1] <= max_addition) {
                                predictFrelements[i].amplitude += fremen->predictFrelements[0].amplitude;
                                predictFrelements[i].phase = ((float)freq_indices[j][1] * predictFrelements[i].phase) / (float)(freq_indices[j][1] + 1);
                                freq_indices[j][1] += 1;
                            }
                            new_added_order = false;
                        }
                        j++;
                    }
                    if (new_added_order == true) {
                        freq_indices[j][0] = predictFrelements[i].period;
                        freq_indices[j][1] = 1;
                        predictFrelements[i].amplitude = fremen->predictFrelements[0].amplitude;
                        predictFrelements[i].phase = fremen->predictFrelements[0].phase;
                        freq_counter++;
                    }
                }
                else {
                    freq_indices[0][0] = predictFrelements[i].period;
                    freq_indices[0][1] = 1;
                    predictFrelements[i].amplitude = fremen->predictFrelements[0].amplitude;
                    predictFrelements[i].phase = fremen->predictFrelements[0].phase;
                    freq_counter++;
                }
            }
        }
        exit_counter++;
        if (exit_counter >= max_iteration)
            break;
    }
}

/*text representation of the fremen model*/
void CFrePoisson::print(bool verbose)
{
	std::cout << "Model: " << id << " Prior: " << fremen->predictGain << " Size: " << measurements << " ";
	if (verbose){
		for (int i = 0;i<order;i++){
			std::cout << "Frelement " << i << " " << predictFrelements[i].amplitude << " " << predictFrelements[i].phase << " " << predictFrelements[i].period << " ";
		}
	}
	std::cout << endl; 
}

float CFrePoisson::estimate(uint32_t time)
{
	float saturation = 0.01;
	float estimate =  fremen->predictGain;
	for (int i = 0;i<order;i++) estimate+=2*predictFrelements[i].amplitude*cos(time/predictFrelements[i].period*2*M_PI-predictFrelements[i].phase);
	if (estimate > 1.0-saturation) estimate =  1.0-saturation;
	if (estimate < 0.0+saturation) estimate =  0.0+saturation;
	return estimate;
}

float CFrePoisson::predict(uint32_t time)
{
	float saturation = 0.01;
	float estimate =  fremen->predictGain;
	for (int i = 0;i<order;i++) estimate+=2*predictFrelements[i].amplitude*cos(time/predictFrelements[i].period*2*M_PI-predictFrelements[i].phase);
	if (estimate > 1.0-saturation) estimate =  1.0-saturation;
	if (estimate < 0.0+saturation) estimate =  0.0+saturation;
	return estimate;
}

int CFrePoisson::save(char* name,bool lossy)
{
	FILE* file = fopen(name,"w");
	save(file);
	fclose(file);
	return 0;
}

int CFrePoisson::load(char* name)
{
	FILE* file = fopen(name,"r");
	load(file);
	fclose(file);
	return 0;
}


int CFrePoisson::save(FILE* file,bool lossy)
{
	int frk = numElements;
	fwrite(&frk,sizeof(uint32_t),1,file);
	fwrite(&fremen->storedGain,sizeof(float),1,file);
	fwrite(predictFrelements,sizeof(SFrelement),numElements,file);
	return 0;
}

int CFrePoisson::load(FILE* file)
{
	int frk = numElements;
	fwrite(&frk,sizeof(uint32_t),1,file);
	fwrite(&fremen->storedGain,sizeof(float),1,file);
	fwrite(predictFrelements,sizeof(SFrelement),numElements,file);
	return 0;
}


