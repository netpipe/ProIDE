/*
 * utility program that reads windows headers and creates a description of imported functions
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#define READ_IMPORT_STATE_INIT          0
#define READ_IMPORT_STATE_COMMENT       10
#define READ_IMPORT_STATE_EXTERN_C      20
#define READ_IMPORT_STATE_FUNCTION      30
#define READ_IMPORT_STATE_FUNCTION_CODE 35
#define READ_IMPORT_STATE_IGNORE        40
#define READ_IMPORT_STATE_INTERFACE     50
#define READ_IMPORT_STATE_TYPEDEF       60


#define READ_IMPORT_DIR                 "intel_winpe"

//#include <stdio.h>
#include <string.h>

#include <iostream>                 
#include <list>
#include <iterator>
#include <string>
#include <vector> 
#include <fstream>
#include <sstream>

#include <sys/types.h>
#include <sys/dir.h>      
                
                                                                               
#include "../libdis/function.hh"                 
#include "../libdis/reference.hh"                 
#include "../libutil/utilities.hh"                 
#include "../libmem/memory_pool.hh"                 
    
namespace prep {
    class   Read_Windows_Imports;
}    

class prep::Read_Windows_Imports
{
public:
    
    Read_Windows_Imports();

    virtual ~Read_Windows_Imports();    

    int     Init();                                             // open output files
    void    Load_Functions();                                   // find .h files
    void    Load_Idts();                                        // find .idt files
    void    Load_Functions_From_File(std::string file_name);    // find all functions in a .h file
    void    Load_Idts_From_File(std::string file_name);         // find all functions in a .dll file
    void    Write_Statistics();

private:

    std::vector <dis::Import>    v_import;                       // list of known functions for this platform

    std::list <string>           l_ftypes;                       // list of known function types

    std::vector <pair <string, vector<string> > >  v_p_typedef;  // list of known typedefs

    std::vector <pair <string, vector<string> > >  v_p_function_names;  
                                                                 // list of functions, per dll

    int                         count; 
    uint                        n_types, longest_type;

    struct direct              **files;    

    util::Utilities              u;                      // ... conversions

    mem::Memory_Pool             mp;                     // for allocating contiguous areas of non specific memory  

    std::ofstream                intel_winpe_file;

    std::ofstream                intel_winpe_stats;

    void                         Add_Import(std::vector<std::string> *import_strings);

    void                         Add_Typedef(std::vector<std::string> *t_strings);

    std::string                  Find_Dll_Name(std::string function_name);

    bool                         Compare_String_To_Function_Name(const char *function_name, const char *input_string, 
                                                                 int l1, int l2,
                                                                 bool inverted);
};

