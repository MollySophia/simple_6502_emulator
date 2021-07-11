case INC_ZP: {
  addr_buf = readByte(reg_PC++);
  data_bus = readByte(addr_buf);
  if(data_bus <= 0xfe) data_bus++;
  else data_bus = 0x00;
  writeByte(data_bus, addr_buf);
  checkNZ(data_bus);
  cycle(3);break;
}
case INC_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  data_bus = readByte(addr_buf);
  if(data_bus <= 0xfe) data_bus++;
  else data_bus = 0x00;
  writeByte(data_bus, addr_buf);
  checkNZ(data_bus);
  cycle(4);break;
}
case INC_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  data_bus = readByte(addr_buf);
  if(data_bus <= 0xfe) data_bus++;
  else data_bus = 0x00;
  writeByte(data_bus, addr_buf);
  checkNZ(data_bus);
  cycle(4);break;
}
case INC_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  data_bus = readByte(addr_buf);
  if(data_bus <= 0xfe) data_bus++;
  else data_bus = 0x00;
  writeByte(data_bus, addr_buf);
  checkNZ(data_bus);
  cycle(5);break;
}

case DEC_ZP: {
  addr_buf = readByte(reg_PC++);
  data_bus = readByte(addr_buf);
  if(data_bus >= 0x01) data_bus--;
  else data_bus = 0xff;
  writeByte(data_bus, addr_buf);
  checkNZ(data_bus);
  cycle(3);break;
}
case DEC_ZPX: {
  addr_buf = (readByte(reg_PC++) + reg_X) & 0xff;
  data_bus = readByte(addr_buf);
  if(data_bus >= 0x01) data_bus--;
  else data_bus = 0xff;
  writeByte(data_bus, addr_buf);
  checkNZ(data_bus);
  cycle(4);break;
}
case DEC_AB: {
  addr_buf = readWord(reg_PC);
  reg_PC += 2;
  data_bus = readByte(addr_buf);
  if(data_bus >= 0x01) data_bus--;
  else data_bus = 0xff;
  writeByte(data_bus, addr_buf);
  checkNZ(data_bus);
  cycle(4);break;
}
case DEC_ABX: {
  addr_buf = readWord(reg_PC) + reg_X;
  reg_PC += 2;
  data_bus = readByte(addr_buf);
  if(data_bus >= 0x01) data_bus--;
  else data_bus = 0xff;
  writeByte(data_bus, addr_buf);
  checkNZ(data_bus);
  cycle(5);break;
}
