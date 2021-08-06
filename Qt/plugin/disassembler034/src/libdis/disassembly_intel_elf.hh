/*
 * disassembly_elf: disassembly of a windows Portable Executable
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DISASSEMBLY_INTEL_ELF_HH
#define DISASSEMBLY_INTEL_ELF_HH  

#define DISASSEMBLY_INTEL_ELF_EXTRA_INTERP  1
#define DISASSEMBLY_INTEL_ELF_EXTRA_NOTE    2

#define DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_INTERP   ".interp"
#define DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_NOTE     ".note"
#define DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_SECTIONS "Sections"

#include "disassembly_intel.hh"

namespace dis {
	class  Disassembly_Elf;
    struct Relocation;
}   

struct dis::Relocation
{            
    int                             source;            //  before relocation
    int                             target;            //  after  relocation
};           


class dis::Disassembly_Elf : public Disassembly_Intel
{
 public:
    /*
	Construct a new Disassembly.
	*/
 	Disassembly_Elf(
        string* file_name, 
        dis::Main_Gui *mg,
        dis::Disassembly_Options *options);

    Disassembly_Elf(
        string* file_name, 
        dis::Disassembly_Options *options);

 	/*
	Destroy a Disassembly.
	*/
 	virtual ~Disassembly_Elf(); 	   	
        
 protected:

    virtual int             Phase_1a_File();                       // Read a Windows PE file.

 private:

                            // functions for storing info in the extra treeview
    void                    Extra_Info(int section_type, ELF32_SHDR *shdr, char *file_data, Extra *extra);

    void                    Section_Data(int section_number,  
                                         ELF32_SHDR *section,
                                         int *result_type);

    void                    Section_Symbol(char *symbol_name, ELF32_SYM *sym, Extra *extra, bool import_function);


 	int						nSections;                             // number of sections in the executable.
    int                     Offset_to_first_instruction;           // first code instruction of the executable.
    int                     Load_address;                      // preferred load address
    int                     Imports_Offset;                        // offset to imports section
    int                     Imports_Offset_Raw;                    // raw offset to imports section
    int                     Imports_Size;                          // size of imports section
   
};

#endif
