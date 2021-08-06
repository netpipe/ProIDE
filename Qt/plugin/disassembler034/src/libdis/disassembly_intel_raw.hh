/*
 * disassembly_winpe: disassembly of a windows Portable Executable
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DISASSEMBLY_INTEL_RAW_HH
#define DISASSEMBLY_INTEL_RAW_HH

#include "disassembly_intel.hh"

namespace dis {
	class Disassembly_Intel_Raw;
}    


class dis::Disassembly_Intel_Raw : public Disassembly_Intel
{
 public:
    /*
	Construct a new Disassembly.
	*/
 	Disassembly_Intel_Raw(
        string* file_name, 
        dis::Main_Gui *mg,
        dis::Disassembly_Options *options);

    Disassembly_Intel_Raw(
        string* file_name, 
        dis::Disassembly_Options *options);

 	/*
	Destroy a Disassembly.
	*/
 	virtual ~Disassembly_Intel_Raw(); 	   	
        
 protected:

    virtual int    Phase_1a_File();                   //  Read a Raw Intel file.

 private:

    void   Fill_Disassembly_Node(int pif, int n_read);     //  Fill a DisassemblyNode with the bytes read
    
    char   raw_data[DEF_BYTES_PER_NODE];                   // buffer for the bytes
   
};

#endif
