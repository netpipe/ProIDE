/*
bin2c - produce C code from binary input
Copyright (C) 2001 Ed Cashin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<ctype.h>
#include	<errno.h>

#define	DIE_OUTPUT() do {						\
    fprintf(stderr, PROGNAME " Error: could not print to output: %s\n",	\
	    strerror(errno));						\
    exit(EXIT_FAILURE);							\
} while (0)    

typedef struct program_options {
    char	*inname;
    char	*arrayname;
} options;

void bin2c(options *o, FILE *in, FILE *out)
{
    int		c = 0;
    int		i;
    const int	cols	 = 6;
    int		len	 = 0;

    if (fprintf(out, "char	%s[]	 = {\n", o->arrayname) < 0)
      DIE_OUTPUT();
    do {
      for (i = 0; i < cols; ++i) {
	if ( (c = getc(in)) == EOF )
	  break;
	if (fprintf(out, "0x%x,\t", c) == -1)
	  DIE_OUTPUT();
	++len;
      }
      putc('\n', out);
    } while (c != EOF);
    
    if (fprintf(out,
		"};\nconst int\t%s_len\t = %d;\n", o->arrayname, len) < 0)
      DIE_OUTPUT();
}    

void usage(void)
{
    puts("usage:\n"
	 "  " PROGNAME " [-n arrayname] [input_file]");
}

void parse_commandline(options *o, int argc, char *argv[])
{
    int		c;

    while ( (c = getopt(argc, argv, "hn:")) != -1) {
      switch (c) {
	case 'h':
	  usage();
	  exit(EXIT_SUCCESS);
	  break;
	case 'n':
	  o->arrayname	 = optarg;
	  break;
	case '?':
	  if (isprint(optopt))
	    fprintf(stderr, "Error: unknown option `-%c'.\n", optopt);
	  else
	    fprintf(stderr,
		    "Error: unknown option character `\\x%x'.\n", optopt);
	  usage();
	  exit(EXIT_FAILURE);
	  break;
	default:
	  abort();		/* this shouldn't happen */
	  break;
      }	/* end switch */
    } /* end while getopt */

    if (optind < argc)
      o->inname	 = argv[optind++];
}

int main(int argc, char *argv[])
{
    options	o	 = { "-", PROGNAME "_data" };
    FILE	*in;

    parse_commandline(&o, argc, argv);
    
    if (! strcmp(o.inname, "-")) {
      in	 = stdin;
    } else {
      if (! (in = fopen(o.inname, "r")) ) {
	fprintf(stderr, PROGNAME " Error: could not open file (%s): %s\n",
		o.inname, strerror(errno));
	exit(EXIT_FAILURE);
      }
    }
    bin2c(&o, in, stdout);

    fclose(in);
    return 0;
}

