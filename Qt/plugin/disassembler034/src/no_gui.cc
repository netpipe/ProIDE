/*
 *       no_gui.cc : for disassembling without a gui
 *
 +       author: Danny Van Elsen
 */

#include <no_gui.hh>
                    
dis::gui::no_gui::no_gui(Disassembly_Options *d_o)
{  
   std::cout << "Constructor no_gui" << "\n";
   
   /////////////////////////////////////////////////////////////////

   analysis = 0;

   options = d_o;
}


dis::gui::no_gui::~no_gui() 
{
   std::cout << "Destructor no_gui" << "\n";

   if (analysis)
   {
    delete (analysis);   
   }
   
}

void
dis::gui::no_gui::Run()
{
   std::cout << "Opening file " << options->input_file_name << " and commencing analysis..." << "\n";

   if (options->output_file_name == "")
   {
    if (options->type_of_save == DISASSEMBLY_SAVE_DATABASE)
    { options->output_file_name = options->input_file_name + GUI_FILE_SAVER_DIS; }
    else if (options->type_of_save == DISASSEMBLY_SAVE_LISTING)
    { options->output_file_name = options->input_file_name + GUI_FILE_SAVER_LST; }
   }                                                                              

   existing_disassembly = Determine_Existing_Disassembly(options->input_file_name);       

   Start_Analysis();
}

bool
dis::gui::no_gui::Determine_Existing_Disassembly(string filename)
{
  bool      result;

  //////////////////////////////

  result = false;

  if (options->input_file_name.find(GUI_FILE_SAVER_DIS) != string::npos)
  {
   result = true;
  }

  return result;
}

void 
dis::gui::no_gui::Start_Analysis()
{
  if (analysis) { delete (analysis); }

  analysis = new Analysis (0, options);     

  if (existing_disassembly != true)
  {
   Start_New_Analysis();       
  }
  else 
  {
   Start_Open();
  }   
}     

void 
dis::gui::no_gui::Start_New_Analysis()
{   
    int ret_code;

    ////////////////////////////////////////
   
    if (analysis) 
    { ret_code = analysis->Perform(); }

    if (analysis) 
    { analysis->Callback_Save((&(options->output_file_name)), options->type_of_save); }
}

void 
dis::gui::no_gui::Start_Open()
{   
    ////////////////////////////////////////

    if (analysis) 
    { analysis->Callback_Open(); }

    if (analysis) 
    { analysis->Callback_Save((&(options->output_file_name)), options->type_of_save); }
}




