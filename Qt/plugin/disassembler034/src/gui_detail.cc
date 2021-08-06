/*
 * gui_detail: showing the details of one instruction
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_GTK                               
 
#include "gui_detail.hh"

dis::gui::gui_detail::gui_detail()
{
   int                          l, t, w, h,              // left, top, width, height
                                lt;                      // total length

   Glib::RefPtr<Gdk::Screen>    screen;

   /////////////////////////////////////////////////////////////////////

   // screen size
   screen = get_screen();
   l = screen->get_width(); 
   t = screen->get_height();                           

   w = (l - (l / GUI_MARGIN_RIGHT)) / 25;        
   h = (t - (t / GUI_MARGIN_BOTTOM)) / 20;

   l = l - (l / GUI_MARGIN_RIGHT);        
   t = t - (t / GUI_MARGIN_BOTTOM);

   lt = int (l - (l / GUI_MARGIN_DIVIDER) - (GUI_MARGIN_RIGHT * 1.5)); // total width for this pane   

   l = int ((l - (l / GUI_MARGIN_DIVIDER)) / 2);    // pixels per column
   t = t / 25;                                      // pixels per row

   // labels
   lb_file_offset = Gtk::manage(new class Gtk::Label("file offset"));   lb_file_offset->set_alignment(0.5,0.5);
   lb_file_offset->set_padding(0,0);                                    lb_file_offset->set_justify(Gtk::JUSTIFY_LEFT);
   lb_file_offset->set_line_wrap(false);                                lb_file_offset->set_use_markup(false);
   lb_file_offset->set_selectable(false);

   lb_memory_offset = Gtk::manage(new class Gtk::Label("memory offset")); lb_memory_offset->set_alignment(0.5,0.5);
   lb_memory_offset->set_padding(0,0);                                  lb_memory_offset->set_justify(Gtk::JUSTIFY_LEFT);
   lb_memory_offset->set_line_wrap(false);                              lb_memory_offset->set_use_markup(false);
   lb_memory_offset->set_selectable(false);

   lb_type = Gtk::manage(new class Gtk::Label("type "));   lb_type->set_alignment(0.5,0.5);
   lb_type->set_padding(0,0);                              lb_type->set_justify(Gtk::JUSTIFY_LEFT);
   lb_type->set_line_wrap(false);                          lb_type->set_use_markup(false);
   lb_type->set_selectable(false);

   lb_status = Gtk::manage(new class Gtk::Label("status ")); lb_status->set_alignment(0.5,0.5);
   lb_status->set_padding(0,0);                            lb_status->set_justify(Gtk::JUSTIFY_LEFT);
   lb_status->set_line_wrap(false);                        lb_status->set_use_markup(false);
   lb_status->set_selectable(false);

   lb_opcode = Gtk::manage(new class Gtk::Label("opcodes ")); lb_opcode->set_alignment(0.5,0.5);
   lb_opcode->set_padding(0,0);                            lb_opcode->set_justify(Gtk::JUSTIFY_LEFT);
   lb_opcode->set_line_wrap(false);                        lb_opcode->set_use_markup(false);
   lb_opcode->set_selectable(false);

   lb_comment = Gtk::manage(new class Gtk::Label("comment "));lb_comment->set_alignment(0.5,0.5);
   lb_comment->set_padding(0,0);                           lb_comment->set_justify(Gtk::JUSTIFY_LEFT);
   lb_comment->set_line_wrap(false);                       lb_comment->set_use_markup(false);
   lb_comment->set_selectable(false);

   lb_section = Gtk::manage(new class Gtk::Label("section "));lb_section->set_alignment(0.5,0.5);
   lb_section->set_padding(0,0);                           lb_section->set_justify(Gtk::JUSTIFY_LEFT);
   lb_section->set_line_wrap(false);                       lb_section->set_use_markup(false);
   lb_section->set_selectable(false);                       

   lb_label = Gtk::manage(new class Gtk::Label("label ")); lb_label->set_alignment(0.5,0.5);
   lb_label->set_padding(0,0);                             lb_label->set_justify(Gtk::JUSTIFY_LEFT);
   lb_label->set_line_wrap(false);                         lb_label->set_use_markup(false);
   lb_label->set_selectable(false);

   lb_references_in = Gtk::manage(new class Gtk::Label("Referencing nodes "));
   lb_references_in->set_alignment(0.5,0.5);
   lb_references_in->set_padding(0,0);                            lb_references_in->set_justify(Gtk::JUSTIFY_LEFT);
   lb_references_in->set_line_wrap(false);                        lb_references_in->set_use_markup(false);
   lb_references_in->set_selectable(false);

   lb_references_out = Gtk::manage(new class Gtk::Label("Referenced nodes "));
   lb_references_out->set_alignment(0.5,0.5);
   lb_references_out->set_padding(0,0);                           lb_references_out->set_justify(Gtk::JUSTIFY_LEFT);
   lb_references_out->set_line_wrap(false);                       lb_references_out->set_use_markup(false);
   lb_references_out->set_selectable(false);

   // edit fields
   ed_file_offset = Gtk::manage(new class Gtk::Entry());   ed_file_offset->set_flags(Gtk::CAN_FOCUS);
   ed_file_offset->set_visibility(true);                   ed_file_offset->set_editable(false);
   ed_file_offset->set_max_length(0);                      ed_file_offset->set_text("");
   ed_file_offset->set_has_frame(true);                    ed_file_offset->set_activates_default(false);
   ed_file_offset->set_size_request(int (w * 3.75), -1);

   ed_memory_offset = Gtk::manage(new class Gtk::Entry()); ed_memory_offset->set_flags(Gtk::CAN_FOCUS);
   ed_memory_offset->set_visibility(true);                 ed_memory_offset->set_editable(false);
   ed_memory_offset->set_max_length(0);                    ed_memory_offset->set_text("");
   ed_memory_offset->set_has_frame(true);                  ed_memory_offset->set_activates_default(false);
   ed_memory_offset->set_size_request(int (w * 3.75), -1);

   ed_type = Gtk::manage(new class Gtk::Entry());          ed_type->set_flags(Gtk::CAN_FOCUS);
   ed_type->set_visibility(true);                          ed_type->set_editable(false);
   ed_type->set_max_length(0);                             ed_type->set_text("");
   ed_type->set_has_frame(true);                           ed_type->set_activates_default(false);
   ed_type->set_size_request(int (w * 3.75), -1);

   ed_status = Gtk::manage(new class Gtk::Entry());        ed_status->set_flags(Gtk::CAN_FOCUS);
   ed_status->set_visibility(true);                        ed_status->set_editable(false);
   ed_status->set_max_length(0);                           ed_status->set_text("");
   ed_status->set_has_frame(true);                         ed_status->set_activates_default(false);
   ed_status->set_size_request(int (w * 3.75), -1);

   ed_section = Gtk::manage(new class Gtk::Entry());        ed_section->set_flags(Gtk::CAN_FOCUS);
   ed_section->set_visibility(true);                        ed_section->set_editable(false);
   ed_section->set_max_length(0);                           ed_section->set_text("");
   ed_section->set_has_frame(true);                         ed_section->set_activates_default(false);
   ed_section->set_size_request(int (w * 3.75), -1);

   // text buffers and scrolledwindows
   tb_comment = Gtk::TextBuffer::create();
   tb_opcode  = Gtk::TextBuffer::create();
   tb_label   = Gtk::TextBuffer::create();

   sw_opcode  = Gtk::manage(new class Gtk::ScrolledWindow()); 
   sw_comment = Gtk::manage(new class Gtk::ScrolledWindow()); 
   sw_label   = Gtk::manage(new class Gtk::ScrolledWindow()); 
   sw_ref_in  = Gtk::manage(new class Gtk::ScrolledWindow()); 
   sw_ref_out  = Gtk::manage(new class Gtk::ScrolledWindow()); 

   te_opcode = Gtk::manage(new class Gtk::TextView());     te_opcode->set_flags(Gtk::CAN_FOCUS);
   te_opcode->set_editable(false);                         te_opcode->set_size_request(int (w * 3.75), -1);
   te_opcode->set_buffer(tb_opcode);                       

   te_label = Gtk::manage(new class Gtk::TextView());      te_label->set_flags(Gtk::CAN_FOCUS);
   te_label->set_editable(false);                          te_label->set_size_request(int (w * 3.75), -1);
   te_label->set_buffer(tb_label);                         

   te_comment = Gtk::manage(new class Gtk::TextView());    te_comment->set_flags(Gtk::CAN_FOCUS);
   te_comment->set_editable(false);                        te_comment->set_size_request(int (w * 3.75), -1);
   te_comment->set_buffer(tb_comment);            

   sw_opcode->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   sw_label->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   sw_comment->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   sw_ref_in->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
   sw_ref_out->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

   tv_references_in = Gtk::manage(new class Gtk::TreeView()); 
   tv_references_in->set_flags(Gtk::CAN_FOCUS);
   tv_references_in->set_size_request(lt, 3 * t);    
   
   tv_references_out = Gtk::manage(new class Gtk::TreeView()); 
   tv_references_out->set_flags(Gtk::CAN_FOCUS);
   tv_references_out->set_size_request(lt, 3 * t);   

   ls_ref_in = Gtk::ListStore::create(mc_ref_in);
   tv_references_in->set_model(ls_ref_in);
   tv_references_in->append_column("offset", mc_ref_in.col_offset);
   tv_references_in->append_column("offset memory", mc_ref_in.col_offset_memory);
   tv_references_in->get_column(1)->set_visible(false);

   ls_ref_out = Gtk::ListStore::create(mc_ref_out);
   tv_references_out->set_model(ls_ref_out);
   tv_references_out->append_column("offset", mc_ref_out.col_offset);
   tv_references_out->append_column("type", mc_ref_out.col_type);
   tv_references_out->append_column("label", mc_ref_out.col_label);
   tv_references_out->append_column("offset memory", mc_ref_out.col_offset_memory);
   tv_references_out->get_column(3)->set_visible(false);
   
   sw_opcode->add(*te_opcode);        
   sw_label->add(*te_label);        
   sw_comment->add(*te_comment);        
   sw_ref_in->add(*tv_references_in);        
   sw_ref_out->add(*tv_references_out);        

   // grouping of widgets
   fixed = Gtk::manage(new class Gtk::Fixed());

   fixed->put(*lb_file_offset,      GUI_MARGIN_LEFT,            (int) GUI_MARGIN_TOP);
   fixed->put(*lb_memory_offset,    l,                          (int) GUI_MARGIN_TOP);
   fixed->put(*ed_file_offset,      GUI_MARGIN_LEFT,            (int) (GUI_MARGIN_TOP + (0.75 * t)));
   fixed->put(*ed_memory_offset,    l,                          (int) (GUI_MARGIN_TOP + (0.75 * t)));

   fixed->put(*lb_type,             GUI_MARGIN_LEFT,            (int) (GUI_MARGIN_TOP + (2 * t)));
   fixed->put(*lb_status,           l,                          (int) (GUI_MARGIN_TOP + (2 * t)));
   fixed->put(*ed_type,             (int) (GUI_MARGIN_LEFT),    (int) (GUI_MARGIN_TOP + (2.75 * t)));
   fixed->put(*ed_status,           l,                          (int) (GUI_MARGIN_TOP + (2.75 * t)));

   fixed->put(*lb_label,            GUI_MARGIN_LEFT,            (int) (GUI_MARGIN_TOP + (4 * t)));
   fixed->put(*lb_opcode,           l,                          (int) (GUI_MARGIN_TOP + (4 * t)));
   fixed->put(*sw_label,            (int) (GUI_MARGIN_LEFT),    (int) (GUI_MARGIN_TOP + (4.75 * t)));
   fixed->put(*sw_opcode,           l,                          (int) (GUI_MARGIN_TOP + (4.75 * t)));
   
   fixed->put(*lb_comment,          (int) (GUI_MARGIN_LEFT),    (int) (GUI_MARGIN_TOP + (8 * t)));
   fixed->put(*sw_comment,          (int) (GUI_MARGIN_LEFT),    (int) (GUI_MARGIN_TOP + (8.75 * t)));

   fixed->put(*lb_section,          l,                          (int) (GUI_MARGIN_TOP + (8    * t)));
   fixed->put(*ed_section,          l,                          (int) (GUI_MARGIN_TOP + (8.75 * t)));

   fixed->put(*lb_references_in,    (int) (GUI_MARGIN_LEFT),    (int) (GUI_MARGIN_TOP + (12 * t)));
   fixed->put(*sw_ref_in,           (int) (GUI_MARGIN_LEFT),    (int) (GUI_MARGIN_TOP + (12.75 * t)));

   fixed->put(*lb_references_out,   (int) (GUI_MARGIN_LEFT),    (int) (GUI_MARGIN_TOP + (16 * t)));
   fixed->put(*sw_ref_out,          (int) (GUI_MARGIN_LEFT),    (int) (GUI_MARGIN_TOP + (16.75 * t)));

   pack_start(*fixed, Gtk::PACK_EXPAND_WIDGET, 0);

   show();
}

dis::gui::gui_detail::~gui_detail()
{
}

#endif
