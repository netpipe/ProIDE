/*
 * gui_extra: showing extra info about the disassembled file
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               

#ifndef GUI_EXTRA_HH
#define GUI_EXTRA_HH

#include <iostream>
                                         
#include <gtkmm/box.h>                              
#include <gtkmm/fixed.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treestore.h>
#include <gtkmm/treeview.h>

#include "libdis/extra.hh"

namespace dis {
	namespace gui {
        class Model_Columns_Extra;
		class gui_extra;                 
	}
}


//////////////////////////////////////////////////////////////////////////////////////////



class dis::gui::Model_Columns_Extra: public Gtk::TreeModel::ColumnRecord
  {
  public:

    Model_Columns_Extra()
    { add(col_description); }

    Gtk::TreeModelColumn<std::string> col_description;
  };
                        
class dis::gui::gui_extra: public Gtk::VBox
{
public:
    
    gui_extra();

    virtual ~gui_extra();   

    void                            Fill_Extra_Info(Extra *extra);

private:

    Extra                          *save_extra;

    //////////////////////////// gui ///////////////////////

    Model_Columns_Extra             mc_extra;

    Glib::RefPtr<Gtk::TreeStore>    ts_extra;

    Gtk::TreeView                  *tv_extra;

    Gtk::TreeModel::Row             row, childrow2, childrow3;

    Gtk::ScrolledWindow            *sw_extra;                   
};

#endif

#endif

