/*
 * disassembly_options: collecting user's choices 
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#include <iostream>

#include "disassembly_options.hh"

 
dis::Disassembly_Options::Disassembly_Options()
{
    std::cout << "Constructor Disassembly_Options()" << "\n";
}

dis::Disassembly_Options::~Disassembly_Options()
{
    std::cout << "Destructor Disassembly_Options" << "\n";
}

void
dis::Disassembly_Options::Initialize()
{
   type_of_disassembly  = DISASSEMBLY_TYPE_GENERIC;
   type_of_save         = DISASSEMBLY_SAVE_DATABASE;
   type_of_gui          = DISASSEMBLY_GUI_GTK;

   show_data_sections = show_import_sections = collect_strings = true;

   input_file_name.clear();
   output_file_name.clear();
}

