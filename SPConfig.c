#include "SPConfig.h"


struct sp_config_t{
	char spImagesDirectory[MAX_LEN];
	char spImagesPrefix[MAX_LEN];
	char spImagesSuffix[MAX_LEN];
	int spNumOfImages;
	int spPCADimension;
	char spPCAFilename[MAX_LEN];
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_TREE_SPLIT_METHOD spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char spLoggerFilename[MAX_LEN];
};

//initialize all default values and allocate wanted memory space
void SPConfigInitialize(SPConfig config){
	config->spPCADimension = 20;
	strcpy(config->spPCAFilename, "pca.yml");
	config->spNumOfFeatures = 100;
	config->spExtractionMode = true;
	config->spNumOfSimilarImages = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;
	config->spKNN = 1;
	config->spMinimalGUI = false;
	config->spLoggerLevel = 3;
	strcpy(config->spLoggerFilename, "stdout");
}


SPConfig spConfigCreate(char* filename, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	char* env_var = (char*) malloc(MAX_LEN*sizeof(char));
	char* param;
	if (filename == NULL)
		{
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			goto leave;
		}


	FILE* input = fopen(filename,"r");
	if (input == NULL)
	{
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		goto leave;
	}
	SPConfig config = (SPConfig) malloc(sizeof(SPConfig));
		if (config == NULL)
		{
			*msg = SP_CONFIG_ALLOC_FAIL;
			goto leave;
		}

	SPConfigInitialize(config);

	char line[MAX_LEN] = "";
	const char delim[2] = "=";

	SP_CONFIG_MSG changeFieldMsg;

	spLoggerCreate(NULL,SP_LOGGER_INFO_WARNING_ERROR_LEVEL);
	int lineNumber = 0;

	while (fgets(line,MAX_LEN,input) != NULL)
	{

		char commentCheck[2];
		commentCheck[0] = trimWhiteSpace(line)[0];
		if (strcmp(line,"") && strcmp(commentCheck,"#")) //check for comments and empty lines
		{
			param = (char*) malloc(MAX_LEN*sizeof(char));
			sprintf(env_var,"%s",strtok(line,delim));
			sprintf(param,"%s",strtok(NULL,delim));

			if (strtok(NULL,delim) != NULL)//invalid line, since there is more than one '='
			{
				*msg = SP_CONFIG_INVALID_LINE;
				break;
			}

			env_var = trimWhiteSpace(env_var);
			param = trimWhiteSpace(param);

			SP_CONFIG_ENV_VAR enum_env_var = getSPConfigEnvVar(env_var);

			changeFieldMsg = changeSPConfigField(config,enum_env_var,param);

			if (changeFieldMsg != SP_CONFIG_SUCCESS)
			{
				*msg = changeFieldMsg;
				break;
			}
		}
		lineNumber++;
	}

	if (changeFieldMsg != SP_CONFIG_SUCCESS)
	{
		printRegErr(filename, lineNumber, changeFieldMsg);

		free(config->spImagesDirectory);
		free(config->spImagesPrefix);
		free(config->spImagesSuffix);
		free(config->spLoggerFilename);
		free(config);

		goto leave;
	}

leave:
	if (input != NULL)fclose(input);
	spLoggerDestroy();
	free(env_var);
	return config;
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	bool returnValue = false;
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
	}

	returnValue = config->spExtractionMode;
	*msg = SP_CONFIG_SUCCESS;
	leave:
	return returnValue;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	bool returnValue = false;
	assert(msg != NULL);
	if (config == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
	}

	returnValue = config->spMinimalGUI;
	*msg = SP_CONFIG_SUCCESS;
	leave:
	return returnValue;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	int returnValue = -1;
	assert(msg != NULL);
	if (config == NULL)
		{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
		}

	returnValue = config->spNumOfImages;
	*msg = SP_CONFIG_SUCCESS;
	leave:
	return returnValue;
}

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	int returnValue = -1;
	assert(msg != NULL);
	if (config == NULL)
		{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
		}

	returnValue = config->spNumOfSimilarImages;
	*msg = SP_CONFIG_SUCCESS;
	leave:
	return returnValue;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	int returnValue = -1;
	assert(msg != NULL);
	if (config == NULL)
		{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
		}

	returnValue = config->spNumOfFeatures;
	*msg = SP_CONFIG_SUCCESS;
	leave:
	return returnValue;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	int returnValue = -1;
		assert(msg != NULL);
		if (config == NULL)
			{
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			goto leave;
			}

		returnValue = config->spPCADimension;
		*msg = SP_CONFIG_SUCCESS;
		leave:
		return returnValue;
}

SP_CONFIG_MSG spConfigGetFeatsPath(char* imagePath, const SPConfig config, int index)
{
	SP_CONFIG_MSG returnValue = SP_CONFIG_SUCCESS;
	if (imagePath == NULL || config == NULL)
		{
			returnValue = SP_CONFIG_INVALID_ARGUMENT;
			goto leave;
		}
	if (index >= config->spNumOfImages)
		{
			returnValue = SP_CONFIG_INDEX_OUT_OF_RANGE;
			goto leave;
		}

	sprintf(imagePath,"%s%s%d%s",config->spImagesDirectory,config->spImagesPrefix,index,".feats");

	leave:
	return returnValue;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index)
{
	SP_CONFIG_MSG returnValue = SP_CONFIG_SUCCESS;
	if (imagePath == NULL || config == NULL)
	{
		returnValue = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
	}
	if (index >= config->spNumOfImages)
	{
		returnValue = SP_CONFIG_INDEX_OUT_OF_RANGE;
		goto leave;
	}

	sprintf(imagePath,"%s%s%d%s",config->spImagesDirectory,config->spImagesPrefix,index,config->spImagesSuffix);

	leave:
	return returnValue;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
	SP_CONFIG_MSG returnValue = SP_CONFIG_SUCCESS;
	if (pcaPath == NULL || config == NULL)
		{
		returnValue = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
		}

	sprintf(pcaPath,"%s%s",config->spImagesDirectory,config->spPCAFilename);

	leave:
	return returnValue;
}

void spConfigDestroy(SPConfig config)
{
	if (config == NULL)
	{
		return;
	}
	free(config);
}

void printRegErr(char* file, int line, SP_CONFIG_MSG errorType)
{
/*	spLoggerPrintFile(file);
	spLoggerPrintLine(line);*/

	char* msg = (char*) malloc(sizeof(char)*MAX_LEN);
	switch(errorType)
	{
	case SP_CONFIG_INVALID_LINE:
		msg = "Invalid configuration line";
		break;

	case SP_CONFIG_INVALID_STRING:
	case SP_CONFIG_INVALID_INTEGER:
		msg = "Invalid value - constraint not met";
		break;
	case SP_CONFIG_MISSING_DIR:
		msg = "Parameter spImagesDirectory is not set";
		break;
	case SP_CONFIG_MISSING_PREFIX:
		msg = "Parameter spImagesPrefix is not set";
		break;
	case SP_CONFIG_MISSING_SUFFIX:
		msg = "Parameter spImagesSuffix is not set";
		break;
	case SP_CONFIG_MISSING_NUM_IMAGES:
		msg = "Parameter spNumOfImages is not set";
		break;

	case SP_CONFIG_CANNOT_OPEN_FILE:
	case SP_CONFIG_ALLOC_FAIL:
	case SP_CONFIG_INVALID_ARGUMENT:
	case SP_CONFIG_INDEX_OUT_OF_RANGE:
	case SP_CONFIG_SUCCESS:
		break;
	}

	printf(msg);
	free(msg);
}

char* trimWhiteSpace(char* str)
{
  char *end;

  while(isspace(*str)) str++;

  if(*str == 0)
    return str;

  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  *(end+1) = 0;

  return str;
}

SP_CONFIG_ENV_VAR getSPConfigEnvVar(char* field)
{
	SP_CONFIG_ENV_VAR return_value;

		if (!strcmp(field,"spImagesDirectory")) return_value = EnumspImagesDirectory;
		else if (!strcmp(field,"spImagesPrefix")) return_value = EnumspImagesPrefix;
		else if (!strcmp(field,"spImagesSuffix")) return_value = EnumspImagesSuffix;
		else if (!strcmp(field,"spNumOfImages")) return_value = EnumspNumOfImages;
		else if (!strcmp(field,"spPCADimension")) return_value = EnumspPCADimension;
		else if (!strcmp(field,"spPCAFilename")) return_value = EnumspPCAFilename;
		else if (!strcmp(field,"spNumOfFeatures")) return_value = EnumspNumOfFeatures;
		else if (!strcmp(field,"spExtractionMode")) return_value = EnumspExtractionMode;
		else if (!strcmp(field,"spNumOfSimilarImages")) return_value = EnumspNumOfSimilarImages;
		else if (!strcmp(field,"spKDTreeSplitMethod")) return_value = EnumspKDTreeSplitMethod;
		else if (!strcmp(field,"spKNN")) return_value = EnumspKNN;
		else if (!strcmp(field,"spMinimalGUI")) return_value = EnumspMinimalGUI;
		else if (!strcmp(field,"spLoggerLevel")) return_value = EnumspLoggerLevel;
		else if (!strcmp(field,"spLoggerFilename")) return_value = EnumspLoggerFilename;

		return return_value;
}

SP_CONFIG_MSG changeSPConfigField(SPConfig config, SP_CONFIG_ENV_VAR field, char* param){

	SP_CONFIG_MSG return_value = SP_CONFIG_SUCCESS;

	switch(field)
	{
	case EnumspImagesDirectory:
		if (strcmp(param,""))//not empty
		{
			if (strchr(param,' ') == NULL || strchr(param,'\t')) //no whitespaces
			{
				strcpy(config->spImagesDirectory,param);
			}
			else //contains whitespaces
			{
				return_value = SP_CONFIG_INVALID_STRING;
			}
		}
		else //empty
			{
				return_value = SP_CONFIG_MISSING_DIR;
			}
		break;
	case EnumspImagesPrefix:
		if (strcmp(param,""))//not empty
			{
			if (strchr(param,' ') == NULL || strchr(param,'\t')) //no whitespaces
				{
				strcpy(config->spImagesPrefix ,param);
				}
				else //contains whitespaces
				{
					return_value = SP_CONFIG_INVALID_STRING;
				}
			}
		else //empty
			{
			return_value = SP_CONFIG_MISSING_PREFIX;
			}
		break;

	case EnumspImagesSuffix:
		if (strcmp(param,""))//not empty
			{
			if (strchr(param,' ') == NULL || strchr(param,'\t')) //no whitespaces
				{
				if (strcmp(param,".jpg") || strcmp(param,".png") || strcmp(param,".bmp") || strcmp(param,".gif")) // is in the right format
					{
					strcpy(config->spImagesSuffix,param);
					}
				}
			else //contains whitespaces OR wrong format
				{
				return_value = SP_CONFIG_INVALID_STRING;
				}
			}
		else //empty
			{
			return_value = SP_CONFIG_MISSING_PREFIX;
			}
		break;

	case EnumspNumOfImages:
		if (strcmp(param,"")) //not empty
		{
			if (atoi(param)<1)//invalid value
			{
				return_value = SP_CONFIG_INVALID_INTEGER;
			}
			else //valid value
			{
				config->spNumOfImages = atoi(param);
			}
		}
		else //is empty
		{
			return_value = SP_CONFIG_MISSING_NUM_IMAGES;
		}
		break;

	case EnumspPCADimension:
		if (strcmp(param,"")) //not empty
			{
			int intParam = atoi(param);
			if (intParam<10 || intParam>28) //invalid value
				{
				return_value = SP_CONFIG_INVALID_INTEGER;
				}
			else //valid value
				{
				config->spPCADimension = intParam;
				}
			}
		break;

	case EnumspPCAFilename:
		if (strcmp(param,""))//not empty
			{
			if (strchr(param,' ') == NULL || strchr(param,'\t')) //no whitespaces
				{
				strcpy(config->spPCAFilename ,param);
				}
			else //contains whitespaces
				{
				return_value = SP_CONFIG_INVALID_STRING;
				}
			}
		break; //is empty, take default value

	case EnumspNumOfFeatures:

		if (strcmp(param,"")) //not empty
			{
			if (atoi(param)<1)//invalid value
				{
				return_value = SP_CONFIG_INVALID_INTEGER;
				}
			else //valid value
				{
				config->spNumOfFeatures = atoi(param);
				}
			}
		break; //might've took default value if here

	case EnumspExtractionMode:
		if (strcmp(param,"")) //not empty
			{
			if (!strcmp(param,"true"))//valid value = true
				{
				config->spExtractionMode = true;
				}
			else if (!strcmp(param,"false"))
					{
					config->spExtractionMode = false;
					}
			else //invalid value
					{
					return_value = SP_CONFIG_INVALID_STRING;
					}
			}
		break; //might've gotten default value here


	case EnumspNumOfSimilarImages:
		if (strcmp(param,"")) //not empty
				{
					if (atoi(param)<0)//invalid value
					{
						return_value = SP_CONFIG_INVALID_INTEGER;
					}
					else //valid value
					{
						config->spNumOfSimilarImages = atoi(param);
					}
				}
		break; //is empty, take default value
	case EnumspKDTreeSplitMethod:

		if (!strcmp(param,"RANDOM")){
			config->spKDTreeSplitMethod = RANDOM;}

		else if (!strcmp(param,"MAX_SPREAD"))
			{config->spKDTreeSplitMethod = MAX_SPREAD;}

		else if (!strcmp(param,"INCREMENTAL"))
			{config->spKDTreeSplitMethod = INCREMENTAL;}
		else if (strcmp(param,"")){ //not empty, but invalid value
			return_value = SP_CONFIG_INVALID_STRING;
		}
		break;
	case EnumspKNN:
		if (strcmp(param,"")) //not empty
						{
							if (atoi(param)<1)//invalid value
							{
								return_value = SP_CONFIG_INVALID_INTEGER;
							}
							else //valid value
							{
								config->spKNN = atoi(param);
							}
						}
		break; // is empty, take default value

	case EnumspMinimalGUI:
		if (strcmp(param,"")) //not empty
			{
			if (!strcmp(param,"true"))//valid value = true
				{
				config->spMinimalGUI = true;
			}
			else if (!strcmp(param,"false"))
			{
				config->spMinimalGUI = false;
			}
			else //invalid value
			{
				return_value = SP_CONFIG_INVALID_STRING;
			}
			}
		break; // is empty, take default value

	case EnumspLoggerLevel:

		if (strcmp(param,"")) //not empty
		{
			int intParam = atoi(param);
			if (intParam == 1 || intParam == 2 || intParam == 3 || intParam == 4 ) //valid value
			{
				config->spLoggerLevel = intParam;
			}
			else //not empty but invalid value
			{
				return_value = SP_CONFIG_INVALID_INTEGER;
			}
		}
		break; //is empty, take default value

	case EnumspLoggerFilename:
		if (strcmp(param,""))//not empty
				{
					if (strchr(param,' ') == NULL || strchr(param,'\t')) //no whitespaces
					{
						strcpy(config->spLoggerFilename,param);
					}
					else //contains whitespaces
					{
						return_value = SP_CONFIG_INVALID_STRING;
					}
				}
		break;
	default:
		return_value = SP_CONFIG_INVALID_LINE;
	}

	free(param);
	return return_value;

}

SP_TREE_SPLIT_METHOD SPConfigGetSplitMethod(SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL)
		{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
		}

	SP_TREE_SPLIT_METHOD returnValue = config->spKDTreeSplitMethod;
	*msg = SP_CONFIG_SUCCESS;
	leave:
	return returnValue;
}

int SPConfigGetspKNN(SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL)
		{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		goto leave;
		}

	int returnValue = config->spKNN;
	*msg = SP_CONFIG_SUCCESS;
	leave:
	return returnValue;
}


char* spConfigGetLoggerFilename(const SPConfig config){
	return config->spLoggerFilename;
}

SP_LOGGER_LEVEL spConfigGetLoggerLevel(const SPConfig config){
	switch (config->spLoggerLevel){
	case 0:
		return SP_LOGGER_ERROR_LEVEL;
	case 1:
		return SP_LOGGER_WARNING_ERROR_LEVEL;
	case 2:
		return SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
	case 3:
		return SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
	}
	return SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
}
