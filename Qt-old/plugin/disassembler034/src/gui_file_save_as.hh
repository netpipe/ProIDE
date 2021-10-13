/*
 * gui_file_save_as: class for saving a file
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               

#ifndef GUI_FILE_SAVE_AS_HH
#define GUI_FILE_SAVE_AS_HH


#define GUI_FILE_SAVE_AS_PATH_DELIMITER "/"
                                  
#include <iostream>
    
#include <gtkmm/comboboxtext.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>

namespace dis {
	namespace gui {
		class fs_save_as;
        struct save_as_lst;
	}
}

struct dis::gui::save_as_lst     
{                       
    std::string      description;            //  description of file type
    std::string      extension;              //  extension   of file type
};  

                        
class dis::gui::fs_save_as: public Gtk::HBox
{
public:
    
    fs_save_as();

    virtual    ~fs_save_as(); 	

    std::string Get_Save_As_Lst();

    void        Set_Save_As_Lst(std::list<save_as_lst> *lsa); 

private:

    Gtk::ComboBoxText combo1;

    std::list<save_as_lst> *lsal; 
};

#endif

#endif


