case CMP_IMM: {
  compare(reg_A, readByte(reg_PC++));
  break;
}
case CMP_ZP: {
  addr_buf = readByte(reg_PC++);
  compare(reg_A, readByte(addr_buf));
  cycle(1);break;
}
case CMP_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  compare(reg_A, readByte(addr_buf));
  cycle(2);break;
}
case CMP_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  compare(reg_A, readByte(addr_buf));
  cycle(2);break;
}
case CMP_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  compare(reg_A, readByte(addr_buf));
  cycle(2);break;
}
case CMP_ABY: {
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  compare(reg_A, readByte(addr_buf));
  cycle(2);break;
}
case CMP_INX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  addr_buf = readWord(addr_buf);
  compare(reg_A, readByte(addr_buf));
  cycle(4);break;
}
case CMP_INY: {
  addr_buf = readByte(reg_PC++);
  addr_buf = readWord(addr_buf) + reg_Y;
  compare(reg_A, readByte(addr_buf));
  cycle(3);break;
}
//CPX
case CPX_IMM: {
  compare(reg_X, readByte(reg_PC++));
  break;
}
case CPX_ZP: {
  addr_buf = readByte(reg_PC++);
  compare(reg_X, readByte(addr_buf));
  cycle(1);break;
}
case CPX_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  compare(reg_X, readByte(addr_buf));
  cycle(2);break;
}
//CPY
case CPY_IMM: {
  compare(reg_Y, readByte(reg_PC++));
  break;
}
case CPY_ZP: {
  addr_buf = readByte(reg_PC++);
  compare(reg_Y, readByte(addr_buf));
  cycle(1);break;
}
case CPY_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  compare(reg_Y, readByte(addr_buf));
  cycle(2);break;
}
