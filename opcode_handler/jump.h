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
  stack_push((uint8_t)((reg_PC & 0xFF00) >> 8));
  stack_push((uint8_t)((reg_PC + 1) & 0xff));
  //printf("JSR push %x\n", reg_PC+1);
  reg_PC = readWord(reg_PC);
  cycle(4);break;
}

case RTS: {
  reg_PC = stack_pop() & 0xff;
  reg_PC |= (uint16_t)(stack_pop() << 8);
  reg_PC++;
  //printf("RTS pop %x\n", reg_PC);
  cycle(4);break;
}
