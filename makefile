CC = gcc
CPP = g++
# put your object files here. added: main_aux, kdarray, kdtree. need to check with nova!!!! 
OBJS = main.o SPImageProc.o SPPoint.o SPConfig.o SPLogger.o SPBPriorityQueue.o SPList.o SPListElement.o KDTree.o KDArray.o main_aux.o
# The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core

CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp SPImageProc.h SPConfig.h SPLogger.h SPPoint.h SPBPriorityQueue.h main_aux.h KDTree.h KDArray.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
# a rule for building a simple c source file
# use gcc -MM SPPoint.c to see the dependencies

main_aux.o: main_aux.c main_aux.h SPConfig.h SPLogger.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
	
SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPBPriorityQueue.o: SPBPriorityQueue.c SPList.h SPListElement.h SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPList.o: SPList.c SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c

KDArray.o: KDArray.c KDArray.h SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c

KDTree.o: KDTree.c KDTree.h KDArray.h SPPoint.h SPConfig.h SPLogger.h SPBPriorityQueue.h SPListElement.h SPList.h
	$(CC) $(C_COMP_FLAG) -c $*.c


clean:
	rm -f $(OBJS) $(EXEC)