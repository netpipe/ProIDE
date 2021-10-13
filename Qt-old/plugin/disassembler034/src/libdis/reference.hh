/*
 * reference.hh: organizes the information about references to an instruction / location
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef REFERENCE_HH
#define REFERENCE_HH


const static char reference_type [5] [11] = {"UNEXPLORED", "IMPORT", "FUNCTION", "CODE", "VARIABLE"};
#define REFERENCE_TYPE_UNEXPLORED   0           // not analysed yet
#define REFERENCE_TYPE_IMPORT       1           // imported function
#define REFERENCE_TYPE_FUNCTION     2           // function
#define REFERENCE_TYPE_CODE         3           // ordinary jump to code
#define REFERENCE_TYPE_VARIABLE     4           // variable
#define REFERENCE_TYPE_MAX          4           // ordinary jump to code


namespace dis {
	struct Reference;
    struct Disassembly_Node;     
    struct Import;     
    struct Function;   
    struct Variable;     
}                          



struct dis::Reference     
{   
  ///////////////////////////////////// in  //////////////////////////////////////////////////

  long                            memory_offset; // location of the reference pointed to

  short                           type;  

  std::string                     label;         // description              

  std::list<dis::Disassembly_Node>::iterator 
                                  it_dn;         // the complete info about the memory referenced

  std::list<dis::Import>::iterator   
                                  it_i;          // if referring to an imported function

  std::list<dis::Function>::iterator   
                                  it_f;          // if referring to an internal function

  std::list<dis::Variable>::iterator   
                                  it_v;          // if referring to a variable



  ///////////////////////////////////// out //////////////////////////////////////////////////

  std::vector <long>              ref_out;       //  elements in the disassembly list that refer to the
                                                 //  ref_in    (= the referencing offsets) 
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///// the following structs organize information that was actually found in the executable, //////
///// whereas the structs in function.hh contain info about theoretical funtiions, not      //////
///// necessarily actually present in the disassembled file                                 //////
//////////////////////////////////////////////////////////////////////////////////////////////////

struct dis::Import    
{   
  long                            memory_offset; // location in the exe of the import pointed to

  std::string                     name;          // name of the function
  std::string                     library;       // name of the library

  Routine                        *routine;       // details about parameters
};

struct dis::Function
{   
  long                            memory_offset; // location in the exe of the function pointed to

  std::string                     name;          // name of the function

  Routine                        *routine;       // details about parameters
};


struct dis::Variable
{   
  long                            memory_offset; // location in the exe of the data pointed to

  std::string                     name;          // name of the variable
};


#endif
