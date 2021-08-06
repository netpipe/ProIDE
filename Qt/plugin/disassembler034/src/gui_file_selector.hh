/*
 * gui_file_selector: FileSelection classes
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               

#ifndef GUI_FILE_SELECTOR_HH
#define GUI_FILE_SELECTOR_HH

#include <iostream>
#include "gui_file_saver.hh"
#include "libdis/disassembly_options.hh"

                   
namespace dis {
	namespace gui {
		class fs_selector;
	}
}

                        
class dis::gui::fs_selector: public Gtk::FileChooserDialog
{
public:
    
    fs_selector(char   *title);

    virtual ~fs_selector(); 	

};

#endif

#endif


