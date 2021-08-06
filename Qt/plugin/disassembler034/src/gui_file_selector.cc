/*
 * gui_file_selector: FileSelection classes
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                                
 
#include "gui_file_selector.hh"

dis::gui::fs_selector::fs_selector(char *title):
                 Gtk::FileChooserDialog(title, Gtk::FILE_CHOOSER_ACTION_OPEN)
{
  Gtk::FileFilter filter_any;
  filter_any.set_name("All files");
  filter_any.add_pattern("*");
  add_filter(filter_any);

  Gtk::FileFilter filter_dis;
  filter_dis.set_name("disassembly databases");
  filter_dis.add_pattern('*' + GUI_FILE_SAVER_DIS);
  add_filter(filter_dis);

  Gtk::FileFilter filter_exe;
  filter_exe.set_name("executable files");
  filter_dis.add_pattern('*' + GUI_FILE_SAVER_EXE);
  add_filter(filter_exe);

  set_filter(filter_any);

  add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  add_button("Select", Gtk::RESPONSE_OK);
}


dis::gui::fs_selector::~fs_selector()
{
}

#endif

