/******************************
 * Copyright 2022, Lee B. Hinkle, all rights reserved
 * Based on prior work by Martin Burtscher and Molly O'Neil
 * Redistribution in source or binary form, with or without modification,
 * is *not* permitted. Use in source or binary form, with or without
 * modification, is only permitted for academic use in CS 3339 at
 * Texas State University.
 ******************************/

/*******************************
 * In addition to enhancing the CacheStats.h/.cpp skeleton, you will need to modify your existing 
 * Stats.h/Stats.cpp in order to implement memory stalls.  You will also need to modify CPU.h to 
 * instantiate a CacheStats object, and CPU.cpp to call both CacheStats and Stats class functions 
 * appropriately to model cache behavior and resulting pipeline stalls.  You’ll also need to change 
 * CPU::printFinalStats() to match my expected output format (see below).
*******************************/

#ifndef __STATS_H
#define __STATS_H
#include <iostream>
#include <iomanip>
#include "Debug.h"
using namespace std;

enum PIPESTAGE { IF1 = 0, IF2 = 1, ID = 2, EXE1 = 3, EXE2 = 4, MEM1 = 5, 
                 MEM2 = 6, WB = 7, PIPESTAGES = 8 };

class Stats {
  private:
    long long cycles;
    int flushes;
    int stalls;
    int bubbles;

    int memops;
    int branches;
    int taken;

    int resultReg[PIPESTAGES];
    int resultStage[PIPESTAGES];  // Advance every time you advance resultReg[]
    int RAWhazards[PIPESTAGES];    // Tracks RAW control hazards

  public:
    Stats();

    void clock();

    void flush(int count);
    void stall(int count);

    void registerSrc(int r, PIPESTAGE needed);
    void registerDest(int r, PIPESTAGE valid);

    void countMemOp() { memops++; }
    void countBranch() { branches++; }
    void countTaken() { taken++; }
	
    void showPipe();

    // getters
    long long getCycles() { return cycles; }
    int getFlushes() { return flushes; }
    int getStalls() { return stalls; }
    int getBubbles() { return bubbles; }
    int getMemOps() { return memops; }
    int getBranches() { return branches; }
    int getTaken() { return taken; }

  private:
    void bubble();
};

#endif
