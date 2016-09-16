CC = gcc
OBJS = KDArrayTest.o testingUtils.o KDArray.o
EXEC = KDArrayTest
TESTS_DIR = ./unitTesting
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
KDArrayTest.o: $(TESTS_DIR)/KDArrayTest.c $(TESTS_DIR)/testingUtils.h KDArray.h 
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
KDArray.o: KDArray.c KDArray.h SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
