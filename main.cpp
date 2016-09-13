/*

#include <stdio.h>
#include "main_aux.h"
//#include "sp_image_proc_util.h"

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
#include "SPConfig.h"
#include "SPLogger.h"
#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include "main_aux.h"
#include "KDTree.h"

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

int main(int argc, char *argv[])
{
	//Creating the log file and logger object
	spLoggerCreate("SPCBIR Log",SP_LOGGER_INFO_WARNING_ERROR_LEVEL);

	//MUST declare all variables before first label jump, otherwise it's a compiler error
	char* configPath = (char*) malloc(sizeof(char)*MAX_LEN);
	SP_CONFIG_MSG configMsg;
	SPConfig config;
	ImageProc proc = NULL;
	int numOfImages, numOfFeats,numOfExtFeats,numOfSimilarImages=spConfigGetNumOfSimilarImages(config,&configMsg),totalNumOfFeats,count=0;
	int numOfFeatsPerImage[numOfImages];
	char* path = (char*) malloc(sizeof(char)*MAX_LEN);
	SPPoint** imagesPointsArray;
	SPPoint* imagesForTreeInit;
	FILE* writingFile;
	FILE* readingFile;

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
		goto leave;
	}

	//Creating the config file
	config = spConfigCreate(configPath,&configMsg);
	//Checking if creating it failed
	if (config == NULL && configMsg == SP_CONFIG_CANNOT_OPEN_FILE)
	{
		//Checking if the config file is the default file
		if(!strcmp(configPath,"spcbir.config"))
		{
			printf("The default configuration file spcbir.config couldn't be opened\n");
			goto leave;
		}
		//Getting here means the config file was input by the user
		else
		{
			char stringToPrint[MAX_LEN];
			sprintf(stringToPrint,"%s%s%s","The configuration file ",configPath," couldn't be open\n");
			printf(stringToPrint);
			goto leave;
		}

	}
	//Create the image processing object
	proc = ImageProc(config);
	//Getting the images' details
	numOfImages = spConfigGetNumOfImages(config,&configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
		{
		spLoggerPrintError("Getting spConfigGetNumOfImages failed","main.c",__func__,__LINE__);
		goto leave;
		}
	numOfFeats = spConfigGetNumOfFeatures(config,&configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
		{
			spLoggerPrintError("Getting spConfigGetNumOfFeatures failed","main.c",__func__,__LINE__);
			goto leave;
		}
	numOfSimilarImages = spConfigGetNumOfSimilarImages(config,&configMsg);
	if (configMsg != SP_CONFIG_SUCCESS)
		{
			spLoggerPrintError("Getting spConfigGetNumOfSimilarImages failed","main.c",__func__,__LINE__);
			goto leave;
		}
	//Create array to store the images
	imagesPointsArray = (SPPoint**)malloc(numOfImages*sizeof(SPPoint*));
	//Check if we are in extraction mode
	if (spConfigIsExtractionMode(config,&configMsg))
		{
		//Go over all of the images and write the feats file
		for (int i = 0; i < numOfImages; i++)
		{
			configMsg = spConfigGetImagePath (path,config,i);
			//Checking if getting the image path failed
			if (configMsg != SP_CONFIG_SUCCESS)
			{
				char stringToPrint[MAX_LEN];
				sprintf(stringToPrint,"%s%d%s","Getting the image path of image number ",i," failed");
				spLoggerPrintError(stringToPrint,"main.c",__func__,__LINE__);
				fclose(writingFile);
				goto leave;
			}
			//extracting the image features
			*(imagesPointsArray+i) = proc.getImageFeatures(path,i,&numOfExtFeats);
			totalNumOfFeats += numOfExtFeats;
			*(numOfFeatsPerImage+i) = numOfExtFeats;
			//Getting the feats file path
			configMsg = spConfigGetFeatsPath(path,config,i);
			//Checking if getting the feats file path failed
			if (configMsg != SP_CONFIG_SUCCESS)
			{
				char stringToPrint[MAX_LEN];
				sprintf(stringToPrint,"%s%d%s","Getting the feats path of image number ",i," failed");
				spLoggerPrintError(stringToPrint,"main.c",__func__,__LINE__);
				fclose(writingFile);
				goto leave;
			}
			writingFile = fopen(path,"w");
			if (writingFile == NULL)
			{
				spLoggerPrintError("Failed opening the file for writing feats","main.c",__func__,__LINE__);
				goto leave;
			}
			writeFeatsToFile(writingFile,*(imagesPointsArray+i),numOfExtFeats);
		}

			fclose(writingFile);

			imagesForTreeInit = (SPPoint*)malloc(sizeof(SPPoint)*totalNumOfFeats);

			for (int i = 0; i < numOfImages; i++)
			{
				for (int j = 0; j < numOfFeatsPerImage[i]; j++)
				{
					imagesForTreeInit[count] = spPointCopy(imagesPointsArray[i][j]);
					count++;
				}
			}
			free(imagesPointsArray);
		}
		else //non-Extraction mode
		{
			//Go over all the images
			for (int i = 0; i < numOfImages; i++)
			{
				//Get the feats path of each image
				configMsg = spConfigGetFeatsPath(path,config,i);
				if (configMsg != SP_CONFIG_SUCCESS)
				{
					char stringToPrint[MAX_LEN];
					sprintf(stringToPrint,"%s%d%s","Getting the feats path of image number ",i," failed");
					spLoggerPrintError(stringToPrint,"main.c",__func__,__LINE__);
					fclose(writingFile);
					goto leave;
				}
				readingFile = fopen(path,"r");
				if (readingFile == NULL)
				{
					spLoggerPrintError("Failed opening the file for writing feats","main.c",__func__,__LINE__);
					goto leave;
				}
				*(imagesPointsArray+i) = readFeatsFromFile(readingFile,(numOfFeatsPerImage+i));
				totalNumOfFeats += *(numOfFeatsPerImage+i);
			}

			fclose(readingFile);
			imagesForTreeInit = (SPPoint*)malloc(sizeof(SPPoint)*totalNumOfFeats);

			for (int i = 0; i < numOfImages; ++i)
			{
				for (int j = 0; j < *(numOfFeatsPerImage+i); ++j)
				{
					imagesForTreeInit[count] = spPointCopy(*( *(imagesPointsArray+i) +j ));
					count++;
				}
			}
			free(imagesPointsArray);
		}

		//Initialize KDtree of images
		SP_TREE_MSG treeMsg;
		KDArray kdArr = init(imagesForTreeInit, totalNumOfFeats, &treeMsg);
		//write the if statements of configMsg

		KDTreeNode head = buildKDTree(kdArr, config, &treeMsg);
		//write the if statements of configMsg

		//loop for receiving a query image from the user
		//This loop keeps going until the user enters the exit string <>
		while(1){

				printf("Please enter an image path:\n");
				scanf("%s",path);

				//Check if the user entered the exit string <>
				if(!strcmp(path,"<>"))
				{
					printf(EXIT_MSG);
					break;
				}


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
				free(featsOfQueryImage);

				//Check if minimal GUI mode is on
				if(spConfigMinimalGui(config,&configMsg))
				{
					//Go over all the images which are most similar
					for (int i = 0; i <numOfSimilarImages ; i++)
					{
						spConfigGetImagePath(path,config,images[i]->index);
						proc.showImage(path);
					}
				}
				else //minimal GUI mode is off
				{
					printf("Best candidates for - %s - are:\n",path);
					for (int i = 0; i < numOfSimilarImages; i++)
					{
						spConfigGetImagePath(path,config,images[i]->index);
						printf("%s\n",path);
					}
				}
			}

leave:
	free(configPath);
	free(path);
	free(imagesPointsArray);
	free(imagesForTreeInit);
	spLoggerDestroy();
	return 0;
}
*/
