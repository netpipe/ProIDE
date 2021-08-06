/*
 * gui_detail: showing the details of one Disassembly_Node
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               

#ifndef GUI_DETAIL_HH
#define GUI_DETAIL_HH

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/fixed.h>
#include <gtkmm/entry.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

#include "gui_commands.hh"   
#include "libutil/utilities.hh"   

namespace dis {
	namespace gui {
        class Model_Columns_References_In;
        class Model_Columns_References_Out;
		class gui_detail;                 
	}
}


//////////////////////////////////////////////////////////////////////////////////////////



class dis::gui::Model_Columns_References_In: public Gtk::TreeModel::ColumnRecord
{
  public:

    Model_Columns_References_In()
    { add(col_offset); add(col_offset_memory);}

    Gtk::TreeModelColumn<string> col_offset;
    Gtk::TreeModelColumn<long> col_offset_memory;
};

class dis::gui::Model_Columns_References_Out: public Gtk::TreeModel::ColumnRecord
  {
  public:

    Model_Columns_References_Out()
    { add(col_offset); add(col_type); add(col_label); add(col_offset_memory);}

    Gtk::TreeModelColumn<string> col_offset;
    Gtk::TreeModelColumn<string> col_type;
    Gtk::TreeModelColumn<string> col_label;      
    Gtk::TreeModelColumn<long> col_offset_memory;
  };
                        
class dis::gui::gui_detail: public Gtk::VBox
{
public:
    
    gui_detail();

    virtual ~gui_detail(); 	

    Gtk::Entry              *ed_file_offset,
                            *ed_memory_offset,
                            *ed_type,
                            *ed_section,
                            *ed_status;

    Glib::RefPtr<Gtk::TextBuffer> 
                            tb_comment,
                            tb_opcode,
                            tb_label;

    Model_Columns_References_In     mc_ref_in;
    Glib::RefPtr<Gtk::ListStore>    ls_ref_in;

    Model_Columns_References_Out    mc_ref_out;
    Glib::RefPtr<Gtk::ListStore>    ls_ref_out;

    Gtk::TreeView                  *tv_references_in,
                                   *tv_references_out;

private:

    //////////////////////////// gui ///////////////////////

    Gtk::Label              *lb_file_offset,            *lb_memory_offset,
                            *lb_type,                   *lb_status,
                            *lb_opcode,                             
                            *lb_comment,                *lb_section,
                            *lb_label,                  *lb_references,
                            *lb_references_in,          *lb_references_out;

    Gtk::TextView           *te_comment,
                            *te_opcode,
                            *te_label;  

    Gtk::Fixed              *fixed;

    Gtk::ScrolledWindow     *sw_opcode,
                            *sw_comment,
                            *sw_label,
                            *sw_ref_in,
                            *sw_ref_out;
};

#endif

#endif


