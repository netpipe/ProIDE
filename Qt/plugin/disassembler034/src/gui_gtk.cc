/*
 *       gui_gtk.cc : main window in gtk gui
 *
 +       author: Danny Van Elsen
 */


#ifdef HAVE_GTK                               
  
#include <config.h> 
#include <gui_gtk.hh>

                   
dis::gui::gui_gtk::gui_gtk(Gtk::Main *m, Disassembly_Options *d_o)
: Gnome::UI::App("gui_gtk", PACKAGE_STRING)
{  
   std::cout << "Constructor Mainwindow" << "\n";

  std::vector<Gnome::UI::Items::SubTree> main_menu;     //whole menu.
   std::vector<Gnome::UI::Items::Info>    menu_analysis,    //Analysis
                                          menu_options,
                                          menu_help;

   Glib::RefPtr<Gdk::Screen> screen;

   int  w, h;

   /////////////////////////////////////////////////////////////////

   analysis = 0;

   mw_m = m;

   options = d_o;

   navigation_column = new Gtk::TreeViewColumn ();

   ////////////////// menus ////////////////////////////////////////

   //Analysis - New
   menu_analysis.push_back( Gnome::UI::MenuItems::New(("Open"), 
                                ("Opens a file or disassembly"), 
                                sigc::mem_fun(*this, &gui_gtk::Callback_Open)) );

   //Analysis - separator
   menu_analysis.push_back(Gnome::UI::Items::Separator());

   //Analysis - Quit
   menu_analysis.push_back(Gnome::UI::MenuItems::Exit(sigc::mem_fun(*this, &gui_gtk::Callback_Analysis_Quit)));

   //Options - Quit
   menu_options.push_back(Gnome::UI::MenuItems::Exit(sigc::mem_fun(*this, &gui_gtk::Callback_Analysis_Quit)));

   //Help - About
   menu_help.push_back( Gnome::UI::MenuItems::About(
                                sigc::mem_fun(*this, &gui_gtk::Callback_Help_About)) );
    
   //Add Analysis:
   main_menu.push_back(Gnome::UI::Items::SubTree ("_Analysis", menu_analysis));

   //Add Options:
   main_menu.push_back(Gnome::UI::Items::SubTree ("_Options", menu_options));    

   //Add Help:
   main_menu.push_back(Gnome::UI::Items::SubTree ("_Help", menu_help));    

   //Create main menu
   create_menus(main_menu);                                                    

   ////////////////// statusbar ////////////////////////////////////////         

   //main_gui.sb_context_id = statusbar.get_context_id("Ready");

   ////////////////// toolbar ////////////////////////////////////////         

   tb_open = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::OPEN));
   toolbar.append(*tb_open);
   tb_open->signal_clicked().connect(sigc::mem_fun(*this, &gui_gtk::Callback_Open));

   tb_save = Gtk::manage(new Gtk::ToolButton(Gtk::Stock::SAVE));
   toolbar.append(*tb_save);
   tb_save->signal_clicked().connect(sigc::mem_fun(*this, &gui_gtk::Callback_Save));

   Set_Toolbar(GUI_ACTIVE_DEFAULT);
   

   //////////////////// model //////////////////////////////////////////

   tm = dis::gui::Gui_Disassembly_TreeModel::create();
   
   //////////////////// tv ///////////////////////////////////////

   treeview.append_column("Offset", dc.col_offset_hex);
   treeview.append_column("Instruction / Data", dc.col_instruction);
   treeview.append_column("Label", dc.col_label);
   treeview.append_column("Opcodes", dc.col_opcodes);
   treeview.append_column("Comment", dc.col_comment);

   treeview.signal_cursor_changed().connect(sigc::mem_fun(*this,&gui_gtk::Callback_Cursor_Changed), false );

   ///////////////////// scrolledwindow_disassembly ////////////////////////////////

   scrolledwindow_disassembly.add(treeview);              

   ///////////////////// detail //////////////////////////////////////////

   page_detail.tv_references_in->signal_button_press_event().connect
                                          ( sigc::mem_fun(*this, &gui_gtk::Callback_Navigation_References_In), false );

   page_detail.tv_references_out->signal_button_press_event().connect
                                          ( sigc::mem_fun(*this, &gui_gtk::Callback_Navigation_References_Out), false );

   ///////////////////// navigation //////////////////////////////////////

   page_navigator.button1->signal_clicked().connect
                                          ( sigc::mem_fun(*this, &gui_gtk::Callback_Navigation) );

   ///////////////////// notebook ////////////////////////////////////////

   notebook.append_page(page_detail, "This Instruction");
   notebook.append_page(page_navigator, "Navigation");
   notebook.append_page(page_extra, "This file");

   extra_page = notebook.page_num(page_extra);

   notebook.signal_switch_page().connect(sigc::mem_fun(*this, &gui_gtk::Callback_Switch_Tab));

   ///////////////////// hpaned ////////////////////////////////////////

   hpaned.add1(scrolledwindow_disassembly);              
   hpaned.add2(notebook);              

   ///////////////////// vbox //////////////////////////////////////////

   vbox.pack_start(toolbar, Gtk::PACK_SHRINK);
   vbox.pack_start(hpaned, Gtk::PACK_EXPAND_WIDGET);
   vbox.pack_start(statusbar, false, true, 0);

   ///////////////////// main window ///////////////////////////////////

   set_contents(vbox);

   ///////////////////// positioning ///////////////////////////////////
                                     //main window /////////////////////
   screen = get_screen();
   w = screen->get_width(); 
   h = screen->get_height();
   w = w - (w / GUI_MARGIN_RIGHT);        
   h = h - (h / GUI_MARGIN_BOTTOM);
   set_default_size(w,h);
   set_position(Gtk::WIN_POS_CENTER);

                                    //hpaned ////////////////////////////
   hpaned.set_position(int (w / GUI_MARGIN_DIVIDER));

   ///////////////////// initializing ///////////////////////////////////

   show_all_children();

   treeview.hide();

   if (rand() < RAND_MAX / 10)
   { gui_about.run (); }                   

   Treat_Options();
}


dis::gui::gui_gtk::~gui_gtk() 
{
   std::cout << "Destructorrr gui_gtk" << "\n";

   tm->Set_Disassembly(0);

   if (analysis)
   {
    delete (analysis);   
   }

   delete (navigation_column);
}

void
dis::gui::gui_gtk::Treat_Options()
{
   if (options->input_file_name != "")
   {
    std::cout << "Opening file " << options->input_file_name << " and commencing analysis..." << "\n";

    existing_disassembly = Determine_Existing_Disassembly(options->input_file_name);       

    Start_Analysis();
   }
}

void
dis::gui::gui_gtk::Set_Toolbar(int active)
{
   switch (active)
   { 
    case GUI_ACTIVE_DEFAULT:                 // default: no details active
        {
         tb_save->set_sensitive(false);
         break;
        }
    case GUI_ACTIVE_BUSY:                    // analysis has begun
        {
         tb_save->set_sensitive(true);
         break;
        }  
   }
}

void 
dis::gui::gui_gtk::Start_Save()
{   

    ////////////////////////////////////////

    options->type_of_save = DISASSEMBLY_SAVE_DATABASE;

    if (options->output_file_name.find(GUI_FILE_SAVER_DIS) != string::npos)
    { options->type_of_save = DISASSEMBLY_SAVE_DATABASE; }

    else if (options->output_file_name.find(GUI_FILE_SAVER_LST) != string::npos)
    { options->type_of_save = DISASSEMBLY_SAVE_LISTING; }    

    if (analysis) 
    { analysis->Callback_Save((&(options->output_file_name)), options->type_of_save); }
}

void 
dis::gui::gui_gtk::Start_Open()
{   
    ////////////////////////////////////////

    Prepare_Analysis();

    if (!analysis) {analysis = new Analysis (&main_gui, options);}

    analysis->Callback_Open();

    tm->Set_Disassembly(analysis->Callback_Get_Disassembly());
    treeview.set_model(tm);
}


void 
dis::gui::gui_gtk::Prepare_Analysis()
{                                           

    ////////////////////////////////////////


    main_gui.sb = &statusbar;
    main_gui.sb_context_id = statusbar.get_context_id("Ready");
    main_gui.m  = mw_m;

    treeview.show();

    Set_Toolbar(GUI_ACTIVE_BUSY);

    while(mw_m->events_pending()) mw_m->iteration();
}

void 
dis::gui::gui_gtk::Shutdown_Analysis()
{   
    ////////////////////////////////////////

    Set_Toolbar(GUI_ACTIVE_DEFAULT);
}

bool
dis::gui::gui_gtk::Determine_Existing_Disassembly(string filename)
{
  bool      result;

  //////////////////////////////

  result = false;

  if (options->input_file_name.find(GUI_FILE_SAVER_DIS) != string::npos)
  {
   result = true;
  }

  return result;
}

int
dis::gui::gui_gtk::Get_File_Name()
{  
  
  int       result;

  /////////////////////////////////////:

  existing_disassembly = false;

  fs_selector dlg_open ("What file do you want looked at?"); 

  dlg_open.set_transient_for(*this);
  dlg_open.show_all();

  result = dlg_open.run();

  options->input_file_name = dlg_open.get_filename();

  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {                                         
      existing_disassembly = Determine_Existing_Disassembly(options->input_file_name);

      break;
    }
    default:
    {
     return RET_ERR_FILE_INPUT;
    }                          
  }                            

  return RET_OK;
}

void 
dis::gui::gui_gtk::Start_Analysis()
{
  if (existing_disassembly != true)
  {
   Start_New_Analysis();       
  }
  else 
  {
   Start_Open();
  }   
}     

void 
dis::gui::gui_gtk::Start_New_Analysis()
{   
    int ret_code;

    ////////////////////////////////////////

    Prepare_Analysis();

    if (analysis)   { delete (analysis); }

    analysis = new Analysis (&main_gui, options);     

    ret_code = analysis->Perform();

    tm->Set_Disassembly(analysis->Callback_Get_Disassembly());
    treeview.set_model(tm);

    if (ret_code != RET_OK)
    { 
     cout << "Error : " << ret_code << "\n";
     Set_Toolbar(GUI_ACTIVE_DEFAULT);
    }
}                                          

void
dis::gui::gui_gtk::Callback_Open()
{  
  
  int       result;

  /////////////////////////////////////:

  result = Get_File_Name();

  switch(result)
  {
    case(RET_OK):
    {                                         
      Start_Analysis();       

      break;
    }
  }                                       
}

void
dis::gui::gui_gtk::Callback_Analysis_Quit()
{  
  std::cout << "hide Mainwindow" << "\n";
  
  hide ();
}

void
dis::gui::gui_gtk::Callback_Help_About()
{  
  gui_about.run ();
}

void
dis::gui::gui_gtk::Callback_Cursor_Changed()
{ 
  uint                    row,
                          i;         // index

  dis::Disassembly_Node  *dn;

  string                  s;

  dis::Reference         *ref;

  ///////////////////////////////////////////////////

  if (!analysis) { return; }

  // first, get currently selected row
  treeview.get_cursor(navigation_path, navigation_column);                   
  if (navigation_path.get_depth() == 0)
    { return; }
  row = navigation_path[0]; 

  // and from there, find the corresponding Disassmbly_Node
  dn = analysis->Callback_Get_nth_Row(row);
  // after which e can fill in the details for this node

  //offset
  page_detail.ed_file_offset->set_text(u.int_to_hexstring(dn->file_offset));
  page_detail.ed_memory_offset->set_text(u.int_to_hexstring(dn->memory_offset));

  // type
  if (dn->type <= NODE_TYPE_MAX)
  { page_detail.ed_type->set_text(node_type[dn->type]); }
  else { page_detail.ed_type->set_text(""); }

  // status
  if (dn->status <= NODE_STATUS_MAX)
  { page_detail.ed_status->set_text(node_status[dn->status]); }
  else { page_detail.ed_status->set_text(""); }

  // label
  if (dn->label)
  { page_detail.tb_label->set_text(dn->label); }
  else { page_detail.tb_label->set_text(""); }

  // section                              
  analysis->Callback_Translate_Section(dn->section, &s);
  page_detail.ed_section->set_text(s);

  // opcodes
  analysis->Callback_Translate_Opcodes(dn, false, &s, GUI_MAX_BYTES_PER_ROW);
  page_detail.tb_opcode->set_text(s);

  // comment
  if (dn->comment)
  { page_detail.tb_comment->set_text(dn->comment); }
  else { page_detail.tb_comment->set_text(""); }

  // references in: see if this memory location is pointed to by something else
  page_detail.ls_ref_in->clear();
  if (analysis->Callback_Is_Valid_Reference(&(dn->ref_in)))
  {
   ref = &(*(dn->ref_in));

   for (i = 0; i < ref->ref_out.size(); i++)
   {
    Gtk::TreeModel::Row row = *(page_detail.ls_ref_in->append());               
    row[page_detail.mc_ref_in.col_offset] = u.int_to_hexstring(ref->ref_out[i]);
    row[page_detail.mc_ref_in.col_offset_memory] = ref->ref_out[i];
    i++;
   }
  }                                                                                                

  // references out: handle the memory location this instruction is pointing to 
  page_detail.ls_ref_out->clear();
  if (analysis->Callback_Is_Valid_Reference(&(dn->ref_out)))
  {
   ref = &(*(dn->ref_out));

   Gtk::TreeModel::Row row = *(page_detail.ls_ref_out->append());

   row[page_detail.mc_ref_out.col_offset] = u.int_to_hexstring(ref->memory_offset);
   row[page_detail.mc_ref_out.col_offset_memory] = ref->memory_offset;

   if (ref->type <= REFERENCE_TYPE_MAX)
    { row[page_detail.mc_ref_out.col_type] = reference_type[ref->type];}
   else 
    { row[page_detail.mc_ref_out.col_type] = "";}

   row[page_detail.mc_ref_out.col_label] = ref->label;
  }                                            
}

void
dis::gui::gui_gtk::Callback_Save()
{                             
  int       result;

  /////////////////////////////////////:

  fs_saver dlg_save ("Save Analysis As ...", options->input_file_name.c_str());

  dlg_save.set_transient_for(*this);
  dlg_save.show_all();

  result = dlg_save.run();

  dlg_save.hide();

  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      options->output_file_name = dlg_save.get_filename();

      Start_Save();
      break;
    }
  }                
}

bool
dis::gui::gui_gtk::Callback_Navigation_References_In(GdkEventButton *event)
{
  NAVIGATION_OPTIONS      no;
  int                     o, r;                         // offset, row

  Gtk::TreeModel::iterator         iter;                // gtk iterator
  Gtk::TreeModel::Row              row;                 // row in the gui
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;    // row selected in the gui
  Gtk::TreeModel::Path             p;                   // path defining a row

  ////////////////////////////////////////

  if (event->type == GDK_2BUTTON_PRESS)
    {
      if (analysis)
      {   
       // first, get currently selected row
       refTreeSelection = page_detail.tv_references_in->get_selection();
       iter = refTreeSelection->get_selected();
       if (!iter) { return false; }                     // nothing is selected
       row = *iter;
       o = row[page_detail.mc_ref_in.col_offset_memory];

       navigation_path.clear();                      
       r = analysis->Callback_Get_Row_From_Offset(o);

       if (r >= 0)
       {
        navigation_path.push_back(r);
        treeview.scroll_to_row(navigation_path, 0.5); treeview.set_cursor(navigation_path);
       }

       return true;
      }
    }

  return false;
}

bool
dis::gui::gui_gtk::Callback_Navigation_References_Out(GdkEventButton *event)
{
  NAVIGATION_OPTIONS      no;
  int                     o, r;                         // offset, row

  Gtk::TreeModel::iterator         iter;                // gtk iterator
  Gtk::TreeModel::Row              row;                 // row in the gui
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;    // row selected in the gui
  Gtk::TreeModel::Path             p;                   // path defining a row

  ////////////////////////////////////////

  if (event->type == GDK_2BUTTON_PRESS)
    {
      if (analysis)
      {   
       // first, get currently selected row
       refTreeSelection = page_detail.tv_references_out->get_selection();
       iter = refTreeSelection->get_selected();
       if (!iter) { return false; }                     // nothing is selected
       row = *iter;
       o = row[page_detail.mc_ref_out.col_offset_memory];

       navigation_path.clear();                      
       r = analysis->Callback_Get_Row_From_Offset(o);

       if (r >= 0)
       {
        navigation_path.push_back(r);
        treeview.scroll_to_row(navigation_path, 0.5); treeview.set_cursor(navigation_path);
       }

       return true;
      }
  }

  return false;
}

void
dis::gui::gui_gtk::Callback_Switch_Tab(GtkNotebookPage* page, guint page_num)
{   
    Extra                   *extra;

    ////////////////////////////////////
    
    if ((page_num == extra_page) && (analysis))
    {
     extra = analysis->Callback_Get_Extra();

     if (extra)
     { page_extra.Fill_Extra_Info(extra); }
    }
}

void
dis::gui::gui_gtk::Callback_Navigation()
{   
    NAVIGATION_OPTIONS      no;
    int                     row;

    ////////////////////////////////////
    
    if (analysis)
    {
     page_navigator.Get_Navigation(&no);

     navigation_path.clear(); 

     switch (no.nav_id) 
     {                                               
      case NAVIGATION_TYPE_DIRECT_OFFSET:      
        {
           row = analysis->Callback_Get_Row_From_Offset(no.nav_direct_offset);

           if (row >= 0)
           {
            navigation_path.push_back(row);

            treeview.scroll_to_row(navigation_path, 0.5); treeview.set_cursor(navigation_path);
           }

           break;
        }

       case NAVIGATION_TYPE_SEARCH_BYTE:      
         {  
            treeview.get_cursor(navigation_path, navigation_column);                   

            if (navigation_path.get_depth() == 0)
            { row = 0; }
            else 
            { row = navigation_path[0]; }

            row = analysis->Callback_Navigation_Search_Byte(row, &no.nav_search_byte, no.nav_direction, no.nav_wrap);

            navigation_path.clear(); navigation_path.push_back(row); 
            treeview.scroll_to_row(navigation_path, 0.5); treeview.set_cursor(navigation_path);
             
            break;
         }

        default:
        {
            cout << "unknown navigation type: " << no.nav_id << "\n";
        }

       }
    }
}


void 
dis::gui::gui_gtk::Not_Yet_Implemented ()
{
    Gtk::MessageDialog md(*this, "Not yet implemented ...");

    md.run();                                            
}


#endif
