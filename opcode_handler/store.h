case STA_ZP: {
  addr_buf = readByte(reg_PC++);
  writeByte(reg_A, addr_buf);
  cycle(1);break;
}
case STA_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  writeByte(reg_A, addr_buf);
  cycle(2);break;
}
case STA_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  writeByte(reg_A, addr_buf);
  cycle(2);break;
}
case STA_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  writeByte(reg_A, addr_buf);
  cycle(3);break;
}
case STA_ABY: {
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  writeByte(reg_A, addr_buf);
  cycle(3);break;
}
case STA_INX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  addr_buf = readWord(addr_buf);
  writeByte(reg_A, addr_buf);
  cycle(4);break;
}
case STA_INY: {
  addr_buf = readByte(reg_PC++);
  addr_buf = readWord(addr_buf) + reg_Y;
  writeByte(reg_A, addr_buf);
  cycle(4);break;
}
case STX_ZP: {
  addr_buf = readByte(reg_PC++);
  writeByte(reg_X, addr_buf);
  cycle(1);break;
}
case STX_ZPY: {
  addr_buf = (readByte(reg_PC++) + reg_Y) & 0xff;
  writeByte(reg_X, addr_buf);
  cycle(2);break;
}
case STX_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  writeByte(reg_X, addr_buf);
  cycle(2);break;
}
case STY_ZP: {
  addr_buf = readByte(reg_PC++);
  writeByte(reg_Y, addr_buf);
  cycle(1);break;
}
case STY_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  writeByte(reg_Y, addr_buf);
  cycle(2);break;
}
case STY_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  writeByte(reg_Y, addr_buf);
  cycle(2);break;
}
