#ifndef _PRESS_H_
#define _PRESS_H_
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Stream.h"
using namespace std;

class presser
{
#define REVERSE       0
#define SETBIT(DATA, OFF) DATA |= (1 << OFF)   //将X的第Y位置1
#define CLSBIT(DATA, OFF) DATA &= ~(1 << OFF) //将X的第Y位清0
    typedef unsigned char   byte;
public:
    presser(FILE* fp = NULL);
    presser(const char* filename, const char* mode = "rb+");
    ~presser();
public:
    bool encode(const char* indata = NULL, size_t size = 0);
    bool decode(const char* indata = NULL, size_t size = 0);
    bool encode(char* outdata, size_t* osize, const char* indata = NULL, size_t size = 0);
    bool decode(char* outdata, size_t* osize, const char* indata = NULL, size_t size = 0);
private:
    inline int A(char* buffer, byte cur_char, byte cur_size);
    inline int D(char* buffer, byte cur_char, byte next_char, byte third_char, size_t* cur_off);
    inline int I(byte ch) {
        int size = 1;
        while (ch >>= 1) {
            ++size;
        }
        return size;
    }
    inline void _flush() { if (_data) { free(_data); _data = NULL; _size = 0; } }
    bool _encode(char* outdata = NULL, size_t* osize = 0);
    bool _decode(char* outdata = NULL, size_t* osize = 0);
private:
    char*    _data = NULL;
    size_t   _size = 0;
    byte     _left_char = 0;
    byte     _left_size = 0;
    Stream*  _stream = NULL;
};


#endif // !_PRESS_H_
