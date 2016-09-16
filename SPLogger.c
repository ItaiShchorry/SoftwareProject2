#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file, const char* function, const int line){
	if (logger==NULL)return SP_LOGGER_UNDIFINED;
	if (msg==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (file==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (function==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (line<0)return SP_LOGGER_INVAlID_ARGUMENT;

	fprintf(logger->outputChannel,"---ERROR---\n");
	fflush(NULL);

	spLoggerPrintFile(file);

	spLoggerPrintFunc(function);

	spLoggerPrintLine(line);

	spLoggerPrintMsg(msg);

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file, const char* function, const int line){
	if (logger==NULL)return SP_LOGGER_UNDIFINED;
	if (msg==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (file==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (function==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (line<0)return SP_LOGGER_INVAlID_ARGUMENT;

	if(logger->level == SP_LOGGER_WARNING_ERROR_LEVEL || logger->level == SP_LOGGER_INFO_WARNING_ERROR_LEVEL ||
			logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL){

		fprintf(logger->outputChannel,"---WARNING---\n");

		spLoggerPrintFile(file);

		spLoggerPrintFunc(function);

		spLoggerPrintLine(line);

		spLoggerPrintMsg(msg);
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg){
	if (logger==NULL)return SP_LOGGER_UNDIFINED;
	if (msg==NULL)return SP_LOGGER_INVAlID_ARGUMENT;

	if(logger->level == SP_LOGGER_INFO_WARNING_ERROR_LEVEL || logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL){

		fprintf(logger->outputChannel,"---INFO---\n");

		spLoggerPrintMsg(msg);
	}

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file, const char* function, const int line){
	if (logger==NULL)return SP_LOGGER_UNDIFINED;
	if (msg==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (file==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (function==NULL)return SP_LOGGER_INVAlID_ARGUMENT;
	if (line<0)return SP_LOGGER_INVAlID_ARGUMENT;

	if(logger->level == SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL){
		fprintf(logger->outputChannel,"---DEBUG---\n");

		spLoggerPrintFile(file);

		spLoggerPrintFunc(function);

		spLoggerPrintLine(line);

		spLoggerPrintMsg(msg);
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg){
	if (logger==NULL)return SP_LOGGER_UNDIFINED;
	if (msg==NULL)return SP_LOGGER_INVAlID_ARGUMENT;

	char* out = (char*) malloc(1024*sizeof(char));
	snprintf(out,sizeof(char)*1024,"%s%s%s","Message: ",msg,"\n");

	int result = fprintf(logger->outputChannel,"%s",out);
	if(result<0)return SP_LOGGER_WRITE_FAIL;
	fflush(NULL);
	free(out);

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintFile(const char* file){
	if (logger==NULL)return SP_LOGGER_UNDIFINED;
	if (file==NULL)return SP_LOGGER_INVAlID_ARGUMENT;

	char* out = (char*) malloc(1024*sizeof(char));
	snprintf(out,sizeof(char)*1024,"%s%s%s","File: ",file,"\n");

	int check = fprintf(logger->outputChannel,"%s",out);
	if(check<0)return SP_LOGGER_WRITE_FAIL;
	free(out);

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintFunc(const char* func){
	if (logger==NULL)return SP_LOGGER_UNDIFINED;
	if (func==NULL)return SP_LOGGER_INVAlID_ARGUMENT;

	char* out = (char*) malloc(1024*sizeof(char));
	snprintf(out,sizeof(char)*1024,"%s%s%s","- function: ",func,"\n");

	int check = fprintf(logger->outputChannel,"%s",out);
	if(check<0)return SP_LOGGER_WRITE_FAIL;

	free(out);

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintLine(const int line){
	if (logger==NULL)return SP_LOGGER_UNDIFINED;
	if (line<0) return SP_LOGGER_INVAlID_ARGUMENT;

	char* out = (char*) malloc(1024*sizeof(char));
	char buffer[sizeof(int)];
	sprintf(buffer, "%d", line);
	snprintf(out,sizeof(char)*1024,"%s%s%s","Line: ",buffer,"\n");

	int check = fprintf(logger->outputChannel,"%s",out);
	if(check<0)return SP_LOGGER_WRITE_FAIL;

	free(out);

	return SP_LOGGER_SUCCESS;
}
