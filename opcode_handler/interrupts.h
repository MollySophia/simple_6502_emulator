case BRK: {
  reg_PC++;
  stack_push((uint8_t)((reg_PC & 0xFF00) >> 8));
  stack_push((uint8_t)(reg_PC & 0xff));
  uint8_t status = (flag_N<<7)|(flag_O<<6)|0x20|0x10|(flag_D<<3)|(flag_I<<2)|(flag_Z<<1)|flag_C;
  stack_push(status);
  //printf("BRK push %x %x\n", reg_PC, status);
  reg_PC = readByte(0xfffe) | (uint16_t)(readByte(0xffff) << 8);
  flag_I = 1;
  cycle(5);
  break;
}

case RTI: { //Return from Interrupt
  uint8_t status = stack_pop();
  flag_N = (status & 0x80) >> 7;
  flag_O = (status & 0x40) >> 6;
  flag_B = (status & 0x10) >> 4;
  flag_D = (status & 0x08) >> 3;
  flag_I = (status & 0x04) >> 2;
  flag_Z = (status & 0x02) >> 1;
  flag_C = (status & 0x01);
  reg_PC = stack_pop() & 0xff;
  reg_PC |= (uint16_t)(stack_pop() << 8);
  //printf("RTI pop %x %x\n", reg_PC, status);
  break;
}

case WAI: { //Wait For Interrupt
  break;
}
