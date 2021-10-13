/*
 * gui_extra: allowing the user to jump to and fro in the disassembly
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               
 
#include "gui_extra.hh"

dis::gui::gui_extra::gui_extra()
{

   /////////////////////////////////////////////////////////////////////

   ts_extra = Gtk::TreeStore::create(mc_extra);

   tv_extra = Gtk::manage(new class Gtk::TreeView()); 
   tv_extra->set_model(ts_extra);
   tv_extra->append_column("description", mc_extra.col_description);

   sw_extra = Gtk::manage(new class Gtk::ScrolledWindow()); 
   sw_extra->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   sw_extra->add(*tv_extra);        

   pack_start(*sw_extra, Gtk::PACK_EXPAND_WIDGET, 0);

   show();
}

dis::gui::gui_extra::~gui_extra()
{
}

void
dis::gui::gui_extra::Fill_Extra_Info(Extra *extra)
{
   char                             *c;

   Extra                            *e2, *e3;

   /////////////////////////////////////////////////////////////////////


   if (extra == save_extra)
   { return ; }
   else 
   { save_extra = extra; }

   ts_extra->clear();

   while (extra)                                            // we presuppose only three levels
   {
    row = *(ts_extra->append());

    c = extra->description;
    if (c)
    { row[mc_extra.col_description] = c; }
    else 
    { row[mc_extra.col_description] = "no extra info"; }

    e2 = extra->next_level;
    while (e2)
    {   
     c = e2->description;
     if (c)
     { 
      childrow2 = *(ts_extra->append(row.children()));
      childrow2[mc_extra.col_description] = c; 
     }

     e3 = e2->next_level;
     while (e3)
     {   
      c = e3->description;
      if (c)
      { 
       childrow3 = *(ts_extra->append(childrow2.children()));
       childrow3[mc_extra.col_description] = c; 
      }

      e3 = e3->next;
     }  

     e2 = e2->next;
    }                

    extra = extra->next;
   }
}

#endif



