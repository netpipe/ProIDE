/*
 * gui_navigator: allowing the user to jump to and fro in the disassembly
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               
 
#include "gui_navigator.hh"

dis::gui::gui_navigator::gui_navigator()
{
   int       i;

   NAVIGATION_TYPE nt;

   std::list<Glib::ustring> listStrings;

   ////////////////////////////////////////////////////

   // init values
   navigation_last_direct_offset = navigation_last_search_byte = " ";

   navigation_hexadecimal = false;
   navigation_wrap = false;
   navigation_direction = true;

   direct_offset_list.push_front(" ");
   search_byte_list.push_front(" ");

   navigation_id = NAVIGATION_TYPE_DIRECT_OFFSET;

   // widgets
   combo1 = Gtk::manage(new class Gtk::Combo());

   hseparator1 = Gtk::manage(new class Gtk::HSeparator());
   
   label1 = Gtk::manage(new class Gtk::Label(""));

   combo2 = Gtk::manage(new class Gtk::Combo() );  
   checkbutton1 = Gtk::manage(new class Gtk::CheckButton(""));
   checkbutton1->set_label("hexadecimal");

   checkbutton2 = Gtk::manage(new class Gtk::CheckButton(""));
   checkbutton2->set_label("wrap");

   checkbutton3 = Gtk::manage(new class Gtk::CheckButton(""));
   checkbutton3->set_label("search backwards");

   button1 = Gtk::manage(new class Gtk::Button("Search"));

   // combo box
   combo1->get_entry()->set_flags(Gtk::CAN_FOCUS);
   combo1->get_entry()->set_editable(false);
   combo1->get_entry()->set_max_length(0);
   combo1->set_case_sensitive(false);
   for (i = 0; i < MAX_NAVIGATION_TYPE; i++)
   {
      nt = navigation_type_list[i];                        
      listStrings.push_back((char *) &(nt.type_name));
   }
   combo1->set_popdown_strings(listStrings);
   combo1->get_entry()->set_text( navigation_type_list[navigation_id].type_name);
   
   combo2->get_entry()->set_flags(Gtk::CAN_FOCUS);
   combo2->get_entry()->set_editable(true);
   combo2->get_entry()->set_max_length(0);
   combo2->set_case_sensitive(false);
   combo2->get_entry()->set_text("");

   // label
   label1->set_alignment(0,0.5);
   label1->set_padding(0,0);
   label1->set_justify(Gtk::JUSTIFY_LEFT);
   label1->set_line_wrap(false);
   label1->set_use_markup(false);
   
   // grouping of widgets
   pack_start(*combo1, Gtk::PACK_SHRINK, 0);
   pack_start(*label1, Gtk::PACK_EXPAND_WIDGET, 0);
   pack_start(*combo2, Gtk::PACK_SHRINK, 0);
   pack_start(*checkbutton1, Gtk::PACK_SHRINK, 0);
   pack_start(*checkbutton2, Gtk::PACK_SHRINK, 0);
   pack_start(*checkbutton3, Gtk::PACK_SHRINK, 0);
   pack_start(*hseparator1);
   pack_start(*button1, Gtk::PACK_SHRINK, 0);

   // connect signals
   combo1->get_entry()->signal_changed().connect( sigc::mem_fun(*this, &gui_navigator::Change_Type) );

   // initialise gui
   Show_Relevant_Parts();           // depending on type of navigation

   // show components
   combo1->show();
   hseparator1->show();
   show();
}

dis::gui::gui_navigator::~gui_navigator()
{
}

void 
dis::gui::gui_navigator::Get_Navigation(NAVIGATION_OPTIONS *no)
{
   Collect_Options();

   no->nav_id            = navigation_id;
   no->nav_direct_offset = navigation_direct_offset;
   no->nav_search_byte   = navigation_search_byte;
   no->nav_direction     = navigation_direction;
   no->nav_wrap          = navigation_wrap;  
}

void
dis::gui::gui_navigator::Show_Relevant_Parts()
{

  ////////////////////////////////////////////////////////
  ////////// combo2         : direct offset
  ////////// checkbutton1   : hexa
  ////////// checkbutton2   : swap
  ////////// checkbutton3   : forward
  ////////// button1        : ok
  ////////////////////////////////////////////////////////

  switch (navigation_id)
  {
  case NAVIGATION_TYPE_DIRECT_OFFSET:
      {
       label1->set_label("Go to...");
       label1->show();

       combo2->set_popdown_strings(direct_offset_list);
       combo2->show();
       combo2->grab_focus();
       combo2->get_entry()->set_text(navigation_last_direct_offset);

       checkbutton1->show();
       checkbutton1->set_active(navigation_hexadecimal == true);
       checkbutton2->hide();
       checkbutton3->hide();

       button1->set_label("Jump");
       button1->show();
       
       show();
       break;
      }

  case NAVIGATION_TYPE_SEARCH_BYTE:
      {
       label1->set_label("Look for...");
       label1->show();

       combo2->set_popdown_strings(search_byte_list);
       combo2->show();
       combo2->grab_focus();
       combo2->get_entry()->set_text(navigation_last_search_byte);
       
       checkbutton1->show();
       checkbutton1->set_active(navigation_hexadecimal == true);
       checkbutton2->show();
       checkbutton2->set_active(navigation_wrap == true);
       checkbutton3->show();
       checkbutton3->set_active(navigation_direction == false);

       button1->set_label("Search");
       button1->show();
       
       show();
       break;
      }

  default: 
      {
       label1->set_label("Not yet implemented...");
       combo2->hide();
       checkbutton1->hide();
       checkbutton2->hide();
       checkbutton3->hide();
       button1->hide();

       show();
       break;
      }    
  }   
}

void       
dis::gui::gui_navigator::Collect_Options()
{
 int            i;

 std::string    s;         // temp value

 ///////////////////////////////////////////////////////

 s = combo2->get_entry()->get_text();             // get navigation info

 navigation_hexadecimal     = checkbutton1->get_active();
 navigation_wrap            = checkbutton2->get_active();
 navigation_direction       = (checkbutton3->get_active() == false);

 switch (navigation_id) 
 {                                               
    case NAVIGATION_TYPE_DIRECT_OFFSET:      
       {
        direct_offset_list.push_front(s);
        direct_offset_list.sort();       
        direct_offset_list.unique();

        combo2->set_popdown_strings(direct_offset_list);
        combo2->get_entry()->set_text(s);
        
        navigation_last_direct_offset = s;

        if (navigation_hexadecimal)
            {navigation_direct_offset = u.hexstring_to_int( &s );}
        else
            {navigation_direct_offset = atoi(s.c_str());}

        break;
       }               
       
    case NAVIGATION_TYPE_SEARCH_BYTE:      
      {
        navigation_last_search_byte = s;
          
        search_byte_list.push_front(s);
        search_byte_list.sort();       
        search_byte_list.unique();     

        combo2->set_popdown_strings(search_byte_list);
        combo2->get_entry()->set_text(s);

        i = u.string_to_vector_char( &s, &navigation_search_byte, navigation_hexadecimal );

        if (i == RET_ERR_GENERAL)
        {
         Gtk::MessageDialog dialog("Invalid search string");
         dialog.run();

         navigation_search_byte.clear(); 
        }

        break;
       }               
    default:
      {

      }                                    
 }                                         
}

void
dis::gui::gui_navigator::Change_Type()
{
 std::string    s;         // temp value

 ///////////////////////////////////////////////////////

 s = combo1->get_entry()->get_text();             // get navigation info

 navigation_id = Get_Navigation_Type_Id(s);       // get its id

 Show_Relevant_Parts();
}

int
dis::gui::gui_navigator::Get_Navigation_Type_Id(std::string cmp_id)
{
 int                i;           // index
 NAVIGATION_TYPE    nt;
 bool               stop;
 int                t;           // temp value

 /////////////////////////////////////////////////////////////////////
 // we want to find the type_id that goes with the input type_name ///
 /////////////////////////////////////////////////////////////////////
 
    
 i = 0;
 t = NAVIGATION_TYPE_DIRECT_OFFSET;
 stop = false;

 while (!stop)
 {
  nt = navigation_type_list[i];

  if (strcmp((char *) &(nt.type_name), cmp_id.c_str()) == 0)
  {
   stop = true;
   t = nt.type_id;
  }
  else
  {
   i++;

   if (i >= MAX_NAVIGATION_TYPE)
   { stop = true; }
  }
 }

 return t;
}

#endif

