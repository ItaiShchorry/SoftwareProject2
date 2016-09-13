#include "main_aux.h"

struct image
{
	int index;
	int hits;
};

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
