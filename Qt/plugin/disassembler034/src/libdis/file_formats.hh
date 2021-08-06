/*
 * file_formats.hh: for determining the format of a binary file
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/
#ifndef FILE_FORMATS_HH
#define FILE_FORMATS_HH

#include "disassembly.hh"
#include "disassembly_options.hh"    


#define TYPE_OF_FILE_UNKNOWN        0
#define TYPE_OF_FILE_DISASSEMBLY    1
#define TYPE_OF_FILE_INTEL_ELF      DISASSEMBLY_TYPE_INTEL_ELF
#define TYPE_OF_FILE_WINDOWS_PE     DISASSEMBLY_TYPE_INTEL_WINPE
#define TYPE_OF_FILE_INTEL_RAW      DISASSEMBLY_TYPE_INTEL_RAW

#define IMAGE_DOS_SIGNATURE    0x5A4D     /* MZ   */                    // DOS / WINDOWS signatures
#define IMAGE_NT_SIGNATURE     0x00004550 /* PE00 */                    // windows portable executable
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

namespace dis {
    //     Microsoft DOS / Windows
	struct IMAGE_DOS_HEADER;
    struct IMAGE_FILE_HEADER;
    struct IMAGE_OPTIONAL_HEADER;
    struct IMAGE_DATA_DIRECTORY;
    struct IMAGE_NT_HEADERS;
    struct IMAGE_SECTION_HEADER;
    struct IMAGE_IMPORT_BY_NAME;
    struct IMAGE_THUNK_DATA;
    struct IMAGE_IMPORT_DESCRIPTOR;

    //     Elf
    struct IMAGE_ELF_HEADER;
    struct ELF32_HDR;
    struct ELF64_HDR;
    struct ELF32_SHDR;
    struct ELF32_SYM;
    struct ELF32_DYN;
    struct ELF32_REL;
    struct ELF32_RELA;
}    

//////////////////////////////////////////////: file types ////////////////////////////////////////////////////

#define MAX_FILE_TYPE_NAME_LENGTH   40
#define MAX_FILE_TYPE               4

struct FILE_TYPE                           
{
    char   type_name [MAX_FILE_TYPE_NAME_LENGTH];
    int    type_id; 
};
#define FILE_TYPE_LENGTH (int) sizeof (FILE_TYPE)

const static FILE_TYPE  file_type_list [MAX_FILE_TYPE] =    // all possible file types
        {
            {"Unknown File Type              ", (int) TYPE_OF_FILE_UNKNOWN }, 
            {"Elf Executable                 ", (int) TYPE_OF_FILE_INTEL_ELF},
            {"Intel Architecture Raw Binary  ", (int) TYPE_OF_FILE_INTEL_RAW }, 
            {"Windows Portable Executable    ", (int) TYPE_OF_FILE_WINDOWS_PE }
        };

//////////////////////////////////////////////: DOS - Windows //////////////////////////////////////////////////

#define IMAGE_DOS_SIGNATURE    0x5A4D     /* MZ   */                    // DOS / WINDOWS signatures
#define IMAGE_NT_SIGNATURE     0x00004550 /* PE00 */                    // windows portable executable
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16


struct dis::IMAGE_DOS_HEADER                                            // begins a DOS / WINDOWS binary
{
    short  e_magic;      /* 00: MZ Header signature */
    short  e_cblp;       /* 02: Bytes on last page of file */
    short  e_cp;         /* 04: Pages in file */
    short  e_crlc;       /* 06: Relocations */
    short  e_cparhdr;    /* 08: Size of header in paragraphs */
    short  e_minalloc;   /* 0a: Minimum extra paragraphs needed */
    short  e_maxalloc;   /* 0c: Maximum extra paragraphs needed */
    short  e_ss;         /* 0e: Initial (relative) SS value */
    short  e_sp;         /* 10: Initial SP value */
    short  e_csum;       /* 12: Checksum */
    short  e_ip;         /* 14: Initial IP value */
    short  e_cs;         /* 16: Initial (relative) CS value */
    short  e_lfarlc;     /* 18: File address of relocation table */
    short  e_ovno;       /* 1a: Overlay number */
    short  e_res[4];     /* 1c: Reserved words */
    short  e_oemid;      /* 24: OEM identifier (for e_oeminfo) */
    short  e_oeminfo;    /* 26: OEM information; e_oemid specific */
    short  e_res2[10];   /* 28: Reserved words */
    int    e_lfanew;     /* 3c: Offset to extended header */
};                                     
#define IMAGE_DOS_HEADER_LENGTH (int) sizeof (IMAGE_DOS_HEADER)

struct dis::IMAGE_FILE_HEADER {
  short  Machine;
  short  NumberOfSections;
  int    TimeDateStamp;
  int    PointerToSymbolTable;
  int    NumberOfSymbols;
  short  SizeOfOptionalHeader;
  short  Characteristics;
};

struct dis::IMAGE_DATA_DIRECTORY {
  int VirtualAddress;
  int Size;
};

struct dis::IMAGE_OPTIONAL_HEADER {

  /* Standard fields */

  short  Magic; /* 0x10b or 0x107 */	/* 0x00 */
  char  MajorLinkerVersion;
  char  MinorLinkerVersion;
  int SizeOfCode;
  int SizeOfInitializedData;
  int SizeOfUninitializedData;
  int AddressOfEntryPoint;		/* 0x10 */
  int BaseOfCode;
  int BaseOfData;

  /* NT additional fields */

  int ImageBase;
  int SectionAlignment;		/* 0x20 */
  int FileAlignment;
  short  MajorOperatingSystemVersion;
  short  MinorOperatingSystemVersion;
  short  MajorImageVersion;
  short  MinorImageVersion;
  short  MajorSubsystemVersion;		/* 0x30 */
  short  MinorSubsystemVersion;
  int Win32VersionValue;
  int SizeOfImage;
  int SizeOfHeaders;
  int CheckSum;			/* 0x40 */
  short  Subsystem;
  short  DllCharacteristics;
  int SizeOfStackReserve;
  int SizeOfStackCommit;
  int SizeOfHeapReserve;		/* 0x50 */
  int SizeOfHeapCommit;
  int LoaderFlags;
  int NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]; /* 0x60 */
  /* 0xE0 */
};

struct dis::IMAGE_NT_HEADERS {
  int Signature;                                          /* "PE"\0\0 */	/* 0x00 */
  IMAGE_FILE_HEADER FileHeader;	                          /* 0x04 */
  IMAGE_OPTIONAL_HEADER OptionalHeader;                   /* 0x18 */
};
#define IMAGE_NT_HEADERS_LENGTH (int) sizeof (IMAGE_NT_HEADERS)

struct dis::IMAGE_IMPORT_BY_NAME {
	short	Hint;
	char	Name[1];
} ;       

struct dis::IMAGE_THUNK_DATA {
	union {
		char                   *ForwarderString;
		int                    *Function;
		int                     Ordinal;
		IMAGE_IMPORT_BY_NAME   *AddressOfData;
	} u1;
} ;       
#define IMAGE_THUNK_DATA_LENGTH (int) sizeof (IMAGE_THUNK_DATA)

struct dis::IMAGE_IMPORT_DESCRIPTOR {
	union {
		int	              Characteristics;          /* 0 for terminating null import descriptor  */
		IMAGE_THUNK_DATA *OriginalFirstThunk;	    /* RVA to original unbound IAT */
	} u;
	int	TimeDateStamp;	       /* 0 if not bound,
				                * -1 if bound, and real date\time stamp in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT 
				                *                                           (new BIND)
				                * otherwise date/time stamp of DLL bound to (Old BIND)
				                */
	int	ForwarderChain;	       /* -1 if no forwarders */
	int	Name;                  /* RVA to IAT (if bound this IAT has actual addresses) */
	IMAGE_THUNK_DATA *FirstThunk;	
} ;
#define IMAGE_IMPORT_DESCRIPTOR_LENGTH (int) sizeof (IMAGE_IMPORT_DESCRIPTOR)

struct dis::IMAGE_SECTION_HEADER 
{
  char  Name[8];
  union 
  {
    int PhysicalAddress;
    int VirtualSize;
  };
  int VirtualAddress;
  int SizeOfRawData;
  int PointerToRawData;
  int PointerToRelocations;
  int PointerToLinenumbers;
  short  NumberOfRelocations;
  short  NumberOfLinenumbers;
  int Characteristics;
};
#define  IMAGE_SECTION_HEADER_LENGTH (int) sizeof (IMAGE_SECTION_HEADER)

#define IMAGE_SCN_CNT_CODE                      0x00000020
#define IMAGE_SCN_CNT_INITIALIZED_DATA          0x00000040
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA        0x00000080
#define	IMAGE_SCN_LNK_OTHER			            0x00000100 
#define	IMAGE_SCN_LNK_INFO			            0x00000200  
#define	IMAGE_SCN_LNK_REMOVE			        0x00000800
#define	IMAGE_SCN_LNK_COMDAT			        0x00001000
#define	IMAGE_SCN_MEM_FARDATA			        0x00008000
#define	IMAGE_SCN_MEM_PURGEABLE			        0x00020000
#define	IMAGE_SCN_MEM_16BIT			            0x00020000
#define	IMAGE_SCN_MEM_LOCKED			        0x00040000
#define	IMAGE_SCN_MEM_PRELOAD			        0x00080000
#define	IMAGE_SCN_ALIGN_1BYTES			        0x00100000
#define	IMAGE_SCN_ALIGN_2BYTES			        0x00200000
#define	IMAGE_SCN_ALIGN_4BYTES			        0x00300000
#define	IMAGE_SCN_ALIGN_8BYTES			        0x00400000
#define	IMAGE_SCN_ALIGN_16BYTES			        0x00500000  
#define IMAGE_SCN_ALIGN_32BYTES			        0x00600000
#define IMAGE_SCN_ALIGN_64BYTES			        0x00700000
#define IMAGE_SCN_LNK_NRELOC_OVFL		        0x01000000  
#define IMAGE_SCN_MEM_DISCARDABLE		        0x02000000
#define IMAGE_SCN_MEM_NOT_CACHED		        0x04000000
#define IMAGE_SCN_MEM_NOT_PAGED			        0x08000000
#define IMAGE_SCN_MEM_SHARED			        0x10000000
#define IMAGE_SCN_MEM_EXECUTE			        0x20000000
#define IMAGE_SCN_MEM_READ			            0x40000000
#define IMAGE_SCN_MEM_WRITE			            0x80000000


//////////////////////////////////////////////: ELF - portable format //////////////////////////////////////////////////
                                              // mostly from /usr/src/linux/include/asm-i386/types.h
typedef __signed__ int       __s32;                
typedef __signed__ short     __s16;
typedef unsigned short       __u16;
typedef unsigned int         __u32;                 
typedef __signed__ long long __s64;
typedef unsigned long long   __u64;


/* 32-bit ELF base types. */
typedef __u32	Elf32_Addr;
typedef __u16	Elf32_Half;
typedef __u32	Elf32_Off;
typedef __s32	Elf32_Sword;
typedef __u32	Elf32_Word;

/* 64-bit ELF base types. */
typedef __u64	Elf64_Addr;
typedef __u16	Elf64_Half;
typedef __s16	Elf64_SHalf;
typedef __u64	Elf64_Off;
typedef __s32	Elf64_Sword;
typedef __u32	Elf64_Word;
typedef __u64	Elf64_Xword;
typedef __s64	Elf64_Sxword;

#define EI_NIDENT	16

struct dis::ELF32_HDR
{                            // ELF binary header, from /usr/src/linux/include/linux/elf.h
  unsigned char	e_ident[EI_NIDENT];
  Elf32_Half	e_type;
  Elf32_Half	e_machine;
  Elf32_Word	e_version;
  Elf32_Addr	e_entry;  /* Entry point */
  Elf32_Off	    e_phoff;
  Elf32_Off	    e_shoff;
  Elf32_Word	e_flags;
  Elf32_Half	e_ehsize;
  Elf32_Half	e_phentsize;
  Elf32_Half	e_phnum;
  Elf32_Half	e_shentsize;
  Elf32_Half	e_shnum;
  Elf32_Half	e_shstrndx;
};
#define ELF32_HDR_LENGTH (int) sizeof (ELF32_HDR)

struct dis::ELF64_HDR
{
  unsigned char	e_ident[16];		/* ELF "magic number" */
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;		/* Entry point virtual address */
  Elf64_Off e_phoff;		/* Program header table file offset */
  Elf64_Off e_shoff;		/* Section header table file offset */
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
};           
#define ELF64_HDR_LENGTH (int) sizeof (ELF64_HDR)

struct dis::ELF32_SYM
{
  Elf32_Word	st_name;
  Elf32_Addr	st_value;
  Elf32_Word	st_size;
  unsigned char	st_info;
  unsigned char	st_other;
  Elf32_Half	st_shndx;
};
#define ELF32_SYM_LENGTH (int) sizeof (ELF32_SYM)

struct dis::ELF32_DYN
{
  Elf32_Sword d_tag;
  union{
    Elf32_Sword	d_val;
    Elf32_Addr	d_ptr;
  } d_un;
};
#define ELF32_DYN_LENGTH (int) sizeof (ELF32_DYN)

#define	ELFMAG0		0x7f		/* EI_MAG */
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'
#define	ELFMAG		"\177ELF"

#define	ELFCLASSNONE	0		/* EI_CLASS */
#define	ELFCLASS32	    1
#define	ELFCLASS64      2
#define	ELFCLASSNUM     3

/* These constants define the various ELF target machines */
#define EM_NONE         0
#define EM_M32          1
#define EM_SPARC        2
#define EM_386          3
#define EM_68K          4
#define EM_88K          5
#define EM_486          6   /* Perhaps disused */
#define EM_860          7
#define EM_MIPS		    8	/* MIPS R3000 (officially, big-endian only) */
#define EM_MIPS_RS4_BE  10	/* MIPS R4000 big-endian */               
#define EM_PARISC       15	/* HPPA */                                
#define EM_SPARC32PLUS  18	/* Sun's "v8plus" */                      
#define EM_PPC	        20	/* PowerPC */
#define EM_PPC64        21       /* PowerPC64 */                       
#define EM_SH	        42	/* SuperH */                              
#define EM_SPARCV9      43	/* SPARC v9 64-bit */                     
#define EM_IA_64	    50	/* HP/Intel IA-64 */                          
#define EM_X86_64	    62	/* AMD x86-64 */                              
#define EM_S390		    22	/* IBM S/390 */                               
#define EM_CRIS         76      /* Axis Communications 32-bit embedded processor */
#define EM_V850		    87	/* NEC v850 */                                             
#define EM_M32R		    88	/* Renesas M32R */                                         
#define EM_H8_300       46      /* Renesas H8/300,300H,H8S */                      
#define EM_ALPHA	    0x9026                                                         
#define EM_CYGNUS_V850	0x9080
#define EM_CYGNUS_M32R	0x9041
#define EM_S390_OLD     0xA390

struct dis::ELF32_SHDR
{
  Elf32_Word	sh_name;
  Elf32_Word	sh_type;
  Elf32_Word	sh_flags;
  Elf32_Addr	sh_addr;
  Elf32_Off	sh_offset;
  Elf32_Word	sh_size;
  Elf32_Word	sh_link;
  Elf32_Word	sh_info;
  Elf32_Word	sh_addralign;
  Elf32_Word	sh_entsize;
};
#define ELF32_SHDR_LENGTH (int) sizeof (ELF32_SHDR)


struct dis::ELF32_REL
{
  Elf32_Addr	r_offset;
  Elf32_Word	r_info;
};

struct dis::ELF32_RELA
{
  Elf32_Addr	r_offset;
  Elf32_Word	r_info;
  Elf32_Sword	r_addend;
};


/* sh_type */
#define SHT_NULL	0
#define SHT_PROGBITS	1
#define SHT_SYMTAB	2
#define SHT_STRTAB	3
#define SHT_RELA	4
#define SHT_HASH	5
#define SHT_DYNAMIC	6
#define SHT_NOTE	7
#define SHT_NOBITS	8
#define SHT_REL		9
#define SHT_SHLIB	10
#define SHT_DYNSYM	11
#define SHT_NUM		12
#define SHT_LOPROC	0x70000000
#define SHT_HIPROC	0x7fffffff
#define SHT_LOUSER	0x80000000
#define SHT_HIUSER	0xffffffff

/* sh_flags */
#define SHF_WRITE	0x1
#define SHF_ALLOC	0x2
#define SHF_EXECINSTR	0x4
#define SHF_MASKPROC	0xf0000000

/* This info is needed when parsing the symbol table */
#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4

#define ELF_ST_BIND(x)		((x) >> 4)
#define ELF_ST_TYPE(x)		(((unsigned int) x) & 0xf)
#define ELF32_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x)	ELF_ST_TYPE(x)
#define ELF64_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x)	ELF_ST_TYPE(x)

#define ELF32_R_SYM(x)      ((x) >> 8)
#define ELF32_R_TYPE(x)     ((x) & 0xff)


#endif
