case TAX: {
  reg_X = reg_A;
  checkNZ(reg_X);
  break;
}
case TAY: {
  reg_Y = reg_A;
  checkNZ(reg_Y);
  break;
}
case TXA: {
  reg_A = reg_X;
  checkNZ(reg_A);
  break;
}
case TYA: {
  reg_A = reg_Y;
  checkNZ(reg_A);
  break;
}
case DEX: {
  if (reg_X >= 0x01) {
    reg_X --;
  }
  else reg_X = 0xff;
  checkNZ(reg_X);
  break;
}
case INX: {
  if (reg_X <= 0xfe) {
    reg_X ++;
  }
  else reg_X = 0x00;
  checkNZ(reg_X);
  break;
}
case DEY: {
  if (reg_Y >= 0x01) {
    reg_Y --;
  }
  else reg_Y = 0xff;
  checkNZ(reg_Y);
  break;
}
case INY: {
  if (reg_Y <= 0xfe) {
    reg_Y ++;
  }
  else reg_Y = 0x00;
  checkNZ(reg_Y);
  break;
}
