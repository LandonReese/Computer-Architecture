/******************************
 * Submitted by: Landon Reese LJR69
 * CS 3339 - Spring 2023, Texas State University
 * Project 2 Emulator
 * Copyright 2021, Lee B. Hinkle, all rights reserved
 * Based on prior work by Martin Burtscher and Molly O'Neil
 * Redistribution in source or binary form, with or without modification,
 * is *not* permitted. Use in source or binary form, with or without
 * modification, is only permitted for academic use in CS 3339 at
 * Texas State University.
 ******************************/

#include "CPU.h"
#include "Stats.h"
#include "CacheStats.h"

/*********************************
 * You will also need to modify CPU.cpp to call both CacheStats and Stats class functions 
 * appropriately to model cache behavior and resulting pipeline stalls.  You’ll also need to change 
 * CPU::printFinalStats() to match my expected output format (see below).
*********************************/

Stats stats = Stats();
CacheStats cStats = CacheStats();

const string CPU::regNames[] = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
                                "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
                                "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
                                "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};

CPU::CPU(uint32_t pc, Memory &iMem, Memory &dMem) : pc(pc), iMem(iMem), dMem(dMem) {
  for(int i = 0; i < NREGS; i++) {
    regFile[i] = 0;
  }
  hi = 0;
  lo = 0;
  regFile[28] = 0x10008000; // gp
  regFile[29] = 0x10000000 + dMem.getSize(); // sp

  instructions = 0;
  stop = false;
}

void CPU::run() {
  while(!stop) {
    instructions++;

    fetch();
    decode();
    execute();
    mem();
    writeback();

    D(printRegFile());

    stats.clock(); // Place here after writeback stats.clock(IF1);
    // to ensure you don't need to call clock after every function
  }
}

void CPU::fetch() {
  instr = iMem.loadWord(pc);
  pc = pc + 4;
}

/////////////////////////////////////////
// ALL YOUR CHANGES GO IN THIS FUNCTION 
/////////////////////////////////////////
void CPU::decode() {
  uint32_t opcode;      // opcode field
  uint32_t rs, rt, rd;  // register specifiers
  uint32_t shamt;       // shift amount (R-type)
  uint32_t funct;       // funct field (R-type)
  uint32_t uimm;        // unsigned version of immediate (I-type)
  int32_t simm;         // signed version of immediate (I-type)
  uint32_t addr;        // jump address offset field (J-type)

  opcode = (instr >> 26); //26 ensures last 6 are chosen
  rs = (instr >> 21) & 0x1f; 
  rt = (instr >> 16) & 0x1f;
  rd = (instr >> 11) & 0x1f;
  shamt = (instr >> 6) & 0x1f;
  funct = (instr & 0x3f);
  uimm = (instr & 0xffff);
  simm = (int16_t)uimm;       //casting as a 16 bit int to sign the bits
  addr = instr & 0x03ffffff;  //26 bits for addr

  // Hint: you probably want to give all the control signals some "safe"
  // default value here, and then override their values as necessary in each
  // case statement below!

  opIsLoad = false;
  opIsStore = false;
  opIsMultDiv = false;
  aluOp = ADD; //defaulting with ADD
  writeDest = false;
  destReg = 0;
  aluSrc1 = 0;
  aluSrc2 = 0;
  storeData = 0;

// The provided implementations do not give meaningful values to all of the control signals.  
// Remember never to leave 
// a control signal floating at an undefined or unsafe value. 

  D(cout << "  " << hex << setw(8) << pc - 4 << ": ");

  //REMOVE THE LINE BELOW THIS ONE ASAP
  cout << "OPCODE: " << hex << opcode << endl;

  switch(opcode) {
    case 0x00:
      switch(funct) {
        case 0x00: D(cout << "sll " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
                   writeDest = true;      
                   destReg = rd;                stats.registerDest(rd, PIPESTAGE::MEM1);
                   aluOp = SHF_L;
                   aluSrc1 = regFile[rs];       stats.registerSrc(rs, PIPESTAGE::EXE1);
                   aluSrc2 = shamt;          
                   break; // use prototype above, not the greensheet
        case 0x03: D(cout << "sra " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
                   writeDest = true;      
                   destReg = rd;                stats.registerDest(rd, PIPESTAGE::MEM1);
                   aluOp = SHF_R;
                   aluSrc1 = regFile[rs];       stats.registerSrc(rs, PIPESTAGE::EXE1);
                   aluSrc2 = shamt;
                   break; // use prototype above, not the greensheet
        case 0x08: D(cout << "jr " << regNames[rs]);
                   pc = regFile[rs];            stats.registerSrc(rs, PIPESTAGE::ID);
                   stats.flush(ID - IF1);
                   break;
        case 0x10: D(cout << "mfhi " << regNames[rd]);
        // Note that mfhi and mflo
        // read the hi/lo registers, and mult and div write them
                   writeDest = true;
                   destReg = rd;                stats.registerDest(rd, PIPESTAGE::MEM1);
                   aluOp = ADD;
                   aluSrc1 = hi;                stats.registerSrc(REG_HILO, PIPESTAGE::EXE1);
                   aluSrc2 = regFile[REG_ZERO]; //stats.registerSrc(REG_ZERO);
                   break;
        case 0x12: D(cout << "mflo " << regNames[rd]);
                   writeDest = true;
                   destReg = rd;                stats.registerDest(rd, PIPESTAGE::MEM1);
                   aluOp = ADD;
                   aluSrc1 = lo;                stats.registerSrc(REG_HILO, PIPESTAGE::EXE1);
                   aluSrc2 = regFile[REG_ZERO]; //stats.registerSrc(REG_ZERO);
                   break;
        case 0x18: D(cout << "mult " << regNames[rs] << ", " << regNames[rt]);
                   writeDest = true;
                   destReg = REG_HILO;                stats.registerDest(REG_HILO, PIPESTAGE::WB); //FIX THIS LINE
                   aluOp = MUL;
                   opIsMultDiv = true;
                   aluSrc1 = regFile[rs];       stats.registerSrc(rs, PIPESTAGE::EXE1);
                   aluSrc2 = regFile[rt];       stats.registerSrc(rt, PIPESTAGE::EXE1);
                   break;
        case 0x1a: D(cout << "div " << regNames[rs] << ", " << regNames[rt]);
                   writeDest = true;
                   destReg = REG_HILO;                stats.registerDest(REG_HILO, PIPESTAGE::WB);
                   aluOp = DIV;
                   opIsMultDiv = true;
                   aluSrc1 = regFile[rs];       stats.registerSrc(rs, PIPESTAGE::EXE1);
                   aluSrc2 = regFile[rt];       stats.registerSrc(rt, PIPESTAGE::EXE1);
                   break;
        case 0x21: D(cout << "addu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   writeDest = true;
                   destReg = rd;                stats.registerDest(rd, PIPESTAGE::MEM1);
                   aluOp = ADD;
                   aluSrc1 = regFile[rs];       stats.registerSrc(rs, PIPESTAGE::EXE1);
                   aluSrc2 = regFile[rt];       stats.registerSrc(rt, PIPESTAGE::EXE1);
                   break;
        case 0x23: D(cout << "subu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   writeDest = true;
                   destReg = rd;                stats.registerDest(rd, PIPESTAGE::MEM1);
                   aluOp = ADD;
                   aluSrc1 = regFile[rs];       stats.registerSrc(rs, PIPESTAGE::EXE1);
                   aluSrc2 = -(regFile[rt]);    stats.registerSrc(rt, PIPESTAGE::EXE1);
                   break; //hint: subtract is the same as adding a negative
        case 0x2a: D(cout << "slt " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   writeDest = true;
                   destReg = rd;                stats.registerDest(rd, PIPESTAGE::MEM1);
                   aluOp = CMP_LT;
                   aluSrc1 = regFile[rs];       stats.registerSrc(rs, PIPESTAGE::EXE1);
                   aluSrc2 = regFile[rt];       stats.registerSrc(rt, PIPESTAGE::EXE1);
                   break;
        default: cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
      }
      break;
    case 0x02: D(cout << "j " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
               pc = ((pc & 0xf0000000) | addr << 2);
               stats.flush(ID - IF1);
               break;
    case 0x03: D(cout << "jal " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
               writeDest = true; 
               destReg = REG_RA;                stats.registerDest(REG_RA, PIPESTAGE::EXE1); // writes PC+4 to $ra
               aluOp = ADD; //pc needs to pass through the ALU unchanged
               aluSrc1 = pc;                    
               aluSrc2 = regFile[REG_ZERO];     //stats.registerSrc(REG_ZERO, PIPESTAGE::);// always reads zero
               pc = (pc & 0xf0000000) | addr << 2;
               stats.flush(ID - IF1);
               break;
    case 0x04: D(cout << "beq " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
               /* Need to check before if */    stats.registerSrc(rs, PIPESTAGE::ID);
               /* statement is called.    */    stats.registerSrc(rt, PIPESTAGE::ID); 
               stats.countBranch();  
               if(regFile[rs] == regFile[rt]){
                 pc = pc + (simm << 2);
                 stats.countTaken();
                 stats.flush(ID - IF1);
               }
               break;  // read the handout carefully, update PC directly here as in jal example
    case 0x05: D(cout << "bne " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
                                                stats.registerSrc(rs, PIPESTAGE::ID); 
                                                stats.registerSrc(rt, PIPESTAGE::ID);
    stats.countBranch();
               if(regFile[rs] != regFile[rt]){
                 stats.countTaken();
                 pc = pc + (simm << 2);
                 stats.flush(ID - IF1);
               }
               break;  // same comment as beq
    case 0x09: D(cout << "addiu " << regNames[rt] << ", " << regNames[rs] << ", " << dec << simm);
               writeDest = true; 
               destReg = rt;                    stats.registerDest(rt, PIPESTAGE::MEM1);
               aluOp = ADD;
               aluSrc1 = regFile[rs];           stats.registerSrc(rs, PIPESTAGE::EXE1);
               aluSrc2 = simm;
               break;
    case 0x0c: D(cout << "andi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << uimm);
               writeDest = true;
               destReg = rt;                    stats.registerDest(rt, PIPESTAGE::MEM1);
               aluOp = AND;
               aluSrc1 = regFile[rs];           stats.registerSrc(rs, PIPESTAGE::EXE1);
               aluSrc2 = uimm;
               break;
    case 0x0f: D(cout << "lui " << regNames[rt] << ", " << dec << simm);
               //op is load bool needs to be true
               writeDest = true;
               destReg = rt;                    stats.registerDest(rt, PIPESTAGE::MEM1);
               aluOp = SHF_L; //need to shift left by 16
               aluSrc1 = simm; //simm needs to shift simm bits to upper part of word
               aluSrc2 = 16;  //fulfills the left shift of 16bits
               break; //use the ALU to execute necessary op, you may set aluSrc2 = xx directly
    case 0x1a: D(cout << "trap " << hex << addr);
               switch(addr & 0xf) {
                 case 0x0: cout << endl; break;
                 case 0x1: cout << " " << (signed)regFile[rs];
                           stats.registerSrc(rs, PIPESTAGE::EXE1);
                           break;
                 case 0x5: cout << endl << "? "; cin >> regFile[rt];
                           stats.registerSrc(rt, PIPESTAGE::MEM1);
                           break;
                 case 0xa: stop = true; break;
                 default: cerr << "unimplemented trap: pc = 0x" << hex << pc - 4 << endl;
                          stop = true;
               }
               break;
    case 0x23: D(cout << "lw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
               opIsLoad = true;
               writeDest = true;
               destReg = rt;              stats.registerDest(rt, PIPESTAGE::WB);
               aluOp = ADD;
               aluSrc1 = simm;
               aluSrc2 = regFile[rs];     stats.registerSrc(rs, PIPESTAGE::EXE1);
               stats.countMemOp();
               stats.stall(cStats.access((simm + regFile[rs]), LOAD));
               break;  // do not interact with memory here - setup control signals for mem()
    case 0x2b: D(cout << "sw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
               opIsStore = true;
               writeDest = true;
               storeData = regFile[rt];   stats.registerSrc(rt, PIPESTAGE::MEM1); //maybe switch the order of these stats.registerSrc() commands if it doesn't test correctly
               aluSrc1 = simm;
               aluSrc2 = regFile[rs];     stats.registerSrc(rs, PIPESTAGE::EXE1);
               stats.countMemOp();
               stats.stall(cStats.access((simm + regFile[rs]), STORE));
               break;  // same comment as lw
    default: cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
  }
  D(cout << endl);
}

void CPU::execute() {
  aluOut = alu.op(aluOp, aluSrc1, aluSrc2);
}

void CPU::mem() {
  if(opIsLoad)
    writeData = dMem.loadWord(aluOut);
  else
    writeData = aluOut;

  if(opIsStore)
    dMem.storeWord(storeData, aluOut);
}

void CPU::writeback() {
  if(writeDest && destReg > 0) // skip when write is to zero_register
    regFile[destReg] = writeData;
  
  if(opIsMultDiv) {
    hi = alu.getUpper();
    lo = alu.getLower();
  }
}

void CPU::printRegFile() {
  cout << hex;
  for(int i = 0; i < NREGS; i++) {
    cout << "    " << regNames[i];
    if(i > 0) cout << "  ";
    cout << ": " << setfill('0') << setw(8) << regFile[i];
    if( i == (NREGS - 1) || (i + 1) % 4 == 0 )
      cout << endl;
  }
  cout << "    hi   : " << setfill('0') << setw(8) << hi;
  cout << "    lo   : " << setfill('0') << setw(8) << lo;
  cout << dec << endl;
}

void CPU::printFinalStats() {
  cout << "Program finished at pc = 0x" << hex << pc << "  ("
       << dec << instructions << " instructions executed)" << endl << endl;
  cout << "Cycles: " << stats.getCycles() << endl;
  cout << "CPI: " << fixed << setprecision(2) << (stats.getCycles()/instructions) << endl;
  cout << endl;
  cout << "Bubbles: " << stats.getBubbles() << endl;
  cout << "Flushes: " << stats.getFlushes() << endl;
  cout << "Stalls: "  << stats.getStalls()  << endl;
  cout << endl;
  
  cStats.printFinalStats();
  /*******************************
   * The exact number of clock cycles it would take to execute the program on this CPU
   * The CPI(cycle count / instruction count)
   * The number of bubble cycles injected due to data dependencies(unchanged from Proj. 4)
   * The number of flush cycles in the shadows of jumps and taken branches(also unchanged)
   * The number of stall cycles due to cache/memory latency
   * The data cache statistics reported by the cache model
  *******************************/
}
