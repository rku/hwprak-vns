#ifndef UNITTEST_H
#define UNITTEST_H

// plain simple test "framework" inspired by MinUnit
// at http://www.jera.com/techinfo/jtns/jtn002.html

#define ASSERT(cond, message) \
    do { \
         if (!(cond)) return message; \
       } while (0);
#define TEST(test) char *test(void)
#define RUN_TEST(test) \
    do { \
         printf("Running %-63s ", #test); \
         fflush(stdout); \
         char *message = test(); \
         tests_run++; \
         if (NULL != message) { \
             printf("\033[1;31mFAIL\033[m\n"); \
             return message; \
         } \
         printf("\033[1;32mPASS\033[m\n"); \
       } while(0)
#define TEST_OK NULL

extern unsigned int tests_run;

#endif /* UNITTEST_H */
