/*
 * gui_file_saver: class for saving a file
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               
 
#include "gui_file_saver.hh"

dis::gui::fs_saver::fs_saver(char   *title, const char *file_name):
    Gtk::FileChooserDialog(title, Gtk::FILE_CHOOSER_ACTION_SAVE)
{
  dis::gui::save_as_lst sa;

  //////////////////////////////////////////////////

  filter_any.set_name("All files");
  filter_any.add_pattern("*");
  add_filter(filter_any);

  filter_asm.set_name("asm source files");
  filter_asm.add_pattern('*' + GUI_FILE_SAVER_ASM);
  add_filter(filter_asm);

  filter_lst.set_name("listing files");
  filter_lst.add_pattern('*' + GUI_FILE_SAVER_LST);
  add_filter(filter_lst);

  filter_dis.set_name("disassembly databases");
  filter_dis.add_pattern('*' + GUI_FILE_SAVER_DIS);
  add_filter(filter_dis);

  set_filter(filter_dis);
  
  add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  add_button("Select", Gtk::RESPONSE_OK);

  // indicate possible file types
  sa.extension = GUI_FILE_SAVER_DIS;
  sa.description = GUI_FILE_SAVER_DIS_TXT;
  lsa.push_back(sa);
  sa.extension = GUI_FILE_SAVER_LST;
  sa.description = GUI_FILE_SAVER_LST_TXT;
  lsa.push_back(sa);
  save_as.Set_Save_As_Lst(&lsa);

  if (file_name != 0)
  { set_current_name(file_name); }
  else { set_current_name("*.*"); }
  
  f_name = get_filename();
  f_type = save_as.Get_Save_As_Lst();
  Correct_File_Name();

  set_extra_widget(save_as);
}

dis::gui::fs_saver::~fs_saver()
{
}

std::string 
dis::gui::fs_saver::Get_Saver()
{
    ////////////////////////////////////////////

    return save_as.Get_Save_As_Lst();
}

bool
dis::gui::fs_saver::on_leave_notify_event (GdkEventCrossing* event)
{
    f_name = get_filename();

    f_type = save_as.Get_Save_As_Lst();

    if (f_name.find(f_type) == std::string::npos)
    {
     Correct_File_Name();
    }

    return false;
}

void        
dis::gui::fs_saver::Correct_File_Name()
{
    //////////////////////////////////////

    // delete path
    pos = f_name.rfind(GUI_FILE_SAVE_AS_PATH_DELIMITER); 

    if ((pos != std::string::npos) && (f_name.size() > pos))
    { temp = f_name.substr(pos + 1, f_name.size()); }                                              

    // delete extension
    temp = temp.substr(0, temp.find(".")); 

    f_name = temp + f_type;

    set_current_name(f_name);
}

#endif



