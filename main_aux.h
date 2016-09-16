
#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_


#include <stdio.h>
#include <stdlib.h>
#include "KDTree.h"

#define EXIT_MSG "Exiting...\n"

typedef struct image* Image;

/*wrap function for getting numOfImages */
void getNumOfImagesWrapper(int* numOfImages, SPConfig config, SP_CONFIG_MSG* configMsg);

/*wrap function for getting numOfFeats */
void getNumOfFeatsWrapper(int* numOfFeats, SPConfig config, SP_CONFIG_MSG* configMsg);

/*wrap function for getting numOfSimilarImages */
void getNumOfSimilarImagesWrapper(int* NumOfSimilarImages, SPConfig config, SP_CONFIG_MSG* configMsg);

void writeFeatsToFile(FILE* writerFile,SPPoint* point,int numOfFeats);

SPPoint* readFeatsFromFile(FILE* readerFile,int* numOfFeatsPerImage);

int compareImagesByHits (const void * a, const void * b);

/*helper function to print correct message for failing to create configuration file*/
void printFailedCreatingMessage(char* configPath);

/*wrap function for getting imagePath */
void getImagePathWrapper(int i, SP_CONFIG_MSG* configMsg, char* path, SPConfig config, FILE* writingFile);

/*wrap function for getFeatsPath function */
void getFeatsPathWrapper(int i, SP_CONFIG_MSG* configMsg, char* path, SPConfig config, FILE* writingFile);

#endif /* MAIN_AUX_H_ */
