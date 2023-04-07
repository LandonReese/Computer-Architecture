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
