case ADC_IMM: {
  uint16_t tmp = readByte(reg_PC++);
  add(tmp);
  break;
}
case ADC_ZP: {
  addr_buf = readByte(reg_PC++);
  uint16_t tmp = readByte(addr_buf);
  add(tmp);
  cycle(1);break;
}
case ADC_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  uint16_t tmp = readByte(addr_buf);
  add(tmp);
  cycle(2);break;
}
case ADC_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  uint16_t tmp = readByte(addr_buf);
  add(tmp);
  cycle(2);break;
}
case ADC_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  uint16_t tmp = readByte(addr_buf);
  add(tmp);
  cycle(2);break;
}
case ADC_ABY: {
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  uint16_t tmp = readByte(addr_buf);
  add(tmp);
  cycle(2);break;
}
case ADC_INX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  addr_buf = readWord(addr_buf);
  uint16_t tmp = readByte(addr_buf);
  add(tmp);
  cycle(4);break;
}
case ADC_INY: {
  addr_buf = readByte(reg_PC++);
  addr_buf = readWord(addr_buf) + reg_Y;
  uint16_t tmp = readByte(addr_buf);
  add(tmp);
  cycle(3);break;
}

case SBC_IMM: {
  uint16_t tmp = readByte(reg_PC++);
  sub(tmp);
  break;
}
case SBC_ZP: {
  addr_buf = readByte(reg_PC++);
  uint16_t tmp = readByte(addr_buf);
  sub(tmp);
  cycle(1);break;
}
case SBC_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X);
  uint16_t tmp = readByte(addr_buf);
  sub(tmp);
  cycle(2);break;
}
case SBC_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  uint16_t tmp = readByte(addr_buf);
  sub(tmp);
  cycle(2);break;
}
case SBC_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  uint16_t tmp = readByte(addr_buf);
  sub(tmp);
  cycle(2);break;
}
case SBC_ABY: {
  addr_buf = readWord(reg_PC) + reg_Y;
  reg_PC += 2;
  uint16_t tmp = readByte(addr_buf);
  sub(tmp);
  cycle(2);break;
}
case SBC_INX: {
  addr_buf = (readByte(reg_PC++) + reg_X);
  addr_buf = readWord(addr_buf);
  uint16_t tmp = readByte(addr_buf);
  sub(tmp);
  cycle(4);break;
}
case SBC_INY: {
  addr_buf = readByte(reg_PC++);
  addr_buf = readWord(addr_buf) + reg_Y;
  uint16_t tmp = readByte(addr_buf);
  sub(tmp);
  cycle(3);break;
}
