/******************************
 * Copyright 2023, Lee B. Hinkle, all rights reserved
 * Based on prior work by Martin Burtscher and Molly O'Neil
 * Redistribution in source or binary form, with or without modification,
 * is *not* permitted. Use in source or binary form, with or without
 * modification, is only permitted for academic use in CS 3339 at
 * Texas State University.
 ******************************/

/*******************************
 * A CacheStats.h class specification file, to which you’ll need to add member variables 
 * A CacheStats.cpp class implementation file, which you should enhance with code to model the 
 * described cache and count accesses, misses, and writebacks
 * 
 * Inside your Stats class, add a new function similar to the bubble() and flush() functions.  This stall() 
 * function should stall the entire pipeline for a specified number of cycles.  The Stats class should track the 
 * total number of stall cycles that occur during program execution. 
********************************/

#ifndef __CACHE_STATS_H
#define __CACHE_STATS_H

#include <cstdint>
#include "Debug.h"
using namespace std;

#ifndef CACHE_EN
#define CACHE_EN 1
#endif

#ifndef BLOCKSIZE
#define BLOCKSIZE 32
#endif

#ifndef SETS
#define SETS 8
#endif

#ifndef WAYS
#define WAYS 4
#endif

#ifndef LOOKUP_LATENCY
#define LOOKUP_LATENCY 0
#endif

#ifndef READ_LATENCY
#define READ_LATENCY 30
#endif

#ifndef WRITE_LATENCY
#define WRITE_LATENCY 10
#endif

enum ACCESS_TYPE { LOAD, STORE };

class CacheStats {
  private:
    /* TODO: you probably want to add some member variables here to represent
     * the parts of the cache contents you need to model! */
    enum DIRTY_BITS { CLEAN, DIRTY };           // 0 Clean bit, 1 Dirty Bit
    enum VALID_BITS { INVALID, VALID };         // 0 Invalid bit, 1 Valid bit
    uint32_t  cacheTags[SETS][WAYS];  // = {0};     // Tags
    uint32_t roundRobin[SETS];        // = {0};     // Round robin bits 0-3 (Always % 4)
    bool cacheDirtyBits[SETS][WAYS];  // = {CLEAN}; // Dirty bit either clean, 0 or dirty, 1
    bool cacheValidBits[SETS][WAYS];  // = {VALID}; // Valid bit either 0 or 1

    int loads;
    int stores;
    int load_misses;
    int store_misses;
    int writebacks;

  public:
    CacheStats();
    int access(uint32_t, ACCESS_TYPE);
    void printFinalStats();
    void drainFinalWritebacks();
};

#endif
