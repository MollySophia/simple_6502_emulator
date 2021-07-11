case BPL: {
  if(flag_N == 0) {
    char offset = readByte(reg_PC);
    //printf("Offset:%x\n", offset);
    reg_PC = reg_PC + offset + 1;
    cycle(1);
  }
  else reg_PC++;
  break;
}
case BMI: {
  if(flag_N == 1) {
    char offset = readByte(reg_PC);
    //printf("Offset:%x\n", offset);
    reg_PC = reg_PC + offset + 1;
    cycle(1);
  }
  else reg_PC++;
  break;
}
case BVC: {
  if(flag_O == 0) {
    char offset = readByte(reg_PC);
    //printf("Offset:%x\n", offset);
    reg_PC = reg_PC + offset + 1;
  }
  else reg_PC++;
  cycle(1);
  break;
}
case BVS: {
  if(flag_O == 1) {
    char offset = readByte(reg_PC);
    //printf("Offset:%x\n", offset);
    reg_PC = reg_PC + offset + 1;
    cycle(1);
  }
  else reg_PC++;
  break;
}
case BCC: {
  if(flag_C == 0) {
    char offset = readByte(reg_PC);
    //printf("Offset:%x\n", offset);
    reg_PC = reg_PC + offset + 1;
    cycle(1);
  }
  else reg_PC++;
  break;
}
case BCS: {
  if(flag_C == 1) {
    char offset = readByte(reg_PC);
    //printf("Offset:%x\n", offset);
    reg_PC = reg_PC + offset + 1;
    cycle(1);
  }
  else reg_PC++;
  break;
}
case BNE: {
 if(flag_Z == 0) {
   char offset = readByte(reg_PC);
   //printf("Offset:%x\n", offset);
    reg_PC = reg_PC + offset + 1;
   cycle(1);
 }
 else reg_PC++;
 break;
}
case BEQ: {
  if(flag_Z == 1) {
    char offset = readByte(reg_PC);
    //printf("Offset:%x\n", offset);
    reg_PC = reg_PC + offset + 1;
    cycle(1);
  }
  else reg_PC++;
  break;
}
