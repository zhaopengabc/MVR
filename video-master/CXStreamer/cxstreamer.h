#ifndef CXSTREAMER_H
#define CXSTREAMER_H
#include<stdio.h>
namespace CXS{
    #define DEBUG(format, ...) printf (format, ##__VA_ARGS__)
    #define ERROR(format, ...) printf (format, ##__VA_ARGS__)
}
#endif // CXSTREAMER_H
