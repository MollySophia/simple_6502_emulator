#include <iostream>
#include "emu_core.h"
#include "console.h"
#include <sys/stat.h>
#include <fstream>
#include <termios.h>
using namespace std;

Emulator emu_core;
Console con;

char getch(void) {
     struct termios tm, tm_old;
     int fd = 0;
		 char ch;

     if (tcgetattr(fd, &tm) < 0) {//保存现在的终端设置
          return -1;
     }

     tm_old = tm;
     cfmakeraw(&tm);//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
     if (tcsetattr(fd, TCSANOW, &tm) < 0) {//设置上更改之后的设置
          return -1;
     }
     fflush(stdin);
     ch = getchar();
     if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {//更改设置为最初的样子
          return -1;
     }

     return ch;
}

int main() {
  con.clear_screen();
  fputs("\033[32m", stdout);
  while(1) {
		emu_core.IR_step();
		if(emu_core.pin_RW == 1) { //Read MEM
      if((emu_core.addr_bus) == 0xD010) {
				emu_core.mem[0xD010] = 0x00;
      }
			if((emu_core.addr_bus) == 0xD011) {
				emu_core.mem[emu_core.addr_bus] = 0xff;
				uint8_t c = getch();
        //printf(" %i" , c);
				if(c == 3) return 0;
				if(c == 10) c = 13;
				if(c == 0x7F) c = 0xDF;
        if(c >= 97) c -= 32;
				c |= 0x80;
				emu_core.mem[0xD010] = c;
			}
      if((emu_core.addr_bus) == 0xD012) {
      }
    }
    else { //Write MEM
      if((emu_core.addr_bus) == 0xD012) {
        uint8_t ch = emu_core.mem[emu_core.addr_bus] & 0x7F;
        if(ch == 13) ch = 10;
				if(ch == (0xDF & 0x7F)) ch = 0x7F;
        if(ch >= 97) ch = 0;
        putc((char)ch,stdout);
        fflush(stdout);
        emu_core.mem[emu_core.addr_bus] = 0x00;
      }
    }
  }
  fclose(emu_core.fp);
  return 0;
}
