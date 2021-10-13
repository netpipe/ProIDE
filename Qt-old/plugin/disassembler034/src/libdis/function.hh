/*
 * function.hh: organizes the information about all theoretically known functions 
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef FUNCTION_HH
#define FUNCTION_HH

namespace dis {
	struct Api;
    struct Routine;
    struct Parameter;
}                          

    

struct dis::Parameter
{   
  char                            *name;         // name of the parameter

  int                             type;          // type of parameter

  char                           *type_name;     // type of parameter

  Parameter                       *next;         // for list of Parameters
};

struct dis::Routine
{   
  char                           *name;          // name of the function

  Parameter                      *input;         // input parameters

  Parameter                      *output;        // output parameters

  Routine                        *next;   
};


struct dis::Api                                  // list of known functions
{   
  std::string                    file_name;     // file that contains the functions

  Routine                        *routine;  
};                                      


#endif
