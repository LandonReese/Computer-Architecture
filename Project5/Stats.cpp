/******************************
 * Submitted by: Landon Reese LJR69
 * CS 3339 - Spring 2022, Texas State University
 * Project 3 Pipelining
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

#include "Stats.h"

Stats::Stats() {
  cycles = PIPESTAGES - 1; // pipeline startup cost
  flushes = 0;
  stalls = 0;
  bubbles = 0;

  memops = 0;
  branches = 0;
  taken = 0;

  for(int i = IF1; i < PIPESTAGES; i++) {
    resultReg[i] = -1;
    resultStage[i] = -1;
  }
}

//if you copy clock code, make sure you also use cycles++ because they will increment during a flush
void Stats::clock() {
  cycles++;

  // advance all stages in pipeline
  for(int i = WB; i > IF1; i--) {
    resultReg[i] = resultReg[i-1];
    resultStage[i] = resultStage[i-1];
  }
  // inject a NOP in pipestage IF1
  resultReg[IF1] = -1;
  resultStage[IF1] = -1;
}

void Stats::registerSrc(int r, PIPESTAGE needed) { // r == the register being read
  // When selecting the register, we need to make sure that there won't be a hazard
  int totalBubbles = 0;
  for(int i = EXE1; i < WB; i++){
    //registerSrc check for regZero, r != 0
    if(resultReg[i] == r && r != 0){ //checks to see if the register is being used for instructions in EXE1 -> WB
      // If it is being used, insert the appropriate amount of bubbles to get the instruction out of the pipe
      // Update totalBubbles to be representational of the amount of bubbles needed with forwarding paths
      totalBubbles = (resultStage[i] - i) - (needed - ID); // Update RAW Hazards if there is a conflict
      RAWhazards[i]++; //Increment hazards
      for(int j = 0; j < totalBubbles; j++){
        totalBubbles--;
        bubble();
      }
    }
  }
  /* From Hinkle
  if(r == 0)
    return;
  for(int i = EXE1; i < WB; i++){
    if(r == resultReg[i]){
      // Ready calculates how many bubbles you need in the pipe for the instr to be ready
      int ready = WB - i;

      while(ready > 0){
        bubble();
        ready--;
      }
    }
  }
  */
}

void Stats::registerDest(int r, PIPESTAGE valid) { // r == the register to be written to
  resultReg[ID] = r;
  resultStage[ID] = valid;
}

void Stats::flush(int count) { // count == how many ops to flush
  // a flush advances the entire pipeline, does not clear
  // for(int i = 0; i < count; i++){
  //   //flush all pipe stages
  // }
  // //increment flushes

  // loop count times {
  //   advance pipeline 1 cycle
  // }
  // 
  while(count > 0){
    clock();        // Everytime a clock cycle is advanced,
    flushes++;      // a flush is added.
    count--;
  }
}

void Stats::stall(int count){
  while(count > 0){
    clock();
    stalls++;
    count--;
  }
}


void Stats::bubble() {
  // in a bubble IF1 IF2 and IFD are frozen.
  // We advance from EXE1 and Beyond.
  // Advance the pipe, until instructions in IF1 or IF2 are ready to go!
  // Common problem, you wrote something in ID that got transferred forward instead of frozen
  bubbles++;                    // increment bubbles
  cycles++;                     // Need to advance one cycle to begin, then add into ID?

  // Cycle backwards through the pipe, advancing until instruction in ID can begin
  for(int i = WB; i > EXE1; i--){ // Advances through the pipe backwards, updating each stage.
    resultReg[i] = resultReg[i-1];
    resultStage[i] = resultStage[i-1];
  }
  resultReg[EXE1] = -1; //Because a bubble is necessary, add a NOP
  resultStage[EXE1] = -1;
}

void Stats::showPipe() {
  // this method is to assist testing and debug, please do not delete or edit
  // you are welcome to use it but remove any debug outputs before you submit
  cout << "              IF1  IF2 *ID* EXE1 EXE2 MEM1 MEM2 WB         #C      #B      #F" << endl; 
  cout << "  resultReg ";
  for(int i = 0; i < PIPESTAGES; i++) {
    cout << "  " << dec << setw(2) << resultReg[i] << " ";
  }
  cout << "   " << setw(7) << cycles << " " << setw(7) << bubbles << " " << setw(7) << flushes;
  cout << endl;
}
