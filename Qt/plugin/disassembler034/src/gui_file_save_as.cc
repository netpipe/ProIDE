/*
 * gui_file_saver: class for saving a file
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               
 
#include "gui_file_save_as.hh"

dis::gui::fs_save_as::fs_save_as():
    Gtk::HBox()
{
   /////////////////////////////////////////////////

   Gtk::Label *label1 = Gtk::manage(new class Gtk::Label("save as type: "));
   //Gtk::ComboBoxText *combo1 = Gtk::manage(new class Gtk::ComboBoxText());
   Gtk::HBox *hbox1 = Gtk::manage(new class Gtk::HBox(false, 0));

   label1->set_alignment(0.5,0.5);
   label1->set_padding(0,0);
   label1->set_justify(Gtk::JUSTIFY_LEFT);
   label1->set_line_wrap(false);
   label1->set_use_markup(false);
   label1->set_selectable(false);

   hbox1->pack_start(*label1, Gtk::PACK_SHRINK, 0);
   hbox1->pack_start(combo1);

   add(*hbox1);

   label1->show();
   combo1.show();
   hbox1->show();

   show();
}

dis::gui::fs_save_as::~fs_save_as()
{
}

std::string 
dis::gui::fs_save_as::Get_Save_As_Lst()
{
    bool stop;

    std::string s;

    std::list<save_as_lst>::iterator it;

    ////////////////////////////////////////////

     s = combo1.get_active_text();    

     stop = false;
     it = lsal->begin();

     while (stop == false)
     {
      if (it == lsal->end())
      { stop = true; }
      else if (it->description == s)
      { 
        stop = true;
        return it->extension;
      }                 
      else 
      { it ++; }
     }          

     return " ";
}

void
dis::gui::fs_save_as::Set_Save_As_Lst(std::list<save_as_lst> *lsa)
{   
    save_as_lst *sa;

    std::string s;

    std::list<save_as_lst>::iterator it;


    ////////////////////////////////////////////

    lsal = lsa;

    for (it = lsa->begin(); it != lsa->end(); it++)
    {
     sa = &(*it);

     s = sa->description;
     combo1.append_text(s.c_str());     
    }

    if (lsa->size() > 0)
    {
     combo1.set_active(0);    
    }                     
}

#endif



