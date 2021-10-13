/*
 * gui_disassembly_treeview: Custom gui
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               
 
#ifndef GUI_DISASSEMBLY_TREEMODEL_HH
#define GUI_DISASSEMBLY_TREEMODEL_HH

#define GUI_DISASSEMBLY_TREEMODEL_COL_OFFSET        0
#define GUI_DISASSEMBLY_TREEMODEL_COL_OFFSET_HEX    1
#define GUI_DISASSEMBLY_TREEMODEL_COL_OPCODES       2
#define GUI_DISASSEMBLY_TREEMODEL_COL_INSTRUCTION   3
#define GUI_DISASSEMBLY_TREEMODEL_COL_LABEL         4
#define GUI_DISASSEMBLY_TREEMODEL_COL_COMMENT       5

#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gdkmm/rectangle.h>
#include "libdis/analysis.hh"
#include "libutil/utilities.hh"

namespace dis
  {
	namespace gui 
        {
		class Gui_Disassembly_TreeModel;
        class Gui_Disassembly_Columns;
	    }
  }                


class dis::gui::Gui_Disassembly_Columns: public Gtk::TreeModel::ColumnRecord
{
public:
    
    Gui_Disassembly_Columns();

    virtual ~Gui_Disassembly_Columns(); 	

    int Get_Columns_Count() const;

    Gtk::TreeModelColumn<int> col_offset;

    Gtk::TreeModelColumn<Glib::ustring> col_offset_hex;

    Gtk::TreeModelColumn<Glib::ustring> col_opcodes;

    Gtk::TreeModelColumn<Glib::ustring> col_instruction;

    Gtk::TreeModelColumn<Glib::ustring> col_label;

    Gtk::TreeModelColumn<Glib::ustring> col_comment;

private:
    
    int                                 n_columns;
};


class  dis::gui::Gui_Disassembly_TreeModel
  : public Glib::Object,
    public Gtk::TreeModel
{
public:

  Gui_Disassembly_TreeModel();
  virtual ~Gui_Disassembly_TreeModel();

  static    Glib::RefPtr<Gui_Disassembly_TreeModel> create();

  void      Set_Disassembly(dis::Disassembly *d);

protected:

   // Overrides:
   virtual Gtk::TreeModelFlags get_flags_vfunc() const;
   virtual int get_n_columns_vfunc() const;
   virtual GType get_column_type_vfunc(int index) const;
   virtual void get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const;
  
   bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const;

   virtual bool iter_children_vfunc(const iterator& parent, iterator& iter) const;
   virtual bool iter_has_child_vfunc(const iterator& iter) const;
   virtual int iter_n_children_vfunc(const iterator& iter) const;
   virtual int iter_n_root_children_vfunc() const;
   virtual bool iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const;
   virtual bool iter_nth_root_child_vfunc(int n, iterator& iter) const;
   virtual bool iter_parent_vfunc(const iterator& child, iterator& iter) const;
   virtual Path get_path_vfunc(const iterator& iter) const;
   virtual bool get_iter_vfunc(const Path& path, iterator& iter) const;

   virtual bool iter_is_valid(const iterator& iter) const;

private:

   typedef Gtk::TreeModelColumn<Glib::ustring>  StringColumn;
   typedef Gtk::TreeModelColumn<int>            IntColumn;

   mutable IntColumn::ValueType                 ic;
   mutable StringColumn::ValueType              sc;

   mutable StringColumn                         s_c;
   mutable IntColumn                            i_c;

   mutable int                                  result_int;
   mutable std::string                          result_string;

   Gui_Disassembly_Columns              g_d_columns;
   dis::Disassembly                    *disassembly;

   dis::Statistics                      statistics;

   util::Utilities                     *u;

   int                                  max_offset_in_file;
};


#endif

#endif
