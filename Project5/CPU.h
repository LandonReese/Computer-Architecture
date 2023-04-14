#ifndef __CPU_H
#define __CPU_H

#include <iostream>
#include <cstdint>
#include <string>
#include <iomanip>
#include <cstdlib>
#include "Memory.h"
#include "ALU.h"
#include "Debug.h"
#include "Stats.h"
#include "CacheStats.h"
using namespace std;
/*********************************
 * You will also need to modify CPU.h to instantiate a CacheStats object, and CPU.cpp to call both CacheStats and Stats class functions 
 * appropriately to model cache behavior and resulting pipeline stalls.  You’ll also need to change 
 * CPU::printFinalStats() to match my expected output format (see below).
*********************************/
class CPU {
  private:
    Stats stats;
    CacheStats cStats;

    static const int NREGS = 32;
    static const string regNames[];

    static const int REG_ZERO = 0;
    static const int REG_RA = 31;
    static const int REG_HILO = 32;

    uint32_t pc;
    uint32_t instr;

    // Register file
    uint32_t regFile[NREGS];
    uint32_t hi, lo;

    ALU alu;
    
    Memory &iMem;
    Memory &dMem;

    int instructions;
    bool stop;

    // Control signals
    bool opIsLoad, opIsStore, opIsMultDiv;
    ALU_OP aluOp;
    bool writeDest;
    int destReg;
    uint32_t aluSrc1, aluSrc2;
    uint32_t storeData;

    // Wires between stages
    uint32_t aluOut;
    uint32_t writeData;

  public:
    CPU(uint32_t pc, Memory &iMem, Memory &dMem);

    void run();
    void printFinalStats();

  private:
    void fetch();
    void decode();
    void execute();
    void mem();
    void writeback();
    
    void printRegFile();
};

#endif
