/*
 * gui_disassembly_model: columns that are whown in the treeview gui
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               
 
#include "gui_disassembly_treemodel.hh"

dis::gui::Gui_Disassembly_Columns::Gui_Disassembly_Columns()
{
  n_columns = 0;

  add(col_offset);          n_columns++;
  add(col_offset_hex);      n_columns++;
  add(col_opcodes);         n_columns++;
  add(col_instruction);     n_columns++;
  add(col_label);           n_columns++;
  add(col_comment);         n_columns++;
}

dis::gui::Gui_Disassembly_Columns::~Gui_Disassembly_Columns()
{
}

inline
int
dis::gui::Gui_Disassembly_Columns::Get_Columns_Count() const
{
  return n_columns;
}

dis::gui::Gui_Disassembly_TreeModel::Gui_Disassembly_TreeModel()
: Glib::ObjectBase( typeid(Gui_Disassembly_TreeModel) ), 
  Glib::Object()
{
 Gtk::TreeModel::add_interface( Glib::Object::get_type() );

 disassembly = 0;

 u = new util::Utilities();

 sc.init( StringColumn::ValueType::value_type() ); 
 ic.init( IntColumn::ValueType::value_type() );  
}

dis::gui::Gui_Disassembly_TreeModel::~Gui_Disassembly_TreeModel()
{
 free(u);
}

Glib::RefPtr<dis::gui::Gui_Disassembly_TreeModel> 
dis::gui::Gui_Disassembly_TreeModel::create()
{
  return Glib::RefPtr<dis::gui::Gui_Disassembly_TreeModel>

  (new dis::gui::Gui_Disassembly_TreeModel );
}

void
dis::gui::Gui_Disassembly_TreeModel::Set_Disassembly(dis::Disassembly *d)
{
  disassembly = d;

  if (d)
 { d->Callback_Get_Statistics(&statistics); }                                               
}             

Gtk::TreeModelFlags
dis::gui::Gui_Disassembly_TreeModel::get_flags_vfunc() const
{
   return Gtk::TreeModelFlags(0);
}

int 
dis::gui::Gui_Disassembly_TreeModel::get_n_columns_vfunc() const
{
   return g_d_columns.Get_Columns_Count();
}

void
dis::gui::Gui_Disassembly_TreeModel::get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const
{
  dis::Disassembly_Node *dn;  

  //////////////////////////////////////////////////////////

  //  cout << "get_value_vfunc" << "\n";

  dn = (dis::Disassembly_Node *) iter.gobj()->user_data;

  if (dn == 0) { return; }

  switch (column)
  {
  case GUI_DISASSEMBLY_TREEMODEL_COL_OFFSET:
    {
     result_int = dn->memory_offset;

     ic.set(result_int); 
     value.init( Glib::Value< int >::value_type() ); 
     value = ic;
     break;    
    }      

  case GUI_DISASSEMBLY_TREEMODEL_COL_OFFSET_HEX:
    {
     result_string = u->int_to_hexstring(dn->memory_offset);

     sc.set(result_string); 
     value.init( Glib::Value< std::string >::value_type() ); 
     value = sc;

     break;
    }      
  case GUI_DISASSEMBLY_TREEMODEL_COL_OPCODES:
    {
     disassembly->Callback_Translate_Opcodes(dn, false, &result_string, 0);

     sc.set(result_string); 
     value.init( Glib::Value< std::string >::value_type() ); 
     value = sc;

     break;
    }      
  case GUI_DISASSEMBLY_TREEMODEL_COL_LABEL:
    {
     if (dn->label)
     { result_string.assign(dn->label); }
     else
     { result_string.assign(""); }

     sc.set(result_string); 
     value.init( Glib::Value< std::string >::value_type() ); 
     value = sc;

     break;
    }      
  case GUI_DISASSEMBLY_TREEMODEL_COL_COMMENT:
    {
     if (dn->comment)
     { result_string.assign(dn->comment); }
     else
     { result_string.assign(""); }

     sc.set(result_string); 
     value.init( Glib::Value< std::string >::value_type() ); 
     value = sc;

     break;
    }      
  case GUI_DISASSEMBLY_TREEMODEL_COL_INSTRUCTION:
    {
     if (dn->type == NODE_TYPE_DATA)
     { disassembly->Callback_Translate_Opcodes(dn, true, &result_string, 0); }
     else
     { disassembly->Callback_Translate_Instruction( &(dn->instruction),  &result_string); }

     sc.set(result_string); 
     value.init( Glib::Value< std::string >::value_type() ); 
     value = sc;

     break;
    }                                     
  }
}

bool
dis::gui::Gui_Disassembly_TreeModel::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{ 
  dis::Disassembly_Node *dn;  

  int                   new_offset;

  //////////////////////////////////////////////////////////

  //cout << "iter_next_vfunc" << "\n";

  if ((void*) iter.gobj()->user_data == 0) 
  { iter_next = iterator(); return false; }  //There is no next row.

  dn = (dis::Disassembly_Node *) iter.gobj()->user_data;

  new_offset = dn->memory_offset + dn->n_used;

  if (new_offset >  statistics.max_row_offset)
  { iter_next = iterator(); return false; }  //There is no next row.

  dn = disassembly->Callback_Get_Next_Disassembly_Node_From_Offset(dn->memory_offset);

  iter_next.gobj()->user_data = (void*) dn;

  return true;
}

bool
dis::gui::Gui_Disassembly_TreeModel::iter_children_vfunc(const iterator& parent, iterator& iter) const
{
  return iter_nth_child_vfunc(parent, 0, iter);
}

bool
dis::gui::Gui_Disassembly_TreeModel::iter_has_child_vfunc(const iterator& iter) const
{
  return (iter_n_children_vfunc(iter) > 0);
}

int
dis::gui::Gui_Disassembly_TreeModel::iter_n_children_vfunc(const iterator& iter) const
{
  return 0; //There are no children
}

int
dis::gui::Gui_Disassembly_TreeModel::iter_n_root_children_vfunc() const
{
  return statistics.n_rows;  // number of rows
}

bool
dis::gui::Gui_Disassembly_TreeModel::iter_nth_child_vfunc(const iterator& parent, int /* n */, iterator& iter) const
{
  iter = iterator(); 
  return false; //There are no children.
}

bool
dis::gui::Gui_Disassembly_TreeModel::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
  dis::Disassembly_Node *dn;  

  //////////////////////////////////////////////////////////

  if (!disassembly) {return false; }

  if (n < statistics.n_rows)
  {
   dn = disassembly->Callback_Get_nth_Row(n); 

   iter.gobj()->user_data = (void*) dn;

   return true;    // n <= available rows
  }
  
  return false;     // n > available rows
}

bool
dis::gui::Gui_Disassembly_TreeModel::iter_parent_vfunc(const iterator& child, iterator& iter) const
{
  iter = iterator(); 
  return false; //There are no children, so no parents.
}

bool
dis::gui::Gui_Disassembly_TreeModel::get_iter_vfunc(const Path& path, iterator& iter) const
{
   unsigned               ps;     // path_size
   int                    ri;     // row_index 

   dis::Disassembly_Node *dn;  

   /////////////////////////////////////////////

   if (!disassembly) {return false; }

   ps = path.size();
   if (   (!ps)
       || (ps > 1))                //There are no children.
   { iter = iterator(); return false; }

   iter = iterator(); //clear the input parameter.
   //iter.set_stamp(m_stamp);
   
   ri = path[0];

   if (ri >= statistics.n_rows)
   { iter = iterator(); return false; }

   dn = disassembly->Callback_Get_nth_Row(ri); 

   iter.gobj()->user_data = (void*) dn;

   return true;
}


bool 
dis::gui::Gui_Disassembly_TreeModel::iter_is_valid(const iterator& iter) const
{
  if (!disassembly) {return false; }

  return Gtk::TreeModel::iter_is_valid(iter);
}

Gtk::TreeModel::Path
dis::gui::Gui_Disassembly_TreeModel::get_path_vfunc(const iterator& /* iter */) const
{
   //TODO:
   return Path();
}

GType
dis::gui::Gui_Disassembly_TreeModel::get_column_type_vfunc(int index) const
{
  switch (index)
  {
  case GUI_DISASSEMBLY_TREEMODEL_COL_OFFSET:
    {
     return i_c.type();
     break;    
    }        
  case GUI_DISASSEMBLY_TREEMODEL_COL_OFFSET_HEX:
  case GUI_DISASSEMBLY_TREEMODEL_COL_OPCODES:
  case GUI_DISASSEMBLY_TREEMODEL_COL_LABEL:
  case GUI_DISASSEMBLY_TREEMODEL_COL_COMMENT:
  case GUI_DISASSEMBLY_TREEMODEL_COL_INSTRUCTION:
    {
     return s_c.type();
     break;    
    }      
  default:
    {
     return 0;
     break;    
    }        
  }
}

#endif










