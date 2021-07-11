//LDA
case LDA_IMM: {// LDA(Immediate)
  reg_A = readByte(reg_PC++);
  checkNZ(reg_A);
  break;
}
case LDA_ZP: { //LDA(Zero Page)
  addr_buf = readByte(reg_PC++);
  reg_A = readByte(addr_buf);
  checkNZ(reg_A);
  cycle(1);break;
}
case LDA_ZPX: { //LDA(ZeroPage,X)
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  reg_A = readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case LDA_AB: { //LDA(Absolute)
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  reg_A = readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case LDA_ABX: { //LDA(Absolute,X)
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  reg_A = readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case LDA_ABY: { //LDA(Absolute,Y)
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  reg_A = readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case LDA_INX: { //LDA Indirect,X ($44,X)
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  addr_buf = readWord(addr_buf);
  reg_A = readByte(addr_buf);
  checkNZ(reg_A);
  cycle(4);break;
}
case LDA_INY: { //LDA Indirect,Y ($44),Y
  addr_buf = readByte(reg_PC++);
  addr_buf = readWord(addr_buf) + reg_Y;
  reg_A = readByte(addr_buf);
  checkNZ(reg_A);
  cycle(3);break;
}
//LDX
case LDX_IMM: pin_RW = 1;reg_X = readByte(reg_PC++);checkNZ(reg_X);break; //LDX Immediate
case LDX_ZP: { //LDX ZeroPage
  addr_buf = readByte(reg_PC++);
  reg_X = readByte(addr_buf);
  checkNZ(reg_X);
  cycle(1);break;
}
case LDX_ZPY: { //LDX ZeroPage,Y
  addr_buf = (readByte(reg_PC++) + reg_Y) & 0xff;
  reg_X = readByte(addr_buf);
  checkNZ(reg_X);
  cycle(2);break;
}
case LDX_AB: { //LDX Absolute
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  reg_X = readByte(addr_buf);
  checkNZ(reg_X);
  cycle(2);break;
}
case LDX_ABY: { //LDX Absolute,Y
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  reg_X = readByte(addr_buf);
  checkNZ(reg_X);
  cycle(2);break;
}
//LDY
case LDY_IMM: { //LDY Immediate
  reg_Y = readByte(reg_PC++);
  checkNZ(reg_Y);
  break;
}
case LDY_ZP: { //LDY ZeroPage
  addr_buf = readByte(reg_PC++);
  reg_Y = readByte(addr_buf);
  checkNZ(reg_Y);
  cycle(1);break;
}
case LDY_ZPX: { //LDY ZeroPage,X
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  reg_Y = readByte(addr_buf);
  checkNZ(reg_Y);
  cycle(2);break;
}
case LDY_AB: { //LDY Absolute
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  reg_Y = readByte(addr_buf);
  checkNZ(reg_Y);
  cycle(2);break;
}
case LDY_ABX: { //LDY Absolute,X
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  reg_Y = readByte(addr_buf);
  checkNZ(reg_Y);
  cycle(2);break;
}
