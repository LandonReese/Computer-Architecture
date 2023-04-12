/******************************
 * Submitted by: enter your first and last name and net ID
 * CS 3339 - Spring 2023, Texas State University
 * Project 5 Data Cache
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
 * Every time an access is made to the cache model, the cache model should return the number of cycles that 
 * the processor must stall in order for that access to complete.  It takes 0 cycles to do a lookup or to hit in the 
 * cache (i.e., data that is hit will be returned or written immediately).  A read access to the next level of the 
 * memory hierarchy (e.g., main memory) has a latency of 30 cycles, and a write access has a latency of 10 
 * cycles.  Note that an access resulting in the replacement of a dirty line requires both a main memory write 
 * (to write back the dirty block) and a read (to fetch the new block) consecutively.  Because the cache has no 
 * write buffer, all stores must stall until the write is complete.
 * 
 * Inside your Stats class, add a new function similar to the bubble() and flush() functions.  This stall() 
 * function should stall the entire pipeline for a specified number of cycles.  The Stats class should track the 
 * total number of stall cycles that occur during program execution. 
********************************/


#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "CacheStats.h"
using namespace std;

CacheStats::CacheStats() {
  cout << "Cache Config: ";
  if(!CACHE_EN) {
    cout << "cache disabled" << endl;
  } else {
    cout << (SETS * WAYS * BLOCKSIZE) << " B (";
    cout << BLOCKSIZE << " bytes/block, " << SETS << " sets, " << WAYS << " ways)" << endl;
    cout << "  Latencies: Lookup = " << LOOKUP_LATENCY << " cycles, ";
    cout << "Read = " << READ_LATENCY << " cycles, ";
    cout << "Write = " << WRITE_LATENCY << " cycles" << endl;
  }

  loads = 0;
  stores = 0;
  load_misses = 0;
  store_misses = 0;
  writebacks = 0;

  /* TODO: your code to initialize your datastructures here */
}

int CacheStats::access(uint32_t addr, ACCESS_TYPE type) {
  if(!CACHE_EN) { // full latency if the cache is disabled
    return (type == LOAD) ? READ_LATENCY : WRITE_LATENCY;
  }

  /* TODO: your code to compute number of stall cycles here */
}

void CacheStats::printFinalStats() {
  /* TODO: your code here "drain" the cache of writebacks */

  int accesses = loads + stores;
  int misses = load_misses + store_misses;
  cout << "Accesses: " << accesses << endl;
  cout << "  Loads: " << loads << endl;
  cout << "  Stores: " << stores << endl;
  cout << "Misses: " << misses << endl;
  cout << "  Load misses: " << load_misses << endl;
  cout << "  Store misses: " << store_misses << endl;
  cout << "Writebacks: " << writebacks << endl;
  cout << "Hit Ratio: " << fixed << setprecision(1) << 100.0 * (accesses - misses) / accesses;
  cout << "%" << endl;
}
