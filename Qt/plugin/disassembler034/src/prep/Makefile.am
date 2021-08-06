INCLUDES = \
        $(GTK_CFLAGS) 
	
AM_CXXFLAGS = \
	 -Wall -g 
#-DLOGGING                               
#profiling -pg

bin_PROGRAMS = read_imports

read_imports_SOURCES = \
	read_imports.hh read_imports.cc \
	../libutil/utilities.hh ../libutil/utilities.cc \
	../libmem/memory_pool.hh ../libmem/memory_pool.cc \
	../libdis/disassembly.hh ../libdis/function.hh ../libdis/reference.hh
