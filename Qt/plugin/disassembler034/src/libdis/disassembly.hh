/*
 * disassembly: disassembly of a binary file
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DISASSEMBLY_HH
#define DISASSEMBLY_HH

#define DISASSEMBLY_PHASE_NONE  0
#define DISASSEMBLY_PHASE_1A    100
#define DISASSEMBLY_PHASE_1B    110
#define DISASSEMBLY_PHASE_2A    200
#define DISASSEMBLY_PHASE_2B    210
#define DISASSEMBLY_PHASE_3A    300
#define DISASSEMBLY_PHASE_3B    310
#define DISASSEMBLY_PHASE_3C    320
#define DISASSEMBLY_PHASE_3D    330
#define DISASSEMBLY_PHASE_3E    340
#define DISASSEMBLY_PHASE_3F    350

                       
#define DISASSEMBLY_BITNESS_08 1
#define DISASSEMBLY_BITNESS_16 2                                
#define DISASSEMBLY_BITNESS_32 4
#define DISASSEMBLY_BITNESS_64 8
#define DISASSEMBLY_BITNESS_80 10

#define DISASSEMBLY_MAX_INSTRUCTION_LENGTH          15
#define DISASSEMBLY_MAX_MNEMONIC_LENGTH             8
#define DISASSEMBLY_MAX_DIFF_JUMP_TABLE       (int) 0xFFFF

#define DISASSEMBLY_COND_JUMP_TABLE 1

#define DISASSEMBLY_MODIFIED_COUNTER_MAX 1000
#define DISASSEMBLY_SHORT_CUT_MAX        750            // max number of Short_Cuts in l_sc
#define DISASSEMBLY_SHORT_CUT_MIN_SIZE   25             // min number of nodes indexed by 1 Short_Cut

#define DISASSEMBLY_SEPARATOR_RESULT                "disassembly result from 'disassembler for linux'"
#define DISASSEMBLY_SEPARATOR_FILE                  "?file?"
#define DISASSEMBLY_SEPARATOR_GLOBAL                "/*/*/*/*/*/*/*/*/*/*/*/*"
#define DISASSEMBLY_SEPARATOR_CATEGORY              "/*cat*/"
#define DISASSEMBLY_SEPARATOR_DISASSEMBLY           "/*disassembly*/"
#define DISASSEMBLY_SEPARATOR_STATISTICS            "/*statistics*/"
#define DISASSEMBLY_SEPARATOR_ITEM                  "\t"

#define DISASSEMBLY_SEPARATOR_FILESIZE              "?fsz?"                                                            
#define DISASSEMBLY_SEPARATOR_LDN                   "?ldn?"
#define DISASSEMBLY_SEPARATOR_VAPI                  "?vapi?"
#define DISASSEMBLY_SEPARATOR_LR                    "?lr?"
#define DISASSEMBLY_SEPARATOR_LI                    "?li?"
#define DISASSEMBLY_SEPARATOR_LV                    "?lv?"
#define DISASSEMBLY_SEPARATOR_END                   "?end?"
#define DISASSEMBLY_SEPARATOR_API                   "?api?"
#define DISASSEMBLY_SEPARATOR_API_ROUTINE           "?apr?"
#define DISASSEMBLY_SEPARATOR_CALL                  "?cal?"
#define DISASSEMBLY_SEPARATOR_CALL_NAME             "?can?"
#define DISASSEMBLY_SEPARATOR_COMMENT               "?com?"
#define DISASSEMBLY_SEPARATOR_DISPLACEMENT          "?dis?"
#define DISASSEMBLY_SEPARATOR_IMPORT                "?imp?"
#define DISASSEMBLY_SEPARATOR_IMPORT_NAME           "?imn?"
#define DISASSEMBLY_SEPARATOR_IMPORT_ROUTINE        "?imr"
#define DISASSEMBLY_SEPARATOR_INSTRUCTION           "?ins?"
#define DISASSEMBLY_SEPARATOR_INPUT                 "?inp?"
#define DISASSEMBLY_SEPARATOR_JUMPTABLE             "?jum?"
#define DISASSEMBLY_SEPARATOR_LABEL                 "?lab?"
#define DISASSEMBLY_SEPARATOR_LIB_NAME              "?lna?"
#define DISASSEMBLY_SEPARATOR_MNEMONIC              "?mne?"
#define DISASSEMBLY_SEPARATOR_NODE                  "?nod?"
#define DISASSEMBLY_SEPARATOR_OUTPUT                "?out?"
#define DISASSEMBLY_SEPARATOR_PARAMETER             "?par?"
#define DISASSEMBLY_SEPARATOR_PARAMETER_NAME        "?pna?"
#define DISASSEMBLY_SEPARATOR_PARAMETER_TYPE_NAME   "?ptn?"
#define DISASSEMBLY_SEPARATOR_REFERENCE_DN_IN       "?rdi?"
#define DISASSEMBLY_SEPARATOR_REFERENCE_IT_DN       "?rdn?"
#define DISASSEMBLY_SEPARATOR_REFERENCE_DN_OUT      "?rdo?"
#define DISASSEMBLY_SEPARATOR_REFERENCE_IT_IMPORT   "?rim?"
#define DISASSEMBLY_SEPARATOR_REFERENCE_IT_OUT      "?rio?"
#define DISASSEMBLY_SEPARATOR_REFERENCE_IT_VARIABLE "?riv?"
#define DISASSEMBLY_SEPARATOR_REFERENCE_REF         "?ref?"
#define DISASSEMBLY_SEPARATOR_REFERENCE_STRING      "?res?"
#define DISASSEMBLY_SEPARATOR_REGMEMPART            "?rmp?"
#define DISASSEMBLY_SEPARATOR_ROUTINE               "?rou?"
#define DISASSEMBLY_SEPARATOR_ROUTINE_NAME          "?ron?"
#define DISASSEMBLY_SEPARATOR_TO_EXPLORE            "?tex?"
#define DISASSEMBLY_SEPARATOR_TO_EXPLORE_UNCERTAIN  "?teu?"
#define DISASSEMBLY_SEPARATOR_VARIABLE              "?var?"
#define DISASSEMBLY_SEPARATOR_VARIABLE_NAME         "?van?"

#define DISASSEMBLY_STATE_API                      10
#define DISASSEMBLY_STATE_API_ROUTINE              20
#define DISASSEMBLY_STATE_CALL                     30
#define DISASSEMBLY_STATE_CALL_NAME                40
#define DISASSEMBLY_STATE_COMMENT                  50
#define DISASSEMBLY_STATE_DISASSEMBLY              60
#define DISASSEMBLY_STATE_DISPLACEMENT             70
#define DISASSEMBLY_STATE_EXPLORE                  80
#define DISASSEMBLY_STATE_EXPLORE_UNCERTAIN        90
#define DISASSEMBLY_STATE_FILE                    100
#define DISASSEMBLY_STATE_FILESIZE                110
#define DISASSEMBLY_STATE_IMPORT                  120
#define DISASSEMBLY_STATE_IMPORT_NAME             130
#define DISASSEMBLY_STATE_IMPORT_ROUTINE          140
#define DISASSEMBLY_STATE_INSTRUCTION             150
#define DISASSEMBLY_STATE_INPUT                   160
#define DISASSEMBLY_STATE_JUMPTABLE               170
#define DISASSEMBLY_STATE_LABEL                   180
#define DISASSEMBLY_STATE_LDN                     190
#define DISASSEMBLY_STATE_LIB_NAME                200
#define DISASSEMBLY_STATE_LR                      210
#define DISASSEMBLY_STATE_LI                      220
#define DISASSEMBLY_STATE_LV                      225
#define DISASSEMBLY_STATE_MNEMONIC                230
#define DISASSEMBLY_STATE_NODE                    240
#define DISASSEMBLY_STATE_OUTPUT                  250
#define DISASSEMBLY_STATE_PARAMETER               260
#define DISASSEMBLY_STATE_PARAMETER_NAME          270
#define DISASSEMBLY_STATE_PARAMETER_TYPE_NAME     280
#define DISASSEMBLY_STATE_REFERENCE_DN_IN         290
#define DISASSEMBLY_STATE_REFERENCE_DN_OUT        300
#define DISASSEMBLY_STATE_REFERENCE_IT_DN         310 
#define DISASSEMBLY_STATE_REFERENCE_IT_IMPORT     320
#define DISASSEMBLY_STATE_REFERENCE_IT_OUT        330
#define DISASSEMBLY_STATE_REFERENCE_IT_VARIABLE   335
#define DISASSEMBLY_STATE_REFERENCE_REF           340
#define DISASSEMBLY_STATE_REFERENCE_STRING        350
#define DISASSEMBLY_STATE_REGMEMPART              360
#define DISASSEMBLY_STATE_ROUTINE                 370
#define DISASSEMBLY_STATE_ROUTINE_NAME            380
#define DISASSEMBLY_STATE_ROUTINE_KNOWN           390
#define DISASSEMBLY_STATE_STATISTICS              395
#define DISASSEMBLY_STATE_VAPI                    400
#define DISASSEMBLY_STATE_VARIABLE                450
#define DISASSEMBLY_STATE_VARIABLE_NAME           455

#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include <pthread.h>

#ifdef HAVE_GTK  
#include <gtkmm/main.h>                                       
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/statusbar.h>
#endif
  
#include "../return_codes.hh"
#include "file_formats.hh"
#include "../gui_commands.hh"

#include "extra.hh"
#include "function.hh"
#include "jump_table.hh"
#include "section.hh"
#include "disassembly_node.hh"
#include "disassembly_options.hh"    
#include "../libmem/memory_pool.hh"                 
#include "../libutil/utilities.hh"   
#include "../liblog/logger.hh"   
#include "../gui_navigator.hh"   
                            
                            
extern "C" void *Start_Phase_2(void *);

#define FUNCTION_TYPE_UNKNOWN          0     
#define FUNCTION_TYPE_MAX              36     
#define FUNCTION_TYPE_MAX_SIZE         10     
const static char function_type [FUNCTION_TYPE_MAX] [FUNCTION_TYPE_MAX_SIZE] =
{
    "bool", "BOOL", "byte", "BYTE", "dword", "DWORD", "dword_ptr", "DWORD_PTR",
    "int", "INT", 
    "long", "LONG",
    "long_ptr", "LONG_PTR", 
    "lpbyte", "LPBYTE", "lpdword", "LPDWORD", "lpvoid", "LPVOID", "lpword", "LPWORD", 
    "pbyte", "PBYTE", 
    "ptr", "PTR",
    "short",
    "size_t",
    "uint", "UINT", "uint_ptr", "UINT_PTR",
    "puint", "PUINT", 
    "void", "VOID"
};


namespace dis {
	class  Disassembly;
    struct Next_String;
    struct Main_Gui;
    struct Statistics;
} 


struct dis::Main_Gui  
{   
    unsigned int                         sb_context_id;        

#ifdef HAVE_GTK  
    Gtk::Statusbar                      *sb;
    Gtk::Main                           *m; 
#endif

};  

struct dis::Statistics
{                
    int                             max_row_offset;
    int                             n_rows;
};


struct dis::Next_String     
{

    int                             offset;            //  how many chars away is the next string
    int                             length;            //  how long is the next string
    int                             length_following;  //  how many non-printables follow the string
    int                             offset_next;       //  offset of the next string 
};  


class dis::Disassembly      
{
                                           
 public:
    /*
	Construct a new Analysis.
	*/
 	Disassembly();

    Disassembly(string* file_name, 
                dis::Main_Gui *mg,
                dis::Disassembly_Options *options);

    Disassembly(string* file_name, 
                dis::Disassembly_Options *options);

 	/*
	Destroy a Disassembly.
	*/
    virtual ~Disassembly();    	

     int            Perform();                        // Perform the disassembly. 

     void           Phases_In_Thread();               // disassemble the opcodes in a separate thread

     /*
     virtual void   Callback_Gui_Double_Clicked();    // allow double click navigating in the gui

     */
       
     void           Callback_Save_Database(std::string *file_name);      
                                                      // save a disassembly
     void           Callback_Save_Listing(std::string *file_name);      
                                                      // save a disassembly

     int            Callback_Open();                  // reload a disassembly

     virtual void   Callback_Get_Statistics(Statistics* s);

     Extra*         Callback_Get_Extra();

     Disassembly_Node*
                    Callback_Get_nth_Row(uint n);

     int            Callback_Get_Row_From_Offset(int pos); 

     bool           Callback_Is_Valid_Reference(std::list<dis::Reference>::iterator *it_ref);

     int            Callback_Navigation_Search_Byte(int row, std::vector<char> *search_string,  
                                                    bool direction, bool wrap); 
                                                       // find the next node with search_string

     virtual void   Callback_Translate_Opcodes(Disassembly_Node *dn, bool data, string *s, int max_bytes_per_row);
                                                       // show opcodes in hexadecimal and alpha


     virtual void   Callback_Translate_Instruction(Instruction *instruction, string *str_instr);   
                                                      // from Instruction to assembler 

     virtual void   Callback_Translate_Section(uint section, string *str_section);   
                                                      // describing the section


     Disassembly_Node*
                    Callback_Get_Next_Disassembly_Node_From_Offset(int pos); 

     void           Callback_Navigation_Do();


 protected:

     void           Phase_0_Init();                   // when beginning a new disassembly

     virtual int    Phase_1a_File();                  // Read a binary file.

     virtual int    Phase_1b_Imports();               // Read known imports.

     virtual int    Phase_2a_Naive();                 // just begin from the start and see where this gets us

     virtual int    Phase_2b_Platform_Specific();     // try and exploit peculiarities of all executables on this platform

     virtual int    Phase_3a_Review_Data();           // try and put meaningfull data together
     
     virtual int    Phase_3b_Review_References();     // try and delete meaningless references

     virtual int    Phase_3c_Review_Imports();        // try and name imported routines

     virtual int    Phase_3d_Review_Functions();      // try and name functions

     virtual int    Phase_3e_Review_Variables();      // try and recognize variables

     virtual int    Phase_3f_Review_Parameters();     // try and recognize parameters to functions  

     void           Add_Extra_Info_Level_2(char *title1, char *extra_info, Extra *extra);
     void           Add_Extra_Info_Level_3(char *title1, char *title2, char *extra_info, Extra *extra);
                                                      // add info to the 'This File' notebook pane

     virtual int    Convert_Opcodes(std::vector<int> &v);
                                                      // convert opcodes to assembler

     void           Convert_Jump_Tables();            // convert what we think are jump tables

     int            Undo_From_Offset(int o, int max, int condition); 
                                                      // reinitialize all nodes from the given offset o
                                                      //  up to offset max, until condition is met  

     //int            Perform_Background();             // put all real disassembly in a background thread

     bool           Isolate_l_dn(list<Disassembly_Node>::iterator it, int pos, int length, int status); 
                                                      // for isolating a Disassembler_Node of desired length

     void           Split_l_dn(list<Disassembly_Node>::iterator it, int diff, bool keep_status); 
                                                      // for splitting up a Disassembler_Node

     int            Get_Byte_From_Disassembly_Node(list<Disassembly_Node>::iterator it, short pos, char *byte); 
                                                      // add 1 byte from the next Disassembly_Node

     list<Disassembly_Node>::iterator
                    Get_Disassembly_Node_From_Offset(int pos, bool exact_match); 
                                                      // find a Disassembly_Node from an offset

     Section*       Get_Section_From_Offset(int pos); // find a Section from an offset

     list<Disassembly_Node>::iterator
                    Get_Disassembly_Node_From_Row(uint pos); 

     list<Reference>::iterator  
                    Get_Reference_From_Offset(int pos, bool exact_match); 

     list<Import>::iterator
                    Get_Import_From_Name(const char *function_name, const char *file_name); // find an import by name

     list<Variable>::iterator
                    Get_Variable_From_Name(const char *variable_name); // find a variable by name

     virtual Routine*
                    Get_Routine_From_Name(const char *file_name, const char *routine_name); // find a routine by name

     dis::Next_String   
                    Get_Next_Printable_String(list<Disassembly_Node>::iterator it, int start_from); 

     void           Initialize();                                   // Disassembly = 0

     void           Initialize(RegMemPart *r);                      // RegMemPart r = 0
     
     void           Initialize(Call *c);                            // Call c = 0;
          
     void           Initialize(Instruction *i);                     // Instruction i = 0

     void           Initialize(Displacement *d);                    // Displacement d = 0

     void           Initialize(Reference *r);                       // Reference r = 0

     void           Initialize(Import *i);                          // Import i = 0

     void           Initialize(Function *f);                        // Function f = 0

     void           Initialize(Variable *v);                        // Variable v = 0

     void           Initialize(Disassembly_Node *d);                // Disassembly_Node d = 0

     void           Initialize(Api *p);                             // Api a = 0 

     void           Initialize(Routine *r);                         // Routine r = 0 

     void           Initialize (Short_Cut *s);                      // Short_Cut s = 0;    

     void           Initialize(Parameter *p);                       // Parameter p = 0 

     void           Initialize(Extra *e);                           // Extra e = 0 

     virtual int    Get_Function_Type(string *s);      // convert string representation
                                                       // of function result to int 

     virtual string Get_Function_Type(int i);          // convert int representation
                                                       // of function result to string

     virtual void   Translate_Mnemonic(Instruction *i, string *str_instr);     // take in to account the mnemonic

     virtual void   Translate_RegMemPart(RegMemPart *rmp,  int use_override, string *str_instr);
                                                                               // from RegMemPart to assembler 

     void           Add_Reference(list<Disassembly_Node>::iterator it, int ref, bool certain); 
                                                       // add a reference from node 'it' to a memory location ref, 

     void           Add_Routine(Api *a, Routine *r);   // add a routine to the list in v_api

     void           Add_Address_To_Explore(int adr);   // add an entry to the list of addresses to explore

     void           Add_Uncertain_Address_To_Explore(int unc);  
                                                       // add an entry to the list of uncertain addresses

     void           Add_Code_To_Explore(list<Disassembly_Node>::iterator it, int adr);    
                                                       // when target is code

     void           Update_Reference_For_Import(list<Import>::iterator it, int ref); 
                                                        // add an Import to an existing reference 

     void           Update_Reference_For_Variable(list<Variable>::iterator it, int ref); 
                                                        // add a Variable to an existing reference 

     void           Update_Reference_For_Function(list<Function>::iterator it, int ref); 
                                                        // add a Function to an existing reference 

     void           Update_Disassembly_Node_For_Function(list<Function>::iterator it, 
                                                       list<Disassembly_Node>::iterator it_n);
                                                                         // update the node with Function info

     void           Update_Disassembly_Node_For_Import(list<Import>::iterator it, 
                                                       list<Disassembly_Node>::iterator it_n,
                                                       bool force_call); // update the node with Import info

     void           Update_Ldn_For_Delete (Disassembly_Node *d);         // clean up references to d

     void           Update_Short_Cut_List(bool force_update);            // keep track of changes to l_dn

     /*
     void           Navigation_Direct_Offset(Gtk::TreeModel::iterator iter, int direct_offset);

     void           Navigation_Search_Byte(Gtk::TreeModel::iterator iter_init, string * search_byte,
                                           bool direction, bool wrap);
     */

     vector <Api>                 v_api;               // list of known functions for this platform

     list <Disassembly_Node>      l_dn;                // Each entry will represent one instruction.

     list <Short_Cut>             l_sc;                // For faster navigation in l_dn.
                                                                                                   
     list <Reference>             l_r;                 // Each entry will represent one memory location pointed to

     list <Import>                l_i;                 // Each entry will represent one imported function

     list <Function>              l_f;                 // Each entry will represent one function

     list <Variable>              l_v;                 // Each entry will represent one detected variable

     vector <Section>             v_s;                 // Each entry will represent one section

     string                       known_imports_file_name; // file with known imports for this type of executable  

     mem::Memory_Pool             mp,                  // for allocating contiguous areas of non specific memory  
                                  opcodes_mp;          // for allocating contiguous areas of opcodes memory

     vector <int>                 v_to_explore;        // addresses yet to explore 

     vector <int>                 v_to_explore_uncertain; // uncertain addresses yet to explore 

     vector <Jump_Table_Request>  v_jump_table;        // possible jump tables
     
     string                       input_file;

     int                          input_file_size;  

     pthread_t                    thr_dis;             // actual disassembly will happen in a thread, not to disturb the GUI

     short			              mbpi;                // max_bytes_per_instruction; 

     short                        operand_bitness,     // the bitness of the code
                                  address_bitness;     // the bitness of the addresses

     list<Disassembly_Node>::iterator       bookmark_ldn;   // for faster navigation in l_dn   

     list<Short_Cut>::iterator              bookmark_lsc;   // for faster navigation in l_sc

     list<Reference>::iterator              bookmark_lr;    // for faster navigation in l_r

     int                                    range_offset_low, range_offset_high;
                                                          // indicate lowest and highest offset in the binary

     util::Utilities                        u;            // ... conversions

     dis::Disassembly_Options              *opt;          // users's choices

     dis::Extra                             e;            // extra info

     int                                    disassembly_type;

     ///////////////////////////////// gui  general ///////////////////////////////////////////

     int                                    type_of_gui;

     ///////////////////////////////// gui  gtk 2 /////////////////////////////////////////////


     dis::Main_Gui                         *gui_mg;       // gui interface 

     int                                    gui_counter;          // for updating the screen
     bool                                   gui_init;             // has gui been initialised?
     bool                                   gui_navigation_requested;
     
     void           Gui_Command(int gui_command, char *gui_text);  
     void           Gui_Command(int gui_command);  

     ///////////////////////////////// debug //////////////////////////////////////////////

     void           Debug_Show_Ldn(int debug_ldn);               // show the content of l_dn
     
     void           Debug_Show_To_Explore(int debug_status);     // show the addresses yet to explore
     void           Debug_Show_Lr(int debug_status);             // show the content of l_r
     void           Debug_Show_Li(int debug_status);             // show the content of l_i
     void           Debug_Show_Vapi(int debug_status);           // show the content of v_api

     void           Debug_Show_It(int debug_command, list<Disassembly_Node>::iterator debug_it);  // Show one it             

     int            debug_counter;

#ifdef LOGGING
     util::Logger   debug_log;
#endif 

 private:

     void           Callback_Save_Routine(ofstream& of, Routine *routine);

     int            completed_phase;                              // which phases have been completed?

     bool           phases_completed;

     int            l_dn_modified_counter;                        // for updating Short_Cut_List l_sh
};



#endif
