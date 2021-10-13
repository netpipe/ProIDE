/*
 * analysis.cpp: Routines for analysing a binary file
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

using namespace std;

#include <stdio.h>
#include <unistd.h>

#include <iostream>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "analysis.hh"

dis::Analysis::Analysis(dis::Main_Gui  *mg, dis::Disassembly_Options *d_o)
{  
    disassembly = 0; 
    
    input_file = &(d_o->input_file_name);

    gui_mg = mg;

    options = d_o;
}

dis::Analysis::~Analysis()
{
    cout << "Destructor Analysis" << "\n";

    if (disassembly) { delete disassembly; }
}

int 
dis::Analysis::Determine_Type_Raw()
{
    return TYPE_OF_FILE_INTEL_RAW;
}

int 
dis::Analysis::Determine_Type_WinPE()         
{
    int tobf = TYPE_OF_FILE_UNKNOWN;  
    int t;                                          // temp value

    void*                  file_data;               // for memory mapping the input file
    int                    file_des;                // file descriptor
    struct stat            buf;

    IMAGE_DOS_HEADER       *idh;                     // buffer for the DOS info
    int                    *signature; 

    //////////////////////////////////////////////////////////////////////////////////

    file_des = open(input_file->c_str(), O_RDONLY);
    if (file_des < 0)                               // if we can't open the file
    { return TYPE_OF_FILE_UNKNOWN; }    

    fstat(file_des, &buf);
    if (buf.st_size < IMAGE_DOS_HEADER_LENGTH)    // file is too small 
    { close(file_des); return TYPE_OF_FILE_UNKNOWN; }    

    file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, file_des, 0);
    if (file_data == MAP_FAILED)                   // can't memory map the file
    { close(file_des); return TYPE_OF_FILE_UNKNOWN; }    
     
    idh = (IMAGE_DOS_HEADER * ) file_data;                      // the DOS_HEADER starts at the beginning of the file

    if (idh->e_magic == IMAGE_DOS_SIGNATURE )
     {
       t = idh->e_lfanew ;

       if (t >= buf.st_size)                                    // signature info is incorrect
       { return TYPE_OF_FILE_UNKNOWN; }

       signature = (int *) ((char*) file_data + t);
       if (*signature == IMAGE_NT_SIGNATURE)
       { tobf = TYPE_OF_FILE_WINDOWS_PE; }    
     }      

    close(file_des); 

    return tobf;
}

int 
dis::Analysis::Determine_Type_Elf()         
{
    int                    tobf;  

    void*                  file_data;               // for memory mapping the input file
    int                    file_des;                // file descriptor
    struct stat            buf;

    ELF32_HDR             *eh;

    //////////////////////////////////////////////////////////////////////////////////

    tobf = TYPE_OF_FILE_UNKNOWN;  

    file_des = open(input_file->c_str(), O_RDONLY);
    if (file_des < 0)                              // if we can't open the file
    { return TYPE_OF_FILE_UNKNOWN; }    

    fstat(file_des, &buf);
    if (buf.st_size < ELF32_HDR_LENGTH)            // file is too small 
    { close(file_des); return TYPE_OF_FILE_UNKNOWN; }    

    file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, file_des, 0);
    if (file_data == MAP_FAILED)                   // can't memory map the file
    { close(file_des); return TYPE_OF_FILE_UNKNOWN; }    
     
    eh = (ELF32_HDR *) file_data;                  // the ELF HEADER starts at the beginning of the file

    if (   (eh->e_ident[0] == ELFMAG0)
        && (eh->e_ident[1] == ELFMAG1)
        && (eh->e_ident[2] == ELFMAG2)
        && (eh->e_ident[3] == ELFMAG3)             // ELF magical header
        && (eh->e_machine  == EM_386)              // intel machine
       )
    { 
     if (eh->e_ident[4] == ELFCLASS32)             // 32 bit
     {
      tobf = TYPE_OF_FILE_INTEL_ELF; 
     }
    }                                

    close(file_des); 

    return tobf;
}

int 
dis::Analysis::Determine_Type_Disassembly()         
{
    int                    tobf;
    int                    t;                       // temp value

    void*                  file_data;               // for memory mapping the input file
    int                    file_des;                // file descriptor
    struct stat            buf;

    string                 s1;                      // temp value
    char                  *s2;                      // temp value

    //////////////////////////////////////////////////////////////////////////////////

    tobf = TYPE_OF_FILE_UNKNOWN;  

    file_des = open(input_file->c_str(), O_RDONLY);
    if (file_des < 0)                               // if we can't open the file
    { return TYPE_OF_FILE_UNKNOWN; }    

    s1 = DISASSEMBLY_SEPARATOR_RESULT;
    t  = s1.size();

    fstat(file_des, &buf);

    if (buf.st_size < t)                            // file is too small 
    { close(file_des); return TYPE_OF_FILE_UNKNOWN; }    

    file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, file_des, 0);
    s2 = (char *) file_data;
    if (s1.compare(0, t, s2, t) == 0)                   // equality
    { close(file_des); return TYPE_OF_FILE_DISASSEMBLY; }    

    close(file_des); 
    return tobf;
}

int
dis::Analysis::Determine_Type_of_Binary_File()  // so we know which kind of disassembly to start
{
    int tobf;  

    ////////////////////////////////////////////////////////////////////////////

    tobf = Determine_Type_Disassembly();        // existing disassembly

    if (tobf == TYPE_OF_FILE_UNKNOWN)
    {
     tobf = Determine_Type_WinPE();             // intel, windows Portable Executable
    }

    if (tobf == TYPE_OF_FILE_UNKNOWN)
    {
      tobf = Determine_Type_Elf();              // executable and linking format
    }                             

    if (tobf == TYPE_OF_FILE_UNKNOWN)
    {
      tobf = Determine_Type_Raw();              // intel, raw binary
    }                        

    return tobf;
}

int
dis::Analysis::Confirm_Type_of_Binary_File()
{   
#ifdef HAVE_GTK                               

    int i;

    ////////////////////////////////////////////////////////

    dft = new dis::gui::dlg_file_type(&tobf, options);    

    i = dft->run();                             

    dft->hide();

    delete(dft);

    return i;

#endif
}

int
dis::Analysis::Init_Disassembly(int type_disassembly) 
{

    ///////////////////////////////////////////////////

    switch (type_disassembly)
    { 
    case TYPE_OF_FILE_WINDOWS_PE:
        {
           cout << "TYPE_OF_FILE_WINDOWS_PE: yo!" << "\n";

           if (options->type_of_gui == DISASSEMBLY_GUI_GTK)
           { disassembly = new  Disassembly_WinPE (input_file, gui_mg, options); }
           else if (options->type_of_gui == DISASSEMBLY_GUI_NONE)
           { disassembly = new  Disassembly_WinPE (input_file, options); }

           break;
        }
    case TYPE_OF_FILE_INTEL_ELF:
        {
           cout << "TYPE_OF_FILE_ELF: yo!" << "\n";

           if (options->type_of_gui == DISASSEMBLY_GUI_GTK)
           { disassembly = new  Disassembly_Elf (input_file, gui_mg, options); }
           else if (options->type_of_gui == DISASSEMBLY_GUI_NONE)
           { disassembly = new  Disassembly_Elf (input_file, options); }

           break;
        }
    case TYPE_OF_FILE_INTEL_RAW:
        {
           cout << "TYPE_OF_FILE_RAW: yo!" << "\n";

           if (options->type_of_gui == DISASSEMBLY_GUI_GTK)
           { disassembly = new  Disassembly_Intel_Raw (input_file, gui_mg, options); }
           else  if (options->type_of_gui == DISASSEMBLY_GUI_NONE)
           { disassembly = new  Disassembly_Intel_Raw (input_file, options); }

           break;
        }  
    default:
        {
           return RET_ERR_UNKNOWN_FILETYPE;
        }
    }

    return RET_OK;
}


int
dis::Analysis::Perform()                                          
{   
    int i;                      // temp value

    ///////////////////////////////////////////////////////

    cout << "Perform analysis on :" << input_file->c_str() << "\n";

    if (disassembly) { delete disassembly; }

    status = ANALYSIS_STATUS_BUSY;

    tobf = Determine_Type_of_Binary_File();                   // auto recognition

    if (tobf == TYPE_OF_FILE_DISASSEMBLY)
    {
     i = Callback_Open();
    }
    else
    {

#ifdef HAVE_GTK                               
     if (options->type_of_gui == DISASSEMBLY_GUI_GTK)
     {
      if (Confirm_Type_of_Binary_File()                       // user confirmation and options
                                        != Gtk::RESPONSE_OK)
      { return RET_ERR_CANCEL; } 
     }
#endif

     Init_Disassembly(tobf);

     i = disassembly->Perform();

     status = ANALYSIS_STATUS_IDLE;
    }
                               
    return i;
}

/*
void
dis::Analysis::Callback_Gui_Double_Clicked()
{
  if (!disassembly) {return;}
      
  disassembly->Callback_Gui_Double_Clicked();
}

void
dis::Analysis::Callback_Navigation()
{
  if (!disassembly) {return;}
      
  disassembly->Callback_Navigation();
}
*/

void
dis::Analysis::Callback_Save(std::string *file_name, int type_of_save)
{
  if (!disassembly) {return;}

  switch (type_of_save)
  {
   case DISASSEMBLY_SAVE_DATABASE:                              // internal format
   { disassembly->Callback_Save_Database(file_name); break; }

   case DISASSEMBLY_SAVE_LISTING:                               // readable listing
   { disassembly->Callback_Save_Listing(file_name); break; }
  }      
}

int
dis::Analysis::Callback_Open()
{
  int               tod,            // type of disassembly
                    result;

  bool              stop, read_type;

  string            s;

  ifstream          i_file;         // input_file

  /////////////////////////////////////////////

  if (disassembly) { delete disassembly; }

  i_file.open(input_file->c_str());

  if (!i_file)
  {
   cout << "Disassembly: no file to be read!" << "\n";
   return RET_ERR_FILE_INPUT;
  }    

  read_type = stop = false;
  tod = 0;

  while ((!stop) && (getline(i_file, s)))
  {
   if (s == DISASSEMBLY_SEPARATOR_DISASSEMBLY)
   { read_type = true; }

   else if (read_type == true)
   {
    tod = atoi(s.c_str());
    stop = true;
   }                      
  }                                    

  result = Init_Disassembly(tod);
  if (result != RET_OK) {return result;}

  result = disassembly->Callback_Open();
  if (result != RET_OK) {return result;}

  disassembly->Phases_In_Thread();

  return RET_OK;
}

int
dis::Analysis::Callback_Get_Row_From_Offset(int pos)
{
  if (disassembly)
  { return disassembly->Callback_Get_Row_From_Offset(pos); }

  return 0;
}

dis::Disassembly_Node*
dis::Analysis::Callback_Get_nth_Row(uint n)
{
  if (disassembly)
  { return disassembly->Callback_Get_nth_Row(n); }

  return 0;
}                                             


int
dis::Analysis::Callback_Navigation_Search_Byte(int row, std::vector<char> *search_string, bool direction, bool wrap)
{
  if (disassembly)
  { return disassembly->Callback_Navigation_Search_Byte(row, search_string, direction, wrap); }

  return -1;
}



dis::Disassembly*    
dis::Analysis::Callback_Get_Disassembly()
{
  return disassembly;
}

dis::Extra*  
dis::Analysis::Callback_Get_Extra()
{
  if (disassembly)
  {
   return disassembly->Callback_Get_Extra();
  }

  return 0;
}

bool
dis::Analysis::Callback_Is_Valid_Reference(std::list<dis::Reference>::iterator *it_ref)
{
  if (disassembly)
  { return disassembly->Callback_Is_Valid_Reference(it_ref); }

  return false;
}

void
dis::Analysis::Callback_Translate_Opcodes(Disassembly_Node *dn, bool data, string *s, int max_bytes_per_row)
{
  if (disassembly)
  { disassembly->Callback_Translate_Opcodes(dn, data, s, max_bytes_per_row); }
}


void
dis::Analysis::Callback_Translate_Section(uint section, string *str_section)
{
  if (disassembly)
  { disassembly->Callback_Translate_Section(section, str_section); }
}




