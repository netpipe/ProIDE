/*
 *       gui_about.cc : 'About' window in gtk gui
 *
 +       author: Danny Van Elsen
 */

#ifdef HAVE_GTK                               

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <gui_about.hh>

                    
dis::gui::gui_about::gui_about()
{  
   std::cout << "Constructor About Window" << "\n";

   set_name("Disassembler for linux");
   set_version(VERSION);
   set_copyright("Danny Van Elsen");
   set_comments("This is a one man project, at least for the time being; so, if you have got this far, and have managed \
to compile and execute this piece of coding : sending an acknowledgment email or postcard to the author \
would be a wonderful way to encourage him to go on ... :-) Help would be appreciated especially in adding a new gui \
or recognizing new executable formats ... \ ");


   set_website("http://users.skynet.be/bk329156/disassembler.html");
   set_website_label("velsd website");

  
   list_authors.push_back("Danny Van Elsen = velsd@users.sourceforge.net");
   set_authors(list_authors);    
}


dis::gui::gui_about::~gui_about()
{
}

#endif



