case AND_IMM: {
  reg_A &= readByte(reg_PC++);
  checkNZ(reg_A);
  break;
}
case AND_ZP: {
  addr_buf = readByte(reg_PC++);
  reg_A &= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(1);break;
}
case AND_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  reg_A &= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case AND_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  reg_A &= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case AND_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  reg_A &= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case AND_ABY: {
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  reg_A &= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case AND_INX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  addr_buf = readWord(addr_buf);
  reg_A &= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(4);break;
}
case AND_INY: {
  addr_buf = readByte(reg_PC++);
  addr_buf = readWord(addr_buf) + reg_Y;
  reg_A &= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(3);break;
}

case BIT_ZP: {
  addr_buf = readByte(reg_PC++);
  data_bus = readByte(addr_buf);
  flag_Z = !(data_bus & reg_A);
  flag_O = (data_bus & 0x40) >> 6;
  flag_N = (data_bus & 0x80) >> 7;
  cycle(1);break;
}
case BIT_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  data_bus = readByte(addr_buf);
  flag_Z = !(data_bus & reg_A);
  flag_O = (data_bus & 0x40) >> 6;
  flag_N = (data_bus & 0x80) >> 7;
  cycle(2);break;
}

case EOR_IMM: {
  reg_A ^= readByte(reg_PC++);
  checkNZ(reg_A);
  break;
}
case EOR_ZP: {
  addr_buf = readByte(reg_PC++);
  reg_A ^= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(1);break;
}
case EOR_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  reg_A ^= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case EOR_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  reg_A ^= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case EOR_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  reg_A ^= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case EOR_ABY: {
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  reg_A ^= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case EOR_INX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  addr_buf = readWord(addr_buf);
  reg_A ^= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(4);break;
}
case EOR_INY: {
  addr_buf = readByte(reg_PC++);
  addr_buf = readWord(addr_buf) + reg_Y;
  reg_A ^= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(3);break;
}

case ORA_IMM: {
  reg_A |= readByte(reg_PC++);
  checkNZ(reg_A);
  break;
}
case ORA_ZP: {
  addr_buf = readByte(reg_PC++);
  reg_A |= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(1);break;
}
case ORA_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  reg_A |= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case ORA_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  reg_A |= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case ORA_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  reg_A |= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case ORA_ABY: {
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  reg_A |= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(2);break;
}
case ORA_INX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  addr_buf = readWord(addr_buf);
  reg_A |= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(4);break;
}
case ORA_INY: {
  addr_buf = readByte(reg_PC++);
  addr_buf = readWord(addr_buf) + reg_Y;
  reg_A |= readByte(addr_buf);
  checkNZ(reg_A);
  cycle(3);break;
}
