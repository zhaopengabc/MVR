#ifndef CXS_COMM_H
#define CXS_COMM_H

#define CXS_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)

#endif // CXS_COMM_H
