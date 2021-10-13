# bin2c - produce C code from binary input
# Copyright (C) 2001 Ed Cashin
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
.PHONY	: clean
CC	 = gcc
PROG	 = bin2c
OBJ	 = bin2c.o
CFLAGS	 = -g -Wall -O2
LDFLAGS	 = 
LIBS	 = 
DEFS	 = -DPROGNAME='"$(PROG)"'

$(PROG) : $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

%.o	: %.c makefile
	$(CC) $(CFLAGS) $(DEFS) -o $@ -c $<

clean	: 
	rm -f $(OBJ) $(PROG) 

realclean : clean
	rm -f *~ core a.out
