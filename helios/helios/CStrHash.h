#pragma once
#include "lava_base.h"

class CStrHash
{
public:
    // BKDR Hash Function
    static uint32_t BKDRHash(const char* str);

    // AP Hash Function
    static uint32_t APHash(const char* str);

    // JS Hash Function
    static uint32_t JSHash(const char* str);

    // RS Hash Function
    static uint32_t RSHash(const char* str);
};

