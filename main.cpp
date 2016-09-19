
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include <vector>
#include <limits.h>
#include "SPImageProc.h"
#include <cstdbool>
#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C"{
#include "main_aux.h"
#include "SPLogger.h"
}
using namespace std;
using namespace cv;
using namespace sp;
//Struct for sorting images according to hits
struct image
{
	int index;
	int hits;
};


void leaveFunc(KDTreeNode head, char* configPath, char* path, SPPoint** imagesPointsArray,int numOfImages
		, int* numOfFeatsPerImage, SPPoint* imagesForTreeInit, int total, SPConfig config){
	int i;
	int j;
	free(configPath);
	free(path);
	if(imagesPointsArray != NULL){
		for(i=0; i<numOfImages; i++){
			for(j=0; j<numOfFeatsPerImage[i]; j++){
				spPointDestroy(imagesPointsArray[i][j]);
			}
			free(imagesPointsArray + i);
		}
		free(imagesPointsArray);
	}
	if(imagesForTreeInit != NULL){
		for(i=0; i<total; i++) spPointDestroy(*(imagesForTreeInit+i));
		free(imagesForTreeInit);
	}
	spConfigDestroy(config);
	if(head != NULL)KDTreeDestroy(head);
	spLoggerDestroy();
}

void loopForQueries(KDTreeNode head, int numOfImages, int numOfSimilarImages, SPConfig config, SP_CONFIG_MSG* configMsg, ImageProc proc){
	int queryNumOfFeats = 0;
	char* path = (char*)malloc(MAX_LEN*sizeof(char));
	int max;
	int maxIndex;
	int j = 0;
	int i = 0;
	int numOfTriesForEnteringPath = 0;
	int* nearestFeatureCnt;
	int* mostRankedFeatures;
	SPBPQueue bpq;
	SPPoint* featuresOfQueryImage;
	while(1){

				printf("Please enter an image path:\n");
				fflush(stdout);
				scanf("%s",path);
				//Check if the user entered the exit string <>
				featuresOfQueryImage = proc.getImageFeatures(path,numOfImages,&queryNumOfFeats);
				if(!strcmp(path,"<>"))
				{
					printf(EXIT_MSG);
					break;
				}
				while((featuresOfQueryImage == NULL)){
					if(numOfTriesForEnteringPath > 4){
						spLoggerPrintError("exceeded number of tries for entering image path. program will exit\n","main.c",__func__,__LINE__);
						printf(EXIT_MSG);
						break;
					}
					numOfTriesForEnteringPath++;
					spLoggerPrintWarning("invalid image path, please try entering a new path:\n","main.c",__func__,__LINE__);
					scanf("%s",path);
					if(!strcmp(path,"<>"))
					{
						printf(EXIT_MSG);
						break;
					}
					featuresOfQueryImage = proc.getImageFeatures(path,numOfImages,&queryNumOfFeats);
				}

				//allocate memory for helper variables
				nearestFeatureCnt = (int*)malloc(sizeof(int)*numOfImages);
				mostRankedFeatures = (int*)malloc(sizeof(int)*numOfSimilarImages);

				for(i=0;i<numOfImages;i++){ //initialize count to -1
					nearestFeatureCnt[i] = -1;
				}

				//find k nearest neighbors according to each feature of query images.
				for (i = 0; i < queryNumOfFeats; ++i) {
					bpq = spBPQueueCreate(numOfSimilarImages);
					KNearestNeighbors(head, bpq, *(featuresOfQueryImage+i));
					while(!spBPQueueIsEmpty(bpq)){
						int tempIndex = spListElementGetIndex(spBPQueuePeek(bpq));
						nearestFeatureCnt[tempIndex]++;
						spBPQueueDequeue(bpq);
					}

					spBPQueueDestroy(bpq);
				}

				//choose most appeared features
				for (i = 0; i < numOfSimilarImages; ++i) {
					max = -1;
					maxIndex = -1;
					for (j = 0; j < numOfImages; ++j) {
						if(nearestFeatureCnt[j] > max){
							maxIndex = j;
							max = nearestFeatureCnt[j];
						}
					}
					nearestFeatureCnt[maxIndex] = -1;
					mostRankedFeatures[i] = maxIndex;
				}

				for(i=0; i<queryNumOfFeats; i++){
					spPointDestroy(*(featuresOfQueryImage+i));
				}
				free(featuresOfQueryImage);

				//displaying results in wanted manner according to configuration file
				if(spConfigMinimalGui(config,configMsg)){
					for (i = 0; i <numOfSimilarImages ; i++) {
						spConfigGetImagePath(path,config,mostRankedFeatures[i]);
						proc.showImage(path);
					}
				}
				else{
					printf("Best candidates for - %s - are:\n",path);
					fflush(NULL);
					for (i = 0; i < numOfSimilarImages; i++) {
						spConfigGetImagePath(path,config,mostRankedFeatures[i]);
						printf("%s\n",path);
						fflush(NULL);
					}


				}
				free(nearestFeatureCnt);
				free(mostRankedFeatures);
			}
	free(path);
}

int main(int argc, char *argv[])
{

	char* configPath = (char*) malloc(sizeof(char)*MAX_LEN);
	SP_CONFIG_MSG configMsg;
	SPConfig config = NULL;
	int numOfImages = 0;
	int count=0;
	int totalNumOfFeats = 0;
	int numOfExtFeats = 0;
	int numOfSimilarImages=0;
	char* path = (char*) malloc(sizeof(char)*MAX_LEN);
	SPPoint** imagesPointsArray = NULL;
	SPPoint* imagesForTreeInit = NULL;
	FILE* writingFile = NULL;
	FILE* readingFile = NULL;
	KDArray kdArr = NULL;
	SP_TREE_SPLIT_METHOD splitMethod;
	KDTreeNode head = NULL;

	//Check the number of arguments the user entered
	if(argc == 3)//The user entered command line arguments, meaning the config file path
	{
		if(!strcmp(argv[1],"-c"))
		{
			configPath = argv[2];
		}
	}
	else if(argc == 1)//The user DIDN'T enter a config file path, so we take the default
	{
		strcpy(configPath,"spcbir.config");
	}
	else //The user entered more than 2 command line arguments, error
	{
		printf("Invalid command line : use -c <config_filename>\n");
		leaveFunc(head, configPath, path, imagesPointsArray, numOfImages, NULL, imagesForTreeInit, totalNumOfFeats, config);
		return 0;
	}

	//Creating the config file
	config = spConfigCreate(configPath,&configMsg);
	//Checking if creating it failed
	if (config == NULL && configMsg == SP_CONFIG_CANNOT_OPEN_FILE)
	{
		printFailedCreatingMessage(configPath);
		leaveFunc(head, configPath, path, imagesPointsArray, numOfImages, NULL, imagesForTreeInit, totalNumOfFeats, config);
		return 0;
	}

	//Creating the log file and logger object
	spLoggerCreate(spConfigGetLoggerFilename(config),spConfigGetLoggerLevel(config));

	//Create the image processing object
	ImageProc proc = ImageProc(config);

	//Getting the images' details
	getNumOfImagesWrapper(&numOfImages, config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS){
		leaveFunc(head, configPath, path, imagesPointsArray, numOfImages, NULL, imagesForTreeInit, totalNumOfFeats, config);
		return 0;
	}

	int numOfFeatsPerImage[numOfImages];

	//Create array to store the images
	imagesPointsArray = (SPPoint**)malloc(numOfImages*sizeof(SPPoint*));
	//Check if we are in extraction mode
	if (spConfigIsExtractionMode(config,&configMsg)) {
		for (int i = 0; i < numOfImages; i++)  //Go over all of the images and write the feats file
		{
			getImagePathWrapper(i, &configMsg, path, config, writingFile);
			if (configMsg != SP_CONFIG_SUCCESS){
				leaveFunc(head, configPath, path, imagesPointsArray, i, numOfFeatsPerImage, imagesForTreeInit, totalNumOfFeats, config);
				return 0;
			}

			*(imagesPointsArray+i) = proc.getImageFeatures(path,i,&numOfExtFeats); 	//extracting the image features
			totalNumOfFeats += numOfExtFeats;
			numOfFeatsPerImage[i] = numOfExtFeats;
			getFeatsPathWrapper(i, &configMsg, path, config, writingFile); 	//Getting the feats file path
			if (configMsg != SP_CONFIG_SUCCESS){
				leaveFunc(head, configPath, path, imagesPointsArray, i, numOfFeatsPerImage, imagesForTreeInit, totalNumOfFeats, config);
				return 0;
			}
			writingFile = fopen(path,"w");
			if (writingFile == NULL)
			{
				spLoggerPrintError("Failed opening the file for writing feats","main.c",__func__,__LINE__);
				leaveFunc(head, configPath, path, imagesPointsArray, i, numOfFeatsPerImage, imagesForTreeInit, totalNumOfFeats, config);
				return 0;
			}
			writeFeatsToFile(writingFile,*(imagesPointsArray+i),numOfExtFeats);
		}

			fclose(writingFile);
		}
		else //non-Extraction mode
		{
			//Go over all the images
			for (int i = 0; i < numOfImages; i++)
			{
				//Get the feats path of each image
				getFeatsPathWrapper(i, &configMsg, path, config, writingFile); 	//Getting the feats file path
				if (configMsg != SP_CONFIG_SUCCESS){
					leaveFunc(head, configPath, path, imagesPointsArray, 0 ,NULL, imagesForTreeInit, totalNumOfFeats, config);
					return 0;
				}

				readingFile = fopen(path,"r");
				if (readingFile == NULL)
				{
					spLoggerPrintError("Failed opening the file for writing feats","main.c",__func__,__LINE__);
					leaveFunc(head, configPath, path, imagesPointsArray,0 ,NULL, imagesForTreeInit, totalNumOfFeats, config);
					return 0;
				}
				*(imagesPointsArray+i) = readFeatsFromFile(readingFile,(numOfFeatsPerImage+i));
				totalNumOfFeats += *(numOfFeatsPerImage+i);
			}

			fclose(readingFile);
		}

		imagesForTreeInit = (SPPoint*)malloc(sizeof(SPPoint)*totalNumOfFeats);
		for (int i = 0; i < numOfImages; i++)
		{
			for (int j = 0; j < *(numOfFeatsPerImage+i); j++)
			{
				*(imagesForTreeInit+count) = spPointCopy(imagesPointsArray[i][j]);
				count++;
			}
		}
		//Initialize KDtree of images
		kdArr = init(imagesForTreeInit, totalNumOfFeats);
		if (kdArr == NULL)
		{
			spLoggerPrintDebug("Failed to load images to data structure","main.c",__func__,__LINE__);
			leaveFunc(head, configPath, path, imagesPointsArray, numOfImages, numOfFeatsPerImage, imagesForTreeInit, totalNumOfFeats, config);
			return 0;
		}
		splitMethod = SPConfigGetSplitMethod(config, &configMsg);
		if(configMsg != SP_CONFIG_SUCCESS){
			spLoggerPrintDebug("problem with getting the split method from configuration","main.c",__func__,__LINE__);
			leaveFunc(head, configPath, path, imagesPointsArray,numOfImages, numOfFeatsPerImage, imagesForTreeInit, totalNumOfFeats, config);
			return 0;
		}
		head = buildKDTree(kdArr, config, splitMethod);
		KDArrayDestroy(kdArr); // KDArray no longer needed;
		if (head == NULL)
		{
			spLoggerPrintDebug("Failed to load images to data structure","main.c",__func__,__LINE__);
			leaveFunc(head, configPath, path, imagesPointsArray,numOfImages, numOfFeatsPerImage, imagesForTreeInit, totalNumOfFeats, config);
			return 0;
		}

		getNumOfSimilarImagesWrapper(&numOfSimilarImages, config, &configMsg);
		if (configMsg != SP_CONFIG_SUCCESS){
			leaveFunc(head, configPath, path, imagesPointsArray,numOfImages, numOfFeatsPerImage, imagesForTreeInit, totalNumOfFeats, config);
			return 0;
		}

		//loop for receiving a query image from the user
		//This loop keeps going until the user enters the exit string <>
		loopForQueries(head, numOfImages, numOfSimilarImages, config, &configMsg, proc);
		leaveFunc(head, configPath, path, imagesPointsArray,numOfImages, numOfFeatsPerImage, imagesForTreeInit, totalNumOfFeats, config);
	return 0;
}


