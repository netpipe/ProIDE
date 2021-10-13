/*
 * doc.hh: documentation for the program
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef DOC_HH
#define DOC_HH



/*! 
\mainpage Disassembler for linux
* \section intro_sec Introduction
* The purpose of this program is to provide a disassembler on linux systems. This is meant to be a gui driven process,
* but a command line interface is also present.
* \section using_sec Using the Disassembler
* Instructions on the program's usage: \ref user1
* \section programming_sec Programming the Disassembler
* Instructions on the inner workings of the program and how to add functionality, for instance 
* in order to disassemble new binaries: \ref programming1
*/

/*! \page programming1 For The Programmer
  
  This project is still a one man project... I am a keen programmer, have succeeded of making this my job as well, and 
  somewhere in 2003, I started thinking of perhaps trying at home to program something useful that could actually be used 
  by other people. For various reasons, the challenge of writing a disassembler presented itself, and stuck in my mind. 

  As with any other hobby project (of mine, anyway), progress is slow... Professional life, travel, marriage, 
  kids, there are so many reasons I wish there were 50 hours in a day... But over the years, I think the result is beginning
  to be a useful program. Other people are welcome, however, to add functionality, and therefore I also came up with this 
  documentation. 

  The diassembler was written in c++, presently using gcc 4.0.2. I chose c++ because I started with the idea that most binary 
  formats will share a lot of logic, and that the inheritance mechanisms of c++ would be well suited to exploit this. For instance,
  all executables for the intel 80x86 platform will share the same opcodes, so decoding can be done with one method. Reading
  in the actual files (windows PE, DOS COM, linux ELF and so on) can be performed by virtual functions of a corresponding
  class, however.                                                                                          

  I will now first discuss the graphical user interface, and then the elements that are represented by this gui.

  <HR>
  
  \section gui The Gui
  I've tried to make the gui as independent of the actual disassembly logic as possible. GTK / Gnome is my personal 
  favourite, but lots of other people won't agree, so I at least built in the possibility of adding interaction with 
  another gui framework. 

  The program allows for options to be passed by a command line interface; 'disassembler -h' shows the usage. Some future 
  version of the Makefile will check for the libraries found on the current system, and only build the gui that is really
  present. For now, however, the Makefile still expects to encounter relatively recent versions of GTK / Gnome, and the 
  compilation will fail if these constraints aren't met.

  disassembler.cc is the source file for the main() function of the program: it constructs a util::Command_Line 
  object, determines which gui was asked for, and starts up the appropriate interface.
  
  \subsection gui_sub1 GTK.

  Most of what you'll find in gui_gtk.cc is pretty ordinary stuff: opening and saving files, navigating throught the disassembled 
  file, responding to cursor events, and so on. 

  dis::gui::gui_gtk::tm is special: the treemodel for the disassembly. The disassembled instructions of the binary input file are
  represented in a treelike structure - it's actually more like a long sequence of rows, one for each instruction - and the
  treemodel is, well, the model for the tree. It has methods like dis::gui::Gui_Disassembly_TreeModel::iter_next_vfunc that
  returns the 'next' node in the tree, i.e. the next instuction, and dis::gui::Gui_Disassembly_TreeModel::get_value_vfunc 
  that shows the contents of a particular node. For further details, see gui_disassembly_model.cc .

  Letting this treemodel get its contents from the actual disassembly data structure makes for a very fast gui indeed. I 
  originally intended the disassembly logic to happen in a separate thread, so as not to interfere with the responsiveness 
  of the gui; but seeing that this treemodel provided for an incredibly fast navigation, I just let the initial disassembly 
  process go ahead, and only start up the gui when the results were calculated.
  
  \subsection gui_sub2 Command Line.
  
  All program functionality is meant to be reachable by the command line. In order for this to work, the 
  cl object of type util::Command_Line is created. This class provides methods that indicate legal options, and
  also imposes restrictions on what options can be legally combined.
   
  <HR>
  
  \section disassembly The Disassembler

  \subsection dis_ana1 The Analysis.

  Before beginning to disassemble an executable, there are some administrative tasks to perform: asking which file to look at,
  and confirming standard options for example. This is what the dis::Analysis object does. It is also the Analysis that
  communicates with the gui, so callback_xxx functions are provided to collect disassembly information, among other things
  in order to save the final results to a file.

  The real work, however, is done in a number of phases by the dis::Disassembly.

  \subsection dis_phases Phases in The Disassembly.

<ul>    
<li> First comes dis::Disassembly::Phase_1a_File: this method reads in the executable that is to be disassembled, and sets up
  a number of elementary statistics, such as file size, file type, number of sections with code and data, and so on. This 
  initial treatment needs to know about the specific properties of the binary format of the inputfile, and therefore is
  performed by functions of classes that have a one to one correspondance with the known input formats: windows PE files are
  read by a dis::Disassembly_WinPE::Phase_1a_File method, but linux ELF formats are dealt with by a 
  dis::Disassembly_Elf::Phase_1a_File routine.

  This phase is the only one where we want to know about the peculiarities of the input file format, so it is here that we
  also read in information about the imported functions.

<li>   dis::Disassembly::Phase_1b_Imports is next in line. Its purpose is to read in the definitions of all functions that are
  known to the disassembler, and can possible be called in the input executable. The idea being of course to append labels 
  to the disassembled instructions that are later recognized to call these functions.

<li>  After this comes the first phase of real disassembly: dis::Disassembly::Phase_2a_Naive. This virtual function is
  implemented by the class that represents the platform on which the executable is supposed to run, so for now this is only
  dis::Disassembly_Intel::Phase_2a_Naive. The naivity of this phase lies in its simplicity: it just runs through all 
  sections of code, and tries to disassemble all opcodes encountered. The disassembly starts at the first instruction
  of the executable, continues at all positions that are jumped to, and only stops when there are sections of code left 
  worth looking at.

<li> Next comes a step that is meant to look at elements specific to the platform the input file belongs to; so again, for
  now, there is only a dis::Disassembly_Intel::Phase_2b_Platform_Specific. The approach here is to run through all memory
  areas that could be code, and to look for the instructions <BR> <CODE> 0x55, // push ebp <BR> 0x8B, 0xEC // mov  ebp,esp
  </CODE> <BR> On the intel platform, these opcodes represent the stack frame being saved, and are very likely to point at
  the beginning of a section of code.

<li> dis::Disassembly::Phase_3a_Review_Data() is a method that tries to collect pieces of data section that likely
  belong together, such as strings for example. 

<li> During the previous phases a list of dis::Reference s has also been built; now, 
  dis::Disassembly_Intel::Phase_3b_Review_References() will check on the validity of these references. Memory areas that
  don't appear in the range of valid addresses for the executable are removed, for instance.

<li> dis::Disassembly::Phase_3c_Review_Imports() evaluates each of the imported functions we found in the very first
 dis::Disassembly::Phase_1a_File phase, comparing them to a list of known functions from dis::Disassembly::Phase_1b_Imports. 
 Each instruction that is seen to be calling a known imported function, is also recursively checked by looking at the 
 other instructions calling this one.
                                                                                   
<li> dis::Disassembly::Phase_3d_Review_Functions() does something very similar, but now for functions that aren't really 
 imports. ELF executables, for instance, can mention a list of regular functions in the file.

<li>  dis::Disassembly_Intel::Phase_3e_Review_Variables() doesn't do anything yet, but will provide an opportunity to 
 work on the variables that are detected in the program logic.
  
<li> dis::Disassembly::Phase_3f_Review_Parameters() is currently the final part of the analysis: whenever an instruction
 is revealed to call a function of which we know the parameters, the current instruction and the ones preceding it that
  are responsible for delivering the values for the parameters, are given appropriate labels.
</ul>           

  \subsection dis_data Data Structures.
<ul>    
<li> The key data structure is the dis::Disassembly_Node: one node represents exactly one instruction or piece of data. 
 The complete executable, code as well as data, is therefor represented by a list of nodes: list <Disassembly_Node> 
 dis::Disassembly::l_dn. The bytes of the node are in the char* dis::Disassembly_Node::opcode member. 
 For performance reasons, this is just a char* pointing into a contigous area of opcodes / data 
 (dis::Disassembly::opcodes_mp, to be precise), 
 with always at least one section being contained in one contigous area of allocated 
 memory. This way of working necessitates, of course, another member short dis::Disassembly_Node::n_used 
 that specifies where the bytes of this particular node end, and thus where the next node begins.

 <li> The actual instruction is stored in the Instruction dis::Disassembly_Node::instruction member. 
 
 A dis::Instruction structure holds all the information to detail one instruction. 
 There is the char* dis::Instruction:mnemonic that shows the mnemonic for the instruction, for instance
 'MOV' or 'INC'. The short dis::Instruction:operand_size informs us about the size of the operands of
  this instruction. 

 <li> The actual operands are stored in three dis::RegMemPart members. Most instructions have at most
 two operands, so there are two dis::RegMemPart members part1 and part2; there is also a *part3, but this is
  a pointer to a dis::RegMemPart, and will only be really used for a small number of instructions.

 <li> One of the purposes of the disassembler is to look at the functions that are called in a program, and to recognize the
 parameters of these functions. This is the reason there is also a short n_pushed member in
 dis:: Instruction : it holds the number of bytes pushed onto the stack. 

 <li> If the instruction really calls another function, its details will be held in the dis::Call
  dis::Instruction::*call member. This element will also be used if the instruction is itself called by something 
  else. The name of the function concerned can be found in the char* name member. If more details are known
  about the function, they will be referred to by the dis::Routine member.
  
 <li> A dis::Routine structure will hold information about the functions that are found in the analysed program: the 
  Routine* next members allows for a list of the structures to be built, and each element will stock its input and output
  in dis::Parameter* members.   

 <li> A dis::Parameter basically has a char* name, and a char *type_name.

 <li> Finally, the dis::Disassembly_Node will also list references: there is a dis::Reference ::iterator    
 ref_in, for when this is instruction (or data) is referred to, and a ref_out, for when itself points to something else.

</ul>   

 \subsection dis_example An Example.

 an example will undoubtebly shed the necessary light onto how this all works: suppose we have an executable with the following
 instructions:                                                                                                    
 <CODE> 
 <BR> CODE:00401063 0x83 0xEA 0x04                 //sub edx,4
 <BR> CODE:00401066 0x6A 0x00                     // push 0
 <BR> CODE:00401068 0xE8 0x94 0x07 0x00 0x00      // call GetModulehandleA
 </CODE> 

 dis::Disassembly::opcodes_mp will hold the raw data for the opcodes in a simple form of contiguous chars: 
 <BR> <CODE> 0x0808e535 : 0x83 0xEA 0x04 0x6A 0x00 0xE8 0x94 0x07 0x00 0x00 </CODE> 

 and the list <Disassembly_Node> dis::Disassembly::l_dn will refer to these opcodes and other elements as follows:
<BR> <CODE>
 l_dn.begin(): { </CODE>
 <TABLE> 
<TR> <TD>  <CODE> long </CODE></TD> <TD> <CODE> memory_offset </CODE> </TD> <TD> <CODE> 0x00401063 </CODE></TD> </TR> 
<TR> <TD>  <CODE> char* </CODE> </TD> <TD> <CODE> opcode </CODE> </TD> <TD> <CODE> 0x0808e535 </CODE> </TD> </TR> 
<TR> <TD>  <CODE> short </CODE></TD> <TD> <CODE> n_used </CODE></TD> <TD> <CODE> 3 </CODE> </TD> <TD> <CODE> // 0x83 0xEA 0x04 </CODE></TD> </TR> 
<TR> <TD>  <CODE> Instruction </CODE></TD> <TD> <CODE> instruction</CODE></TD> <TD> <CODE> [Instruction 1]</CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE> </TD> </TR> 
</TABLE> 
<CODE> } </CODE>
<BR> <BR> <CODE> l_dn.begin() + 1: { </CODE>
 <TABLE> 
<TR> <TD>  <CODE> long </CODE></TD> <TD> <CODE> memory_offset </CODE></TD> <TD> <CODE> 0x00401066 </CODE></TD> </TR> 
<TR> <TD>  <CODE> char* </CODE> </TD> <TD> <CODE> opcode </CODE></TD> <TD> <CODE> 0x0808e538</CODE></TD> </TR> 
<TR> <TD>  <CODE> short </CODE> </TD> <TD> <CODE> n_used </CODE></TD> <TD> <CODE> 2 </CODE> </TD>  <TD> <CODE> // 0x6A 0x00 </CODE> </TD> </TR> 
<TR> <TD>  <CODE> char* </CODE> </TD> <TD> <CODE> label </CODE></TD> <TD> <CODE> "lpModuleName" </CODE></TD> </TR> 
<TR> <TD>  <CODE> Instruction </CODE> </TD> <TD> <CODE> instruction </CODE> </TD> <TD> <CODE> [Instruction 2] </CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE> </TD> </TR> 
</TABLE> 
<CODE> } </CODE>
 <BR> <BR> <CODE> l_dn.begin() + 2: { </CODE>
 <TABLE> 
<TR> <TD>  <CODE> long </CODE></TD> <TD> <CODE> memory_offset </CODE></TD> <TD> <CODE> 0x00401068 </CODE></TD> </TR> 
<TR> <TD>  <CODE> char* </CODE></TD> <TD> <CODE> opcode </CODE></TD> <TD> <CODE> 0x0808e53A </CODE></TD> </TR> 
<TR> <TD>  <CODE> short </CODE></TD> <TD> <CODE> n_used </CODE></TD> <TD> <CODE> 5 </CODE></TD> <TD> <CODE> // 0xE8 0x94 0x07 0x00 0x00 </CODE></TD> </TR> 
<TR> <TD>  <CODE> std::list<dis::Reference>::iterator </CODE></TD> <TD> <CODE> ref_out </CODE></TD> <TD> <CODE> [Reference 1]</CODE></TD> </TR> 
<TR> <TD>  <CODE> Instruction </CODE></TD> <TD> <CODE> instruction </CODE></TD> <TD> <CODE> [Instruction 3] </CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE></TD> </TR> 
</TABLE> 
<CODE> } </CODE>
<BR> <BR>
Each instruction is held in a dis::Instruction, so let's look at each of them: 
<CODE> 
<BR> <BR>
CODE:00401063 0x83 0xEA 0x04                 //sub edx,4
<BR> since there are a register and an immediate value that between them completely define the content of the instruction,
the dis::Instruction only holds information about these two elements:
</CODE> 
 <TABLE> 
<TR> <TD>  <CODE> char* </CODE></TD> <TD> <CODE> mnemonic </CODE></TD> <TD> <CODE> "sub" </CODE></TD> </TR> 
<TR> <TD>  <CODE> short</CODE></TD> <TD> <CODE> operand_size</CODE></TD> <TD> <CODE> DISASSEMBLY_BITNESS_32 </CODE></TD> </TR> 
<TR> <TD>  <CODE> RegMemPart </CODE> </TD> <TD> <CODE> part1 </CODE></TD> <TD> <CODE> [RegMemPart 1] </CODE></TD> </TR> 
<TR> <TD>  <CODE> RegMemPart </CODE></TD> <TD> <CODE> part1 </CODE></TD> <TD> <CODE> [RegMemPart 2] </CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE></TD> </TR> 
</TABLE>     

<CODE> 
<BR> <BR> second, we have
<BR> CODE:00401065 0x6A 0x00                     // push 0 
<BR> all we can see here in the dis::Instruction, is an immediate value being pushed onto the stack:
<BR> </CODE> 
 <TABLE> 
<TR> <TD>  <CODE> char* </CODE></TD> <TD> <CODE> mnemonic</CODE></TD> <TD> <CODE> "push" </CODE></TD> </TR> 
<TR> <TD>  <CODE> short </CODE></TD> <TD> <CODE> operand_size </CODE></TD> <TD> <CODE> DISASSEMBLY_BITNESS_32 </CODE></TD> </TR> 
<TR> <TD>  <CODE> short </CODE> </TD> <TD> <CODE> n_pushed</CODE></TD> <TD> <CODE> 4</CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE></TD> </TR> 
</TABLE>     

<CODE> 
 <BR> <BR> and finally: CODE:00401068 0xE8 0x94 0x07 0x00 0x00      // call GetModulehandleA
 </CODE> 
<BR> with its dis::Instruction only showing that a function is being called:
<BR> 
 <TABLE> 
<TR> <TD>  <CODE> char* </CODE></TD> <TD> <CODE> mnemonic</CODE></TD> <TD> <CODE> "call"</CODE></TD> </TR> 
<TR> <TD>  <CODE> Call* </CODE></TD> <TD> <CODE> call </CODE></TD> <TD> <CODE> [Call 1] </CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE></TD> </TR> 
</TABLE>     

<BR>
The two dis::RegMemPart elements for the first instruction will look like this:
<BR> 
<TABLE> 
<TR> <TD>  <CODE> short </CODE> </TD> <TD> <CODE> reg32 </CODE></TD> <TD> <CODE> "EDX" </CODE></TD> <TD> <CODE> // register used</CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ...</CODE></TD> </TR> 
</TABLE>     
<TABLE> 
<TR> <TD>  <CODE> int</CODE></TD> <TD> <CODE> imm </CODE></TD> <TD> <CODE> 4 </CODE></TD> <TD> <CODE> // immediate value </CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE></TD> </TR> 
</TABLE>     

<BR>
The dis::Call* for the third instruction will contain:
<BR> 
<TABLE> 
<TR> <TD>  <CODE> short </CODE></TD> <TD> <CODE> type_of_call </CODE></TD> <TD> <CODE> INSTRUCTION_CALL_ROUTINE </CODE></TD> </TR> 
<TR> <TD>  <CODE> char* </CODE></TD> <TD> <CODE> name </CODE></TD> <TD> <CODE> "GetModulehandleA" </CODE></TD> </TR> 
<TR> <TD>  <CODE> Routine* </CODE></TD> <TD> <CODE> routine </CODE></TD> <TD> <CODE> [Routine 1] </CODE></TD> </TR> 
</TABLE>     

<BR>    
with the details about the actually called routine in a dis::Routine :
<BR> 
<TABLE> 
<TR> <TD>  <CODE> char*</CODE></TD> <TD> <CODE> name </CODE></TD> <TD> <CODE> "GetModulehandleA" </CODE></TD> </TR> 
<TR> <TD>  <CODE> Parameter* </CODE></TD> <TD> <CODE> input </CODE></TD> <TD> <CODE> [Parameter 1] </CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE></TD> </TR> 
</TABLE>     

<BR>                                                                     
and finally, what we know about the input parameters will be held in a dis::Parameter :   
<BR> 
<TABLE> 
<TR> <TD>  <CODE> char* </CODE></TD> <TD> <CODE> name </CODE></TD> <TD> <CODE> "LpModuleName" </CODE></TD> </TR> 
<TR> <TD>  <CODE> Parameter* </CODE></TD> <TD> <CODE> next </CODE></TD> <TD> <CODE> 0 </CODE></TD> <TD> <CODE> // since there is only one parameter to this Windows function </CODE></TD> </TR> 
<TR> <TD>  <CODE> ... other fields omitted ... </CODE> </TD> </TR> 
</TABLE>   

<HR>

 \subsection dis_add_new How To Add New Executable Formats.                                                   

The purpose of the data structure that was described above, is to offer something that is sufficiently generic to 
analyze all kinds of binary files. I think it is reasonably transparent, and complex enough to hold at least the
analysis of an intel based program, the platform on which I know most about its assembly language. 

When adding binaries from other platforms, you should first try and reuse the existing data elements, and only 
when something is really missing, add or modify things. In fact, trying to map a number of instructions to the 
data structures, should be the first thing you do. 

If this succeeds, the hard work is done, really. After this, you only have to

<ul>    
<li> Decide on an inheritance tree for the platform. 

For instance, on the intel platform, the dis::Disassembly_Intel class is the base class, which holds the actual disassembly routines. 

Once we have this real disassembly class for a platform, we can add derived classes that know how to deal with the
concrete file format of a given executable. For intel, we currently have the dis::Disassembly_WinPE class, which deals with 
windows PE files, and the dis::Disassembly_Elf class that knows about linux ELF files. 

Deriving both disassemblies from a common ancestor makes sense, since they of course share the same opcode meanings.

<li> Actually read in the file, ie code a dis::Disassembly_xxx::Phase_1a_File() function for the derived class that corresponds
to the given type of file. The result of this function must be to determine a number of basic statistics about the 
executable, such as the number of code segments, the location of the first instruction, and so on. 

At the end of this function, you must also haven taken note of which functions are imported, and have copied all data and code
opcodes in the dis::Disassembly::opcodes_mp data field.

<li> Code a dis::Disassembly_xxx::Phase_2b_Platform_Specific() function that tries to find additional pieces of code in the
executable, based on the knowledge of this specific platform.
                                        
<li> And that's it! 

</ul>           

\page user1 For The User

usage : disassembler
<ul>    
<li> -d : type of disassembly, valid values are:
                              'intel-winpe' 
                              'intel-elf' 
                              'intel-raw' 
<li> -f : input filename
<li> -g : type of gui, valid values are:
                              'none' for no gui 
                              'gtk'  for gnome gui (default) 
<li> -o : output filename
<li> -s : type of output, valid values are:
                              'db' for 'database' (default)    
                              'list'  for 'listing'  
 
<li> omitting any options will start the gui version of the disassembler
</ul>                        

*/

#endif

