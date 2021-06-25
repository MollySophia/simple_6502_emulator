//Logical Shift Right
case LSR_ACC: { //LSR Accumulator
  reg_A = LogicShiftRight(reg_A);
  check_N_Z(reg_A);
  break;
}
case LSR_ZP: { //LSR ZeroPage
  addr_buf = readByte(reg_PC++);
  data_bus = readByte(addr_buf);
  data_bus = LogicShiftRight(data_bus);
  writeByte(data_bus, addr_buf);
  check_N_Z(data_bus);
  cycle(3);break;
}
case LSR_ZPX: { //LSR ZeroPage,X
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  data_bus = readByte(addr_buf);
  data_bus = LogicShiftRight(data_bus);
  writeByte(data_bus, addr_buf);
  check_N_Z(data_bus);
  cycle(4);break;
}
case LSR_AB: { //LSR Absolute
  addr_buf = readWord(reg_PC);
  data_bus = readByte(addr_buf);
  data_bus = LogicShiftRight(data_bus);
  writeByte(data_bus, addr_buf);
  reg_PC += 2;
  check_N_Z(data_bus);
  cycle(4);break;
}
case LSR_ABX: { //LSR Absolute,X
  addr_buf = readWord(reg_PC) + reg_X;
  data_bus = readByte(addr_buf);
  data_bus = LogicShiftRight(data_bus);
  writeByte(data_bus, addr_buf);
  reg_PC += 2;
  check_N_Z(data_bus);
  cycle(5);break;
}
//Arithmetic Shift Left
case ASL_ACC: {// ASL Accumulator
  data_bus = reg_A;
  flag_C = (data_bus & 0x80) != 0;
  reg_A = (data_bus << 1);  
  check_N_Z(reg_A);
  break;
}
case ASL_ZP: { //ASL ZeroPage
  addr_buf = readByte(reg_PC++);
  data_bus = readByte(addr_buf);
  flag_C = (data_bus & 0x80) != 0;
  data_bus = data_bus << 1;
  writeByte(data_bus, addr_buf);
  check_N_Z(data_bus);
  cycle(3);break;
}
case ASL_ZPX: { //ASL ZeroPage,X
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  data_bus = readByte(addr_buf);
  flag_C = (data_bus & 0x80) != 0;
  data_bus = data_bus << 1;
  writeByte(data_bus, addr_buf);
  check_N_Z(data_bus);
  cycle(4);break;
}
case ASL_AB: { //ASL Absolute
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  data_bus = readByte(addr_buf);
  flag_C = (data_bus & 0x80) != 0;
  data_bus = data_bus << 1;
  writeByte(data_bus, addr_buf);
  check_N_Z(data_bus);
  cycle(4);break;
}
case ASL_ABX: { //ASL Absolute,X
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  data_bus = readByte(addr_buf);
  flag_C = (data_bus & 0x80) != 0;
  data_bus = data_bus << 1;
  writeByte(data_bus, addr_buf);
  check_N_Z(data_bus);
  cycle(5);break;
}

case ROL_ACC: {
  uint8_t carry_tmp = (reg_A & 0x80) != 0;
  reg_A = (reg_A << 1) | (flag_C & 0x01);
  flag_C = carry_tmp & 0x01;
  check_N_Z(reg_A);
  break;
}
case ROL_ZP: {
  addr_buf = readByte(reg_PC++);
  data_bus = readByte(addr_buf);
  uint8_t carry_tmp = (data_bus & 0x80) != 0;
  data_bus = (data_bus << 1) | (flag_C & 0x01);
  flag_C = carry_tmp;
  check_N_Z(data_bus);
  writeByte(data_bus,addr_buf);
  cycle(3);break;
}
case ROL_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  data_bus = readByte(addr_buf);
  uint8_t carry_tmp = (data_bus & 0x80) != 0;
  data_bus = (data_bus << 1) | (flag_C & 0x01);
  flag_C = carry_tmp;
  check_N_Z(data_bus);
  writeByte(data_bus,addr_buf);
  cycle(3);break;
}
case ROL_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC+=2;
  data_bus = readByte(addr_buf);
  uint8_t carry_tmp = (data_bus & 0x80) != 0;
  data_bus = (data_bus << 1) | (flag_C & 0x01);
  flag_C = carry_tmp;
  check_N_Z(data_bus);
  writeByte(data_bus,addr_buf);
  cycle(4);break;
}
case ROL_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC+=2;
  data_bus = readByte(addr_buf);
  uint8_t carry_tmp = (data_bus & 0x80) != 0;
  data_bus = (data_bus << 1) | (flag_C & 0x01);
  flag_C = carry_tmp;
  check_N_Z(data_bus);
  writeByte(data_bus,addr_buf);
  cycle(4);break;
}

case ROR_ACC: {
  uint8_t carry_tmp = reg_A & 0x01;
  reg_A = (reg_A >> 1) | (flag_C << 7);
  flag_C = carry_tmp;
  check_N_Z(reg_A);
  break;
}
case ROR_ZP: {
  addr_buf = readByte(reg_PC++);
  data_bus = readByte(addr_buf);
  uint8_t carry_tmp = data_bus & 0x01;
  data_bus = (data_bus >> 1) | (flag_C << 7);
  flag_C = carry_tmp;
  check_N_Z(data_bus);
  writeByte(data_bus,addr_buf);
  cycle(3);break;
}
case ROR_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  data_bus = readByte(addr_buf);
  uint8_t carry_tmp = data_bus & 0x01;
  data_bus = (data_bus >> 1) | (flag_C << 7);
  flag_C = carry_tmp;
  check_N_Z(data_bus);
  writeByte(data_bus,addr_buf);
  cycle(3);break;
}
case ROR_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC+=2;
  data_bus = readByte(addr_buf);
  uint8_t carry_tmp = data_bus & 0x01;
  data_bus = (data_bus >> 1) | (flag_C << 7);
  flag_C = carry_tmp;
  check_N_Z(data_bus);
  writeByte(data_bus,addr_buf);
  cycle(4);break;
}
case ROR_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC+=2;
  data_bus = readByte(addr_buf);
  uint8_t carry_tmp = data_bus & 0x01;
  data_bus = (data_bus >> 1) | (flag_C << 7);
  flag_C = carry_tmp;
  check_N_Z(data_bus);
  writeByte(data_bus,addr_buf);
  cycle(4);break;
}
