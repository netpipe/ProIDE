/*
 * disassembly_intel: disassembling intel opcodes 
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DISASSEMBLY_INTEL_HH
#define DISASSEMBLY_INTEL_HH


#include "disassembly.hh"
#include "disassembly_intel_opcodes.inc"

namespace dis {
	class Disassembly_Intel;
}    

class dis::Disassembly_Intel : public Disassembly
{
 public:
    /*
	Construct a new Disassembly.
	*/
 	Disassembly_Intel();

    Disassembly_Intel (
        string* file_name, 
        dis::Main_Gui *mg,
        dis::Disassembly_Options *options);

    Disassembly_Intel (
        string* file_name, 
        dis::Disassembly_Options *options);

 	/*
	Destroy a Disassembly.
	*/
 	virtual ~Disassembly_Intel(); 	   	

    virtual void   Callback_Translate_Instruction(Instruction *instruction, string *str_instr);   
                                                      // from Instruction to assembler 
        
 protected:
    
    virtual int     Convert_Opcodes(std::vector<int> &v);
                                                      // convert  all opcodes to assembler

    virtual int     Phase_2a_Naive();                     // just begin from the start and see where this gets us

    virtual int     Phase_2b_Platform_Specific();         // try and exploit peculiarities of all executables
                                                      // on this platform

    virtual void    Translate_Mnemonic(Instruction *i, string *str_instr);   
                                                      // take in to account the mnemonic       

    virtual void    Translate_RegMemPart(RegMemPart *rmp,  int use_override, string *str_instr);
                                                      // from RegMemPart to assembler 

    int             Convert_Opcodes_From_Offset(list<Disassembly_Node>::iterator it, int n_opcodes); 
                                                      // convert opcodes at a particular offset to assembler

    void            Find_Stack_Frames();                  // intel stack frames indicate the start of code 

    int             Use_Override(Instruction *instruction, int part);    
                                                      // wether or not to override operand size  
        

 private:
    
    int Convert_Opcodes_0F(list<Disassembly_Node>::iterator it, 
                           int *ref, int *target, int position, int *n_bytes_pushed,
                           int bitn_a, int *bitn_o, int *type_call,
                           Instruction *instr); 
                                             // convert opcodes following a 0xFF byte

    int Convert_Opcodes_CoProcessor(list<Disassembly_Node>::iterator it, char init_byte,
                           int *ref, int *target, int position, int *n_bytes_pushed,
                           int bitn_a, int *bitn_o, Instruction *instr,
                           bool wait_flag); 
                                             // convert opcodes for a co-processor instruction


    int Convert_Address_16(int bitn,
                        list<Disassembly_Node>::iterator it,
                        int position, 
                        int use_reg_opc,
                        int *refer,
                        Instruction *instr);   // convert opcode at position [position]
                                               // of iterator it to 16 bit addressing mode
                                               // possibly noting the reference to refer

    int Convert_Address_32(int bitn,
                    list<Disassembly_Node>::iterator it,
                    int position, 
                    int use_reg_opc,
                    int *refer,
                    Instruction *instr);      // convert opcode at position [position]
                                              // of iterator it to 32 bit addressing mode
                                              // possibly noting the reference to refer,

    int Convert_SIB_Byte(list<Disassembly_Node>::iterator it,
                          int position,
                          RegMemPart *rmp);   // SIB byte contains extra info about addressing mode

    
};

#endif
