/*
 * disassembly_winpe: disassembly of a windows Portable Executable
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DISASSEMBLY_WINPE_HH
#define DISASSEMBLY_WINPE_HH  

#include "disassembly_intel_winpe.inc"

#include "disassembly_intel.hh"

namespace dis {
	class Disassembly_WinPE;
}    


class dis::Disassembly_WinPE : public Disassembly_Intel
{
 public:
    /*
	Construct a new Disassembly.
	*/
 	Disassembly_WinPE(
        string* file_name, 
        dis::Main_Gui *mg,
        dis::Disassembly_Options *options);

    Disassembly_WinPE(
        string* file_name, 
        dis::Disassembly_Options *options);

 	/*
	Destroy a Disassembly.
	*/
 	virtual ~Disassembly_WinPE(); 	   	
        
 protected:

    virtual int             Phase_1a_File();                       // Read a Windows PE file.

    virtual string          Get_Function_Type(int i);              // convert int representation
                                                                   // of function result to string

    virtual int             Get_Function_Type(string *s);          // convert string representation
                                                                   // of function result to int 

    virtual dis::Routine*   Get_Routine_From_Name(const char *file_name, const char *routine_name);
                                                                   // find a routine by name

 private:
	
    void                    Section_Data(int section_number, int section_characteristics, int *section_type); 
    void                    Optional_Header_Data(IMAGE_NT_HEADERS *inh);

 	int						nSections;                             // number of sections in the executable.
    int                     Offset_to_first_instruction;           // first code instruction of the executable.
    int                     Load_address;                      // preferred load address
    int                     Imports_Offset;                        // offset to imports section
    int                     Imports_Offset_Raw;                    // raw offset to imports section
    int                     Imports_Size;                          // size of imports section
   
};

#endif
