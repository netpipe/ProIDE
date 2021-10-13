/*
 * gui_file_type: for confirming the type of file to be analyzed
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               

#include "gui_file_type.hh"

dis::gui::dlg_file_type::dlg_file_type(int *init_id, dis::Disassembly_Options *options)
{  
   int       i;
   FILE_TYPE ft;

   std::list<Glib::ustring> listStrings;

   ////////////////////////////////////////////////////
   /// mostly copied from glade-2 /////////////////////
   ////////////////////////////////////////////////////

   disassembly_id = init_id;
   o = options;

   Gtk::Dialog *dlg_ft = this;
   //_data = new AccelData(get_accel_group());
   
   add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
   add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

   combo1 = Gtk::manage(new class Gtk::Combo());
   hseparator1 = Gtk::manage(new class Gtk::HSeparator());
   //label1 = Gtk::manage(new class Gtk::Label(""));
   //entry1 = Gtk::manage(new class Gtk::Entry());
   checkbutton1 = Gtk::manage(new class Gtk::CheckButton(""));
   checkbutton2 = Gtk::manage(new class Gtk::CheckButton(""));
   checkbutton3 = Gtk::manage(new class Gtk::CheckButton(""));
   table1 = Gtk::manage(new class Gtk::Table(2, 2, false));
   vbox1 = Gtk::manage(new class Gtk::VBox(false, 0));

   dlg_ft->get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);

   combo1->get_entry()->set_flags(Gtk::CAN_FOCUS);
   combo1->get_entry()->set_editable(false);
   combo1->get_entry()->set_max_length(0);
   combo1->set_case_sensitive(false);
  
   for (i = 0; i < MAX_FILE_TYPE; i++)
   {
      ft = file_type_list[i];                        
      listStrings.push_back((char *) &(ft.type_name));
   }
   combo1->set_popdown_strings(listStrings);
   combo1->get_entry()->set_text(Get_File_Type_Name(*disassembly_id));
   
   /*
   label1->set_alignment(0,0.5);
   label1->set_padding(0,0);
   label1->set_justify(Gtk::JUSTIFY_LEFT);
   label1->set_line_wrap(false);
   label1->set_use_markup(false);
   
   entry1->set_flags(Gtk::CAN_FOCUS);
   entry1->set_editable(true);
   entry1->set_max_length(0);
   //entry1->set_text("");
   */
   
   table1->set_row_spacings(0);
   table1->set_col_spacings(0);
   table1->attach(*checkbutton1, 0, 1, 0, 1, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
   table1->attach(*checkbutton2, 0, 1, 1, 2, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
   table1->attach(*checkbutton3, 0, 1, 2, 3, Gtk::FILL, Gtk::AttachOptions(), 0, 0);
   
   vbox1->pack_start(*combo1, Gtk::PACK_SHRINK, 0);
   vbox1->pack_start(*hseparator1);
   vbox1->pack_start(*table1, Gtk::PACK_EXPAND_WIDGET, 1);
   
   dlg_ft->get_vbox()->set_homogeneous(false);
   dlg_ft->get_vbox()->set_spacing(0);
   dlg_ft->get_vbox()->pack_start(*vbox1);
   dlg_ft->set_title("Please confirm type of disassembly");
   dlg_ft->set_modal(true);
   dlg_ft->property_window_position().set_value(Gtk::WIN_POS_CENTER_ON_PARENT);
   dlg_ft->set_resizable(true);                      

   Show_Relevant_Parts();           // depending on type of disassembly

   combo1->show();
   hseparator1->show();
   vbox1->show();
   dlg_ft->show();                                                                              
   
   // connect signals
   combo1->get_entry()->signal_changed().connect( sigc::mem_fun(*this, &dlg_file_type::Change_Type) );
   checkbutton1->signal_clicked().connect( sigc::mem_fun(*this, &dlg_file_type::Collect_Options) );
   checkbutton2->signal_clicked().connect( sigc::mem_fun(*this, &dlg_file_type::Collect_Options) );
   checkbutton3->signal_clicked().connect( sigc::mem_fun(*this, &dlg_file_type::Collect_Options) );
}

dis::gui::dlg_file_type::~dlg_file_type()
{  
    //delete _data;
}

void
dis::gui::dlg_file_type::Collect_Options()
{
 bool           b;         // temp value

 ///////////////////////////////////////////////////////

 switch (*disassembly_id)
  {
  case TYPE_OF_FILE_WINDOWS_PE:
      {
       b = checkbutton1->get_active();
       o->show_data_sections   = b;
       
       b = checkbutton2->get_active();
       o->show_import_sections = b;

       b = checkbutton3->get_active();
       o->collect_strings      = b;

       break;
      }

  case TYPE_OF_FILE_INTEL_ELF:
     {
      b = checkbutton1->get_active();
      o->show_data_sections   = b;

      b = checkbutton2->get_active();
      o->show_import_sections = b;

      b = checkbutton3->get_active();
      o->collect_strings      = b;

      break;
     }
  }   
}

void
dis::gui::dlg_file_type::Show_Relevant_Parts()
{
  switch (*disassembly_id)
  {
  case TYPE_OF_FILE_UNKNOWN:
  case TYPE_OF_FILE_INTEL_RAW:                                         
      {
       table1->hide();
       break;
      }

  case TYPE_OF_FILE_WINDOWS_PE:
      {
       checkbutton1->set_label("Show Data Sections");
       checkbutton1->set_active(true);
       checkbutton1->show();
       
       checkbutton2->set_label("Show Import Sections");
       checkbutton2->set_active(true);
       checkbutton2->show();

       checkbutton3->set_label("Collect String Bytes");
       checkbutton3->set_active(true);
       checkbutton3->show();

       table1->show();
       break;
      }

  case TYPE_OF_FILE_INTEL_ELF:
      {
       checkbutton1->set_label("Show Data Sections");
       checkbutton1->set_active(true);
       checkbutton1->show();

       checkbutton2->set_label("Show Import Sections");
       checkbutton2->set_active(true);
       checkbutton2->show();

       checkbutton3->set_label("Collect String Bytes");
       checkbutton3->set_active(true);
       checkbutton3->show();

       table1->show();
       break;
      }

    default: 
      {
       table1->hide();
       break;
      }    
  }   

  Collect_Options();
}

void
dis::gui::dlg_file_type::Change_Type()
{
 std::string    s;         // temp value

 ///////////////////////////////////////////////////////

 s = combo1->get_entry()->get_text();       // get name of selected file type

 *disassembly_id = Get_File_Type_Id(s);            // get its id

 Show_Relevant_Parts();
}

std::string 
dis::gui::dlg_file_type::Get_File_Type_Name(int cmp_id)
{
 int           i;           // index
 FILE_TYPE     ft;
 bool          stop;
 std::string   s;           // temp value

 /////////////////////////////////////////////////////////////////////
 // we want to find the type_name that goes with the input type_id ///
 /////////////////////////////////////////////////////////////////////
    
 i = 0;
 stop = false;

 while (!stop)
 {
  ft = file_type_list[i];

  if (ft.type_id == cmp_id)
  {
   stop = true;
   s = ft.type_name;
  }
  else
  {
   i++;

   if (i >= MAX_FILE_TYPE)
   { stop = true; }
  }
 }

 return s;
}

int
dis::gui::dlg_file_type::Get_File_Type_Id(std::string cmp_id)
{
 int           i;           // index
 FILE_TYPE     ft;
 bool          stop;
 int           t;           // temp value

 /////////////////////////////////////////////////////////////////////
 // we want to find the type_id that goes with the input type_name ///
 /////////////////////////////////////////////////////////////////////
 
    
 i = 0;
 t = TYPE_OF_FILE_UNKNOWN;
 stop = false;

 while (!stop)
 {
  ft = file_type_list[i];

  if (strcmp((char *) &(ft.type_name), cmp_id.c_str()) == 0)
  {
   stop = true;
   t = ft.type_id;
  }
  else
  {
   i++;

   if (i >= MAX_FILE_TYPE)
   { stop = true; }
  }
 }

 return t;
}

#endif
