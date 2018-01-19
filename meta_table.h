#ifndef _META_TABLE_H
#define _META_TABLE_H
#include <map>
#include <stdio.h>

typedef unsigned char  byte;
typedef int  unit_key;

static std::map<unit_key, byte> meta =
{
    {  255, 0x00},
    { -255, 0x80},
    {  254, 0x00},
    {  254, 0x40},
    { -254, 0x80},
    { -254, 0xc0},
    {  253, 0x00},
    {  253, 0x00},
    {  253, 0x00},
    { -253, 0x00},
    { -253, 0x00},
    { -253, 0x00},
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
    { },
};

#endif // _META_TABLE_H
