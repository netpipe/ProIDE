/*
 * gui_file_saver: class for saving a file
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               

#ifndef GUI_FILE_SAVER_HH
#define GUI_FILE_SAVER_HH

#include <gtkmm/filechooser.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/radiotoolbutton.h>
#include <gtkmm/stock.h>
#include <gtkmm/filefilter.h>

#include "gui_file_save_as.hh"
#include "libdis/disassembly_options.hh"


namespace dis {
	namespace gui {
		class fs_saver;
	}
}

                        
class dis::gui::fs_saver: public Gtk::FileChooserDialog
{
public:
    
    fs_saver(char   *title, const char *file_name);

    virtual ~fs_saver();  

    std::string Get_Saver();

protected:

    bool on_leave_notify_event (GdkEventCrossing* event);

private:

    Gtk::FileFilter filter_any, filter_asm, filter_dis, filter_lst;

    dis::gui::fs_save_as                save_as;

    void                                Correct_File_Name();

    std::list<dis::gui::save_as_lst>    lsa;

    std::string                         f_name;                     // file_name
    std::string                         f_type;                     // file_type

    std::string                         temp;                       // temp value
    std::string::size_type              pos;                        // position in string

};

#endif

#endif

