/*
 * disassembly_intel_opcodes: disassembling intel opcodes 
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DISASSEMBLY_INTEL_OPCODES_INC
#define DISASSEMBLY_INTEL_OPCODES_INC

const static char override [11] [6] =
  {
    "", "BYTE", "WORD", "", "DWORD", "", "", "", "QWORD", "", "TWORD"
  };

                                 
const static char reg08 [9] [3] =
  {
    "??", "AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH"
  };
        
const static char reg16 [9] [3] =
  {
    "??", "AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI"
  };

const static char reg32 [9] [4] =
  {
    "???", "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"
  };

const static char regseg [7] [3] =
  {
    "??", "ES", "CS", "SS", "DS", "FS", "GS"
  };

const static char regfpu [8] [4] =
  {
    "ST0", "ST1", "ST2", "ST3", "ST4", "ST5", "ST6", "ST7"
  };

const static char regcontr [6] [4] =
  {
    "??", "CR0", "???", "CR2", "CR3", "CR4"
  };

const static char regdebug [9] [4] =
  {
    "??", "DR0", "DR1", "DR2", "DR3", "???", "???", "DR6", "DR7"
  };

const static char regtest [9] [4] =
  {
    "??", "???", "???", "???", "TR3", "TR4", "TR5", "TR6", "TR7"
  };

// main purpose of these tables is to reuse the memory in the Disassembly_Nodes...
//  to avoid having to reallocate memory for the same strings...
const static char intel_mnemonic [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"ADD"},            // 0x0
      {"ADD"},            // 0x1
      {"ADD"},            // 0x2
      {"ADD"},            // 0x3
      {"ADD"},            // 0x4
      {"ADD"},            // 0x5
      {"PUSH"},           // 0x6
      {"POP"},            // 0x7
      {"OR"},             // 0x8
      {"OR"},             // 0x9
      {"OR"},             // 0xA
      {"OR"},             // 0xB
      {"OR"},             // 0xC
      {"OR"},             // 0xD
      {"PUSH"},           // 0xE
      {"---"},            // 0xF
      {"ADC"},            // 0x10
      {"ADC"},            // 0x11
      {"ADC"},            // 0x12
      {"ADC"},            // 0x13
      {"ADC"},            // 0x14
      {"ADC"},            // 0x15
      {"PUSH"},           // 0x16
      {"POP"},            // 0x17
      {"SBB"},            // 0x18
      {"SBB"},            // 0x19
      {"SBB"},            // 0x1A
      {"SBB"},            // 0x1B
      {"SBB"},            // 0x1C
      {"SBB"},            // 0x1D
      {"PUSH"},           // 0x1E
      {"POP"},            // 0x1F
      {"AND"},            // 0x20
      {"AND"},            // 0x21
      {"AND"},            // 0x22
      {"AND"},            // 0x23
      {"AND"},            // 0x24
      {"AND"},            // 0x25
      {"???"},            // 0x26
      {"DAA"},            // 0x27
      {"SUB"},            // 0x28
      {"SUB"},            // 0x29
      {"SUB"},            // 0x2A
      {"SUB"},            // 0x2B
      {"SUB"},            // 0x2C
      {"SUB"},            // 0x2D
      {"???"},            // 0x2E
      {"DAS"},            // 0x2F
      {"XOR"},            // 0x30
      {"XOR"},            // 0x31
      {"XOR"},            // 0x32
      {"XOR"},            // 0x33
      {"XOR"},            // 0x34
      {"XOR"},            // 0x35
      {"???"},            // 0x36
      {"AAA"},            // 0x37
      {"CMP"},            // 0x38
      {"CMP"},            // 0x39
      {"CMP"},            // 0x3A
      {"CMP"},            // 0x3B
      {"CMP"},            // 0x3C
      {"CMP"},            // 0x3D
      {"???"},            // 0x3E
      {"AAS"},            // 0x3F
      {"INC"},            // 0x40
      {"INC"},            // 0x41
      {"INC"},            // 0x42
      {"INC"},            // 0x43
      {"INC"},            // 0x44
      {"INC"},            // 0x45
      {"INC"},            // 0x46
      {"INC"},            // 0x47
      {"DEC"},            // 0x48
      {"DEC"},            // 0x49
      {"DEC"},            // 0x4A
      {"DEC"},            // 0x4B
      {"DEC"},            // 0x4C
      {"DEC"},            // 0x4D
      {"DEC"},            // 0x4E
      {"DEC"},            // 0x4F
      {"PUSH"},           // 0x50
      {"PUSH"},           // 0x51
      {"PUSH"},           // 0x52
      {"PUSH"},           // 0x53
      {"PUSH"},           // 0x54
      {"PUSH"},           // 0x55
      {"PUSH"},           // 0x56
      {"PUSH"},           // 0x57
      {"POP"},            // 0x58
      {"POP"},            // 0x59
      {"POP"},            // 0x5A
      {"POP"},            // 0x5B
      {"POP"},            // 0x5C
      {"POP"},            // 0x5D
      {"POP"},            // 0x5E
      {"POP"},            // 0x5F
      {"---"},            // 0x60
      {"---"},            // 0x61
      {"BOUND"},          // 0x62
      {"ARPL"},           // 0x63
      {"???"},            // 0x64
      {"???"},            // 0x65
      {"???"},            // 0x66
      {"???"},            // 0x67
      {"PUSH"},           // 0x68
      {"IMUL"},           // 0x69
      {"PUSH"},           // 0x6A
      {"IMUL"},           // 0x6B
      {"INSB"},           // 0x6C
      {"---"},            // 0x6D
      {"OUTSB"},          // 0x6E
      {"---"},            // 0x6F
      {"JO "},            // 0x70
      {"JNO"},            // 0x71
      {"JB"},             // 0x72
      {"JNB"},            // 0x73
      {"JE"},             // 0x74
      {"JNE"},            // 0x75
      {"JBE"},            // 0x76
      {"JA"},             // 0x77
      {"JS"},             // 0x78
      {"JNS"},            // 0x79
      {"JP"},             // 0x7A
      {"JNP"},            // 0x7B
      {"JL"},             // 0x7C
      {"JGE"},            // 0x7D
      {"JLE"},            // 0x7E
      {"JG"},             // 0x7F
      {"---"},            // 0x80
      {"---"},            // 0x81
      {"???"},            // 0x82
      {"---"},            // 0x83
      {"TEST"},           // 0x84
      {"TEST"},           // 0x85
      {"XCHG"},           // 0x86
      {"XCHG"},           // 0x87
      {"MOV"},            // 0x88
      {"MOV"},            // 0x89
      {"MOV"},            // 0x8A
      {"MOV"},            // 0x8B
      {"MOV"},            // 0x8C
      {"LEA"},            // 0x8D
      {"MOV"},            // 0x8E
      {"---"},            // 0x8F
      {"NOP"},            // 0x90
      {"XCHG"},           // 0x91
      {"XCHG"},           // 0x92
      {"XCHG"},           // 0x93
      {"XCHG"},           // 0x94
      {"XCHG"},           // 0x95
      {"XCHG"},           // 0x96
      {"XCHG"},           // 0x97
      {"---"},            // 0x98
      {"---"},            // 0x99
      {"CALL"},           // 0x9A
      {"WAIT"},           // 0x9B
      {"???"},            // 0x9C
      {"---"},            // 0x9D
      {"SAHF"},           // 0x9E
      {"LAHF"},           // 0x9F
      {"MOV"},            // 0xA0  
      {"MOV"},            // 0xA1 
      {"MOV"},            // 0xA2
      {"MOV"},            // 0xA3 
      {"MOVSB"},          // 0xA4
      {"---"},            // 0xA5
      {"CMPSB"},          // 0xA6
      {"CMPS"},           // 0xA7
      {"TEST"},           // 0xA8
      {"TEST"},           // 0xA9
      {"STOSB"},          // 0xAA
      {"---"},            // 0xAB
      {"LODSB"},          // 0xAC
      {"---"},            // 0xAD
      {"SCASB"},          // 0xAE
      {"---"},            // 0xAF
      {"MOV"},            // 0xB0
      {"MOV"},            // 0xB1
      {"MOV"},            // 0xB2
      {"MOV"},            // 0xB3
      {"MOV"},            // 0xB4
      {"MOV"},            // 0xB5
      {"MOV"},            // 0xB6
      {"MOV"},            // 0xB7
      {"MOV"},            // 0xB8
      {"MOV"},            // 0xB9
      {"MOV"},            // 0xBA
      {"MOV"},            // 0xBB
      {"MOV"},            // 0xBC
      {"MOV"},            // 0xBD
      {"MOV"},            // 0xBE
      {"MOV"},            // 0xBF
      {"---"},            // 0xC0
      {"---"},            // 0xC1          
      {"RET"},            // 0xC2
      {"RET"},            // 0xC3
      {"LES"},            // 0xC4
      {"LDS"},            // 0xC5
      {"MOV"},            // 0xC6
      {"MOV"},            // 0xC7
      {"ENTER"},          // 0xC8
      {"LEAVE"},          // 0xC9
      {"RETF"},           // 0xCA
      {"RET"},            // 0xCB
      {"INT3"},           // 0xCC
      {"INT"},            // 0xCD
      {"INTO"},           // 0xCE
      {"IRET"},           // 0xCF
      {"---"},            // 0xD0
      {"---"},            // 0xD1
      {"---"},            // 0xD2
      {"---"},            // 0xD3
      {"???"},            // 0xD4
      {"???"},            // 0xD5
      {"???"},            // 0xD6
      {"XLATB"},          // 0xD7
      {"---"},            // 0xD8
      {"???"},            // 0xD9
      {"???"},            // 0xDA
      {"???"},            // 0xDB
      {"FADD"},           // 0xDC
      {"???"},            // 0xDD
      {"FADDP"},          // 0xDE
      {"???"},            // 0xDF
      {"LOOPNE"},         // 0xE0
      {"LOOPE"},          // 0xE1
      {"LOOP"},           // 0xE2
      {"---"},            // 0xE3
      {"IN"},             // 0xE4
      {"IN"},             // 0xE5
      {"OUT"},            // 0xE6
      {"OUT"},            // 0xE7
      {"CALL"},           // 0xE8
      {"JMP"},            // 0xE9
      {"JMP"},            // 0xEA
      {"JMP"},            // 0xEB
      {"IN"},             // 0xEC
      {"IN"},             // 0xED
      {"OUT"},            // 0xEE
      {"OUT"},            // 0xEF
      {"LOCK"},           // 0xF0
      {"???"},            // 0xF1
      {"REPNE"},          // 0xF2
      {"REP"},            // 0xF3
      {"HLT"},            // 0xF4
      {"CMC"},            // 0xF5
      {"---"},            // 0xF6
      {"---"},            // 0xF7
      {"CLC"},            // 0xF8
      {"STC"},            // 0xF9
      {"CLI"},            // 0xFA
      {"STI"},            // 0xFB
      {"CLD"},            // 0xFC
      {"STD"},            // 0xFD
      {"---"},            // 0xFE
      {"---"}             // 0xFF
  }; 
const static char intel_mnemonic_0F [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"---"},            // 0x0                      --- 0F --- 
      {"---"},            // 0x1                      --- 0F ---
      {"LAR"},            // 0x2                      --- 0F ---
      {"LSL"},            // 0x3                      --- 0F ---
      {"???"},            // 0x4                      --- 0F ---
      {"???"},            // 0x5                      --- 0F ---
      {"CLTS"},           // 0x6                      --- 0F ---
      {"???"},            // 0x7                      --- 0F ---
      {"INVD"},           // 0x8                      --- 0F ---
      {"WBINVD"},         // 0x9                      --- 0F ---
      {"???"},            // 0xA                      --- 0F ---
      {"???"},            // 0xB                      --- 0F ---
      {"???"},            // 0xC                      --- 0F ---
      {"???"},            // 0xD                      --- 0F ---
      {"FEMMS"},          // 0xE                      --- 0F ---
      {"???"},            // 0xF                      --- 0F ---
      {"???"},            // 0x10                     --- 0F ---
      {"???"},            // 0x11                     --- 0F ---
      {"???"},            // 0x12                     --- 0F ---
      {"???"},            // 0x13                     --- 0F ---
      {"???"},            // 0x14                     --- 0F ---
      {"???"},            // 0x15                     --- 0F ---
      {"???"},            // 0x16                     --- 0F ---
      {"???"},            // 0x17                     --- 0F ---
      {"???"},            // 0x18                     --- 0F ---
      {"???"},            // 0x19                     --- 0F ---
      {"???"},            // 0x1A                     --- 0F ---
      {"???"},            // 0x1B                     --- 0F ---
      {"???"},            // 0x1C                     --- 0F ---
      {"???"},            // 0x1D                     --- 0F ---
      {"???"},            // 0x1E                     --- 0F ---
      {"???"},            // 0x1F                     --- 0F ---
      {"MOV"},            // 0x20                     --- 0F ---
      {"MOV"},            // 0x21                     --- 0F ---
      {"MOV"},            // 0x22                     --- 0F ---
      {"MOV"},            // 0x23                     --- 0F ---
      {"MOV"},            // 0x24                     --- 0F ---
      {"???"},            // 0x25                     --- 0F ---
      {"MOV"},            // 0x26                     --- 0F ---
      {"???"},            // 0x27                     --- 0F ---
      {"???"},            // 0x28                     --- 0F ---
      {"???"},            // 0x29                     --- 0F ---
      {"???"},            // 0x2A                     --- 0F ---
      {"???"},            // 0x2B                     --- 0F ---
      {"???"},            // 0x2C                     --- 0F ---
      {"???"},            // 0x2D                     --- 0F ---
      {"???"},            // 0x2E                     --- 0F ---
      {"???"},            // 0x2F                     --- 0F ---
      {"WRMSR"},          // 0x30                     --- 0F ---
      {"???"},            // 0x31                     --- 0F ---
      {"RDMSR"},          // 0x32                     --- 0F ---
      {"???"},            // 0x33                     --- 0F ---
      {"???"},            // 0x34                     --- 0F ---
      {"???"},            // 0x35                     --- 0F ---
      {"???"},            // 0x36                     --- 0F ---
      {"???"},            // 0x37                     --- 0F ---
      {"???"},            // 0x38                     --- 0F ---
      {"???"},            // 0x39                     --- 0F ---
      {"???"},            // 0x3A                     --- 0F ---
      {"???"},            // 0x3B                     --- 0F ---
      {"???"},            // 0x3C                     --- 0F ---
      {"???"},            // 0x3D                     --- 0F ---
      {"???"},            // 0x3E                     --- 0F ---
      {"???"},            // 0x3F                     --- 0F ---
      {"???"},            // 0x40                     --- 0F ---
      {"???"},            // 0x41                     --- 0F ---
      {"???"},            // 0x42                     --- 0F ---
      {"???"},            // 0x43                     --- 0F ---
      {"???"},            // 0x44                     --- 0F ---
      {"???"},            // 0x45                     --- 0F ---
      {"???"},            // 0x46                     --- 0F ---
      {"???"},            // 0x47                     --- 0F ---
      {"???"},            // 0x48                     --- 0F ---
      {"???"},            // 0x49                     --- 0F ---
      {"???"},            // 0x4A                     --- 0F ---
      {"???"},            // 0x4B                     --- 0F ---
      {"???"},            // 0x4C                     --- 0F ---
      {"???"},            // 0x4D                     --- 0F ---
      {"???"},            // 0x4E                     --- 0F ---
      {"???"},            // 0x4F                     --- 0F ---
      {"???"},            // 0x50                     --- 0F ---
      {"???"},            // 0x51                     --- 0F ---
      {"???"},            // 0x52                     --- 0F ---
      {"???"},            // 0x53                     --- 0F ---
      {"???"},            // 0x54                     --- 0F ---
      {"???"},            // 0x55                     --- 0F ---
      {"???"},            // 0x56                     --- 0F ---
      {"???"},            // 0x57                     --- 0F ---
      {"???"},            // 0x58                     --- 0F ---
      {"???"},            // 0x59                     --- 0F ---
      {"???"},            // 0x5A                     --- 0F ---
      {"???"},            // 0x5B                     --- 0F ---
      {"???"},            // 0x5C                     --- 0F ---
      {"???"},            // 0x5D                     --- 0F ---
      {"???"},            // 0x5E                     --- 0F ---
      {"???"},            // 0x5F                     --- 0F ---
      {"???"},            // 0x60                     --- 0F ---
      {"???"},            // 0x61                     --- 0F ---
      {"???"},            // 0x62                     --- 0F ---
      {"???"},            // 0x63                     --- 0F ---
      {"???"},            // 0x64                     --- 0F ---
      {"???"},            // 0x65                     --- 0F ---
      {"???"},            // 0x66                     --- 0F ---
      {"???"},            // 0x67                     --- 0F ---
      {"???"},            // 0x68                     --- 0F ---
      {"???"},            // 0x69                     --- 0F ---
      {"???"},            // 0x6A                     --- 0F ---
      {"???"},            // 0x6B                     --- 0F ---
      {"???"},            // 0x6C                     --- 0F ---
      {"???"},            // 0x6D                     --- 0F ---
      {"???"},            // 0x6E                     --- 0F ---
      {"???"},            // 0x6F                     --- 0F ---
      {"???"},            // 0x70                     --- 0F ---
      {"???"},            // 0x71                     --- 0F ---
      {"???"},            // 0x72                     --- 0F ---
      {"???"},            // 0x73                     --- 0F ---
      {"???"},            // 0x74                     --- 0F ---
      {"???"},            // 0x75                     --- 0F ---
      {"???"},            // 0x76                     --- 0F ---
      {"???"},            // 0x77                     --- 0F ---
      {"???"},            // 0x78                     --- 0F ---
      {"???"},            // 0x79                     --- 0F ---
      {"???"},            // 0x7A                     --- 0F ---
      {"???"},            // 0x7B                     --- 0F ---
      {"???"},            // 0x7C                     --- 0F ---
      {"???"},            // 0x7D                     --- 0F ---
      {"???"},            // 0x7E                     --- 0F ---
      {"???"},            // 0x7F                     --- 0F ---
      {"JO"},             // 0x80                     --- 0F ---
      {"JNE"},            // 0x81                     --- 0F ---
      {"JB"},             // 0x82                     --- 0F ---
      {"JAE"},            // 0x83                     --- 0F ---
      {"JZ"},             // 0x84                     --- 0F ---
      {"JNE"},            // 0x85                     --- 0F ---
      {"JBE"},            // 0x86                     --- 0F ---
      {"JA"},             // 0x87                     --- 0F ---
      {"JS"},             // 0x88                     --- 0F ---
      {"JNS"},            // 0x89                     --- 0F ---
      {"JP"},             // 0x8A                     --- 0F ---
      {"JNP"},            // 0x8B                     --- 0F ---
      {"JL"},             // 0x8C                     --- 0F ---
      {"JGE"},            // 0x8D                     --- 0F ---
      {"JLE"},            // 0x8E                     --- 0F ---
      {"JG"},             // 0x8F                     --- 0F ---
      {"SETO"},           // 0x90                     --- 0F ---
      {"SETNO"},          // 0x91                     --- 0F ---
      {"SETC"},           // 0x92                     --- 0F ---
      {"SETAE"},          // 0x93                     --- 0F ---
      {"SETE"},           // 0x94                     --- 0F ---
      {"SETNZ"},          // 0x95                     --- 0F ---
      {"SETBE"},          // 0x96                     --- 0F ---
      {"SETA"},           // 0x97                     --- 0F ---
      {"SETS"},           // 0x98                     --- 0F ---
      {"SETNS"},          // 0x99                     --- 0F ---
      {"SETPE"},          // 0x9A                     --- 0F ---
      {"SETPO"},          // 0x9B                     --- 0F ---
      {"SETL"},           // 0x9C                     --- 0F ---
      {"SETNL"},          // 0x9D                     --- 0F ---
      {"SETNG"},          // 0x9E                     --- 0F ---
      {"SETG"},           // 0x9F                     --- 0F ---
      {"PUSH"},           // 0xA0                     --- 0F ---
      {"POP"},            // 0xA1                     --- 0F ---
      {"CPUID"},          // 0xA2                     --- 0F ---
      {"BT"},             // 0xA3                     --- 0F ---
      {"SHLD"},           // 0xA4                     --- 0F ---
      {"SHLD"},           // 0xA5                     --- 0F ---
      {"???"},            // 0xA6                     --- 0F ---
      {"???"},            // 0xA7                     --- 0F ---
      {"PUSH"},           // 0xA8                     --- 0F ---
      {"POP"},            // 0xA9                     --- 0F ---
      {"RSM"},            // 0xAA                     --- 0F ---
      {"BTS"},            // 0xAB                     --- 0F ---
      {"SHRD"},           // 0xAC                     --- 0F ---
      {"SHRD"},           // 0xAD                     --- 0F ---
      {"???"},            // 0xAE                     --- 0F ---
      {"IMUL"},           // 0xAF                     --- 0F ---
      {"CMPXCHG"},        // 0xB0                     --- 0F ---
      {"CMPXCHG"},        // 0xB1                     --- 0F ---
      {"LSS"},            // 0xB2                     --- 0F ---
      {"BTR"},            // 0xB3                     --- 0F ---
      {"LFS"},            // 0xB4                     --- 0F ---
      {"LGS"},            // 0xB5                     --- 0F ---
      {"MOVZX"},          // 0xB6                     --- 0F ---
      {"MOVZX"},          // 0xB7                     --- 0F ---
      {"???"},            // 0xB8                     --- 0F ---
      {"???"},            // 0xB9                     --- 0F ---
      {"---"},            // 0xBA                     --- 0F ---
      {"BTC"},            // 0xBB                     --- 0F ---
      {"BSF"},            // 0xBC                     --- 0F ---
      {"BSR"},            // 0xBD                     --- 0F ---
      {"MOVSX"},          // 0xBE                     --- 0F ---
      {"MOVSX"},          // 0xBF                     --- 0F ---
      {"XADD"},           // 0xC0                     --- 0F ---
      {"XADD"},           // 0xC1                     --- 0F ---
      {"???"},            // 0xC2                     --- 0F ---
      {"???"},            // 0xC3                     --- 0F ---
      {"???"},            // 0xC4                     --- 0F ---
      {"???"},            // 0xC5                     --- 0F ---
      {"???"},            // 0xC6                     --- 0F ---
      {"***"},            // 0xC7                     --- 0F ---
      {"BSWAP"},          // 0xC8                     --- 0F ---
      {"???"},            // 0xC9                     --- 0F ---
      {"???"},            // 0xCA                     --- 0F ---
      {"???"},            // 0xCB                     --- 0F ---
      {"???"},            // 0xCC                     --- 0F ---
      {"???"},            // 0xCD                     --- 0F ---
      {"???"},            // 0xCE                     --- 0F ---
      {"???"},            // 0xCF                     --- 0F ---
      {"???"},            // 0xD0                     --- 0F ---
      {"???"},            // 0xD1                     --- 0F ---
      {"???"},            // 0xD2                     --- 0F ---
      {"???"},            // 0xD3                     --- 0F ---
      {"???"},            // 0xD4                     --- 0F ---
      {"???"},            // 0xD5                     --- 0F ---
      {"???"},            // 0xD6                     --- 0F ---
      {"???"},            // 0xD7                     --- 0F ---
      {"???"},            // 0xD8                     --- 0F ---
      {"???"},            // 0xD9                     --- 0F ---
      {"???"},            // 0xDA                     --- 0F ---
      {"???"},            // 0xDB                     --- 0F ---
      {"???"},            // 0xDC                     --- 0F ---
      {"???"},            // 0xDD                     --- 0F ---
      {"???"},            // 0xDE                     --- 0F ---
      {"???"},            // 0xDF                     --- 0F ---
      {"???"},            // 0xE0                     --- 0F ---
      {"???"},            // 0xE1                     --- 0F ---
      {"???"},            // 0xE2                     --- 0F ---
      {"???"},            // 0xE3                     --- 0F ---
      {"???"},            // 0xE4                     --- 0F ---
      {"???"},            // 0xE5                     --- 0F ---
      {"???"},            // 0xE6                     --- 0F ---
      {"???"},            // 0xE7                     --- 0F ---
      {"???"},            // 0xE8                     --- 0F ---
      {"???"},            // 0xE9                     --- 0F ---
      {"???"},            // 0xEA                     --- 0F ---
      {"???"},            // 0xEB                     --- 0F ---
      {"???"},            // 0xEC                     --- 0F ---
      {"???"},            // 0xED                     --- 0F ---
      {"???"},            // 0xEE                     --- 0F ---
      {"???"},            // 0xEF                     --- 0F ---
      {"???"},            // 0xF0                     --- 0F ---
      {"???"},            // 0xF1                     --- 0F ---
      {"???"},            // 0xF2                     --- 0F ---
      {"???"},            // 0xF3                     --- 0F ---
      {"???"},            // 0xF4                     --- 0F ---
      {"???"},            // 0xF5                     --- 0F ---
      {"???"},            // 0xF6                     --- 0F ---
      {"???"},            // 0xF7                     --- 0F ---
      {"???"},            // 0xF8                     --- 0F ---
      {"???"},            // 0xF9                     --- 0F ---
      {"???"},            // 0xFA                     --- 0F ---
      {"???"},            // 0xFB                     --- 0F ---
      {"???"},            // 0xFC                     --- 0F ---
      {"???"},            // 0xFD                     --- 0F ---
      {"???"},            // 0xFE                     --- 0F ---
      {"???"}             // 0xFF                     --- 0F ---
  }; 
const static char intel_mnemonic_0F_00 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"SLDT"},           // 0x0                      --- 0F ---
      {"STR"},            // 0x1                      --- 0F ---
      {"LLDT"},           // 0x2                      --- 0F ---
      {"LTR"},            // 0x3                      --- 0F ---
      {"VERR"},           // 0x4                      --- 0F ---
      {"VERW"},           // 0x5                      --- 0F ---
      {"???"},            // 0x6                      --- 0F ---
      {"???"}             // 0x7                      --- 0F ---
};
const static char intel_mnemonic_0F_01 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"SGDT"},           // 0x0                      --- 0F ---
      {"SIDT"},           // 0x1                      --- 0F ---
      {"LGDT"},           // 0x2                      --- 0F ---
      {"LIDT"},           // 0x3                      --- 0F ---
      {"SMSW"},           // 0x4                      --- 0F ---
      {"???"},            // 0x5                      --- 0F ---
      {"LMSW"},           // 0x6                      --- 0F ---
      {"INVLPG"}          // 0x7                      --- 0F ---
};
const static char intel_mnemonic_0F_AE [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FXSAVE"},         // 0x0                      --- 0F ---
      {"FXRSTOR"},        // 0x1                      --- 0F ---
      {"???"},            // 0x2                      --- 0F ---
      {"???"},            // 0x3                      --- 0F ---
      {"???"},            // 0x4                      --- 0F ---
      {"???"},            // 0x5                      --- 0F ---
      {"???"},            // 0x6                      --- 0F ---
      {"???"}             // 0x7                      --- 0F ---
};
const static char intel_mnemonic_0F_BA [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"???"},            // 0x0                      --- 0F ---
      {"???"},            // 0x1                      --- 0F ---
      {"???"},            // 0x2                      --- 0F ---
      {"???"},            // 0x3                      --- 0F ---
      {"BT"},             // 0x4                      --- 0F ---
      {"BTS"},            // 0x5                      --- 0F ---
      {"BTR"},            // 0x6                      --- 0F ---
      {"BTC"}             // 0x7                      --- 0F ---
};
const static char intel_mnemonic_60 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"PUSHAW"},         // 16
      {"PUSHAD"}          // 32
};
const static char intel_mnemonic_61 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"POPAW"},          // 16
      {"POPAD"}           // 32
};
const static char intel_mnemonic_6D [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"INSW"},           // 16
      {"INSD"}            // 32
};
const static char intel_mnemonic_6F [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"OUTSW"},          // 16
      {"OUTSD"}           // 32
};
const static char intel_mnemonic_80 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ADD"},            // 0x0
      {"OR"},             // 0x1
      {"ADC"},            // 0x2
      {"SBB"},            // 0x3
      {"AND"},            // 0x4
      {"SUB"},            // 0x5
      {"XOR"},            // 0x6
      {"CMP"}             // 0x7
};
const static char intel_mnemonic_81 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ADD"},            // 0x0
      {"OR"},             // 0x1
      {"ADC"},            // 0x2
      {"SBB"},            // 0x3
      {"AND"},            // 0x4
      {"SUB"},            // 0x5
      {"XOR"},            // 0x6
      {"CMP"}             // 0x7
};
const static char intel_mnemonic_83 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ADD"},            // 0x0
      {"OR"},             // 0x1
      {"ADC"},            // 0x2
      {"SBB"},            // 0x3
      {"AND"},            // 0x4
      {"SUB"},            // 0x5
      {"XOR"},            // 0x6
      {"CMP"}             // 0x7
};
const static char intel_mnemonic_8F [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"POP"},            // 0x0
      {"???"},            // 0x1
      {"???"},            // 0x2
      {"???"},            // 0x3
      {"???"},            // 0x4
      {"???"},            // 0x5
      {"???"},            // 0x6
      {"???"}             // 0x7
};
const static char intel_mnemonic_98 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"CBW"},          // 16
      {"CWDE"}          // 32
};
const static char intel_mnemonic_99 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"CWD"},          // 16
      {"CDQ"}           // 32
};
const static char intel_mnemonic_9C [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"PUSHFW"},         // 16
      {"PUSHFD"}          // 32
};
const static char intel_mnemonic_9D [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"POPFW"},          // 16
      {"POPFD"}           // 32
};
const static char intel_mnemonic_A5 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"MOVSW"},          // 16
      {"MOVSD"}           // 32
};
const static char intel_mnemonic_AB [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"STOSW"},          // 16
      {"STOSD"}           // 32
};
const static char intel_mnemonic_AD [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"LODSW"},          // 16
      {"LODSD"}           // 32
};
const static char intel_mnemonic_AF [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"SCASW"},          // 16
      {"SCASD"}           // 32
};
const static char intel_mnemonic_C0 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ROL"},            // 0x0
      {"ROR"},            // 0x1
      {"RCL"},            // 0x2
      {"RCR"},            // 0x3
      {"SHL"},            // 0x4
      {"???"},            // 0x5
      {"???"},            // 0x6
      {"SHR"}             // 0x7
};
const static char intel_mnemonic_C1 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ROL"},            // 0x0
      {"ROR"},            // 0x1
      {"RCL"},            // 0x2
      {"RCR"},            // 0x3
      {"SHL"},            // 0x4
      {"SHR"},            // 0x5
      {"???"},            // 0x6
      {"SAR"}             // 0x7
};
const static char intel_mnemonic_D0 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ROL"},            // 0x0
      {"ROR"},            // 0x1
      {"RCL"},            // 0x2
      {"RCR"},            // 0x3
      {"SHL"},            // 0x4
      {"SHR"},            // 0x5
      {"???"},            // 0x6
      {"SAR"}             // 0x7
};
const static char intel_mnemonic_D1 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ROL"},            // 0x0
      {"ROR"},            // 0x1
      {"RCL"},            // 0x2
      {"RCR"},            // 0x3
      {"SHL"},            // 0x4
      {"SHR"},            // 0x5
      {"???"},            // 0x6
      {"SAR"}             // 0x7
};
const static char intel_mnemonic_D2 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ROL"},            // 0x0
      {"ROR"},            // 0x1
      {"RCL"},            // 0x2
      {"RCR"},            // 0x3
      {"SHL"},            // 0x4
      {"SHR"},            // 0x5
      {"???"},            // 0x6
      {"SHR"}             // 0x7
};
const static char intel_mnemonic_D3 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"ROL"},            // 0x0
      {"ROR"},            // 0x1
      {"RCL"},            // 0x2
      {"RCR"},            // 0x3
      {"SHL"},            // 0x4
      {"SHR"},            // 0x5
      {"???"},            // 0x6
      {"SAR"}             // 0x7
};
const static char intel_mnemonic_D8 [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"???"},            // 0x0                      --- D8 --- 
      {"???"},            // 0x1                      --- D8 ---
      {"???"},            // 0x2                      --- D8 ---
      {"???"},            // 0x3                      --- D8 ---
      {"???"},            // 0x4                      --- D8 ---
      {"???"},            // 0x5                      --- D8 ---
      {"???"},            // 0x6                      --- D8 ---
      {"???"},            // 0x7                      --- D8 ---
      {"???"},            // 0x8                      --- D8 ---
      {"???"},            // 0x9                      --- D8 ---
      {"???"},            // 0xA                      --- D8 ---
      {"???"},            // 0xB                      --- D8 ---
      {"???"},            // 0xC                      --- D8 ---
      {"???"},            // 0xD                      --- D8 ---
      {"???"},            // 0xE                      --- D8 ---
      {"???"},            // 0xF                      --- D8 ---
      {"???"},            // 0x10                     --- D8 ---
      {"???"},            // 0x11                     --- D8 ---
      {"???"},            // 0x12                     --- D8 ---
      {"???"},            // 0x13                     --- D8 ---
      {"???"},            // 0x14                     --- D8 ---
      {"???"},            // 0x15                     --- D8 ---
      {"???"},            // 0x16                     --- D8 ---
      {"???"},            // 0x17                     --- D8 ---
      {"???"},            // 0x18                     --- D8 ---
      {"???"},            // 0x19                     --- D8 ---
      {"???"},            // 0x1A                     --- D8 ---
      {"???"},            // 0x1B                     --- D8 ---
      {"???"},            // 0x1C                     --- D8 ---
      {"???"},            // 0x1D                     --- D8 ---
      {"???"},            // 0x1E                     --- D8 ---
      {"???"},            // 0x1F                     --- D8 ---
      {"???"},            // 0x20                     --- D8 ---
      {"???"},            // 0x21                     --- D8 ---
      {"???"},            // 0x22                     --- D8 ---
      {"???"},            // 0x23                     --- D8 ---
      {"???"},            // 0x24                     --- D8 ---
      {"???"},            // 0x25                     --- D8 ---
      {"???"},            // 0x26                     --- D8 ---
      {"???"},            // 0x27                     --- D8 ---
      {"???"},            // 0x28                     --- D8 ---
      {"???"},            // 0x29                     --- D8 ---
      {"???"},            // 0x2A                     --- D8 ---
      {"???"},            // 0x2B                     --- D8 ---
      {"???"},            // 0x2C                     --- D8 ---
      {"???"},            // 0x2D                     --- D8 ---
      {"???"},            // 0x2E                     --- D8 ---
      {"???"},            // 0x2F                     --- D8 ---
      {"???"},            // 0x30                     --- D8 ---
      {"???"},            // 0x31                     --- D8 ---
      {"???"},            // 0x32                     --- D8 ---
      {"???"},            // 0x33                     --- D8 ---
      {"???"},            // 0x34                     --- D8 ---
      {"???"},            // 0x35                     --- D8 ---
      {"???"},            // 0x36                     --- D8 ---
      {"???"},            // 0x37                     --- D8 ---
      {"???"},            // 0x38                     --- D8 ---
      {"???"},            // 0x39                     --- D8 ---
      {"???"},            // 0x3A                     --- D8 ---
      {"???"},            // 0x3B                     --- D8 ---
      {"???"},            // 0x3C                     --- D8 ---
      {"???"},            // 0x3D                     --- D8 ---
      {"???"},            // 0x3E                     --- D8 ---
      {"???"},            // 0x3F                     --- D8 ---
      {"???"},            // 0x40                     --- D8 ---
      {"???"},            // 0x41                     --- D8 ---
      {"???"},            // 0x42                     --- D8 ---
      {"???"},            // 0x43                     --- D8 ---
      {"???"},            // 0x44                     --- D8 ---
      {"???"},            // 0x45                     --- D8 ---
      {"???"},            // 0x46                     --- D8 ---
      {"???"},            // 0x47                     --- D8 ---
      {"???"},            // 0x48                     --- D8 ---
      {"???"},            // 0x49                     --- D8 ---
      {"???"},            // 0x4A                     --- D8 ---
      {"???"},            // 0x4B                     --- D8 ---
      {"???"},            // 0x4C                     --- D8 ---
      {"???"},            // 0x4D                     --- D8 ---
      {"???"},            // 0x4E                     --- D8 ---
      {"???"},            // 0x4F                     --- D8 ---
      {"???"},            // 0x50                     --- D8 ---
      {"???"},            // 0x51                     --- D8 ---
      {"???"},            // 0x52                     --- D8 ---
      {"???"},            // 0x53                     --- D8 ---
      {"???"},            // 0x54                     --- D8 ---
      {"???"},            // 0x55                     --- D8 ---
      {"???"},            // 0x56                     --- D8 ---
      {"???"},            // 0x57                     --- D8 ---
      {"???"},            // 0x58                     --- D8 ---
      {"???"},            // 0x59                     --- D8 ---
      {"???"},            // 0x5A                     --- D8 ---
      {"???"},            // 0x5B                     --- D8 ---
      {"???"},            // 0x5C                     --- D8 ---
      {"???"},            // 0x5D                     --- D8 ---
      {"???"},            // 0x5E                     --- D8 ---
      {"???"},            // 0x5F                     --- D8 ---
      {"???"},            // 0x60                     --- D8 ---
      {"???"},            // 0x61                     --- D8 ---
      {"???"},            // 0x62                     --- D8 ---
      {"???"},            // 0x63                     --- D8 ---
      {"???"},            // 0x64                     --- D8 ---
      {"???"},            // 0x65                     --- D8 ---
      {"???"},            // 0x66                     --- D8 ---
      {"???"},            // 0x67                     --- D8 ---
      {"???"},            // 0x68                     --- D8 ---
      {"???"},            // 0x69                     --- D8 ---
      {"???"},            // 0x6A                     --- D8 ---
      {"???"},            // 0x6B                     --- D8 ---
      {"???"},            // 0x6C                     --- D8 ---
      {"???"},            // 0x6D                     --- D8 ---
      {"???"},            // 0x6E                     --- D8 ---
      {"???"},            // 0x6F                     --- D8 ---
      {"???"},            // 0x70                     --- D8 ---
      {"???"},            // 0x71                     --- D8 ---
      {"???"},            // 0x72                     --- D8 ---
      {"???"},            // 0x73                     --- D8 ---
      {"???"},            // 0x74                     --- D8 ---
      {"???"},            // 0x75                     --- D8 ---
      {"???"},            // 0x76                     --- D8 ---
      {"???"},            // 0x77                     --- D8 ---
      {"???"},            // 0x78                     --- D8 ---
      {"???"},            // 0x79                     --- D8 ---
      {"???"},            // 0x7A                     --- D8 ---
      {"???"},            // 0x7B                     --- D8 ---
      {"???"},            // 0x7C                     --- D8 ---
      {"???"},            // 0x7D                     --- D8 ---
      {"???"},            // 0x7E                     --- D8 ---
      {"???"},            // 0x7F                     --- D8 ---
      {"???"},            // 0x80                     --- D8 ---
      {"???"},            // 0x81                     --- D8 ---
      {"???"},            // 0x82                     --- D8 ---
      {"???"},            // 0x83                     --- D8 ---
      {"???"},            // 0x84                     --- D8 ---
      {"???"},            // 0x85                     --- D8 ---
      {"???"},            // 0x86                     --- D8 ---
      {"???"},            // 0x87                     --- D8 ---
      {"???"},            // 0x88                     --- D8 ---
      {"???"},            // 0x89                     --- D8 ---
      {"???"},            // 0x8A                     --- D8 ---
      {"???"},            // 0x8B                     --- D8 ---
      {"???"},            // 0x8C                     --- D8 ---
      {"???"},            // 0x8D                     --- D8 ---
      {"???"},            // 0x8E                     --- D8 ---
      {"???"},            // 0x8F                     --- D8 ---
      {"???"},            // 0x90                     --- D8 ---
      {"???"},            // 0x91                     --- D8 ---
      {"???"},            // 0x92                     --- D8 ---
      {"???"},            // 0x93                     --- D8 ---
      {"???"},            // 0x94                     --- D8 ---
      {"???"},            // 0x95                     --- D8 ---
      {"???"},            // 0x96                     --- D8 ---
      {"???"},            // 0x97                     --- D8 ---
      {"???"},            // 0x98                     --- D8 ---
      {"???"},            // 0x99                     --- D8 ---
      {"???"},            // 0x9A                     --- D8 ---
      {"???"},            // 0x9B                     --- D8 ---
      {"???"},            // 0x9C                     --- D8 ---
      {"???"},            // 0x9D                     --- D8 ---
      {"???"},            // 0x9E                     --- D8 ---
      {"???"},            // 0x9F                     --- D8 ---
      {"???"},            // 0xA0                     --- D8 ---
      {"???"},            // 0xA1                     --- D8 ---
      {"???"},            // 0xA2                     --- D8 ---
      {"???"},            // 0xA3                     --- D8 ---
      {"???"},            // 0xA4                     --- D8 ---
      {"???"},            // 0xA5                     --- D8 ---
      {"???"},            // 0xA6                     --- D8 ---
      {"???"},            // 0xA7                     --- D8 ---
      {"???"},            // 0xA8                     --- D8 ---
      {"???"},            // 0xA9                     --- D8 ---
      {"???"},            // 0xAA                     --- D8 ---
      {"???"},            // 0xAB                     --- D8 ---
      {"???"},            // 0xAC                     --- D8 ---
      {"???"},            // 0xAD                     --- D8 ---
      {"???"},            // 0xAE                     --- D8 ---
      {"???"},            // 0xAF                     --- D8 ---
      {"???"},            // 0xB0                     --- D8 ---
      {"???"},            // 0xB1                     --- D8 ---
      {"???"},            // 0xB2                     --- D8 ---
      {"???"},            // 0xB3                     --- D8 ---
      {"???"},            // 0xB4                     --- D8 ---
      {"???"},            // 0xB5                     --- D8 ---
      {"???"},            // 0xB6                     --- D8 ---
      {"???"},            // 0xB7                     --- D8 ---
      {"???"},            // 0xB8                     --- D8 ---
      {"???"},            // 0xB9                     --- D8 ---
      {"???"},            // 0xBA                     --- D8 ---
      {"???"},            // 0xBB                     --- D8 ---
      {"???"},            // 0xBC                     --- D8 ---
      {"???"},            // 0xBD                     --- D8 ---
      {"???"},            // 0xBE                     --- D8 ---
      {"???"},            // 0xBF                     --- D8 ---
      {"FADD"},           // 0xC0                     --- D8 ---
      {"FADD"},           // 0xC1                     --- D8 ---
      {"FADD"},           // 0xC2                     --- D8 ---
      {"FADD"},           // 0xC3                     --- D8 ---
      {"FADD"},           // 0xC4                     --- D8 ---
      {"FADD"},           // 0xC5                     --- D8 ---
      {"FADD"},           // 0xC6                     --- D8 ---
      {"FADD"},           // 0xC7                     --- D8 ---
      {"FMUL"},           // 0xC8                     --- D8 ---
      {"FMUL"},           // 0xC9                     --- D8 ---
      {"FMUL"},           // 0xCA                     --- D8 ---
      {"FMUL"},           // 0xCB                     --- D8 ---
      {"FMUL"},           // 0xCC                     --- D8 ---
      {"FMUL"},           // 0xCD                     --- D8 ---
      {"FMUL"},           // 0xCE                     --- D8 ---
      {"FMUL"},           // 0xCF                     --- D8 ---
      {"FCOM"},           // 0xD0                     --- D8 ---
      {"FCOM"},           // 0xD1                     --- D8 ---
      {"FCOM"},           // 0xD2                     --- D8 ---
      {"FCOM"},           // 0xD3                     --- D8 ---
      {"FCOM"},           // 0xD4                     --- D8 ---
      {"FCOM"},           // 0xD5                     --- D8 ---
      {"FCOM"},           // 0xD6                     --- D8 ---
      {"FCOM"},           // 0xD7                     --- D8 ---
      {"FCOMP"},          // 0xD8                     --- D8 ---
      {"FCOMP"},          // 0xD9                     --- D8 ---
      {"FCOMP"},          // 0xDA                     --- D8 ---
      {"FCOMP"},          // 0xDB                     --- D8 ---
      {"FCOMP"},          // 0xDC                     --- D8 ---
      {"FCOMP"},          // 0xDD                     --- D8 ---
      {"FCOMP"},          // 0xDE                     --- D8 ---
      {"FCOMP"},          // 0xDF                     --- D8 ---
      {"FSUB"},           // 0xE0                     --- D8 ---
      {"FSUB"},           // 0xE1                     --- D8 ---
      {"FSUB"},           // 0xE2                     --- D8 ---
      {"FSUB"},           // 0xE3                     --- D8 ---
      {"FSUB"},           // 0xE4                     --- D8 ---
      {"FSUB"},           // 0xE5                     --- D8 ---
      {"FSUB"},           // 0xE6                     --- D8 ---
      {"FSUB"},           // 0xE7                     --- D8 ---
      {"FSUBR"},          // 0xE8                     --- D8 ---
      {"FSUBR"},          // 0xE9                     --- D8 ---
      {"FSUBR"},          // 0xEA                     --- D8 ---
      {"FSUBR"},          // 0xEB                     --- D8 ---
      {"FSUBR"},          // 0xEC                     --- D8 ---
      {"FSUBR"},          // 0xED                     --- D8 ---
      {"FSUBR"},          // 0xEE                     --- D8 ---
      {"FSUBR"},          // 0xEF                     --- D8 ---
      {"FDIV"},           // 0xF0                     --- D8 ---
      {"FDIV"},           // 0xF1                     --- D8 ---
      {"FDIV"},           // 0xF2                     --- D8 ---
      {"FDIV"},           // 0xF3                     --- D8 ---
      {"FDIV"},           // 0xF4                     --- D8 ---
      {"FDIV"},           // 0xF5                     --- D8 ---
      {"FDIV"},           // 0xF6                     --- D8 ---
      {"FDIV"},           // 0xF7                     --- D8 ---
      {"FDIVR"},          // 0xF8                     --- D8 ---
      {"FDIVR"},          // 0xF9                     --- D8 ---
      {"FDIVR"},          // 0xFA                     --- D8 ---
      {"FDIVR"},          // 0xFB                     --- D8 ---
      {"FDIVR"},          // 0xFC                     --- D8 ---
      {"FDIVR"},          // 0xFD                     --- D8 ---
      {"FDIVR"},          // 0xFE                     --- D8 ---
      {"FDIVR"}           // 0xFF                     --- D8 ---
  }; 
const static char intel_mnemonic_D8_ [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FADD"},           // 0x0
      {"FMUL"},           // 0x1
      {"FCOM"},           // 0x2
      {"FCOMP"},          // 0x3
      {"FSUB"},           // 0x4
      {"FSUBR"},          // 0x5
      {"FDIV"},           // 0x6
      {"FDIVR"}           // 0x7
};
const static char intel_mnemonic_D9 [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"???"},            // 0x0                      --- D9 --- 
      {"???"},            // 0x1                      --- D9 ---
      {"???"},            // 0x2                      --- D9 ---
      {"???"},            // 0x3                      --- D9 ---
      {"???"},            // 0x4                      --- D9 ---
      {"???"},            // 0x5                      --- D9 ---
      {"???"},            // 0x6                      --- D9 ---
      {"???"},            // 0x7                      --- D9 ---
      {"???"},            // 0x8                      --- D9 ---
      {"???"},            // 0x9                      --- D9 ---
      {"???"},            // 0xA                      --- D9 ---
      {"???"},            // 0xB                      --- D9 ---
      {"???"},            // 0xC                      --- D9 ---
      {"???"},            // 0xD                      --- D9 ---
      {"???"},            // 0xE                      --- D9 ---
      {"???"},            // 0xF                      --- D9 ---
      {"???"},            // 0x10                     --- D9 ---
      {"???"},            // 0x11                     --- D9 ---
      {"???"},            // 0x12                     --- D9 ---
      {"???"},            // 0x13                     --- D9 ---
      {"???"},            // 0x14                     --- D9 ---
      {"???"},            // 0x15                     --- D9 ---
      {"???"},            // 0x16                     --- D9 ---
      {"???"},            // 0x17                     --- D9 ---
      {"???"},            // 0x18                     --- D9 ---
      {"???"},            // 0x19                     --- D9 ---
      {"???"},            // 0x1A                     --- D9 ---
      {"???"},            // 0x1B                     --- D9 ---
      {"???"},            // 0x1C                     --- D9 ---
      {"???"},            // 0x1D                     --- D9 ---
      {"???"},            // 0x1E                     --- D9 ---
      {"???"},            // 0x1F                     --- D9 ---
      {"???"},            // 0x20                     --- D9 ---
      {"???"},            // 0x21                     --- D9 ---
      {"???"},            // 0x22                     --- D9 ---
      {"???"},            // 0x23                     --- D9 ---
      {"???"},            // 0x24                     --- D9 ---
      {"???"},            // 0x25                     --- D9 ---
      {"???"},            // 0x26                     --- D9 ---
      {"???"},            // 0x27                     --- D9 ---
      {"???"},            // 0x28                     --- D9 ---
      {"???"},            // 0x29                     --- D9 ---
      {"???"},            // 0x2A                     --- D9 ---
      {"???"},            // 0x2B                     --- D9 ---
      {"???"},            // 0x2C                     --- D9 ---
      {"???"},            // 0x2D                     --- D9 ---
      {"???"},            // 0x2E                     --- D9 ---
      {"???"},            // 0x2F                     --- D9 ---
      {"???"},            // 0x30                     --- D9 ---
      {"???"},            // 0x31                     --- D9 ---
      {"???"},            // 0x32                     --- D9 ---
      {"???"},            // 0x33                     --- D9 ---
      {"???"},            // 0x34                     --- D9 ---
      {"???"},            // 0x35                     --- D9 ---
      {"???"},            // 0x36                     --- D9 ---
      {"???"},            // 0x37                     --- D9 ---
      {"???"},            // 0x38                     --- D9 ---
      {"???"},            // 0x39                     --- D9 ---
      {"???"},            // 0x3A                     --- D9 ---
      {"???"},            // 0x3B                     --- D9 ---
      {"???"},            // 0x3C                     --- D9 ---
      {"???"},            // 0x3D                     --- D9 ---
      {"???"},            // 0x3E                     --- D9 ---
      {"???"},            // 0x3F                     --- D9 ---
      {"???"},            // 0x40                     --- D9 ---
      {"???"},            // 0x41                     --- D9 ---
      {"???"},            // 0x42                     --- D9 ---
      {"???"},            // 0x43                     --- D9 ---
      {"???"},            // 0x44                     --- D9 ---
      {"???"},            // 0x45                     --- D9 ---
      {"???"},            // 0x46                     --- D9 ---
      {"???"},            // 0x47                     --- D9 ---
      {"???"},            // 0x48                     --- D9 ---
      {"???"},            // 0x49                     --- D9 ---
      {"???"},            // 0x4A                     --- D9 ---
      {"???"},            // 0x4B                     --- D9 ---
      {"???"},            // 0x4C                     --- D9 ---
      {"???"},            // 0x4D                     --- D9 ---
      {"???"},            // 0x4E                     --- D9 ---
      {"???"},            // 0x4F                     --- D9 ---
      {"???"},            // 0x50                     --- D9 ---
      {"???"},            // 0x51                     --- D9 ---
      {"???"},            // 0x52                     --- D9 ---
      {"???"},            // 0x53                     --- D9 ---
      {"???"},            // 0x54                     --- D9 ---
      {"???"},            // 0x55                     --- D9 ---
      {"???"},            // 0x56                     --- D9 ---
      {"???"},            // 0x57                     --- D9 ---
      {"???"},            // 0x58                     --- D9 ---
      {"???"},            // 0x59                     --- D9 ---
      {"???"},            // 0x5A                     --- D9 ---
      {"???"},            // 0x5B                     --- D9 ---
      {"???"},            // 0x5C                     --- D9 ---
      {"???"},            // 0x5D                     --- D9 ---
      {"???"},            // 0x5E                     --- D9 ---
      {"???"},            // 0x5F                     --- D9 ---
      {"???"},            // 0x60                     --- D9 ---
      {"???"},            // 0x61                     --- D9 ---
      {"???"},            // 0x62                     --- D9 ---
      {"???"},            // 0x63                     --- D9 ---
      {"???"},            // 0x64                     --- D9 ---
      {"???"},            // 0x65                     --- D9 ---
      {"???"},            // 0x66                     --- D9 ---
      {"???"},            // 0x67                     --- D9 ---
      {"???"},            // 0x68                     --- D9 ---
      {"???"},            // 0x69                     --- D9 ---
      {"???"},            // 0x6A                     --- D9 ---
      {"???"},            // 0x6B                     --- D9 ---
      {"???"},            // 0x6C                     --- D9 ---
      {"???"},            // 0x6D                     --- D9 ---
      {"???"},            // 0x6E                     --- D9 ---
      {"???"},            // 0x6F                     --- D9 ---
      {"???"},            // 0x70                     --- D9 ---
      {"???"},            // 0x71                     --- D9 ---
      {"???"},            // 0x72                     --- D9 ---
      {"???"},            // 0x73                     --- D9 ---
      {"???"},            // 0x74                     --- D9 ---
      {"???"},            // 0x75                     --- D9 ---
      {"???"},            // 0x76                     --- D9 ---
      {"???"},            // 0x77                     --- D9 ---
      {"???"},            // 0x78                     --- D9 ---
      {"???"},            // 0x79                     --- D9 ---
      {"???"},            // 0x7A                     --- D9 ---
      {"???"},            // 0x7B                     --- D9 ---
      {"???"},            // 0x7C                     --- D9 ---
      {"???"},            // 0x7D                     --- D9 ---
      {"???"},            // 0x7E                     --- D9 ---
      {"???"},            // 0x7F                     --- D9 ---
      {"???"},            // 0x80                     --- D9 ---
      {"???"},            // 0x81                     --- D9 ---
      {"???"},            // 0x82                     --- D9 ---
      {"???"},            // 0x83                     --- D9 ---
      {"???"},            // 0x84                     --- D9 ---
      {"???"},            // 0x85                     --- D9 ---
      {"???"},            // 0x86                     --- D9 ---
      {"???"},            // 0x87                     --- D9 ---
      {"???"},            // 0x88                     --- D9 ---
      {"???"},            // 0x89                     --- D9 ---
      {"???"},            // 0x8A                     --- D9 ---
      {"???"},            // 0x8B                     --- D9 ---
      {"???"},            // 0x8C                     --- D9 ---
      {"???"},            // 0x8D                     --- D9 ---
      {"???"},            // 0x8E                     --- D9 ---
      {"???"},            // 0x8F                     --- D9 ---
      {"???"},            // 0x90                     --- D9 ---
      {"???"},            // 0x91                     --- D9 ---
      {"???"},            // 0x92                     --- D9 ---
      {"???"},            // 0x93                     --- D9 ---
      {"???"},            // 0x94                     --- D9 ---
      {"???"},            // 0x95                     --- D9 ---
      {"???"},            // 0x96                     --- D9 ---
      {"???"},            // 0x97                     --- D9 ---
      {"???"},            // 0x98                     --- D9 ---
      {"???"},            // 0x99                     --- D9 ---
      {"???"},            // 0x9A                     --- D9 ---
      {"???"},            // 0x9B                     --- D9 ---
      {"???"},            // 0x9C                     --- D9 ---
      {"???"},            // 0x9D                     --- D9 ---
      {"???"},            // 0x9E                     --- D9 ---
      {"???"},            // 0x9F                     --- D9 ---
      {"???"},            // 0xA0                     --- D9 ---
      {"???"},            // 0xA1                     --- D9 ---
      {"???"},            // 0xA2                     --- D9 ---
      {"???"},            // 0xA3                     --- D9 ---
      {"???"},            // 0xA4                     --- D9 ---
      {"???"},            // 0xA5                     --- D9 ---
      {"???"},            // 0xA6                     --- D9 ---
      {"???"},            // 0xA7                     --- D9 ---
      {"???"},            // 0xA8                     --- D9 ---
      {"???"},            // 0xA9                     --- D9 ---
      {"???"},            // 0xAA                     --- D9 ---
      {"???"},            // 0xAB                     --- D9 ---
      {"???"},            // 0xAC                     --- D9 ---
      {"???"},            // 0xAD                     --- D9 ---
      {"???"},            // 0xAE                     --- D9 ---
      {"???"},            // 0xAF                     --- D9 ---
      {"???"},            // 0xB0                     --- D9 ---
      {"???"},            // 0xB1                     --- D9 ---
      {"???"},            // 0xB2                     --- D9 ---
      {"???"},            // 0xB3                     --- D9 ---
      {"???"},            // 0xB4                     --- D9 ---
      {"???"},            // 0xB5                     --- D9 ---
      {"???"},            // 0xB6                     --- D9 ---
      {"???"},            // 0xB7                     --- D9 ---
      {"???"},            // 0xB8                     --- D9 ---
      {"???"},            // 0xB9                     --- D9 ---
      {"???"},            // 0xBA                     --- D9 ---
      {"???"},            // 0xBB                     --- D9 ---
      {"???"},            // 0xBC                     --- D9 ---
      {"???"},            // 0xBD                     --- D9 ---
      {"???"},            // 0xBE                     --- D9 ---
      {"???"},            // 0xBF                     --- D9 ---
      {"FLD"},            // 0xC0                     --- D9 ---
      {"FLD"},            // 0xC1                     --- D9 ---
      {"FLD"},            // 0xC2                     --- D9 ---
      {"FLD"},            // 0xC3                     --- D9 ---
      {"FLD"},            // 0xC4                     --- D9 ---
      {"FLD"},            // 0xC5                     --- D9 ---
      {"FLD"},            // 0xC6                     --- D9 ---
      {"FLD"},            // 0xC7                     --- D9 ---
      {"FXCH"},           // 0xC8                     --- D9 ---
      {"FXCH"},           // 0xC9                     --- D9 ---
      {"FXCH"},           // 0xCA                     --- D9 ---
      {"FXCH"},           // 0xCB                     --- D9 ---
      {"FXCH"},           // 0xCC                     --- D9 ---
      {"FXCH"},           // 0xCD                     --- D9 ---
      {"FXCH"},           // 0xCE                     --- D9 ---
      {"FXCH"},           // 0xCF                     --- D9 ---
      {"FNOP"},           // 0xD0                     --- D9 ---
      {"???"},            // 0xD1                     --- D9 ---
      {"???"},            // 0xD2                     --- D9 ---
      {"???"},            // 0xD3                     --- D9 ---
      {"???"},            // 0xD4                     --- D9 ---
      {"???"},            // 0xD5                     --- D9 ---
      {"???"},            // 0xD6                     --- D9 ---
      {"???"},            // 0xD7                     --- D9 ---
      {"???"},            // 0xD8                     --- D9 ---
      {"???"},            // 0xD9                     --- D9 ---
      {"???"},            // 0xDA                     --- D9 ---
      {"???"},            // 0xDB                     --- D9 ---
      {"???"},            // 0xDC                     --- D9 ---
      {"???"},            // 0xDD                     --- D9 ---
      {"???"},            // 0xDE                     --- D9 ---
      {"???"},            // 0xDF                     --- D9 ---
      {"FCHS"},           // 0xE0                     --- D9 ---
      {"FABS"},           // 0xE1                     --- D9 ---
      {"???"},            // 0xE2                     --- D9 ---
      {"???"},            // 0xE3                     --- D9 ---
      {"FTST"},           // 0xE4                     --- D9 ---
      {"FXAM"},           // 0xE5                     --- D9 ---
      {"???"},            // 0xE6                     --- D9 ---
      {"???"},            // 0xE7                     --- D9 ---
      {"FLD1"},           // 0xE8                     --- D9 ---
      {"FLDL2T"},         // 0xE9                     --- D9 ---
      {"FLDL2E"},         // 0xEA                     --- D9 ---
      {"FLDPI"},          // 0xEB                     --- D9 ---
      {"FLDLG2"},         // 0xEC                     --- D9 ---
      {"FLDLN2"},         // 0xED                     --- D9 ---
      {"FLDZ"},           // 0xEE                     --- D9 ---
      {"???"},            // 0xEF                     --- D9 ---
      {"F2XM1"},          // 0xF0                     --- D9 ---
      {"FYL2X"},          // 0xF1                     --- D9 ---
      {"FPTAN"},          // 0xF2                     --- D9 ---
      {"FPATAN"},         // 0xF3                     --- D9 ---
      {"FXTRACT"},        // 0xF4                     --- D9 ---
      {"FPREM1"},         // 0xF5                     --- D9 ---
      {"FDECSTP"},        // 0xF6                     --- D9 ---
      {"FINCSTP"},        // 0xF7                     --- D9 ---
      {"FPREM"},          // 0xF8                     --- D9 ---
      {"FYL2XP1"},        // 0xF9                     --- D9 ---
      {"FSQRT"},          // 0xFA                     --- D9 ---
      {"FSINCOS"},        // 0xFB                     --- D9 ---
      {"FRNDINT"},        // 0xFC                     --- D9 ---
      {"FSCALE"},         // 0xFD                     --- D9 ---
      {"FSIN"},           // 0xFE                     --- D9 ---
      {"FCOS"}            // 0xFF                     --- D9 ---
  }; 
const static char intel_mnemonic_D9_ [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FLD"},           // 0x0
      {"???"},           // 0x1
      {"FST"},           // 0x2
      {"FSTP"},          // 0x3
      {"FLDENV"},        // 0x4
      {"FLDCW"},         // 0x5
      {"---"},           // 0x6
      {"---"}            // 0x7
};
const static char intel_mnemonic_D9_6_ [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FSTENV"},           // wait
      {"FNSTENV"}           // no wait
};
const static char intel_mnemonic_D9_7_ [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FSTCW"},           // wait
      {"FNSTCW"}           // no wait
};
const static char intel_mnemonic_DA [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"???"},            // 0x0                      --- DA --- 
      {"???"},            // 0x1                      --- DA ---
      {"???"},            // 0x2                      --- DA ---
      {"???"},            // 0x3                      --- DA ---
      {"???"},            // 0x4                      --- DA ---
      {"???"},            // 0x5                      --- DA ---
      {"???"},            // 0x6                      --- DA ---
      {"???"},            // 0x7                      --- DA ---
      {"???"},            // 0x8                      --- DA ---
      {"???"},            // 0x9                      --- DA ---
      {"???"},            // 0xA                      --- DA ---
      {"???"},            // 0xB                      --- DA ---
      {"???"},            // 0xC                      --- DA ---
      {"???"},            // 0xD                      --- DA ---
      {"???"},            // 0xE                      --- DA ---
      {"???"},            // 0xF                      --- DA ---
      {"???"},            // 0x10                     --- DA ---
      {"???"},            // 0x11                     --- DA ---
      {"???"},            // 0x12                     --- DA ---
      {"???"},            // 0x13                     --- DA ---
      {"???"},            // 0x14                     --- DA ---
      {"???"},            // 0x15                     --- DA ---
      {"???"},            // 0x16                     --- DA ---
      {"???"},            // 0x17                     --- DA ---
      {"???"},            // 0x18                     --- DA ---
      {"???"},            // 0x19                     --- DA ---
      {"???"},            // 0x1A                     --- DA ---
      {"???"},            // 0x1B                     --- DA ---
      {"???"},            // 0x1C                     --- DA ---
      {"???"},            // 0x1D                     --- DA ---
      {"???"},            // 0x1E                     --- DA ---
      {"???"},            // 0x1F                     --- DA ---
      {"???"},            // 0x20                     --- DA ---
      {"???"},            // 0x21                     --- DA ---
      {"???"},            // 0x22                     --- DA ---
      {"???"},            // 0x23                     --- DA ---
      {"???"},            // 0x24                     --- DA ---
      {"???"},            // 0x25                     --- DA ---
      {"???"},            // 0x26                     --- DA ---
      {"???"},            // 0x27                     --- DA ---
      {"???"},            // 0x28                     --- DA ---
      {"???"},            // 0x29                     --- DA ---
      {"???"},            // 0x2A                     --- DA ---
      {"???"},            // 0x2B                     --- DA ---
      {"???"},            // 0x2C                     --- DA ---
      {"???"},            // 0x2D                     --- DA ---
      {"???"},            // 0x2E                     --- DA ---
      {"???"},            // 0x2F                     --- DA ---
      {"???"},            // 0x30                     --- DA ---
      {"???"},            // 0x31                     --- DA ---
      {"???"},            // 0x32                     --- DA ---
      {"???"},            // 0x33                     --- DA ---
      {"???"},            // 0x34                     --- DA ---
      {"???"},            // 0x35                     --- DA ---
      {"???"},            // 0x36                     --- DA ---
      {"???"},            // 0x37                     --- DA ---
      {"???"},            // 0x38                     --- DA ---
      {"???"},            // 0x39                     --- DA ---
      {"???"},            // 0x3A                     --- DA ---
      {"???"},            // 0x3B                     --- DA ---
      {"???"},            // 0x3C                     --- DA ---
      {"???"},            // 0x3D                     --- DA ---
      {"???"},            // 0x3E                     --- DA ---
      {"???"},            // 0x3F                     --- DA ---
      {"???"},            // 0x40                     --- DA ---
      {"???"},            // 0x41                     --- DA ---
      {"???"},            // 0x42                     --- DA ---
      {"???"},            // 0x43                     --- DA ---
      {"???"},            // 0x44                     --- DA ---
      {"???"},            // 0x45                     --- DA ---
      {"???"},            // 0x46                     --- DA ---
      {"???"},            // 0x47                     --- DA ---
      {"???"},            // 0x48                     --- DA ---
      {"???"},            // 0x49                     --- DA ---
      {"???"},            // 0x4A                     --- DA ---
      {"???"},            // 0x4B                     --- DA ---
      {"???"},            // 0x4C                     --- DA ---
      {"???"},            // 0x4D                     --- DA ---
      {"???"},            // 0x4E                     --- DA ---
      {"???"},            // 0x4F                     --- DA ---
      {"???"},            // 0x50                     --- DA ---
      {"???"},            // 0x51                     --- DA ---
      {"???"},            // 0x52                     --- DA ---
      {"???"},            // 0x53                     --- DA ---
      {"???"},            // 0x54                     --- DA ---
      {"???"},            // 0x55                     --- DA ---
      {"???"},            // 0x56                     --- DA ---
      {"???"},            // 0x57                     --- DA ---
      {"???"},            // 0x58                     --- DA ---
      {"???"},            // 0x59                     --- DA ---
      {"???"},            // 0x5A                     --- DA ---
      {"???"},            // 0x5B                     --- DA ---
      {"???"},            // 0x5C                     --- DA ---
      {"???"},            // 0x5D                     --- DA ---
      {"???"},            // 0x5E                     --- DA ---
      {"???"},            // 0x5F                     --- DA ---
      {"???"},            // 0x60                     --- DA ---
      {"???"},            // 0x61                     --- DA ---
      {"???"},            // 0x62                     --- DA ---
      {"???"},            // 0x63                     --- DA ---
      {"???"},            // 0x64                     --- DA ---
      {"???"},            // 0x65                     --- DA ---
      {"???"},            // 0x66                     --- DA ---
      {"???"},            // 0x67                     --- DA ---
      {"???"},            // 0x68                     --- DA ---
      {"???"},            // 0x69                     --- DA ---
      {"???"},            // 0x6A                     --- DA ---
      {"???"},            // 0x6B                     --- DA ---
      {"???"},            // 0x6C                     --- DA ---
      {"???"},            // 0x6D                     --- DA ---
      {"???"},            // 0x6E                     --- DA ---
      {"???"},            // 0x6F                     --- DA ---
      {"???"},            // 0x70                     --- DA ---
      {"???"},            // 0x71                     --- DA ---
      {"???"},            // 0x72                     --- DA ---
      {"???"},            // 0x73                     --- DA ---
      {"???"},            // 0x74                     --- DA ---
      {"???"},            // 0x75                     --- DA ---
      {"???"},            // 0x76                     --- DA ---
      {"???"},            // 0x77                     --- DA ---
      {"???"},            // 0x78                     --- DA ---
      {"???"},            // 0x79                     --- DA ---
      {"???"},            // 0x7A                     --- DA ---
      {"???"},            // 0x7B                     --- DA ---
      {"???"},            // 0x7C                     --- DA ---
      {"???"},            // 0x7D                     --- DA ---
      {"???"},            // 0x7E                     --- DA ---
      {"???"},            // 0x7F                     --- DA ---
      {"???"},            // 0x80                     --- DA ---
      {"???"},            // 0x81                     --- DA ---
      {"???"},            // 0x82                     --- DA ---
      {"???"},            // 0x83                     --- DA ---
      {"???"},            // 0x84                     --- DA ---
      {"???"},            // 0x85                     --- DA ---
      {"???"},            // 0x86                     --- DA ---
      {"???"},            // 0x87                     --- DA ---
      {"???"},            // 0x88                     --- DA ---
      {"???"},            // 0x89                     --- DA ---
      {"???"},            // 0x8A                     --- DA ---
      {"???"},            // 0x8B                     --- DA ---
      {"???"},            // 0x8C                     --- DA ---
      {"???"},            // 0x8D                     --- DA ---
      {"???"},            // 0x8E                     --- DA ---
      {"???"},            // 0x8F                     --- DA ---
      {"???"},            // 0x90                     --- DA ---
      {"???"},            // 0x91                     --- DA ---
      {"???"},            // 0x92                     --- DA ---
      {"???"},            // 0x93                     --- DA ---
      {"???"},            // 0x94                     --- DA ---
      {"???"},            // 0x95                     --- DA ---
      {"???"},            // 0x96                     --- DA ---
      {"???"},            // 0x97                     --- DA ---
      {"???"},            // 0x98                     --- DA ---
      {"???"},            // 0x99                     --- DA ---
      {"???"},            // 0x9A                     --- DA ---
      {"???"},            // 0x9B                     --- DA ---
      {"???"},            // 0x9C                     --- DA ---
      {"???"},            // 0x9D                     --- DA ---
      {"???"},            // 0x9E                     --- DA ---
      {"???"},            // 0x9F                     --- DA ---
      {"???"},            // 0xA0                     --- DA ---
      {"???"},            // 0xA1                     --- DA ---
      {"???"},            // 0xA2                     --- DA ---
      {"???"},            // 0xA3                     --- DA ---
      {"???"},            // 0xA4                     --- DA ---
      {"???"},            // 0xA5                     --- DA ---
      {"???"},            // 0xA6                     --- DA ---
      {"???"},            // 0xA7                     --- DA ---
      {"???"},            // 0xA8                     --- DA ---
      {"???"},            // 0xA9                     --- DA ---
      {"???"},            // 0xAA                     --- DA ---
      {"???"},            // 0xAB                     --- DA ---
      {"???"},            // 0xAC                     --- DA ---
      {"???"},            // 0xAD                     --- DA ---
      {"???"},            // 0xAE                     --- DA ---
      {"???"},            // 0xAF                     --- DA ---
      {"???"},            // 0xB0                     --- DA ---
      {"???"},            // 0xB1                     --- DA ---
      {"???"},            // 0xB2                     --- DA ---
      {"???"},            // 0xB3                     --- DA ---
      {"???"},            // 0xB4                     --- DA ---
      {"???"},            // 0xB5                     --- DA ---
      {"???"},            // 0xB6                     --- DA ---
      {"???"},            // 0xB7                     --- DA ---
      {"???"},            // 0xB8                     --- DA ---
      {"???"},            // 0xB9                     --- DA ---
      {"???"},            // 0xBA                     --- DA ---
      {"???"},            // 0xBB                     --- DA ---
      {"???"},            // 0xBC                     --- DA ---
      {"???"},            // 0xBD                     --- DA ---
      {"???"},            // 0xBE                     --- DA ---
      {"???"},            // 0xBF                     --- DA ---
      {"???"},            // 0xC0                     --- DA ---
      {"???"},            // 0xC1                     --- DA ---
      {"???"},            // 0xC2                     --- DA ---
      {"???"},            // 0xC3                     --- DA ---
      {"???"},            // 0xC4                     --- DA ---
      {"???"},            // 0xC5                     --- DA ---
      {"???"},            // 0xC6                     --- DA ---
      {"???"},            // 0xC7                     --- DA ---
      {"???"},            // 0xC8                     --- DA ---
      {"???"},            // 0xC9                     --- DA ---
      {"???"},            // 0xCA                     --- DA ---
      {"???"},            // 0xCB                     --- DA ---
      {"???"},            // 0xCC                     --- DA ---
      {"???"},            // 0xCD                     --- DA ---
      {"???"},            // 0xCE                     --- DA ---
      {"???"},            // 0xCF                     --- DA ---
      {"???"},            // 0xD0                     --- DA ---
      {"???"},            // 0xD1                     --- DA ---
      {"???"},            // 0xD2                     --- DA ---
      {"???"},            // 0xD3                     --- DA ---
      {"???"},            // 0xD4                     --- DA ---
      {"???"},            // 0xD5                     --- DA ---
      {"???"},            // 0xD6                     --- DA ---
      {"???"},            // 0xD7                     --- DA ---
      {"???"},            // 0xD8                     --- DA ---
      {"???"},            // 0xD9                     --- DA ---
      {"???"},            // 0xDA                     --- DA ---
      {"???"},            // 0xDA                     --- DA ---
      {"???"},            // 0xDC                     --- DA ---
      {"???"},            // 0xDD                     --- DA ---
      {"???"},            // 0xDE                     --- DA ---
      {"???"},            // 0xDF                     --- DA ---
      {"???"},            // 0xE0                     --- DA ---
      {"???"},            // 0xE1                     --- DA ---
      {"???"},            // 0xE2                     --- DA ---
      {"???"},            // 0xE3                     --- DA ---
      {"???"},            // 0xE4                     --- DA ---
      {"???"},            // 0xE5                     --- DA ---
      {"???"},            // 0xE6                     --- DA ---
      {"???"},            // 0xE7                     --- DA ---
      {"???"},            // 0xE8                     --- DA ---
      {"FUCOMPP"},        // 0xE9                     --- DA ---
      {"???"},            // 0xEA                     --- DA ---
      {"???"},            // 0xEB                     --- DA ---
      {"???"},            // 0xEC                     --- DA ---
      {"???"},            // 0xED                     --- DA ---
      {"???"},            // 0xEE                     --- DA ---
      {"???"},            // 0xEF                     --- DA ---
      {"???"},            // 0xF0                     --- DA ---
      {"???"},            // 0xF1                     --- DA ---
      {"???"},            // 0xF2                     --- DA ---
      {"???"},            // 0xF3                     --- DA ---
      {"???"},            // 0xF4                     --- DA ---
      {"???"},            // 0xF5                     --- DA ---
      {"???"},            // 0xF6                     --- DA ---
      {"???"},            // 0xF7                     --- DA ---
      {"???"},            // 0xF8                     --- DA ---
      {"???"},            // 0xF9                     --- DA ---
      {"???"},            // 0xFA                     --- DA ---
      {"???"},            // 0xFB                     --- DA ---
      {"???"},            // 0xFC                     --- DA ---
      {"???"},            // 0xFD                     --- DA ---
      {"???"},            // 0xFE                     --- DA ---
      {"???"}             // 0xFF                     --- DA ---
  }; 
const static char intel_mnemonic_DA_ [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FIADD"},          // 0x0
      {"FIMUL"},          // 0x1
      {"FICOM"},          // 0x2
      {"FICOMP"},         // 0x3
      {"FISUB"},          // 0x4
      {"FISUBR"},         // 0x5
      {"FIDIV"},          // 0x6
      {"FIDIVR"}          // 0x7
};
const static char intel_mnemonic_DB [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"???"},            // 0x0                      --- DB --- 
      {"???"},            // 0x1                      --- DB ---
      {"???"},            // 0x2                      --- DB ---
      {"???"},            // 0x3                      --- DB ---
      {"???"},            // 0x4                      --- DB ---
      {"???"},            // 0x5                      --- DB ---
      {"???"},            // 0x6                      --- DB ---
      {"???"},            // 0x7                      --- DB ---
      {"???"},            // 0x8                      --- DB ---
      {"???"},            // 0x9                      --- DB ---
      {"???"},            // 0xA                      --- DB ---
      {"???"},            // 0xB                      --- DB ---
      {"???"},            // 0xC                      --- DB ---
      {"???"},            // 0xD                      --- DB ---
      {"???"},            // 0xE                      --- DB ---
      {"???"},            // 0xF                      --- DB ---
      {"???"},            // 0x10                     --- DB ---
      {"???"},            // 0x11                     --- DB ---
      {"???"},            // 0x12                     --- DB ---
      {"???"},            // 0x13                     --- DB ---
      {"???"},            // 0x14                     --- DB ---
      {"???"},            // 0x15                     --- DB ---
      {"???"},            // 0x16                     --- DB ---
      {"???"},            // 0x17                     --- DB ---
      {"???"},            // 0x18                     --- DB ---
      {"???"},            // 0x19                     --- DB ---
      {"???"},            // 0x1A                     --- DB ---
      {"???"},            // 0x1B                     --- DB ---
      {"???"},            // 0x1C                     --- DB ---
      {"???"},            // 0x1D                     --- DB ---
      {"???"},            // 0x1E                     --- DB ---
      {"???"},            // 0x1F                     --- DB ---
      {"???"},            // 0x20                     --- DB ---
      {"???"},            // 0x21                     --- DB ---
      {"???"},            // 0x22                     --- DB ---
      {"???"},            // 0x23                     --- DB ---
      {"???"},            // 0x24                     --- DB ---
      {"???"},            // 0x25                     --- DB ---
      {"???"},            // 0x26                     --- DB ---
      {"???"},            // 0x27                     --- DB ---
      {"???"},            // 0x28                     --- DB ---
      {"???"},            // 0x29                     --- DB ---
      {"???"},            // 0x2A                     --- DB ---
      {"???"},            // 0x2B                     --- DB ---
      {"???"},            // 0x2C                     --- DB ---
      {"???"},            // 0x2D                     --- DB ---
      {"???"},            // 0x2E                     --- DB ---
      {"???"},            // 0x2F                     --- DB ---
      {"???"},            // 0x30                     --- DB ---
      {"???"},            // 0x31                     --- DB ---
      {"???"},            // 0x32                     --- DB ---
      {"???"},            // 0x33                     --- DB ---
      {"???"},            // 0x34                     --- DB ---
      {"???"},            // 0x35                     --- DB ---
      {"???"},            // 0x36                     --- DB ---
      {"???"},            // 0x37                     --- DB ---
      {"???"},            // 0x38                     --- DB ---
      {"???"},            // 0x39                     --- DB ---
      {"???"},            // 0x3A                     --- DB ---
      {"???"},            // 0x3B                     --- DB ---
      {"???"},            // 0x3C                     --- DB ---
      {"???"},            // 0x3D                     --- DB ---
      {"???"},            // 0x3E                     --- DB ---
      {"???"},            // 0x3F                     --- DB ---
      {"???"},            // 0x40                     --- DB ---
      {"???"},            // 0x41                     --- DB ---
      {"???"},            // 0x42                     --- DB ---
      {"???"},            // 0x43                     --- DB ---
      {"???"},            // 0x44                     --- DB ---
      {"???"},            // 0x45                     --- DB ---
      {"???"},            // 0x46                     --- DB ---
      {"???"},            // 0x47                     --- DB ---
      {"???"},            // 0x48                     --- DB ---
      {"???"},            // 0x49                     --- DB ---
      {"???"},            // 0x4A                     --- DB ---
      {"???"},            // 0x4B                     --- DB ---
      {"???"},            // 0x4C                     --- DB ---
      {"???"},            // 0x4D                     --- DB ---
      {"???"},            // 0x4E                     --- DB ---
      {"???"},            // 0x4F                     --- DB ---
      {"???"},            // 0x50                     --- DB ---
      {"???"},            // 0x51                     --- DB ---
      {"???"},            // 0x52                     --- DB ---
      {"???"},            // 0x53                     --- DB ---
      {"???"},            // 0x54                     --- DB ---
      {"???"},            // 0x55                     --- DB ---
      {"???"},            // 0x56                     --- DB ---
      {"???"},            // 0x57                     --- DB ---
      {"???"},            // 0x58                     --- DB ---
      {"???"},            // 0x59                     --- DB ---
      {"???"},            // 0x5A                     --- DB ---
      {"???"},            // 0x5B                     --- DB ---
      {"???"},            // 0x5C                     --- DB ---
      {"???"},            // 0x5D                     --- DB ---
      {"???"},            // 0x5E                     --- DB ---
      {"???"},            // 0x5F                     --- DB ---
      {"???"},            // 0x60                     --- DB ---
      {"???"},            // 0x61                     --- DB ---
      {"???"},            // 0x62                     --- DB ---
      {"???"},            // 0x63                     --- DB ---
      {"???"},            // 0x64                     --- DB ---
      {"???"},            // 0x65                     --- DB ---
      {"???"},            // 0x66                     --- DB ---
      {"???"},            // 0x67                     --- DB ---
      {"???"},            // 0x68                     --- DB ---
      {"???"},            // 0x69                     --- DB ---
      {"???"},            // 0x6A                     --- DB ---
      {"???"},            // 0x6B                     --- DB ---
      {"???"},            // 0x6C                     --- DB ---
      {"???"},            // 0x6D                     --- DB ---
      {"???"},            // 0x6E                     --- DB ---
      {"???"},            // 0x6F                     --- DB ---
      {"???"},            // 0x70                     --- DB ---
      {"???"},            // 0x71                     --- DB ---
      {"???"},            // 0x72                     --- DB ---
      {"???"},            // 0x73                     --- DB ---
      {"???"},            // 0x74                     --- DB ---
      {"???"},            // 0x75                     --- DB ---
      {"???"},            // 0x76                     --- DB ---
      {"???"},            // 0x77                     --- DB ---
      {"???"},            // 0x78                     --- DB ---
      {"???"},            // 0x79                     --- DB ---
      {"???"},            // 0x7A                     --- DB ---
      {"???"},            // 0x7B                     --- DB ---
      {"???"},            // 0x7C                     --- DB ---
      {"???"},            // 0x7D                     --- DB ---
      {"???"},            // 0x7E                     --- DB ---
      {"???"},            // 0x7F                     --- DB ---
      {"???"},            // 0x80                     --- DB ---
      {"???"},            // 0x81                     --- DB ---
      {"???"},            // 0x82                     --- DB ---
      {"???"},            // 0x83                     --- DB ---
      {"???"},            // 0x84                     --- DB ---
      {"???"},            // 0x85                     --- DB ---
      {"???"},            // 0x86                     --- DB ---
      {"???"},            // 0x87                     --- DB ---
      {"???"},            // 0x88                     --- DB ---
      {"???"},            // 0x89                     --- DB ---
      {"???"},            // 0x8A                     --- DB ---
      {"???"},            // 0x8B                     --- DB ---
      {"???"},            // 0x8C                     --- DB ---
      {"???"},            // 0x8D                     --- DB ---
      {"???"},            // 0x8E                     --- DB ---
      {"???"},            // 0x8F                     --- DB ---
      {"???"},            // 0x90                     --- DB ---
      {"???"},            // 0x91                     --- DB ---
      {"???"},            // 0x92                     --- DB ---
      {"???"},            // 0x93                     --- DB ---
      {"???"},            // 0x94                     --- DB ---
      {"???"},            // 0x95                     --- DB ---
      {"???"},            // 0x96                     --- DB ---
      {"???"},            // 0x97                     --- DB ---
      {"???"},            // 0x98                     --- DB ---
      {"???"},            // 0x99                     --- DB ---
      {"???"},            // 0x9A                     --- DB ---
      {"???"},            // 0x9B                     --- DB ---
      {"???"},            // 0x9C                     --- DB ---
      {"???"},            // 0x9D                     --- DB ---
      {"???"},            // 0x9E                     --- DB ---
      {"???"},            // 0x9F                     --- DB ---
      {"???"},            // 0xA0                     --- DB ---
      {"???"},            // 0xA1                     --- DB ---
      {"???"},            // 0xA2                     --- DB ---
      {"???"},            // 0xA3                     --- DB ---
      {"???"},            // 0xA4                     --- DB ---
      {"???"},            // 0xA5                     --- DB ---
      {"???"},            // 0xA6                     --- DB ---
      {"???"},            // 0xA7                     --- DB ---
      {"???"},            // 0xA8                     --- DB ---
      {"???"},            // 0xA9                     --- DB ---
      {"???"},            // 0xAA                     --- DB ---
      {"???"},            // 0xAB                     --- DB ---
      {"???"},            // 0xAC                     --- DB ---
      {"???"},            // 0xAD                     --- DB ---
      {"???"},            // 0xAE                     --- DB ---
      {"???"},            // 0xAF                     --- DB ---
      {"???"},            // 0xB0                     --- DB ---
      {"???"},            // 0xB1                     --- DB ---
      {"???"},            // 0xB2                     --- DB ---
      {"???"},            // 0xB3                     --- DB ---
      {"???"},            // 0xB4                     --- DB ---
      {"???"},            // 0xB5                     --- DB ---
      {"???"},            // 0xB6                     --- DB ---
      {"???"},            // 0xB7                     --- DB ---
      {"???"},            // 0xB8                     --- DB ---
      {"???"},            // 0xB9                     --- DB ---
      {"???"},            // 0xBA                     --- DB ---
      {"???"},            // 0xBB                     --- DB ---
      {"???"},            // 0xBC                     --- DB ---
      {"???"},            // 0xBD                     --- DB ---
      {"???"},            // 0xBE                     --- DB ---
      {"???"},            // 0xBF                     --- DB ---
      {"???"},            // 0xC0                     --- DB ---
      {"???"},            // 0xC1                     --- DB ---
      {"???"},            // 0xC2                     --- DB ---
      {"???"},            // 0xC3                     --- DB ---
      {"???"},            // 0xC4                     --- DB ---
      {"???"},            // 0xC5                     --- DB ---
      {"???"},            // 0xC6                     --- DB ---
      {"???"},            // 0xC7                     --- DB ---
      {"???"},            // 0xC8                     --- DB ---
      {"???"},            // 0xC9                     --- DB ---
      {"???"},            // 0xCA                     --- DB ---
      {"???"},            // 0xCB                     --- DB ---
      {"???"},            // 0xCC                     --- DB ---
      {"???"},            // 0xCD                     --- DB ---
      {"???"},            // 0xCE                     --- DB ---
      {"???"},            // 0xCF                     --- DB ---
      {"???"},            // 0xD0                     --- DB ---
      {"???"},            // 0xD1                     --- DB ---
      {"???"},            // 0xD2                     --- DB ---
      {"???"},            // 0xD3                     --- DB ---
      {"???"},            // 0xD4                     --- DB ---
      {"???"},            // 0xD5                     --- DB ---
      {"???"},            // 0xD6                     --- DB ---
      {"???"},            // 0xD7                     --- DB ---
      {"???"},            // 0xD8                     --- DB ---
      {"???"},            // 0xD9                     --- DB ---
      {"???"},            // 0xDA                     --- DB ---
      {"???"},            // 0xDB                     --- DB ---
      {"???"},            // 0xDC                     --- DB ---
      {"???"},            // 0xDD                     --- DB ---
      {"???"},            // 0xDE                     --- DB ---
      {"???"},            // 0xDF                     --- DB ---
      {"???"},            // 0xE0                     --- DB ---
      {"???"},            // 0xE1                     --- DB ---
      {"???"},            // 0xE2                     --- DB ---
      {"???"},            // 0xE3                     --- DB ---
      {"FSETPM"},         // 0xE4                     --- DB ---
      {"???"},            // 0xE5                     --- DB ---
      {"???"},            // 0xE6                     --- DB ---
      {"???"},            // 0xE7                     --- DB ---
      {"FUCOMI"},         // 0xE8                     --- DB ---
      {"FUCOMI"},         // 0xE9                     --- DB ---
      {"FUCOMI"},         // 0xEA                     --- DB ---
      {"FUCOMI"},         // 0xEB                     --- DB ---
      {"FUCOMI"},         // 0xEC                     --- DB ---
      {"FUCOMI"},         // 0xED                     --- DB ---
      {"FUCOMI"},         // 0xEE                     --- DB ---
      {"FUCOMI"},         // 0xEF                     --- DB ---
      {"???"},            // 0xF0                     --- DB ---
      {"???"},            // 0xF1                     --- DB ---
      {"???"},            // 0xF2                     --- DB ---
      {"???"},            // 0xF3                     --- DB ---
      {"???"},            // 0xF4                     --- DB ---
      {"???"},            // 0xF5                     --- DB ---
      {"???"},            // 0xF6                     --- DB ---
      {"???"},            // 0xF7                     --- DB ---
      {"???"},            // 0xF8                     --- DB ---
      {"???"},            // 0xF9                     --- DB ---
      {"???"},            // 0xFA                     --- DB ---
      {"???"},            // 0xFB                     --- DB ---
      {"???"},            // 0xFC                     --- DB ---
      {"???"},            // 0xFD                     --- DB ---
      {"???"},            // 0xFE                     --- DB ---
      {"???"}             // 0xFF                     --- DB ---
  }; 
const static char intel_mnemonic_DB_ [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FILD"},          // 0x0
      {"???"},           // 0x1
      {"FIST"},          // 0x2
      {"FISTP"},         // 0x3
      {"???"},           // 0x4
      {"FLD"},           // 0x5
      {"???"},           // 0x6
      {"FSTP"}           // 0x7
};
const static char intel_mnemonic_DB_E0 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FENI"},           // wait
      {"FNENI"}           // no wait
};
const static char intel_mnemonic_DB_E1 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FDISI"},          // wait
      {"FNDISI"}          // no wait
};
const static char intel_mnemonic_DB_E2 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FCLEX"},          // wait
      {"FNCLEX"}          // no wait
};
const static char intel_mnemonic_DB_E3 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FINIT"},          // wait
      {"FNINIT"}          // no wait
};
const static char intel_mnemonic_DC [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"???"},            // 0x0                      --- DC --- 
      {"???"},            // 0x1                      --- DC ---
      {"???"},            // 0x2                      --- DC ---
      {"???"},            // 0x3                      --- DC ---
      {"???"},            // 0x4                      --- DC ---
      {"???"},            // 0x5                      --- DC ---
      {"???"},            // 0x6                      --- DC ---
      {"???"},            // 0x7                      --- DC ---
      {"???"},            // 0x8                      --- DC ---
      {"???"},            // 0x9                      --- DC ---
      {"???"},            // 0xA                      --- DC ---
      {"???"},            // 0xB                      --- DC ---
      {"???"},            // 0xC                      --- DC ---
      {"???"},            // 0xD                      --- DC ---
      {"???"},            // 0xE                      --- DC ---
      {"???"},            // 0xF                      --- DC ---
      {"???"},            // 0x10                     --- DC ---
      {"???"},            // 0x11                     --- DC ---
      {"???"},            // 0x12                     --- DC ---
      {"???"},            // 0x13                     --- DC ---
      {"???"},            // 0x14                     --- DC ---
      {"???"},            // 0x15                     --- DC ---
      {"???"},            // 0x16                     --- DC ---
      {"???"},            // 0x17                     --- DC ---
      {"???"},            // 0x18                     --- DC ---
      {"???"},            // 0x19                     --- DC ---
      {"???"},            // 0x1A                     --- DC ---
      {"???"},            // 0x1B                     --- DC ---
      {"???"},            // 0x1C                     --- DC ---
      {"???"},            // 0x1D                     --- DC ---
      {"???"},            // 0x1E                     --- DC ---
      {"???"},            // 0x1F                     --- DC ---
      {"???"},            // 0x20                     --- DC ---
      {"???"},            // 0x21                     --- DC ---
      {"???"},            // 0x22                     --- DC ---
      {"???"},            // 0x23                     --- DC ---
      {"???"},            // 0x24                     --- DC ---
      {"???"},            // 0x25                     --- DC ---
      {"???"},            // 0x26                     --- DC ---
      {"???"},            // 0x27                     --- DC ---
      {"???"},            // 0x28                     --- DC ---
      {"???"},            // 0x29                     --- DC ---
      {"???"},            // 0x2A                     --- DC ---
      {"???"},            // 0x2B                     --- DC ---
      {"???"},            // 0x2C                     --- DC ---
      {"???"},            // 0x2D                     --- DC ---
      {"???"},            // 0x2E                     --- DC ---
      {"???"},            // 0x2F                     --- DC ---
      {"???"},            // 0x30                     --- DC ---
      {"???"},            // 0x31                     --- DC ---
      {"???"},            // 0x32                     --- DC ---
      {"???"},            // 0x33                     --- DC ---
      {"???"},            // 0x34                     --- DC ---
      {"???"},            // 0x35                     --- DC ---
      {"???"},            // 0x36                     --- DC ---
      {"???"},            // 0x37                     --- DC ---
      {"???"},            // 0x38                     --- DC ---
      {"???"},            // 0x39                     --- DC ---
      {"???"},            // 0x3A                     --- DC ---
      {"???"},            // 0x3B                     --- DC ---
      {"???"},            // 0x3C                     --- DC ---
      {"???"},            // 0x3D                     --- DC ---
      {"???"},            // 0x3E                     --- DC ---
      {"???"},            // 0x3F                     --- DC ---
      {"???"},            // 0x40                     --- DC ---
      {"???"},            // 0x41                     --- DC ---
      {"???"},            // 0x42                     --- DC ---
      {"???"},            // 0x43                     --- DC ---
      {"???"},            // 0x44                     --- DC ---
      {"???"},            // 0x45                     --- DC ---
      {"???"},            // 0x46                     --- DC ---
      {"???"},            // 0x47                     --- DC ---
      {"???"},            // 0x48                     --- DC ---
      {"???"},            // 0x49                     --- DC ---
      {"???"},            // 0x4A                     --- DC ---
      {"???"},            // 0x4B                     --- DC ---
      {"???"},            // 0x4C                     --- DC ---
      {"???"},            // 0x4D                     --- DC ---
      {"???"},            // 0x4E                     --- DC ---
      {"???"},            // 0x4F                     --- DC ---
      {"???"},            // 0x50                     --- DC ---
      {"???"},            // 0x51                     --- DC ---
      {"???"},            // 0x52                     --- DC ---
      {"???"},            // 0x53                     --- DC ---
      {"???"},            // 0x54                     --- DC ---
      {"???"},            // 0x55                     --- DC ---
      {"???"},            // 0x56                     --- DC ---
      {"???"},            // 0x57                     --- DC ---
      {"???"},            // 0x58                     --- DC ---
      {"???"},            // 0x59                     --- DC ---
      {"???"},            // 0x5A                     --- DC ---
      {"???"},            // 0x5B                     --- DC ---
      {"???"},            // 0x5C                     --- DC ---
      {"???"},            // 0x5D                     --- DC ---
      {"???"},            // 0x5E                     --- DC ---
      {"???"},            // 0x5F                     --- DC ---
      {"???"},            // 0x60                     --- DC ---
      {"???"},            // 0x61                     --- DC ---
      {"???"},            // 0x62                     --- DC ---
      {"???"},            // 0x63                     --- DC ---
      {"???"},            // 0x64                     --- DC ---
      {"???"},            // 0x65                     --- DC ---
      {"???"},            // 0x66                     --- DC ---
      {"???"},            // 0x67                     --- DC ---
      {"???"},            // 0x68                     --- DC ---
      {"???"},            // 0x69                     --- DC ---
      {"???"},            // 0x6A                     --- DC ---
      {"???"},            // 0x6B                     --- DC ---
      {"???"},            // 0x6C                     --- DC ---
      {"???"},            // 0x6D                     --- DC ---
      {"???"},            // 0x6E                     --- DC ---
      {"???"},            // 0x6F                     --- DC ---
      {"???"},            // 0x70                     --- DC ---
      {"???"},            // 0x71                     --- DC ---
      {"???"},            // 0x72                     --- DC ---
      {"???"},            // 0x73                     --- DC ---
      {"???"},            // 0x74                     --- DC ---
      {"???"},            // 0x75                     --- DC ---
      {"???"},            // 0x76                     --- DC ---
      {"???"},            // 0x77                     --- DC ---
      {"???"},            // 0x78                     --- DC ---
      {"???"},            // 0x79                     --- DC ---
      {"???"},            // 0x7A                     --- DC ---
      {"???"},            // 0x7B                     --- DC ---
      {"???"},            // 0x7C                     --- DC ---
      {"???"},            // 0x7D                     --- DC ---
      {"???"},            // 0x7E                     --- DC ---
      {"???"},            // 0x7F                     --- DC ---
      {"???"},            // 0x80                     --- DC ---
      {"???"},            // 0x81                     --- DC ---
      {"???"},            // 0x82                     --- DC ---
      {"???"},            // 0x83                     --- DC ---
      {"???"},            // 0x84                     --- DC ---
      {"???"},            // 0x85                     --- DC ---
      {"???"},            // 0x86                     --- DC ---
      {"???"},            // 0x87                     --- DC ---
      {"???"},            // 0x88                     --- DC ---
      {"???"},            // 0x89                     --- DC ---
      {"???"},            // 0x8A                     --- DC ---
      {"???"},            // 0x8B                     --- DC ---
      {"???"},            // 0x8C                     --- DC ---
      {"???"},            // 0x8D                     --- DC ---
      {"???"},            // 0x8E                     --- DC ---
      {"???"},            // 0x8F                     --- DC ---
      {"???"},            // 0x90                     --- DC ---
      {"???"},            // 0x91                     --- DC ---
      {"???"},            // 0x92                     --- DC ---
      {"???"},            // 0x93                     --- DC ---
      {"???"},            // 0x94                     --- DC ---
      {"???"},            // 0x95                     --- DC ---
      {"???"},            // 0x96                     --- DC ---
      {"???"},            // 0x97                     --- DC ---
      {"???"},            // 0x98                     --- DC ---
      {"???"},            // 0x99                     --- DC ---
      {"???"},            // 0x9A                     --- DC ---
      {"???"},            // 0x9B                     --- DC ---
      {"???"},            // 0x9C                     --- DC ---
      {"???"},            // 0x9D                     --- DC ---
      {"???"},            // 0x9E                     --- DC ---
      {"???"},            // 0x9F                     --- DC ---
      {"???"},            // 0xA0                     --- DC ---
      {"???"},            // 0xA1                     --- DC ---
      {"???"},            // 0xA2                     --- DC ---
      {"???"},            // 0xA3                     --- DC ---
      {"???"},            // 0xA4                     --- DC ---
      {"???"},            // 0xA5                     --- DC ---
      {"???"},            // 0xA6                     --- DC ---
      {"???"},            // 0xA7                     --- DC ---
      {"???"},            // 0xA8                     --- DC ---
      {"???"},            // 0xA9                     --- DC ---
      {"???"},            // 0xAA                     --- DC ---
      {"???"},            // 0xAB                     --- DC ---
      {"???"},            // 0xAC                     --- DC ---
      {"???"},            // 0xAD                     --- DC ---
      {"???"},            // 0xAE                     --- DC ---
      {"???"},            // 0xAF                     --- DC ---
      {"???"},            // 0xB0                     --- DC ---
      {"???"},            // 0xB1                     --- DC ---
      {"???"},            // 0xB2                     --- DC ---
      {"???"},            // 0xB3                     --- DC ---
      {"???"},            // 0xB4                     --- DC ---
      {"???"},            // 0xB5                     --- DC ---
      {"???"},            // 0xB6                     --- DC ---
      {"???"},            // 0xB7                     --- DC ---
      {"???"},            // 0xB8                     --- DC ---
      {"???"},            // 0xB9                     --- DC ---
      {"???"},            // 0xBA                     --- DC ---
      {"???"},            // 0xBB                     --- DC ---
      {"???"},            // 0xBC                     --- DC ---
      {"???"},            // 0xBD                     --- DC ---
      {"???"},            // 0xBE                     --- DC ---
      {"???"},            // 0xBF                     --- DC ---
      {"FADD"},           // 0xC0                     --- DC ---
      {"FADD"},           // 0xC1                     --- DC ---
      {"FADD"},           // 0xC2                     --- DC ---
      {"FADD"},           // 0xC3                     --- DC ---
      {"FADD"},           // 0xC4                     --- DC ---
      {"FADD"},           // 0xC5                     --- DC ---
      {"FADD"},           // 0xC6                     --- DC ---
      {"FADD"},           // 0xC7                     --- DC ---
      {"FMUL"},           // 0xC8                     --- DC ---
      {"FMUL"},           // 0xC9                     --- DC ---
      {"FMUL"},           // 0xCA                     --- DC ---
      {"FMUL"},           // 0xCB                     --- DC ---
      {"FMUL"},           // 0xCC                     --- DC ---
      {"FMUL"},           // 0xCD                     --- DC ---
      {"FMUL"},           // 0xCE                     --- DC ---
      {"FMUL"},           // 0xCF                     --- DC ---
      {"???"},            // 0xD0                     --- DC ---
      {"???"},            // 0xD1                     --- DC ---
      {"???"},            // 0xD2                     --- DC ---
      {"???"},            // 0xD3                     --- DC ---
      {"???"},            // 0xD4                     --- DC ---
      {"???"},            // 0xD5                     --- DC ---
      {"???"},            // 0xD6                     --- DC ---
      {"???"},            // 0xD7                     --- DC ---
      {"???"},            // 0xD8                     --- DC ---
      {"???"},            // 0xD9                     --- DC ---
      {"???"},            // 0xDA                     --- DC ---
      {"???"},            // 0xDB                     --- DC ---
      {"???"},            // 0xDC                     --- DC ---
      {"???"},            // 0xDD                     --- DC ---
      {"???"},            // 0xDE                     --- DC ---
      {"???"},            // 0xDF                     --- DC ---
      {"FSUBR"},          // 0xE0                     --- DC ---
      {"FSUBR"},          // 0xE1                     --- DC ---
      {"FSUBR"},          // 0xE2                     --- DC ---
      {"FSUBR"},          // 0xE3                     --- DC ---
      {"FSUBR"},          // 0xE4                     --- DC ---
      {"FSUBR"},          // 0xE5                     --- DC ---
      {"FSUBR"},          // 0xE6                     --- DC ---
      {"FSUBR"},          // 0xE7                     --- DC ---
      {"FSUB"},           // 0xE8                     --- DC ---
      {"FSUB"},           // 0xE9                     --- DC ---
      {"FSUB"},           // 0xEA                     --- DC ---
      {"FSUB"},           // 0xEB                     --- DC ---
      {"FSUB"},           // 0xEC                     --- DC ---
      {"FSUB"},           // 0xED                     --- DC ---
      {"FSUB"},           // 0xEE                     --- DC ---
      {"FSUB"},           // 0xEF                     --- DC ---
      {"FDIVR"},          // 0xF0                     --- DC ---
      {"FDIVR"},          // 0xF1                     --- DC ---
      {"FDIVR"},          // 0xF2                     --- DC ---
      {"FDIVR"},          // 0xF3                     --- DC ---
      {"FDIVR"},          // 0xF4                     --- DC ---
      {"FDIVR"},          // 0xF5                     --- DC ---
      {"FDIVR"},          // 0xF6                     --- DC ---
      {"FDIVR"},          // 0xF7                     --- DC ---
      {"FDIV"},           // 0xF8                     --- DC ---
      {"FDIV"},           // 0xF9                     --- DC ---
      {"FDIV"},           // 0xFA                     --- DC ---
      {"FDIV"},           // 0xFB                     --- DC ---
      {"FDIV"},           // 0xFC                     --- DC ---
      {"FDIV"},           // 0xFD                     --- DC ---
      {"FDIV"},           // 0xFE                     --- DC ---
      {"FDIV"}            // 0xFF                     --- DC ---
  }; 
const static char intel_mnemonic_DC_ [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FADD"},           // 0x0
      {"FMUL"},           // 0x1
      {"FCOM"},           // 0x2
      {"FCOMP"},          // 0x3
      {"FSUB"},           // 0x4
      {"FSUBR"},          // 0x5
      {"FDIV"},           // 0x6
      {"FDIVR"}           // 0x7
};
const static char intel_mnemonic_DD [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"???"},            // 0x0                      --- DD --- 
      {"???"},            // 0x1                      --- DD ---
      {"???"},            // 0x2                      --- DD ---
      {"???"},            // 0x3                      --- DD ---
      {"???"},            // 0x4                      --- DD ---
      {"???"},            // 0x5                      --- DD ---
      {"???"},            // 0x6                      --- DD ---
      {"???"},            // 0x7                      --- DD ---
      {"???"},            // 0x8                      --- DD ---
      {"???"},            // 0x9                      --- DD ---
      {"???"},            // 0xA                      --- DD ---
      {"???"},            // 0xB                      --- DD ---
      {"???"},            // 0xC                      --- DD ---
      {"???"},            // 0xD                      --- DD ---
      {"???"},            // 0xE                      --- DD ---
      {"???"},            // 0xF                      --- DD ---
      {"???"},            // 0x10                     --- DD ---
      {"???"},            // 0x11                     --- DD ---
      {"???"},            // 0x12                     --- DD ---
      {"???"},            // 0x13                     --- DD ---
      {"???"},            // 0x14                     --- DD ---
      {"???"},            // 0x15                     --- DD ---
      {"???"},            // 0x16                     --- DD ---
      {"???"},            // 0x17                     --- DD ---
      {"???"},            // 0x18                     --- DD ---
      {"???"},            // 0x19                     --- DD ---
      {"???"},            // 0x1A                     --- DD ---
      {"???"},            // 0x1B                     --- DD ---
      {"???"},            // 0x1C                     --- DD ---
      {"???"},            // 0x1D                     --- DD ---
      {"???"},            // 0x1E                     --- DD ---
      {"???"},            // 0x1F                     --- DD ---
      {"???"},            // 0x20                     --- DD ---
      {"???"},            // 0x21                     --- DD ---
      {"???"},            // 0x22                     --- DD ---
      {"???"},            // 0x23                     --- DD ---
      {"???"},            // 0x24                     --- DD ---
      {"???"},            // 0x25                     --- DD ---
      {"???"},            // 0x26                     --- DD ---
      {"???"},            // 0x27                     --- DD ---
      {"???"},            // 0x28                     --- DD ---
      {"???"},            // 0x29                     --- DD ---
      {"???"},            // 0x2A                     --- DD ---
      {"???"},            // 0x2B                     --- DD ---
      {"???"},            // 0x2C                     --- DD ---
      {"???"},            // 0x2D                     --- DD ---
      {"???"},            // 0x2E                     --- DD ---
      {"???"},            // 0x2F                     --- DD ---
      {"???"},            // 0x30                     --- DD ---
      {"???"},            // 0x31                     --- DD ---
      {"???"},            // 0x32                     --- DD ---
      {"???"},            // 0x33                     --- DD ---
      {"???"},            // 0x34                     --- DD ---
      {"???"},            // 0x35                     --- DD ---
      {"???"},            // 0x36                     --- DD ---
      {"???"},            // 0x37                     --- DD ---
      {"???"},            // 0x38                     --- DD ---
      {"???"},            // 0x39                     --- DD ---
      {"???"},            // 0x3A                     --- DD ---
      {"???"},            // 0x3B                     --- DD ---
      {"???"},            // 0x3C                     --- DD ---
      {"???"},            // 0x3D                     --- DD ---
      {"???"},            // 0x3E                     --- DD ---
      {"???"},            // 0x3F                     --- DD ---
      {"???"},            // 0x40                     --- DD ---
      {"???"},            // 0x41                     --- DD ---
      {"???"},            // 0x42                     --- DD ---
      {"???"},            // 0x43                     --- DD ---
      {"???"},            // 0x44                     --- DD ---
      {"???"},            // 0x45                     --- DD ---
      {"???"},            // 0x46                     --- DD ---
      {"???"},            // 0x47                     --- DD ---
      {"???"},            // 0x48                     --- DD ---
      {"???"},            // 0x49                     --- DD ---
      {"???"},            // 0x4A                     --- DD ---
      {"???"},            // 0x4B                     --- DD ---
      {"???"},            // 0x4C                     --- DD ---
      {"???"},            // 0x4D                     --- DD ---
      {"???"},            // 0x4E                     --- DD ---
      {"???"},            // 0x4F                     --- DD ---
      {"???"},            // 0x50                     --- DD ---
      {"???"},            // 0x51                     --- DD ---
      {"???"},            // 0x52                     --- DD ---
      {"???"},            // 0x53                     --- DD ---
      {"???"},            // 0x54                     --- DD ---
      {"???"},            // 0x55                     --- DD ---
      {"???"},            // 0x56                     --- DD ---
      {"???"},            // 0x57                     --- DD ---
      {"???"},            // 0x58                     --- DD ---
      {"???"},            // 0x59                     --- DD ---
      {"???"},            // 0x5A                     --- DD ---
      {"???"},            // 0x5B                     --- DD ---
      {"???"},            // 0x5C                     --- DD ---
      {"???"},            // 0x5D                     --- DD ---
      {"???"},            // 0x5E                     --- DD ---
      {"???"},            // 0x5F                     --- DD ---
      {"???"},            // 0x60                     --- DD ---
      {"???"},            // 0x61                     --- DD ---
      {"???"},            // 0x62                     --- DD ---
      {"???"},            // 0x63                     --- DD ---
      {"???"},            // 0x64                     --- DD ---
      {"???"},            // 0x65                     --- DD ---
      {"???"},            // 0x66                     --- DD ---
      {"???"},            // 0x67                     --- DD ---
      {"???"},            // 0x68                     --- DD ---
      {"???"},            // 0x69                     --- DD ---
      {"???"},            // 0x6A                     --- DD ---
      {"???"},            // 0x6B                     --- DD ---
      {"???"},            // 0x6C                     --- DD ---
      {"???"},            // 0x6D                     --- DD ---
      {"???"},            // 0x6E                     --- DD ---
      {"???"},            // 0x6F                     --- DD ---
      {"???"},            // 0x70                     --- DD ---
      {"???"},            // 0x71                     --- DD ---
      {"???"},            // 0x72                     --- DD ---
      {"???"},            // 0x73                     --- DD ---
      {"???"},            // 0x74                     --- DD ---
      {"???"},            // 0x75                     --- DD ---
      {"???"},            // 0x76                     --- DD ---
      {"???"},            // 0x77                     --- DD ---
      {"???"},            // 0x78                     --- DD ---
      {"???"},            // 0x79                     --- DD ---
      {"???"},            // 0x7A                     --- DD ---
      {"???"},            // 0x7B                     --- DD ---
      {"???"},            // 0x7C                     --- DD ---
      {"???"},            // 0x7D                     --- DD ---
      {"???"},            // 0x7E                     --- DD ---
      {"???"},            // 0x7F                     --- DD ---
      {"???"},            // 0x80                     --- DD ---
      {"???"},            // 0x81                     --- DD ---
      {"???"},            // 0x82                     --- DD ---
      {"???"},            // 0x83                     --- DD ---
      {"???"},            // 0x84                     --- DD ---
      {"???"},            // 0x85                     --- DD ---
      {"???"},            // 0x86                     --- DD ---
      {"???"},            // 0x87                     --- DD ---
      {"???"},            // 0x88                     --- DD ---
      {"???"},            // 0x89                     --- DD ---
      {"???"},            // 0x8A                     --- DD ---
      {"???"},            // 0x8B                     --- DD ---
      {"???"},            // 0x8C                     --- DD ---
      {"???"},            // 0x8D                     --- DD ---
      {"???"},            // 0x8E                     --- DD ---
      {"???"},            // 0x8F                     --- DD ---
      {"???"},            // 0x90                     --- DD ---
      {"???"},            // 0x91                     --- DD ---
      {"???"},            // 0x92                     --- DD ---
      {"???"},            // 0x93                     --- DD ---
      {"???"},            // 0x94                     --- DD ---
      {"???"},            // 0x95                     --- DD ---
      {"???"},            // 0x96                     --- DD ---
      {"???"},            // 0x97                     --- DD ---
      {"???"},            // 0x98                     --- DD ---
      {"???"},            // 0x99                     --- DD ---
      {"???"},            // 0x9A                     --- DD ---
      {"???"},            // 0x9B                     --- DD ---
      {"???"},            // 0x9C                     --- DD ---
      {"???"},            // 0x9D                     --- DD ---
      {"???"},            // 0x9E                     --- DD ---
      {"???"},            // 0x9F                     --- DD ---
      {"???"},            // 0xA0                     --- DD ---
      {"???"},            // 0xA1                     --- DD ---
      {"???"},            // 0xA2                     --- DD ---
      {"???"},            // 0xA3                     --- DD ---
      {"???"},            // 0xA4                     --- DD ---
      {"???"},            // 0xA5                     --- DD ---
      {"???"},            // 0xA6                     --- DD ---
      {"???"},            // 0xA7                     --- DD ---
      {"???"},            // 0xA8                     --- DD ---
      {"???"},            // 0xA9                     --- DD ---
      {"???"},            // 0xAA                     --- DD ---
      {"???"},            // 0xAB                     --- DD ---
      {"???"},            // 0xAC                     --- DD ---
      {"???"},            // 0xAD                     --- DD ---
      {"???"},            // 0xAE                     --- DD ---
      {"???"},            // 0xAF                     --- DD ---
      {"???"},            // 0xB0                     --- DD ---
      {"???"},            // 0xB1                     --- DD ---
      {"???"},            // 0xB2                     --- DD ---
      {"???"},            // 0xB3                     --- DD ---
      {"???"},            // 0xB4                     --- DD ---
      {"???"},            // 0xB5                     --- DD ---
      {"???"},            // 0xB6                     --- DD ---
      {"???"},            // 0xB7                     --- DD ---
      {"???"},            // 0xB8                     --- DD ---
      {"???"},            // 0xB9                     --- DD ---
      {"???"},            // 0xBA                     --- DD ---
      {"???"},            // 0xBB                     --- DD ---
      {"???"},            // 0xBC                     --- DD ---
      {"???"},            // 0xBD                     --- DD ---
      {"???"},            // 0xBE                     --- DD ---
      {"???"},            // 0xBF                     --- DD ---
      {"FFREE"},          // 0xC0                     --- DD ---
      {"FFREE"},          // 0xC1                     --- DD ---
      {"FFREE"},          // 0xC2                     --- DD ---
      {"FFREE"},          // 0xC3                     --- DD ---
      {"FFREE"},          // 0xC4                     --- DD ---
      {"FFREE"},          // 0xC5                     --- DD ---
      {"FFREE"},          // 0xC6                     --- DD ---
      {"FFREE"},          // 0xC7                     --- DD ---
      {"???"},            // 0xC8                     --- DD ---
      {"???"},            // 0xC9                     --- DD ---
      {"???"},            // 0xCA                     --- DD ---
      {"???"},            // 0xCB                     --- DD ---
      {"???"},            // 0xCC                     --- DD ---
      {"???"},            // 0xCD                     --- DD ---
      {"???"},            // 0xCE                     --- DD ---
      {"???"},            // 0xCF                     --- DD ---
      {"FST"},            // 0xD0                     --- DD ---
      {"FST"},            // 0xD1                     --- DD ---
      {"FST"},            // 0xD2                     --- DD ---
      {"FST"},            // 0xD3                     --- DD ---
      {"FST"},            // 0xD4                     --- DD ---
      {"FST"},            // 0xD5                     --- DD ---
      {"FST"},            // 0xD6                     --- DD ---
      {"FST"},            // 0xD7                     --- DD ---
      {"FSTP"},           // 0xD8                     --- DD ---
      {"FSTP"},           // 0xD9                     --- DD ---
      {"FSTP"},           // 0xDA                     --- DD ---
      {"FSTP"},           // 0xDB                     --- DD ---
      {"FSTP"},           // 0xDC                     --- DD ---
      {"FSTP"},           // 0xDD                     --- DD ---
      {"FSTP"},           // 0xDE                     --- DD ---
      {"FSTP"},           // 0xDF                     --- DD ---
      {"FUCOM"},          // 0xE0                     --- DD ---
      {"FUCOM"},          // 0xE1                     --- DD ---
      {"FUCOM"},          // 0xE2                     --- DD ---
      {"FUCOM"},          // 0xE3                     --- DD ---
      {"FUCOM"},          // 0xE4                     --- DD ---
      {"FUCOM"},          // 0xE5                     --- DD ---
      {"FUCOM"},          // 0xE6                     --- DD ---
      {"FUCOM"},          // 0xE7                     --- DD ---
      {"FUCOMP"},         // 0xE8                     --- DD ---
      {"FUCOMP"},         // 0xE9                     --- DD ---
      {"FUCOMP"},         // 0xEA                     --- DD ---
      {"FUCOMP"},         // 0xEB                     --- DD ---
      {"FUCOMP"},         // 0xEC                     --- DD ---
      {"FUCOMP"},         // 0xED                     --- DD ---
      {"FUCOMP"},         // 0xEE                     --- DD ---
      {"FUCOMP"},         // 0xEF                     --- DD ---
      {"???"},            // 0xF0                     --- DD ---
      {"???"},            // 0xF1                     --- DD ---
      {"???"},            // 0xF2                     --- DD ---
      {"???"},            // 0xF3                     --- DD ---
      {"???"},            // 0xF4                     --- DD ---
      {"???"},            // 0xF5                     --- DD ---
      {"???"},            // 0xF6                     --- DD ---
      {"???"},            // 0xF7                     --- DD ---
      {"???"},            // 0xF8                     --- DD ---
      {"???"},            // 0xF9                     --- DD ---
      {"???"},            // 0xFA                     --- DD ---
      {"???"},            // 0xFB                     --- DD ---
      {"???"},            // 0xFC                     --- DD ---
      {"???"},            // 0xFD                     --- DD ---
      {"???"},            // 0xFE                     --- DD ---
      {"???"}             // 0xFF                     --- DD ---
  }; 
const static char intel_mnemonic_DD_ [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FLD"},           // 0x0
      {"???"},           // 0x1
      {"FST"},           // 0x2
      {"FSTP"},          // 0x3
      {"FRSTOR"},        // 0x4
      {"???"},           // 0x5
      {"---"},           // 0x6
      {"???"}            // 0x7
};
const static char intel_mnemonic_DD_6_ [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FSAVE"},           // wait
      {"FNSAVE"}           // no wait
};
const static char intel_mnemonic_DD_7_ [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FSTSW"},           // wait
      {"FNSTSW"}           // no wait
};
const static char intel_mnemonic_DE [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"???"},            // 0x0                      --- DE --- 
      {"???"},            // 0x1                      --- DE ---
      {"???"},            // 0x2                      --- DE ---
      {"???"},            // 0x3                      --- DE ---
      {"???"},            // 0x4                      --- DE ---
      {"???"},            // 0x5                      --- DE ---
      {"???"},            // 0x6                      --- DE ---
      {"???"},            // 0x7                      --- DE ---
      {"???"},            // 0x8                      --- DE ---
      {"???"},            // 0x9                      --- DE ---
      {"???"},            // 0xA                      --- DE ---
      {"???"},            // 0xB                      --- DE ---
      {"???"},            // 0xC                      --- DE ---
      {"???"},            // 0xD                      --- DE ---
      {"???"},            // 0xE                      --- DE ---
      {"???"},            // 0xF                      --- DE ---
      {"???"},            // 0x10                     --- DE ---
      {"???"},            // 0x11                     --- DE ---
      {"???"},            // 0x12                     --- DE ---
      {"???"},            // 0x13                     --- DE ---
      {"???"},            // 0x14                     --- DE ---
      {"???"},            // 0x15                     --- DE ---
      {"???"},            // 0x16                     --- DE ---
      {"???"},            // 0x17                     --- DE ---
      {"???"},            // 0x18                     --- DE ---
      {"???"},            // 0x19                     --- DE ---
      {"???"},            // 0x1A                     --- DE ---
      {"???"},            // 0x1B                     --- DE ---
      {"???"},            // 0x1C                     --- DE ---
      {"???"},            // 0x1D                     --- DE ---
      {"???"},            // 0x1E                     --- DE ---
      {"???"},            // 0x1F                     --- DE ---
      {"???"},            // 0x20                     --- DE ---
      {"???"},            // 0x21                     --- DE ---
      {"???"},            // 0x22                     --- DE ---
      {"???"},            // 0x23                     --- DE ---
      {"???"},            // 0x24                     --- DE ---
      {"???"},            // 0x25                     --- DE ---
      {"???"},            // 0x26                     --- DE ---
      {"???"},            // 0x27                     --- DE ---
      {"???"},            // 0x28                     --- DE ---
      {"???"},            // 0x29                     --- DE ---
      {"???"},            // 0x2A                     --- DE ---
      {"???"},            // 0x2B                     --- DE ---
      {"???"},            // 0x2C                     --- DE ---
      {"???"},            // 0x2D                     --- DE ---
      {"???"},            // 0x2E                     --- DE ---
      {"???"},            // 0x2F                     --- DE ---
      {"???"},            // 0x30                     --- DE ---
      {"???"},            // 0x31                     --- DE ---
      {"???"},            // 0x32                     --- DE ---
      {"???"},            // 0x33                     --- DE ---
      {"???"},            // 0x34                     --- DE ---
      {"???"},            // 0x35                     --- DE ---
      {"???"},            // 0x36                     --- DE ---
      {"???"},            // 0x37                     --- DE ---
      {"???"},            // 0x38                     --- DE ---
      {"???"},            // 0x39                     --- DE ---
      {"???"},            // 0x3A                     --- DE ---
      {"???"},            // 0x3B                     --- DE ---
      {"???"},            // 0x3C                     --- DE ---
      {"???"},            // 0x3D                     --- DE ---
      {"???"},            // 0x3E                     --- DE ---
      {"???"},            // 0x3F                     --- DE ---
      {"???"},            // 0x40                     --- DE ---
      {"???"},            // 0x41                     --- DE ---
      {"???"},            // 0x42                     --- DE ---
      {"???"},            // 0x43                     --- DE ---
      {"???"},            // 0x44                     --- DE ---
      {"???"},            // 0x45                     --- DE ---
      {"???"},            // 0x46                     --- DE ---
      {"???"},            // 0x47                     --- DE ---
      {"???"},            // 0x48                     --- DE ---
      {"???"},            // 0x49                     --- DE ---
      {"???"},            // 0x4A                     --- DE ---
      {"???"},            // 0x4B                     --- DE ---
      {"???"},            // 0x4C                     --- DE ---
      {"???"},            // 0x4D                     --- DE ---
      {"???"},            // 0x4E                     --- DE ---
      {"???"},            // 0x4F                     --- DE ---
      {"???"},            // 0x50                     --- DE ---
      {"???"},            // 0x51                     --- DE ---
      {"???"},            // 0x52                     --- DE ---
      {"???"},            // 0x53                     --- DE ---
      {"???"},            // 0x54                     --- DE ---
      {"???"},            // 0x55                     --- DE ---
      {"???"},            // 0x56                     --- DE ---
      {"???"},            // 0x57                     --- DE ---
      {"???"},            // 0x58                     --- DE ---
      {"???"},            // 0x59                     --- DE ---
      {"???"},            // 0x5A                     --- DE ---
      {"???"},            // 0x5B                     --- DE ---
      {"???"},            // 0x5C                     --- DE ---
      {"???"},            // 0x5D                     --- DE ---
      {"???"},            // 0x5E                     --- DE ---
      {"???"},            // 0x5F                     --- DE ---
      {"???"},            // 0x60                     --- DE ---
      {"???"},            // 0x61                     --- DE ---
      {"???"},            // 0x62                     --- DE ---
      {"???"},            // 0x63                     --- DE ---
      {"???"},            // 0x64                     --- DE ---
      {"???"},            // 0x65                     --- DE ---
      {"???"},            // 0x66                     --- DE ---
      {"???"},            // 0x67                     --- DE ---
      {"???"},            // 0x68                     --- DE ---
      {"???"},            // 0x69                     --- DE ---
      {"???"},            // 0x6A                     --- DE ---
      {"???"},            // 0x6B                     --- DE ---
      {"???"},            // 0x6C                     --- DE ---
      {"???"},            // 0x6D                     --- DE ---
      {"???"},            // 0x6E                     --- DE ---
      {"???"},            // 0x6F                     --- DE ---
      {"???"},            // 0x70                     --- DE ---
      {"???"},            // 0x71                     --- DE ---
      {"???"},            // 0x72                     --- DE ---
      {"???"},            // 0x73                     --- DE ---
      {"???"},            // 0x74                     --- DE ---
      {"???"},            // 0x75                     --- DE ---
      {"???"},            // 0x76                     --- DE ---
      {"???"},            // 0x77                     --- DE ---
      {"???"},            // 0x78                     --- DE ---
      {"???"},            // 0x79                     --- DE ---
      {"???"},            // 0x7A                     --- DE ---
      {"???"},            // 0x7B                     --- DE ---
      {"???"},            // 0x7C                     --- DE ---
      {"???"},            // 0x7D                     --- DE ---
      {"???"},            // 0x7E                     --- DE ---
      {"???"},            // 0x7F                     --- DE ---
      {"???"},            // 0x80                     --- DE ---
      {"???"},            // 0x81                     --- DE ---
      {"???"},            // 0x82                     --- DE ---
      {"???"},            // 0x83                     --- DE ---
      {"???"},            // 0x84                     --- DE ---
      {"???"},            // 0x85                     --- DE ---
      {"???"},            // 0x86                     --- DE ---
      {"???"},            // 0x87                     --- DE ---
      {"???"},            // 0x88                     --- DE ---
      {"???"},            // 0x89                     --- DE ---
      {"???"},            // 0x8A                     --- DE ---
      {"???"},            // 0x8B                     --- DE ---
      {"???"},            // 0x8C                     --- DE ---
      {"???"},            // 0x8D                     --- DE ---
      {"???"},            // 0x8E                     --- DE ---
      {"???"},            // 0x8F                     --- DE ---
      {"???"},            // 0x90                     --- DE ---
      {"???"},            // 0x91                     --- DE ---
      {"???"},            // 0x92                     --- DE ---
      {"???"},            // 0x93                     --- DE ---
      {"???"},            // 0x94                     --- DE ---
      {"???"},            // 0x95                     --- DE ---
      {"???"},            // 0x96                     --- DE ---
      {"???"},            // 0x97                     --- DE ---
      {"???"},            // 0x98                     --- DE ---
      {"???"},            // 0x99                     --- DE ---
      {"???"},            // 0x9A                     --- DE ---
      {"???"},            // 0x9B                     --- DE ---
      {"???"},            // 0x9C                     --- DE ---
      {"???"},            // 0x9D                     --- DE ---
      {"???"},            // 0x9E                     --- DE ---
      {"???"},            // 0x9F                     --- DE ---
      {"???"},            // 0xA0                     --- DE ---
      {"???"},            // 0xA1                     --- DE ---
      {"???"},            // 0xA2                     --- DE ---
      {"???"},            // 0xA3                     --- DE ---
      {"???"},            // 0xA4                     --- DE ---
      {"???"},            // 0xA5                     --- DE ---
      {"???"},            // 0xA6                     --- DE ---
      {"???"},            // 0xA7                     --- DE ---
      {"???"},            // 0xA8                     --- DE ---
      {"???"},            // 0xA9                     --- DE ---
      {"???"},            // 0xAA                     --- DE ---
      {"???"},            // 0xAB                     --- DE ---
      {"???"},            // 0xAC                     --- DE ---
      {"???"},            // 0xAD                     --- DE ---
      {"???"},            // 0xAE                     --- DE ---
      {"???"},            // 0xAF                     --- DE ---
      {"???"},            // 0xB0                     --- DE ---
      {"???"},            // 0xB1                     --- DE ---
      {"???"},            // 0xB2                     --- DE ---
      {"???"},            // 0xB3                     --- DE ---
      {"???"},            // 0xB4                     --- DE ---
      {"???"},            // 0xB5                     --- DE ---
      {"???"},            // 0xB6                     --- DE ---
      {"???"},            // 0xB7                     --- DE ---
      {"???"},            // 0xB8                     --- DE ---
      {"???"},            // 0xB9                     --- DE ---
      {"???"},            // 0xBA                     --- DE ---
      {"???"},            // 0xBB                     --- DE ---
      {"???"},            // 0xBC                     --- DE ---
      {"???"},            // 0xBD                     --- DE ---
      {"???"},            // 0xBE                     --- DE ---
      {"???"},            // 0xBF                     --- DE ---
      {"FADDP"},          // 0xC0                     --- DE ---
      {"FADDP"},          // 0xC1                     --- DE ---
      {"FADDP"},          // 0xC2                     --- DE ---
      {"FADDP"},          // 0xC3                     --- DE ---
      {"FADDP"},          // 0xC4                     --- DE ---
      {"FADDP"},          // 0xC5                     --- DE ---
      {"FADDP"},          // 0xC6                     --- DE ---
      {"FADDP"},          // 0xC7                     --- DE ---
      {"FMULP"},          // 0xC8                     --- DE ---
      {"FMULP"},          // 0xC9                     --- DE ---
      {"FMULP"},          // 0xCA                     --- DE ---
      {"FMULP"},          // 0xCB                     --- DE ---
      {"FMULP"},          // 0xCC                     --- DE ---
      {"FMULP"},          // 0xCD                     --- DE ---
      {"FMULP"},          // 0xCE                     --- DE ---
      {"FMULP"},          // 0xCF                     --- DE ---
      {"???"},            // 0xD0                     --- DE ---
      {"???"},            // 0xD1                     --- DE ---
      {"???"},            // 0xD2                     --- DE ---
      {"???"},            // 0xD3                     --- DE ---
      {"???"},            // 0xD4                     --- DE ---
      {"???"},            // 0xD5                     --- DE ---
      {"???"},            // 0xD6                     --- DE ---
      {"???"},            // 0xD7                     --- DE ---
      {"???"},            // 0xD8                     --- DE ---
      {"FCOMPP"},         // 0xD9                     --- DE ---
      {"???"},            // 0xDA                     --- DE ---
      {"???"},            // 0xDB                     --- DE ---
      {"???"},            // 0xDC                     --- DE ---
      {"???"},            // 0xDD                     --- DE ---
      {"???"},            // 0xDE                     --- DE ---
      {"???"},            // 0xDF                     --- DE ---
      {"FSUBRP"},         // 0xE0                     --- DE ---
      {"FSUBRP"},         // 0xE1                     --- DE ---
      {"FSUBRP"},         // 0xE2                     --- DE ---
      {"FSUBRP"},         // 0xE3                     --- DE ---
      {"FSUBRP"},         // 0xE4                     --- DE ---
      {"FSUBRP"},         // 0xE5                     --- DE ---
      {"FSUBRP"},         // 0xE6                     --- DE ---
      {"FSUBRP"},         // 0xE7                     --- DE ---
      {"FSUBP"},          // 0xE8                     --- DE ---
      {"FSUBP"},          // 0xE9                     --- DE ---
      {"FSUBP"},          // 0xEA                     --- DE ---
      {"FSUBP"},          // 0xEB                     --- DE ---
      {"FSUBP"},          // 0xEC                     --- DE ---
      {"FSUBP"},          // 0xED                     --- DE ---
      {"FSUBP"},          // 0xEE                     --- DE ---
      {"FSUBP"},          // 0xEF                     --- DE ---
      {"FDIVRP"},         // 0xF0                     --- DE ---
      {"FDIVRP"},         // 0xF1                     --- DE ---
      {"FDIVRP"},         // 0xF2                     --- DE ---
      {"FDIVRP"},         // 0xF3                     --- DE ---
      {"FDIVRP"},         // 0xF4                     --- DE ---
      {"FDIVRP"},         // 0xF5                     --- DE ---
      {"FDIVRP"},         // 0xF6                     --- DE ---
      {"FDIVRP"},         // 0xF7                     --- DE ---
      {"FDIVP"},          // 0xF8                     --- DE ---
      {"FDIVP"},          // 0xF9                     --- DE ---
      {"FDIVP"},          // 0xFA                     --- DE ---
      {"FDIVP"},          // 0xFB                     --- DE ---
      {"FDIVP"},          // 0xFC                     --- DE ---
      {"FDIVP"},          // 0xFD                     --- DE ---
      {"FDIVP"},          // 0xFE                     --- DE ---
      {"FDIVP"}           // 0xFF                     --- DE ---
  }; 
const static char intel_mnemonic_DE_ [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FIADD"},          // 0x0
      {"FIMUL"},          // 0x1
      {"FICOM"},          // 0x2
      {"FICOMP"},         // 0x3
      {"FISUB"},          // 0x4
      {"FISUBR"},         // 0x5
      {"FIDIV"},          // 0x6
      {"FIDIVR"}          // 0x7
};
const static char intel_mnemonic_DF [256] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
  {
      {"???"},            // 0x0                      --- DF --- 
      {"???"},            // 0x1                      --- DF ---
      {"???"},            // 0x2                      --- DF ---
      {"???"},            // 0x3                      --- DF ---
      {"???"},            // 0x4                      --- DF ---
      {"???"},            // 0x5                      --- DF ---
      {"???"},            // 0x6                      --- DF ---
      {"???"},            // 0x7                      --- DF ---
      {"???"},            // 0x8                      --- DF ---
      {"???"},            // 0x9                      --- DF ---
      {"???"},            // 0xA                      --- DF ---
      {"???"},            // 0xB                      --- DF ---
      {"???"},            // 0xC                      --- DF ---
      {"???"},            // 0xD                      --- DF ---
      {"???"},            // 0xE                      --- DF ---
      {"???"},            // 0xF                      --- DF ---
      {"???"},            // 0x10                     --- DF ---
      {"???"},            // 0x11                     --- DF ---
      {"???"},            // 0x12                     --- DF ---
      {"???"},            // 0x13                     --- DF ---
      {"???"},            // 0x14                     --- DF ---
      {"???"},            // 0x15                     --- DF ---
      {"???"},            // 0x16                     --- DF ---
      {"???"},            // 0x17                     --- DF ---
      {"???"},            // 0x18                     --- DF ---
      {"???"},            // 0x19                     --- DF ---
      {"???"},            // 0x1A                     --- DF ---
      {"???"},            // 0x1B                     --- DF ---
      {"???"},            // 0x1C                     --- DF ---
      {"???"},            // 0x1D                     --- DF ---
      {"???"},            // 0x1E                     --- DF ---
      {"???"},            // 0x1F                     --- DF ---
      {"???"},            // 0x20                     --- DF ---
      {"???"},            // 0x21                     --- DF ---
      {"???"},            // 0x22                     --- DF ---
      {"???"},            // 0x23                     --- DF ---
      {"???"},            // 0x24                     --- DF ---
      {"???"},            // 0x25                     --- DF ---
      {"???"},            // 0x26                     --- DF ---
      {"???"},            // 0x27                     --- DF ---
      {"???"},            // 0x28                     --- DF ---
      {"???"},            // 0x29                     --- DF ---
      {"???"},            // 0x2A                     --- DF ---
      {"???"},            // 0x2B                     --- DF ---
      {"???"},            // 0x2C                     --- DF ---
      {"???"},            // 0x2D                     --- DF ---
      {"???"},            // 0x2E                     --- DF ---
      {"???"},            // 0x2F                     --- DF ---
      {"???"},            // 0x30                     --- DF ---
      {"???"},            // 0x31                     --- DF ---
      {"???"},            // 0x32                     --- DF ---
      {"???"},            // 0x33                     --- DF ---
      {"???"},            // 0x34                     --- DF ---
      {"???"},            // 0x35                     --- DF ---
      {"???"},            // 0x36                     --- DF ---
      {"???"},            // 0x37                     --- DF ---
      {"???"},            // 0x38                     --- DF ---
      {"???"},            // 0x39                     --- DF ---
      {"???"},            // 0x3A                     --- DF ---
      {"???"},            // 0x3B                     --- DF ---
      {"???"},            // 0x3C                     --- DF ---
      {"???"},            // 0x3D                     --- DF ---
      {"???"},            // 0x3E                     --- DF ---
      {"???"},            // 0x3F                     --- DF ---
      {"???"},            // 0x40                     --- DF ---
      {"???"},            // 0x41                     --- DF ---
      {"???"},            // 0x42                     --- DF ---
      {"???"},            // 0x43                     --- DF ---
      {"???"},            // 0x44                     --- DF ---
      {"???"},            // 0x45                     --- DF ---
      {"???"},            // 0x46                     --- DF ---
      {"???"},            // 0x47                     --- DF ---
      {"???"},            // 0x48                     --- DF ---
      {"???"},            // 0x49                     --- DF ---
      {"???"},            // 0x4A                     --- DF ---
      {"???"},            // 0x4B                     --- DF ---
      {"???"},            // 0x4C                     --- DF ---
      {"???"},            // 0x4D                     --- DF ---
      {"???"},            // 0x4E                     --- DF ---
      {"???"},            // 0x4F                     --- DF ---
      {"???"},            // 0x50                     --- DF ---
      {"???"},            // 0x51                     --- DF ---
      {"???"},            // 0x52                     --- DF ---
      {"???"},            // 0x53                     --- DF ---
      {"???"},            // 0x54                     --- DF ---
      {"???"},            // 0x55                     --- DF ---
      {"???"},            // 0x56                     --- DF ---
      {"???"},            // 0x57                     --- DF ---
      {"???"},            // 0x58                     --- DF ---
      {"???"},            // 0x59                     --- DF ---
      {"???"},            // 0x5A                     --- DF ---
      {"???"},            // 0x5B                     --- DF ---
      {"???"},            // 0x5C                     --- DF ---
      {"???"},            // 0x5D                     --- DF ---
      {"???"},            // 0x5E                     --- DF ---
      {"???"},            // 0x5F                     --- DF ---
      {"???"},            // 0x60                     --- DF ---
      {"???"},            // 0x61                     --- DF ---
      {"???"},            // 0x62                     --- DF ---
      {"???"},            // 0x63                     --- DF ---
      {"???"},            // 0x64                     --- DF ---
      {"???"},            // 0x65                     --- DF ---
      {"???"},            // 0x66                     --- DF ---
      {"???"},            // 0x67                     --- DF ---
      {"???"},            // 0x68                     --- DF ---
      {"???"},            // 0x69                     --- DF ---
      {"???"},            // 0x6A                     --- DF ---
      {"???"},            // 0x6B                     --- DF ---
      {"???"},            // 0x6C                     --- DF ---
      {"???"},            // 0x6D                     --- DF ---
      {"???"},            // 0x6E                     --- DF ---
      {"???"},            // 0x6F                     --- DF ---
      {"???"},            // 0x70                     --- DF ---
      {"???"},            // 0x71                     --- DF ---
      {"???"},            // 0x72                     --- DF ---
      {"???"},            // 0x73                     --- DF ---
      {"???"},            // 0x74                     --- DF ---
      {"???"},            // 0x75                     --- DF ---
      {"???"},            // 0x76                     --- DF ---
      {"???"},            // 0x77                     --- DF ---
      {"???"},            // 0x78                     --- DF ---
      {"???"},            // 0x79                     --- DF ---
      {"???"},            // 0x7A                     --- DF ---
      {"???"},            // 0x7B                     --- DF ---
      {"???"},            // 0x7C                     --- DF ---
      {"???"},            // 0x7D                     --- DF ---
      {"???"},            // 0x7E                     --- DF ---
      {"???"},            // 0x7F                     --- DF ---
      {"???"},            // 0x80                     --- DF ---
      {"???"},            // 0x81                     --- DF ---
      {"???"},            // 0x82                     --- DF ---
      {"???"},            // 0x83                     --- DF ---
      {"???"},            // 0x84                     --- DF ---
      {"???"},            // 0x85                     --- DF ---
      {"???"},            // 0x86                     --- DF ---
      {"???"},            // 0x87                     --- DF ---
      {"???"},            // 0x88                     --- DF ---
      {"???"},            // 0x89                     --- DF ---
      {"???"},            // 0x8A                     --- DF ---
      {"???"},            // 0x8B                     --- DF ---
      {"???"},            // 0x8C                     --- DF ---
      {"???"},            // 0x8D                     --- DF ---
      {"???"},            // 0x8E                     --- DF ---
      {"???"},            // 0x8F                     --- DF ---
      {"???"},            // 0x90                     --- DF ---
      {"???"},            // 0x91                     --- DF ---
      {"???"},            // 0x92                     --- DF ---
      {"???"},            // 0x93                     --- DF ---
      {"???"},            // 0x94                     --- DF ---
      {"???"},            // 0x95                     --- DF ---
      {"???"},            // 0x96                     --- DF ---
      {"???"},            // 0x97                     --- DF ---
      {"???"},            // 0x98                     --- DF ---
      {"???"},            // 0x99                     --- DF ---
      {"???"},            // 0x9A                     --- DF ---
      {"???"},            // 0x9B                     --- DF ---
      {"???"},            // 0x9C                     --- DF ---
      {"???"},            // 0x9D                     --- DF ---
      {"???"},            // 0x9E                     --- DF ---
      {"???"},            // 0x9F                     --- DF ---
      {"???"},            // 0xA0                     --- DF ---
      {"???"},            // 0xA1                     --- DF ---
      {"???"},            // 0xA2                     --- DF ---
      {"???"},            // 0xA3                     --- DF ---
      {"???"},            // 0xA4                     --- DF ---
      {"???"},            // 0xA5                     --- DF ---
      {"???"},            // 0xA6                     --- DF ---
      {"???"},            // 0xA7                     --- DF ---
      {"???"},            // 0xA8                     --- DF ---
      {"???"},            // 0xA9                     --- DF ---
      {"???"},            // 0xAA                     --- DF ---
      {"???"},            // 0xAB                     --- DF ---
      {"???"},            // 0xAC                     --- DF ---
      {"???"},            // 0xAD                     --- DF ---
      {"???"},            // 0xAE                     --- DF ---
      {"???"},            // 0xAF                     --- DF ---
      {"???"},            // 0xB0                     --- DF ---
      {"???"},            // 0xB1                     --- DF ---
      {"???"},            // 0xB2                     --- DF ---
      {"???"},            // 0xB3                     --- DF ---
      {"???"},            // 0xB4                     --- DF ---
      {"???"},            // 0xB5                     --- DF ---
      {"???"},            // 0xB6                     --- DF ---
      {"???"},            // 0xB7                     --- DF ---
      {"???"},            // 0xB8                     --- DF ---
      {"???"},            // 0xB9                     --- DF ---
      {"???"},            // 0xBA                     --- DF ---
      {"???"},            // 0xBB                     --- DF ---
      {"???"},            // 0xBC                     --- DF ---
      {"???"},            // 0xBD                     --- DF ---
      {"???"},            // 0xBE                     --- DF ---
      {"???"},            // 0xBF                     --- DF ---
      {"FFREEP"},         // 0xC0                     --- DF ---
      {"FFREEP"},         // 0xC1                     --- DF ---
      {"FFREEP"},         // 0xC2                     --- DF ---
      {"FFREEP"},         // 0xC3                     --- DF ---
      {"FFREEP"},         // 0xC4                     --- DF ---
      {"FFREEP"},         // 0xC5                     --- DF ---
      {"FFREEP"},         // 0xC6                     --- DF ---
      {"FFREEP"},         // 0xC7                     --- DF ---
      {"???"},            // 0xC8                     --- DF ---
      {"???"},            // 0xC9                     --- DF ---
      {"???"},            // 0xCA                     --- DF ---
      {"???"},            // 0xCB                     --- DF ---
      {"???"},            // 0xCC                     --- DF ---
      {"???"},            // 0xCD                     --- DF ---
      {"???"},            // 0xCE                     --- DF ---
      {"???"},            // 0xCF                     --- DF ---
      {"???"},            // 0xD0                     --- DF ---
      {"???"},            // 0xD1                     --- DF ---
      {"???"},            // 0xD2                     --- DF ---
      {"???"},            // 0xD3                     --- DF ---
      {"???"},            // 0xD4                     --- DF ---
      {"???"},            // 0xD5                     --- DF ---
      {"???"},            // 0xD6                     --- DF ---
      {"???"},            // 0xD7                     --- DF ---
      {"???"},            // 0xD8                     --- DF ---
      {"???"},            // 0xD9                     --- DF ---
      {"???"},            // 0xDA                     --- DF ---
      {"???"},            // 0xDB                     --- DF ---
      {"???"},            // 0xDC                     --- DF ---
      {"???"},            // 0xDD                     --- DF ---
      {"???"},            // 0xDE                     --- DF ---
      {"???"},            // 0xDF                     --- DF ---
      {"---"},            // 0xE0                     --- DF ---
      {"???"},            // 0xE1                     --- DF ---
      {"???"},            // 0xE2                     --- DF ---
      {"???"},            // 0xE3                     --- DF ---
      {"???"},            // 0xE4                     --- DF ---
      {"???"},            // 0xE5                     --- DF ---
      {"???"},            // 0xE6                     --- DF ---
      {"???"},            // 0xE7                     --- DF ---
      {"FUCOMIP"},        // 0xE8                     --- DF ---
      {"FUCOMIP"},        // 0xE9                     --- DF ---
      {"FUCOMIP"},        // 0xEA                     --- DF ---
      {"FUCOMIP"},        // 0xEB                     --- DF ---
      {"FUCOMIP"},        // 0xEC                     --- DF ---
      {"FUCOMIP"},        // 0xED                     --- DF ---
      {"FUCOMIP"},        // 0xEE                     --- DF ---
      {"FUCOMIP"},        // 0xEF                     --- DF ---
      {"???"},            // 0xF0                     --- DF ---
      {"???"},            // 0xF1                     --- DF ---
      {"???"},            // 0xF2                     --- DF ---
      {"???"},            // 0xF3                     --- DF ---
      {"???"},            // 0xF4                     --- DF ---
      {"???"},            // 0xF5                     --- DF ---
      {"???"},            // 0xF6                     --- DF ---
      {"???"},            // 0xF7                     --- DF ---
      {"???"},            // 0xF8                     --- DF ---
      {"???"},            // 0xF9                     --- DF ---
      {"???"},            // 0xFA                     --- DF ---
      {"???"},            // 0xFB                     --- DF ---
      {"???"},            // 0xFC                     --- DF ---
      {"???"},            // 0xFD                     --- DF ---
      {"???"},            // 0xFE                     --- DF ---
      {"???"}             // 0xFF                     --- DF ---
  }; 
const static char intel_mnemonic_DF_E0_ [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FSTSW"},           // wait
      {"FNSTSW"}           // no wait
};
const static char intel_mnemonic_DF_ [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"FILD"},           // 0x0
      {"???"},            // 0x1
      {"FIST"},           // 0x2
      {"FISTP"},          // 0x3
      {"FBLD"},           // 0x4
      {"FILD"},           // 0x5
      {"FBSTP"},          // 0x6
      {"FISTP"}           // 0x7
};
const static char intel_mnemonic_E3 [2] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"JCX"},            // 16
      {"JECX"}            // 32
};
const static char intel_mnemonic_F6 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"TEST"},           // 0x0
      {"???"},            // 0x1
      {"NOT"},            // 0x2
      {"NEG"},            // 0x3
      {"MUL"},            // 0x4
      {"IMUL"},           // 0x5
      {"DIV"},            // 0x6
      {"IDIV"}            // 0x7
};
const static char intel_mnemonic_F7 [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"TEST"},           // 0x0
      {"???"},            // 0x1
      {"NOT"},            // 0x2
      {"NEG"},            // 0x3
      {"MUL"},            // 0x4
      {"IMUL"},           // 0x5
      {"DIV"},            // 0x6
      {"IDIV"}            // 0x7
};
const static char intel_mnemonic_FE [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"INC"},            // 0x0
      {"DEC"},            // 0x1
      {"???"},            // 0x2
      {"???"},            // 0x3
      {"???"},            // 0x4
      {"???"},            // 0x5
      {"???"},            // 0x6
      {"???"}             // 0x7
};
const static char intel_mnemonic_FF [8] [DISASSEMBLY_MAX_MNEMONIC_LENGTH] = 
{
      {"INC"},            // 0x0
      {"DEC"},            // 0x1
      {"CALL"},           // 0x2
      {"CALL"},           // 0x3
      {"JMP"},            // 0x4
      {"JMP"},            // 0x5
      {"PUSH"},           // 0x6
      {"???"}             // 0x7
};

#endif
