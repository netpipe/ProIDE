/*
 * disassembly_options: collecting user's choices 
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DISASSEMBLY_OPTIONS_HH
#define DISASSEMBLY_OPTIONS_HH

#define DISASSEMBLY_GUI_NONE            0
#define DISASSEMBLY_GUI_GTK             100

#define DISASSEMBLY_TYPE_GENERIC        0
#define DISASSEMBLY_TYPE_INTEL          100
#define DISASSEMBLY_TYPE_INTEL_ELF      101
#define DISASSEMBLY_TYPE_INTEL_WINPE    102
#define DISASSEMBLY_TYPE_INTEL_RAW      103

#define DISASSEMBLY_SAVE_DATABASE           0
#define DISASSEMBLY_SAVE_LISTING            1

#define GUI_FILE_SAVER_ASM      ".asm"
#define GUI_FILE_SAVER_ASM_TXT  "assembler source"
#define GUI_FILE_SAVER_DIS      ".dis"
#define GUI_FILE_SAVER_DIS_TXT  "disassembly database"
#define GUI_FILE_SAVER_EXE      ".exe"
#define GUI_FILE_SAVER_EXE_TXT  "executable"
#define GUI_FILE_SAVER_LST      ".lst"
#define GUI_FILE_SAVER_LST_TXT  "opcodes listing"
                          

namespace dis {
	class Disassembly_Options;
}    

class dis::Disassembly_Options
{
 public:
 	Disassembly_Options();

 	virtual ~Disassembly_Options();  

    void     Initialize();   

    int             type_of_disassembly,
                    type_of_gui,
                    type_of_save;

    bool            show_data_sections,
                    show_import_sections,
                    collect_strings;

    std::string      input_file_name,
                     output_file_name;

};

#endif
