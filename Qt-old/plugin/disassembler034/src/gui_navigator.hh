/*
 * gui_navigator: allowing the user to jump to and fro in the disassembly
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               

#ifndef GUI_NAVIGATOR_HH
#define GUI_NAVIGATOR_HH

#include <gtkmm/box.h>
#include <gtkmm/combo.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separator.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <gtkmm/stock.h>

#include "libutil/utilities.hh"   

namespace dis {
	namespace gui {
		class gui_navigator;
	}
}

#define MAX_NAVIGATION_TYPE_NAME_LENGTH   40


#define NAVIGATION_TYPE_DIRECT_OFFSET     0  
#define NAVIGATION_TYPE_LABEL             1  
#define NAVIGATION_TYPE_FUNCTION          2  
#define NAVIGATION_TYPE_SEARCH_BYTE       3  
#define NAVIGATION_TYPE_HISTORY           4  
#define MAX_NAVIGATION_TYPE               5                

struct NAVIGATION_TYPE                           
{
    char   type_name [MAX_NAVIGATION_TYPE_NAME_LENGTH];
    int    type_id; 
};
#define NAVIGATION_TYPE_LENGTH (int) sizeof (NAVIGATION_TYPE)

const static NAVIGATION_TYPE navigation_type_list [MAX_NAVIGATION_TYPE] =   
                                                                     // all possible navigation types
        {
            {"Direct Offset                  ", (int) NAVIGATION_TYPE_DIRECT_OFFSET}, 
            {"Label                          ", (int) NAVIGATION_TYPE_LABEL},
            {"Function                       ", (int) NAVIGATION_TYPE_FUNCTION}, 
            {"Search Byte                    ", (int) NAVIGATION_TYPE_SEARCH_BYTE},
            {"History                        ", (int) NAVIGATION_TYPE_HISTORY}
        };

struct NAVIGATION_OPTIONS                           
{
    int         nav_id,
                nav_direct_offset; 
    
    //std::string nav_search_byte;
    std::vector<char> nav_search_byte;

    bool        nav_direction,      // forward, backward
                nav_wrap;           // wrap or not

};
#define NAVIGATION_OPTIONS_LENGTH (int) sizeof (NAVIGATION_OPTIONS)


//////////////////////////////////////////////////////////////////////////////////////////


                        
class dis::gui::gui_navigator: public Gtk::VBox
{
public:
    
    gui_navigator();

    virtual ~gui_navigator(); 	

    void                        Get_Navigation(NAVIGATION_OPTIONS *no);

    void                        Collect_Options();      // memorize users' choices 

    void                        Show_Relevant_Parts();  // adapt dialog to users' choices

    Gtk::Button                *button1;

private:

    int                         navigation_id, 
                                navigation_direct_offset;

    std::vector<char>           navigation_search_byte;

    std::string                 navigation_last_direct_offset,
                                navigation_last_search_byte;

    bool                        navigation_hexadecimal,
                                navigation_direction,
                                navigation_wrap;

    util::Utilities             u;                      // ... conversions

    void                        Change_Type();

    int                         Get_Navigation_Type_Id(std::string cmp_id);

    //////////////////////////// gui ///////////////////////

    Gtk::Combo              *combo1,                   // types of navigation
                            *combo2; 

    std::list<Glib::ustring> direct_offset_list,       // one list per type of navigation
                             search_byte_list;

    Gtk::HSeparator         *hseparator1;              
    
    Gtk::Label              *label1;
    
    Gtk::CheckButton        *checkbutton1,             // hexadecimal
                            *checkbutton2,             // wrap
                            *checkbutton3;             // from selected

};

#endif

#endif


