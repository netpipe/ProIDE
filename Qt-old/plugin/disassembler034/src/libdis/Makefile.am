#SUBDIRS                 = private

INCLUDES = \
	$(GTK_CFLAGS) \
	$(CFLAGS) \
	$(GUI_GTK)
	
AM_CXXFLAGS = \
	 -Wall -g  -fpermissive
#-DLOGGING                               
#profiling -pg

lib_LTLIBRARIES = libdisassembly.la

libdisassembly_la_SOURCES =							\
	analysis.cc analysis.hh							\
	disassembly.cc disassembly.hh						\
	disassembly_options.cc disassembly_options.hh 				\
	disassembly_intel.cc disassembly_intel.hh disassembly_intel_opcodes.inc \
	disassembly_intel_raw.cc disassembly_intel_raw.hh  			\
	disassembly_intel_winpe.cc disassembly_intel_winpe.hh 			\
	disassembly_intel_elf.cc disassembly_intel_elf.hh 			\
	disassembly_node.hh  							\
	file_formats.hh reference.hh return_codes.hh

libdisassembly_la_LIBADD =							\
	../libmem/libmem.la ../libutil/libutil.la ../liblog/liblog.la

