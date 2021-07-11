//JMP
case JMP_AB: { // Jump(Absolute)
  reg_PC = readWord(reg_PC);
  cycle(1);break;
}
case JMP_IN: { // Jump(Indirect)
  addr_buf = readWord(reg_PC);
  reg_PC = readWord(addr_buf);
  cycle(3);break;
}

case JSR_AB: {
  stackPush((uint8_t)((reg_PC & 0xFF00) >> 8));
  stackPush((uint8_t)((reg_PC + 1) & 0xff));
  //printf("JSR push %x\n", reg_PC+1);
  reg_PC = readWord(reg_PC);
  cycle(4);break;
}

case RTS: {
  reg_PC = stackPop() & 0xff;
  reg_PC |= (uint16_t)(stackPop() << 8);
  reg_PC++;
  //printf("RTS pop %x\n", reg_PC);
  cycle(4);break;
}
