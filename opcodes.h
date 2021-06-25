#define CLC 0x18 //Flags
#define SEC 0x38
#define CLI 0x58
#define SEI 0x78
#define CLV 0xB8
#define CLD 0xD8
#define SED 0xF8
#define NOP 0xEA
#define LSR_ACC 0x4A //LSR
#define LSR_ZP 0x46
#define LSR_ZPX 0x56
#define LSR_AB 0x4E
#define LSR_ABX 0x5E
#define ROL_ACC 0x2A //ROL
#define ROL_ZP 0x26
#define ROL_ZPX 0x36
#define ROL_AB 0x2E
#define ROL_ABX 0x3E
#define ROR_ACC 0x6A //ROR
#define ROR_ZP 0x66
#define ROR_ZPX 0x76
#define ROR_AB 0x6E
#define ROR_ABX 0x7E
#define ASL_ACC 0x0A //ASL
#define ASL_ZP 0x06
#define ASL_ZPX 0x16
#define ASL_AB 0x0E
#define ASL_ABX 0x1E
#define LDA_IMM 0xA9 //LDA
#define LDA_ZP 0xA5
#define LDA_ZPX 0xB5
#define LDA_AB 0xAD
#define LDA_ABX 0xBD
#define LDA_ABY 0xB9
#define LDA_INX 0xA1
#define LDA_INY 0xB1
#define LDX_IMM 0xA2 //LDX
#define LDX_ZP 0xA6
#define LDX_ZPY 0xB6
#define LDX_AB 0xAE
#define LDX_ABY 0xBE
#define LDY_IMM 0xA0 //LDY
#define LDY_ZP 0xA4
#define LDY_ZPX 0xB4
#define LDY_AB 0xAC
#define LDY_ABX 0xBC
#define JMP_AB 0x4C //JMP
#define JMP_IN 0x6C
#define JSR_AB 0x20
#define RTS 0x60
#define PHA 0x48 //Stack Operations
#define PHP 0x08
#define PLA 0x68
#define PLP 0x28
#define TXS 0x9A
#define TSX 0xBA
#define TAX 0xAA //Register Instructions
#define TAY 0xA8
#define TXA 0x8A
#define TYA 0x98
#define DEX 0xCA
#define INX 0xE8
#define DEY 0x88
#define INY 0xC8
#define STA_ZP 0x85 //STA
#define STA_ZPX 0x95
#define STA_AB 0x8D
#define STA_ABX 0x9D
#define STA_ABY 0x99
#define STA_INX 0x81
#define STA_INY 0x91
#define STX_ZP 0x86 //STX
#define STX_ZPY 0x96
#define STX_AB 0x8E
#define STY_ZP 0x84 //STY
#define STY_ZPX 0x94
#define STY_AB 0x8C
#define CMP_IMM 0xC9 //CMP
#define CMP_ZP 0xC5
#define CMP_ZPX 0xD5
#define CMP_AB 0xCD
#define CMP_ABX 0xDD
#define CMP_ABY 0xD9
#define CMP_INX 0xC1
#define CMP_INY 0xD1
#define CPX_IMM 0xE0 //CPX
#define CPX_ZP 0xE4
#define CPX_AB 0xEC
#define CPY_IMM 0xC0 //CPY
#define CPY_ZP 0xC4
#define CPY_AB 0xCC
#define INC_ZP 0xE6 //INC
#define INC_ZPX 0xF6
#define INC_AB 0xEE
#define INC_ABX 0xFE
#define DEC_ZP 0xC6 //DEC
#define DEC_ZPX 0xD6
#define DEC_AB 0xCE
#define DEC_ABX 0xDE
#define ADC_IMM 0x69 //ADC
#define ADC_ZP 0x65
#define ADC_ZPX 0x75
#define ADC_AB 0x6D
#define ADC_ABX 0x7D
#define ADC_ABY 0x79
#define ADC_INX 0x61
#define ADC_INY 0x71
#define SBC_IMM 0xE9 //SBC
#define SBC_ZP 0xE5
#define SBC_ZPX 0xF5
#define SBC_AB 0xED
#define SBC_ABX 0xFD
#define SBC_ABY 0xF9
#define SBC_INX 0xE1
#define SBC_INY 0xF1
#define AND_IMM 0x29 //AND
#define AND_ZP 0x25
#define AND_ZPX 0x35
#define AND_AB 0x2D
#define AND_ABX 0x3D
#define AND_ABY 0x39
#define AND_INX 0x21
#define AND_INY 0x31
#define BIT_ZP 0x24 //BIT
#define BIT_AB 0x2C
#define EOR_IMM 0x49 //EOR
#define EOR_ZP 0x45
#define EOR_ZPX 0x55
#define EOR_AB 0x4D
#define EOR_ABX 0x5D
#define EOR_ABY 0x59
#define EOR_INX 0x41
#define EOR_INY 0x51
#define ORA_IMM 0x09 //ORA
#define ORA_ZP 0x05
#define ORA_ZPX 0x15
#define ORA_AB 0x0D
#define ORA_ABX 0x1D
#define ORA_ABY 0x19
#define ORA_INX 0x01
#define ORA_INY 0x11
#define BPL 0x10 //Branch
#define BMI 0x30
#define BVC 0x50
#define BVS 0x70
#define BCC 0x90
#define BCS 0xB0
#define BNE 0xD0
#define BEQ 0xF0
#define BRK 0x00
#define RTI 0x40
#define WAI 0xCB
