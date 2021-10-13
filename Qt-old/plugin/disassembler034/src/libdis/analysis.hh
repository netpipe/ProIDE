/*
 * analysis.hh: final result of the analysis
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef ANALYSIS_HH
#define ANALYSIS_HH

#define ANALYSIS_STATUS_IDLE 0                       // analysis is not busy
#define ANALYSIS_STATUS_BUSY 1                       // analysis is ongoing

#ifdef HAVE_GTK                                                                             
#include "../gui_file_type.hh"       
#endif

#include "disassembly_intel_elf.hh"                                             
#include "disassembly_intel_winpe.hh"
#include "disassembly_intel_raw.hh"

namespace dis {
	class Analysis;  
}                                         
                              
class dis::Analysis      
{

 public:
 	Analysis(dis::Main_Gui *mg, dis::Disassembly_Options *d_o);   
                                                     // Construct a new Analysis for the given file 	
	
 	~Analysis(); 	   	                             // Destroy an Analysis      

    int  Perform();                                  // Perform the analysis

    //void Callback_Navigation();                    // launch a navigator dialog  
    void            Callback_Save(std::string *file_name, int type_of_save); 
                                                     // save an analysis
    int             Callback_Open();                 // reload an analysis

    Disassembly*    Callback_Get_Disassembly(); 

    Extra*          Callback_Get_Extra(); 

    int             Callback_Get_Row_From_Offset(int pos); 

    Disassembly_Node*
                    Callback_Get_nth_Row(uint n);

    bool            Callback_Is_Valid_Reference(std::list<dis::Reference>::iterator *it_ref);

    int             Callback_Navigation_Search_Byte(int row, std::vector<char> *search_string, 
                                                    bool direction, bool wrap);

    void            Callback_Translate_Opcodes(Disassembly_Node *dn, bool data, string *s, int max_bytes_per_row);
                                                      // show opcodes in hexadecimal and alpha

    void            Callback_Translate_Section(uint section, string *str_section);   
                                                     // describing the section


 protected:
   
    Disassembly           *disassembly;               // contains the disassembly, and shows its result

    int                   Determine_Type_of_Binary_File();  //so we know which kind of disassembly to start

    Disassembly_Options *options;                     // users's choices
 
    ///////////////////////////////// gui   //////////////////////////////////////////////

    dis::Main_Gui         *gui_mg;

private:

    int                   Init_Disassembly(int type_disassembly); 
                                                     // start the disassembly

    int                   Determine_Type_Disassembly(); 
                                                     // see if it is an existing disassembly

    int                   Determine_Type_WinPE();    // see if it is a WinPE file

    int                   Determine_Type_Elf();      // see if it is an ELF file

    int                   Determine_Type_Raw();      // ask if we should treat this a raw binary file

    int                   Confirm_Type_of_Binary_File();   // ask user for go ahead with analysis

    short			       status; 	                 // shows how complete the analysis is 
    string                *input_file;               // name of inputfile
    int                    tobf;                     // type of binary file  

#ifdef HAVE_GTK                               
    dis::gui::dlg_file_type
                          *dft;                   // for confirming the type of file and choosing options
#endif
};



#endif
