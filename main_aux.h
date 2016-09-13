
#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_


#include <stdio.h>
#include <stdlib.h>
#include "SPConfig.h"
#include "SPPoint.h"

#define EXIT_MSG "Exiting...\n"

typedef struct image* Image;

void writeFeatsToFile(FILE* writerFile,SPPoint* point,int numOfFeats);

SPPoint* readFeatsFromFile(FILE* readerFile,int* numOfFeatsPerImage);

int compareImagesByHits (const void * a, const void * b);

#endif /* MAIN_AUX_H_ */
