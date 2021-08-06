/*
 * disassembly_node: one node contains all information relevant to one instruction
 *                   I guess, in theory, all these structs should be objects too;
 *                   but I feel for performance it's better to keep them as structs.
 *                   ( during analysis, we will be allocating and deleting a lot of them...)
 *          
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DISASSEMBLY_NODE_HH
#define DISASSEMBLY_NODE_HH

#define DEF_BYTES_PER_NODE 16                      // default size of Disassembly_Node 

const static char node_type [2] [5] = {"CODE", "DATA"};
#define NODE_TYPE_CODE    0
#define NODE_TYPE_DATA    1
#define NODE_TYPE_MAX     1

const static char node_status [3] [11] = {"UNEXPLORED", "AUTO", "USER"};
#define NODE_STATUS_UNEXPLORED 0                  // not analysed yet
#define NODE_STATUS_AUTO       1                  // analysed automatically
#define NODE_STATUS_USER       2                  // auto analysis overridden by user
#define NODE_STATUS_MAX        2                
                                                              
#define INSTRUCTION_CALL_NONE         0           // no call
#define INSTRUCTION_CALL_ROUTINE      1           // this node calls a routine
#define INSTRUCTION_CALL_JUMP         2           // this node jumps elsewhere
#define INSTRUCTION_CALL_JUMP_COND    3           // this node jumps elsewhere, conditionally
#define INSTRUCTION_CALL_CALLED       100         // this node is called from elsewhere

#define INSTRUCTION_DELIMITER_BEGIN   1           // this node begins a function
#define INSTRUCTION_DELIMITER_END     2           // this node ends   a function
                                              
#include "reference.hh"
       
namespace dis {
    struct Displacement;
    struct RegMemPart;
    struct Instruction;
    struct Disassembly_Node;
    struct Short_Cut;
    struct Call;
}                                         

struct dis::Call
{   
    short                           type_of_call;           // call to routine or itself called?
    char                           *name;                   // name of called routine

    Routine                        *routine;                // if routine is known
};

struct dis::Displacement
{   
    int                             mul;                    // multiplied to reg1
    int                             add;                    // added to reg1

    short                           reg2;                   // second register involved        
    int                             mul2;                   // multiplied to reg2
    int                             add2;                   // added to reg2

    int                             seg_override;           // overriding the default segment
    int                             seg_offset;             // providing an immediate offset for the segment

    int                             seg_reg;                // not really a displacement; rather, a segment 
                                                            //  register is involved
    int                             contr_reg;              // not really a displacement; rather, a control 
                                                            //  register is involved
    int                             debug_reg;              // not really a displacement; rather, a debug
                                                            //  register is involved
    int                             test_reg;               // not really a displacement; rather, a test
                                                            //  register is involved
};

struct dis::RegMemPart
{     
    short                           reg08;                  // if an 8 bit register is concerned
    short                           reg16;                  // if a 16 bit register is concerned
    short                           reg32;                  // if a 32 bit register is concerned
    short                           fp_reg;                 // if a floating point register is concerned
    bool                            abs;                    // absolute value / memory pointed to  
    bool                            used;                   // this rmp is actually used or not
    int                             imm;                    // immediate value                     

    Displacement                    *displ;
};                                       

struct dis::Instruction
{

    char*                           mnemonic;
    short                           operand_size;
    
    bool                            coprocessor;                  // does this instruction use a co-processor?     

    RegMemPart                      part1;
    RegMemPart                      part2;
    RegMemPart                      *part3;                       // only some instructions have 3 parts            

    Jump_Table                      jump_table;                                                                     

    short                           delimiter;                    // beginning / ending a function?  

    short                           n_pushed;                     // number of bytes pushed onto the stack
    Call                           *call;                         // call to routine or itself called?
};


struct dis::Disassembly_Node     
{

    /** offset of first byte in file    */
    long                            file_offset;                  

    /** offset of first byte in memory  */
    long                            memory_offset;                
                                          
    /** code, data, ...                 */
    short                           type; 

    /** unexplored, analysed, ...       */
    short                           status;                       

    /** actual code / data              */
    char                            *opcode;                      

    /** number of bytes used            */
    short                           n_used;                       
                                          
    /** disassembled instruction        */
    Instruction                     instruction;                  

    /** user supplied description       */
    char*                           comment;

    /** will be incorporated in instruction 
        ( relatively few instructions will have a label/comment, so there's
          no point in waisting a string on all of them)                  */
    char*                           label;                        

    /** section this node belongs to    */
    short                           section;                      

     /** referenced in a Short_Cut?      */
    bool                            bookmarked;                  

    std::list<dis::Reference>::iterator    
                                    /** this instruction is pointed to  */
                                    ref_in,

                                    /** this instruction points elsewhere */
                                    ref_out;                      
};

struct dis::Short_Cut                                             //  for faster navigation in the list of  
{                                                                 //   Disassembly_Nodes

    long                            file_offset;                  //  offset of first byte in file   
    long                            memory_offset;                //  offset of first byte in memory
    short                           n_used;                       //  number of bytes used
    uint                            row_number;                   //  order of node in list

    std::list<dis::Disassembly_Node>::iterator                    //  iterator to entry in  list of  
                                    it_s;                         //   Disassembly_Nodes
};


#endif
