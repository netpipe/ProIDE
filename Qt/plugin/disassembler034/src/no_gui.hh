/*
 *       no_gui.hh : for disassembling without a gui
 *
 +       author: Danny Van Elsen
 */


#ifndef NO_GUI_HH
#define NO_GUI_HH

using namespace std;

#include <iostream>

#include "libdis/analysis.hh"   

namespace dis {
	namespace gui {
		class no_gui;
	}
}

class dis::gui::no_gui
{

public:
	no_gui(Disassembly_Options *d_o);
	virtual ~no_gui();

    void Run();                                 // execute the disassembly

protected:
    

private:

    void Start_Analysis();                      // new analysis
    void Start_New_Analysis();                  // new file to analyze

    void Start_Open();                          // read the analysis from a file

    bool Determine_Existing_Disassembly(string filename);
                                                // whether or not this is an existing disassembly

    ////////////////////////////////////////////////////////////////////////////////////

    dis::Analysis               *analysis;            // will execute the disassembly

    Disassembly_Options         *options;             // user specified options

    bool                         existing_disassembly;
};

#endif   

