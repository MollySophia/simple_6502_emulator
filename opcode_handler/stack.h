case PHA: {
  stack_push(reg_A);
  //printf("PHA push %x\n", reg_A);
  cycle(1);break;
}

case PHP: { //Push Processor Status
  uint8_t status = (flag_N<<7)|(flag_O<<6)|0x20|0x10|(flag_D<<3)|(flag_I<<2)|(flag_Z<<1)|flag_C;
  stack_push(status);
  //printf("PHP push %x\n", status);
  cycle(1);break;
}

case PLA: {
  reg_A = stack_pop();
  //printf("PLA pop %x\n", reg_A);
  check_N_Z(reg_A);
  cycle(2);break;
}

case PLP: { //Pop Processor Status
  uint8_t status = stack_pop();
  //printf("PLP pop %x\n", status);
  flag_N = (status & 0x80) >> 7;
  flag_O = (status & 0x40) >> 6;
  flag_B = (status & 0x10) >> 4;
  flag_D = (status & 0x08) >> 3;
  flag_I = (status & 0x04) >> 2;
  flag_Z = (status & 0x02) >> 1;
  flag_C = (status & 0x01);
  cycle(2);break;
}

case TXS: {
  reg_SP = reg_X;
  break;
}

case TSX: {
  reg_X = reg_SP;
  check_N_Z(reg_X);
  break;
}
