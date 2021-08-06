## Process this file with automake to produce Makefile.in

SUBDIRS = libmem libutil liblog libdis prep

INCLUDES = \
	-I$(top_srcdir)/libdis 

AM_CXXFLAGS = \
	$(GTK_CFLAGS) \
	$(CFLAGS) \
	$(GUI_GTK) \
        -DGNOME_LOCALEDIR=\""$(datadir)/locale"\" \
	-DGNOME_ICONDIR=\""$(datadir)/pixmaps"\" \
        -DDISASSEMBLER_ICONDIR=\""$(datadir)/pixmaps/$(PACKAGE)"\" \
         #-DDEBUG \
         #profiling -pg \
	 -Wall -g

bin_PROGRAMS = disassembler

disassembler_SOURCES = \
	gui_gtk.hh gui_gtk.cc \
	no_gui.hh no_gui.cc \
	\
	gui_file_type.hh gui_file_type.cc \
	gui_file_selector.hh gui_file_selector.cc \
	gui_file_saver.hh gui_file_saver.cc \
	gui_file_save_as.hh gui_file_save_as.cc \
	gui_detail.hh gui_detail.cc \
	gui_extra.hh gui_extra.cc \
	gui_navigator.hh gui_navigator.cc \
	gui_disassembly_treemodel.hh gui_disassembly_treemodel.cc \
	gui_about.hh gui_about.cc \
	gui_commands.hh \
	\
	disassembler.cc
	
disassembler_LDADD = \
         libdis/libdisassembly.la \
	$(GTK_LIBS)
