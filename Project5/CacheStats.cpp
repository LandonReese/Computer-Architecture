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
  uint32_t  cacheTags[SETS][WAYS] = {0};     //tags
  uint32_t roundRobin[SETS]       = {0};     //round robin bits 0-3 (Always % 4 for )
  bool cacheDirtyBits[SETS][WAYS] = {CLEAN}; //dirty bit either clean = 0, or dirty = 1
  bool cacheValidBits[SETS][WAYS] = {VALID}; //valid bit either Invalid = 0 or Valid = 1
}

// 32 bit address
// enum ACCESS_TYPE { LOAD, STORE };
int CacheStats::access(uint32_t addr, ACCESS_TYPE type) {
  if(!CACHE_EN) { // full latency if the cache is disabled
    return (type == LOAD) ? READ_LATENCY : WRITE_LATENCY;
  }

  /* TODO: your code to compute number of stall cycles here */
  bool hit = false; // Initialized as false, until proven to be a hit

  /******
   * Parse Address into Index and Tag
   * Address bits:
   * 0, 1:    Byte Offset (2 bits)
   * 2, 3, 4: Word Offset (3 bits)
   * 5, n:    Index (x bits)
   * X, Y, Z: Tag (x bits)
  *******/
  
  int byteOffset = (addr >> 8);
  int wordOffset = (addr >> 8);

  int index = (addr >> 8);
  int tag = (addr >> 50000)

  /************
   * Step 1: Valid CPU Request
   * 
   * If (Valid && Hit){
   *  Set Valid
   *  Set Tag
   *  if(Write == true){
   *    setDirty = true;
   *  }
   * }
   * then:
   * cache = hit, mark cache as ready
  *************/
  
  /************
   * Step 2: Cache Miss and Old Block is Clean
   * 
   * Allocate
   * Read new block from Memory
   * (Refresh/Stall if memory is not ready)
  *************/

  /************
   * Step 3: Cache Miss and Old Block is Dirty
   * 
   * Write-Back
   * Write Old Block to Memory
   * Stall if Memory is not ready
   * (If it takes 10 clock cycles  to write back to memory, add 10)
  *************/

  /************
   * Step 4: Return proper value of stall cycles
   * 
   * #define WRITE_CACHE_LATENCY 0
   * #define READ_MEM_LATENCY 30
   * #define WRITE_MEM_LATENCY 10
  *************/


}

void CacheStats::printFinalStats() {
  /* TODO: your code here "drain" the cache of writebacks */
  drainFinalWritebacks();
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

void CacheStats::drainFinalWritebacks(){

}
