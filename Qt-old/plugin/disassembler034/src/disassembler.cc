
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace std;     

#include "libutil/command_line.hh"

#ifdef HAVE_GTK          
#include <libgnomeuimm.h>                     
#include "gui_gtk.hh"
#endif                               

#include "no_gui.hh"
#include "doc.hh"

int main(int argc, char **argv)
{  
   util::Option                 o;

   dis::Disassembly_Options     d_o;

   std::string                  requested_prefix,
                                str_gui, gui_gtk;

   std::vector <std::string>    v_s;    

   ///////////////////////////////////////
   // deal with command line arguments ///
   ///////////////////////////////////////

   // what options were specified?
   util::Command_Line  cl(argc, argv); 

   // which are legal?
   cl.Add_Legal_Option("-d", "type of disassembly, valid values are:\n \
                         'intel-winpe' \n \
                         'intel-elf' \n \
                         'intel-raw' \n");   

   cl.Add_Legal_Option("-f", "input filename");

#ifdef HAVE_GTK                               
   gui_gtk =  "                 'gtk'  for gnome gui (default) \n";
#else                
   std::cout << "Adding command line option '-g none'\n";
   o.prefix = "-g";
   o.value  = "none";
   cl.Add_Specified_Option(&o);     // as if no gui was requested
   
   gui_gtk = "";
#endif               

   str_gui = "type of gui, valid values are:\n \
                 'none' for no gui \n " + gui_gtk;
   cl.Add_Legal_Option("-g",str_gui);

   cl.Add_Legal_Option("-o", "output filename");

   cl.Add_Legal_Option("-s","type of output, valid values are:\n \
                    'db' for 'database' (default)    \n \
                    'list'  for 'listing'            \n");

   
   // when specifying 'no gui', an inputfile name should be added
   v_s.clear();
   v_s.push_back("-f");   
   cl.Add_Combination(COMMAND_LINE_COMBINATION_COMPULSORY_WITH_VALUE, "-g", "none", &v_s);

   if (cl.Match_Options() != RET_OK)
   {
    return RET_ERR_GENERAL;   
   }                 

   ///////////////////////////////////////////
   // adapt execution to specified options ///
   ///////////////////////////////////////////

   d_o.Initialize();

   //--------------------------------------
   //- type of disassembly ----------------

   requested_prefix = "-d";
   if (cl.Get_Specified_Option(&requested_prefix, &o))
   {
    if      (o.value == "intel-winpe")
    { d_o.type_of_disassembly = DISASSEMBLY_TYPE_INTEL_WINPE; }
    else if (o.value == "intel-elf")
    { d_o.type_of_disassembly = DISASSEMBLY_TYPE_INTEL_ELF; }
    else if (o.value == "intel-raw")
    { d_o.type_of_disassembly = DISASSEMBLY_TYPE_INTEL_RAW; }
   }

   //--------------------------------------
   //- input file name --------------------

   requested_prefix = "-f";
   if (cl.Get_Specified_Option(&requested_prefix, &o))
   { d_o.input_file_name = o.value; }

   //--------------------------------------
   //- output file name -------------------

   requested_prefix = "-o";
   if (cl.Get_Specified_Option(&requested_prefix, &o))
   { d_o.output_file_name = o.value; }

   //--------------------------------------
   //- type of output ---------------------

   d_o.type_of_save = DISASSEMBLY_SAVE_DATABASE; 

   requested_prefix = "-s";                      
   if (cl.Get_Specified_Option(&requested_prefix, &o))
   {
    if      (o.value == "db")
    { d_o.type_of_save = DISASSEMBLY_SAVE_DATABASE; }
    else if (o.value == "list")
    { d_o.type_of_save = DISASSEMBLY_SAVE_LISTING; }
   }                        

   //--------------------------------------
   //- type of gui ------------------------

#ifdef HAVE_GTK                               
   d_o.type_of_gui = DISASSEMBLY_GUI_GTK; 
#else                               
   d_o.type_of_gui = DISASSEMBLY_GUI_NONE; 
#endif               

   requested_prefix = "-g";
   if (cl.Get_Specified_Option(&requested_prefix, &o))
   {
#ifdef HAVE_GTK                               
    if      (o.value == "gtk")
    { d_o.type_of_gui = DISASSEMBLY_GUI_GTK; }
    else
#endif               
     if (o.value == "none")
    { d_o.type_of_gui = DISASSEMBLY_GUI_NONE; }
   }                        

   //--------------------------------------


   ///////////////////////////////////////
   // start program                    ///
   ///////////////////////////////////////

   // gtk is default gui
#ifdef HAVE_GTK                               
   if (d_o.type_of_gui == DISASSEMBLY_GUI_GTK)
   {
    Gnome::Main m(PACKAGE, VERSION, Gnome::UI::module_info_get(), argc, argv);
    dis::gui::gui_gtk mw(&m, &d_o);
    m.run(mw);                        
   }
   
   else
#endif               

        // no gui
        if (d_o.type_of_gui == DISASSEMBLY_GUI_NONE)
   {
    dis::gui::no_gui ng(&d_o);
    ng.Run();
   }


   return 0;
}
