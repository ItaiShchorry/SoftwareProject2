#include "main_aux.h"
#include "String.h"
#include "SPLogger.h"

struct image
{
	int index;
	int hits;
};

void printFailedCreatingMessage(char* configPath){
	//Checking if the config file is the default file
	if(!strcmp(configPath,"spcbir.config"))
	{
		printf("The default configuration file spcbir.config couldn't be opened\n");
	}
	//Getting here means the config file was input by the user
	else
	{
		char stringToPrint[MAX_LEN];
		sprintf(stringToPrint,"%s%s%s","The configuration file ",configPath," couldn't be open\n");
		printf(stringToPrint);
	}
}

void getNumOfImagesWrapper(int* numOfImages, SPConfig config, SP_CONFIG_MSG* configMsg){
	*numOfImages = spConfigGetNumOfImages(config,configMsg);
	if (*configMsg != SP_CONFIG_SUCCESS)
		{
		spLoggerPrintError("Getting spConfigGetNumOfImages failed","main.c",__func__,__LINE__);
		}
}

void getNumOfFeatsWrapper(int* numOfFeats, SPConfig config, SP_CONFIG_MSG* configMsg){
	*numOfFeats = spConfigGetNumOfFeatures(config,configMsg);
	if (*configMsg != SP_CONFIG_SUCCESS)
		{
		spLoggerPrintError("Getting spConfigGetNumOfFeatures failed","main.c",__func__,__LINE__);
		}
}

void getNumOfSimilarImagesWrapper(int* NumOfSimilarImages, SPConfig config, SP_CONFIG_MSG* configMsg){
	*NumOfSimilarImages = spConfigGetNumOfSimilarImages(config,configMsg);
	if (*configMsg != SP_CONFIG_SUCCESS)
		{
		spLoggerPrintError("Getting spConfigGetNumOfSimilarImages failed","main.c",__func__,__LINE__);
		}
}


void getImagePathWrapper(int i, SP_CONFIG_MSG* configMsg, char* path, SPConfig config, FILE* writingFile){
	*configMsg = spConfigGetImagePath (path,config,i);
	//Checking if getting the image path failed
	if (*configMsg != SP_CONFIG_SUCCESS)
		{
		char stringToPrint[MAX_LEN];
		sprintf(stringToPrint,"%s%d%s","Getting the image path of image number ",i," failed");
		spLoggerPrintError(stringToPrint,"main.c",__func__,__LINE__);
		fclose(writingFile);
		}
}

void getFeatsPathWrapper(int i, SP_CONFIG_MSG* configMsg, char* path, SPConfig config, FILE* writingFile){
	*configMsg = spConfigGetFeatsPath(path,config,i);
	//Checking if getting the feats file path failed
	if (*configMsg != SP_CONFIG_SUCCESS)
	{
		char stringToPrint[MAX_LEN];
		sprintf(stringToPrint,"%s%d%s","Getting the feats path of image number ",i," failed");
		spLoggerPrintError(stringToPrint,"main.c",__func__,__LINE__);
		fclose(writingFile);
	}
}

void writeFeatsToFile(FILE* writerFile,SPPoint* point,int numOfFeats)
{

	char* stringToPrint = (char*)malloc(sizeof(char)*MAX_LEN);
	int imageIndex,imageDim;

	imageIndex = spPointGetIndex(point[0]);
	imageDim = spPointGetDimension(point[0]);

	sprintf(stringToPrint,"%s%d%s%d%s%d%s",
			"Image Index: ",imageIndex,
			" Image dimension: ",imageDim,
			" Number of features: ",numOfFeats,"\n");
	fputs(stringToPrint,writerFile);

	int i;
	int j;
	for (i = 0; i < numOfFeats ; i++)
	{
		for (j = 0; j < imageDim; j++)
		{
			sprintf(stringToPrint,"%f%s",spPointGetAxisCoor(point[j],i),"\n");
			fputs(stringToPrint,writerFile);
		}
	}

	free(stringToPrint);
}

SPPoint* readFeatsFromFile(FILE* readerFile,int* numOfFeatsPerImage)
{

	char* stringForRead = (char*)malloc(sizeof(char)*MAX_LEN);

	int imageIndex,imageDim,numOfFeats;


	fscanf(readerFile,"%s %d %s %d %s %d %s",
			stringForRead,&imageIndex,
			stringForRead,&imageDim,
			stringForRead,numOfFeats,stringForRead);

	*numOfFeatsPerImage = numOfFeats;

	double* featsOfImages = malloc(imageDim * sizeof(double));
	SPPoint* result = (SPPoint*) malloc(numOfFeats * sizeof(SPPoint));
	int i;
	int j;
	for (i = 0; i < numOfFeats; i++)
	{
		for (j = 0; j < imageDim; j++)
		{
			fgets(stringForRead,MAX_LEN,readerFile);
			featsOfImages[j] = atof(stringForRead);
		}
		*(result+i) = spPointCreate(featsOfImages,imageDim,imageIndex);
	}
	free(stringForRead);
	free(featsOfImages);

	return result;
}

int compareImagesByHits (const void * a, const void * b)
{
	Image* aImg = (Image*) a;
	Image* bImg = (Image*) b;
	return ( (*aImg)->hits - (*bImg)->hits );
}



