## Process this file with automake to produce Makefile.in

SUBDIRS = \
	src

install-data-am:
	mkdir -p $(datadir)/disass/disassembly_winpe
	cp share/disass/disassembly_winpe/imports $(datadir)/disass/disassembly_winpe

EXTRA_DIST = \
	intltool-extract.in intltool-merge.in intltool-update.in 
	
dist-hook:
	## Copy all the spec files. Of cource, only one is actually used
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done
	
doxygen:
	doxygen Doxyfile
