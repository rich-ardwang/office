#include "stdafx.h"
#include "CStrHash.h"

uint32_t CStrHash::BKDRHash(const char* str)
{
    uint32_t hash = 0;
    uint32_t seed = 131; // 31 131 1313 13131 131313 etc..
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}

uint32_t CStrHash::APHash(const char* str)
{
    uint32_t hash = 0;
    for ( int i = 0; *str; i++ )
    {
        if ( (i & 1) == 0 )
        {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t CStrHash::JSHash(const char* str)
{
    uint32_t hash = 1315423911;
    while (*str)
    {
        hash ^= ((hash << 5) + (*str++) + (hash >> 2));
    }

    return (hash & 0x7FFFFFFF);
}

uint32_t CStrHash::RSHash(const char* str)
{
    uint32_t hash = 0;
    uint32_t a = 63689;
    uint32_t b = 378551;
    while (*str)
    {
        hash = hash * a + (*str++);
        a *= b;
    }
    return (hash & 0x7FFFFFFF);
}


