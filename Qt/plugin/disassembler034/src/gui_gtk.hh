/*
 *       gui_gtk.hh : main window in gtk gui
 *
 +       author: Danny Van Elsen
 */

#ifdef HAVE_GTK                               

#ifndef GUI_GTK_HH
#define GUI_GTK_HH

#define GUI_ACTIVE_DEFAULT   0
#define GUI_ACTIVE_BUSY      1

#include <iostream>

#include <libgnomeuimm/app.h>

#include <gtkmm/main.h>            
#include <gtkmm/messagedialog.h>   
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/paned.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/fileselection.h>
#include <gtkmm/statusbar.h>

#include "gui_file_selector.hh"
#include "gui_extra.hh"
#include "gui_detail.hh"
#include "gui_navigator.hh"
#include "gui_file_saver.hh"
#include "gui_disassembly_treemodel.hh"
#include "gui_about.hh"
#include "libdis/analysis.hh"   
#include "libutil/utilities.hh"   

namespace dis {
	namespace gui {
		class gui_gtk;
	}
}

class dis::gui::gui_gtk
     : public Gnome::UI::App
{

public:
	gui_gtk(Gtk::Main *m, Disassembly_Options *d_o);
	virtual ~gui_gtk();

protected:
    
    ////////////////////////////////////////  gui elements /////////////////////////////
    
    Gtk::VBox vbox;                                   //  will contain everything else
    
    Gtk::HPaned             hpaned;

    Gtk::ScrolledWindow     scrolledwindow_disassembly;
    dis::gui::gui_extra     page_extra;
    dis::gui::gui_detail    page_detail;
    dis::gui::gui_navigator page_navigator;
    dis::gui::gui_about     gui_about;

    Gtk::TreeView           treeview;
    Gtk::Notebook           notebook;
    dis::gui::gui_navigator navigator;

    Gtk::Statusbar          statusbar;
    int                     statusbar_context_id;

    dis::gui::Gui_Disassembly_Columns dc;
    Glib::RefPtr<Gui_Disassembly_TreeModel> tm;
    //Gnome::UI::AppBar appbar;

    Gtk::Main              *mw_m;

    dis::Main_Gui           main_gui;                       // wrapper for passing gui elements

    /////////////////////////////////////////  gui callbacks ////////////////////////////
                           
    
    void                    Callback_Analysis_Quit();
    void                    Callback_Help_About();
    void                    Callback_Cursor_Changed();  
    

private:


    void Prepare_Analysis();                    // prepare to analyze
    void Shutdown_Analysis();                   // stop analyzing

    int  Get_File_Name();                       // gets the name of a new file to analyze
    void Start_Analysis();                      // new analysis
    void Start_New_Analysis();                  // new file to analyze

    void Start_Save();                          // save the analysis to a file
    void Start_Open();                          // read the analysis from a file

    void Callback_Save();               
    void Callback_Open();               
    void Callback_Navigation();                 // launch a navigator dialog 
    void Callback_Switch_Tab(GtkNotebookPage* page, guint page_num); 
                                                // fill out extra info

    bool Callback_Navigation_References_In(GdkEventButton *event);   // navigate to a reference
    bool Callback_Navigation_References_Out(GdkEventButton *event);  // navigate to a reference

    void Not_Yet_Implemented();         

    void Treat_Options();                       // treat user specified options

    bool Determine_Existing_Disassembly(string filename);
                                                // whether or not this is an existing disassembly

    void Set_Toolbar(int active);               // sets available options in the toolbar   

    Gtk::Toolbar                 toolbar;

    //Gtk::ToolButton             *tb_new;
    Gtk::ToolButton             *tb_open;
    Gtk::ToolButton             *tb_save;

    uint                         extra_page;

    ////////////////////////////////////////////////////////////////////////////////////

    dis::Analysis               *analysis;            // will execute the disassembly

    Disassembly_Options         *options;             // user specified options

    bool                         existing_disassembly;

    util::Utilities              u;

    Gtk::TreeModel::Path         navigation_path;
    Gtk::TreeViewColumn         *navigation_column;
};

#endif   

#endif  
