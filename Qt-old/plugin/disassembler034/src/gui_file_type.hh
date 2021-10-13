/*
 * gui_file_type: for confirming the type of file to be analyzed
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               
 
#ifndef GUI_FILE_TYPE_HH
#define GUI_FILE_TYPE_HH

#include <gtkmm/dialog.h>
#include <gtkmm/accelgroup.h>
#include <gdk/gdkkeysyms.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/combo.h>
#include <gtkmm/separator.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/stock.h>


#include "libdis/file_formats.hh"
#include "libdis/disassembly_options.hh"

namespace dis {
	namespace gui {
		class dlg_file_type;
	}
}       

class dis::gui::dlg_file_type: public Gtk::Dialog
{  
        
  //      AccelData *_data;
public:
        
        dlg_file_type(int *init_id, dis::Disassembly_Options *options);
        
        virtual ~dlg_file_type();

private:
        Gtk::VBox *vbox1;                                  // for entire dialog

        Gtk::Combo *combo1;                                // file types 
        Gtk::HSeparator *hseparator1;                      // under combo1

        Gtk::Table *table1;                                // contains all type dependent info 
        Gtk::CheckButton *checkbutton1;                     
        Gtk::CheckButton *checkbutton2;                     
        Gtk::CheckButton *checkbutton3;                     

        std::string Get_File_Type_Name(int cmp_id);
        int         Get_File_Type_Id(std::string cmp_id);

        int         *disassembly_id;

        dis::Disassembly_Options *o;
        
        void        Show_Relevant_Parts();                 // show the parts of the dialog 
                                                           //  that correspond to this type of disassembly
        
        void        Change_Type();                         // suggest a different type of disassembly

        void        Collect_Options();                     // memorize users' choices 

        void        Cancel();                              // cancel analysis
};
#endif

#endif
