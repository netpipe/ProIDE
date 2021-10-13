/*
 *       gui_about.hh : 'about' window in gtk gui
 *
 +       author: Danny Van Elsen
 */

#ifdef HAVE_GTK                               


#ifndef GUI_ABOUT_HH
#define GUI_ABOUT_HH

using namespace std;

#include <iostream>

#include <gtkmm/aboutdialog.h>   

namespace dis {
	namespace gui {
		class gui_about;
	}
}

class dis::gui::gui_about: public Gtk::AboutDialog
{

public:
	                 gui_about();
	virtual         ~gui_about();

protected:
    

private:

    std::list<Glib::ustring> list_authors;

};

#endif   

#endif

