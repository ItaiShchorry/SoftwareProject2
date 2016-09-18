
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


void leaveFunc(char* configPath, char* path, SPPoint** imagesPointsArray, SPPoint* imagesForTreeInit){
	free(configPath);
	free(path);
	free(imagesPointsArray);
	free(imagesForTreeInit);
	spLoggerDestroy();
}

void loopForQueries(KDTreeNode head, char* path, int numOfImages, int numOfExtFeats, int numOfSimilarImages, SPConfig config, SP_CONFIG_MSG* configMsg, ImageProc proc){
	int max;
	int maxIndex;
	int j = 0;
	int i = 0;
	int* nearestFeatureCnt;
	int* mostRankedFeatures;
	SPBPQueue bpq;
	while(1){

				printf("Please enter an image path:\n");
				fflush(stdout);
				strcpy(path ,"C:\\Users\\mally\\workspace2\\FinalProject\\images\\img0.png");
		/*		scanf("%s",path);*/

				//Check if the user entered the exit string <>
				if(!strcmp(path,"<>"))
				{
					printf(EXIT_MSG);
					break;
				}

				nearestFeatureCnt = (int*)malloc(sizeof(int)*numOfImages);
				mostRankedFeatures = (int*)malloc(sizeof(int)*numOfSimilarImages);

				for(i=0;i<numOfImages;i++){//initialize to -1 hits per image
					nearestFeatureCnt[i] = -1;
				}

				SPPoint* featuresOfQueryImage = proc.getImageFeatures(path,numOfImages,&numOfExtFeats);
				for (i = 0; i < numOfExtFeats; ++i) { //count hits per image
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
							max = nearestFeatureCnt[j];
							maxIndex = j;
						}
					}
					nearestFeatureCnt[maxIndex] = -1;
					mostRankedFeatures[i] = maxIndex;
				}

				free(featuresOfQueryImage);

				if(spConfigMinimalGui(config,configMsg)){ // if we are in minimal-GUI mode
					for (i = 0; i <numOfSimilarImages ; ++i) {
						spConfigGetImagePath(path,config,mostRankedFeatures[i]);
						proc.showImage(path);
					}
				}
				else{ // not in minimal-GUI mode
					printf("Best candidates for - %s - are:\n",path);
					for (i = 0; i < numOfSimilarImages; ++i) {
						spConfigGetImagePath(path,config,mostRankedFeatures[i]);
						printf("%s\n",path);
					}


				}
				free(nearestFeatureCnt);
				free(mostRankedFeatures);

/*
				Image images[numOfImages];
				//initialize all the image structs
				for (int i=0 ; i<numOfImages ; i++)
				{
					images[i]->index = i;
					images[i]->hits = 0;
				}

				SPPoint* featsOfQueryImage = proc.getImageFeatures(path,numOfImages,&numOfExtFeats);
				SPBPQueue queue = spBPQueueCreate(numOfSimilarImages);

				//Go over all the features extracted
				for (int i = 0; i < numOfExtFeats; i++)
				{
					//Find the hits with all the images
					KNearestNeighbors(head,queue,*(featsOfQueryImage+i));
					while(!spBPQueueIsEmpty(queue))
					{
						//increase the the hits count of the image which matches in feature
						SPListElement temp = spBPQueuePeek(queue);
						images[spListElementGetIndex(temp)]->hits++;
						spBPQueueDequeue(queue);
						free(temp); //peeking makes a copy.
					}
					spBPQueueClear(queue);
				}

				//Sort all the images according to hits
				qsort(&images,numOfImages,sizeof(Image),compareImagesByHits);
				free(featsOfQueryImage); //probably need to free all items.


			*/


			}
}

int main(int argc, char *argv[])
{
	char* configPath = (char*) malloc(sizeof(char)*MAX_LEN);
	SP_CONFIG_MSG configMsg;
	SPConfig config;
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
		leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
		return 0;
	}

	//Creating the config file
	config = spConfigCreate(configPath,&configMsg);
	//Checking if creating it failed
	if (config == NULL && configMsg == SP_CONFIG_CANNOT_OPEN_FILE)
	{
		printFailedCreatingMessage(configPath);
		leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
		return 0;
	}

	//Creating the log file and logger object
	spLoggerCreate(spConfigGetLoggerFilename(config),spConfigGetLoggerLevel(config));

	//Create the image processing object
	ImageProc proc = ImageProc(config);

	//Getting the images' details
	getNumOfImagesWrapper(&numOfImages, config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS){
		leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
		return 0;
	}

	int numOfFeatsPerImage[numOfImages];
/*
	getNumOfFeatsWrapper(&numOfFeats, config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS){
		leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
		return 0;
	}
*/

/*	getNumOfSimilarImagesWrapper(&numOfSimilarImages, config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS){
		leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
		return 0;
	}*/

	//Create array to store the images
	imagesPointsArray = (SPPoint**)malloc(numOfImages*sizeof(SPPoint*));
	//Check if we are in extraction mode
	if (spConfigIsExtractionMode(config,&configMsg)) {
		for (int i = 0; i < numOfImages; i++)  //Go over all of the images and write the feats file
		{
			getImagePathWrapper(i, &configMsg, path, config, writingFile);
			if (configMsg != SP_CONFIG_SUCCESS){
				leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
				return 0;
			}

			*(imagesPointsArray+i) = proc.getImageFeatures(path,i,&numOfExtFeats); 	//extracting the image features
			totalNumOfFeats += numOfExtFeats;
			numOfFeatsPerImage[i] = numOfExtFeats;
			getFeatsPathWrapper(i, &configMsg, path, config, writingFile); 	//Getting the feats file path
			if (configMsg != SP_CONFIG_SUCCESS){
				leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
				return 0;
			}
			writingFile = fopen(path,"w");
			if (writingFile == NULL)
			{
				spLoggerPrintError("Failed opening the file for writing feats","main.c",__func__,__LINE__);
				leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
				return 0;
			}
			writeFeatsToFile(writingFile,*(imagesPointsArray+i),numOfExtFeats);
		}

			fclose(writingFile);

			imagesForTreeInit = (SPPoint*)malloc(sizeof(SPPoint)*totalNumOfFeats);
			for (int i = 0; i < numOfImages; i++) {
				for (int j = 0; j < numOfFeatsPerImage[i]; j++)
				{
					*(imagesForTreeInit+count) = spPointCopy(imagesPointsArray[i][j]);
					count++;
				}
			}
		}
		else //non-Extraction mode
		{
			//Go over all the images
			for (int i = 0; i < numOfImages; i++)
			{
				//Get the feats path of each image
				getFeatsPathWrapper(i, &configMsg, path, config, writingFile); 	//Getting the feats file path
				if (configMsg != SP_CONFIG_SUCCESS){
					leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
					return 0;
				}

				readingFile = fopen(path,"r");
				if (readingFile == NULL)
				{
					spLoggerPrintError("Failed opening the file for writing feats","main.c",__func__,__LINE__);
					leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
					return 0;
				}
				*(imagesPointsArray+i) = readFeatsFromFile(readingFile,(numOfFeatsPerImage+i));
				totalNumOfFeats += *(numOfFeatsPerImage+i);
			}

			fclose(readingFile);
			imagesForTreeInit = (SPPoint*)malloc(sizeof(SPPoint)*totalNumOfFeats);

			for (int i = 0; i < numOfImages; i++)
			{
				for (int j = 0; j < *(numOfFeatsPerImage+i); j++)
				{
					imagesForTreeInit[count] = spPointCopy(imagesPointsArray[i][j]);
					count++;
				}
			}
		}

		//Initialize KDtree of images
		kdArr = init(imagesForTreeInit, totalNumOfFeats);
		if (kdArr == NULL)
		{
			spLoggerPrintError("Failed to load images to data structure","main.c",__func__,__LINE__);
			leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
			return 0;
		}
		splitMethod = SPConfigGetSplitMethod(config, &configMsg);
		if(configMsg != SP_CONFIG_SUCCESS){
			spLoggerPrintError("problem with getting the split method from configuration","main.c",__func__,__LINE__);
			leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
			return 0;
		}
		head = buildKDTree(kdArr, config, splitMethod);
		if (head == NULL)
		{
			spLoggerPrintError("Failed to load images to data structure","main.c",__func__,__LINE__);
			leaveFunc(configPath, path, imagesPointsArray, imagesForTreeInit);
			return 0;
		}
		//loop for receiving a query image from the user
		//This loop keeps going until the user enters the exit string <>
		loopForQueries(head, path, numOfImages, numOfExtFeats, numOfSimilarImages, config, &configMsg, proc);

	return 0;
}


