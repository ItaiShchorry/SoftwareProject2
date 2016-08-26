/*
 * testingUtils.h
 *
 *  Created on: Aug 25, 2016
 *      Author: mally
 */

#ifndef TESTINGUTILS_H_
#define TESTINGUTILS_H_
#define FAIL(msg) do {\
		fprintf(stderr,"%s Line %d: %s", __FILE__, __LINE__, msg);\
		fflush(NULL);\
		return false;\
	} while(0)

#define ASSERT_TRUE(expression) do { \
                if(!((expression))) { \
                        FAIL("expression is false :: "); \
                } \
        } while (0)

#define ASSERT_FALSE(expression) do { \
                if((expression)) { \
                        FAIL("expression is true  ::"); \
                } \
		} while (0)


#endif /* TESTINGUTILS_H_ */
