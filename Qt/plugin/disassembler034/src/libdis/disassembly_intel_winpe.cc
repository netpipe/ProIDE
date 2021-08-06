/*
 * Disassembly_winpe.cpp: Routines for disassembling a windows portable executable
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

using namespace std;

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

#include "disassembly_intel_winpe.hh"
#include <cstring>

 
dis::Disassembly_WinPE::Disassembly_WinPE(
        string* file_name, 
        dis::Main_Gui *mg,
        dis::Disassembly_Options *options) 
        :
        Disassembly_Intel(file_name, mg, options)  
{
    address_bitness = operand_bitness = DISASSEMBLY_BITNESS_32;

    known_imports_file_name = 
        u.get_executable_path("disassembler") + 
        "/../share/disass/disassembly_winpe/imports"; 

    disassembly_type = DISASSEMBLY_TYPE_INTEL_WINPE;

    cout << "Constructor Disassembly_WinPE(filename, mg, opt)" << "\n";
}

dis::Disassembly_WinPE::Disassembly_WinPE(
        string* file_name, 
        dis::Disassembly_Options *options) 
        :
        Disassembly_Intel(file_name, options)  
{
    address_bitness = operand_bitness = DISASSEMBLY_BITNESS_32;

    known_imports_file_name = 
        u.get_executable_path("disassembler") + 
        "/../share/disass/disassembly_winpe/imports"; 

    disassembly_type = DISASSEMBLY_TYPE_INTEL_WINPE;

    cout << "Constructor Disassembly_WinPE(filename, opt)" << "\n";
}

dis::Disassembly_WinPE::~Disassembly_WinPE()
{
    cout << "Destructor Disassembly_WinPE" << "\n";
}

int
dis::Disassembly_WinPE::Phase_1a_File()
{
    IMAGE_DOS_HEADER                   *idh;                     // buffer for the file header 
    IMAGE_NT_HEADERS                   *inh;                     // buffer for finding file sections
    IMAGE_SECTION_HEADER               *ish;                     // buffer for the section headers

    IMAGE_IMPORT_DESCRIPTOR            *iid;                     // buffer to one list of imports
    IMAGE_THUNK_DATA                   *itd;                     // buffer describing the library
    IMAGE_IMPORT_BY_NAME               *iibn;                    // buffer describing one imported function

    int                                 t, temp, i,              // temp values
                                        o, ioior,                // offset (raw - virtual)  
                                        imp_sec;                 // section containing imports
    vector <IMAGE_SECTION_HEADER>       vsh;
    int                                 pif, pos;                // position in file
    int                                 adr;                     // position in memory
    int                                 ty;                      // type of data in section

    char                               *section_data;            // buffer for the section bytes
    void*                               file_data;               // for memory mapping the input file
    int                                 file_des;                // file descriptor
    struct stat                         buf;
    Disassembly_Node                    dn;

    Section                             s;
    
    Import                              imp;
    char                               *imp_t;                   // temp value

    bool                                iid_empty, itd_empty;           

    list<Disassembly_Node>::iterator    it;
    list<Import>::iterator              imp_it;              

    //////////////////////////////////////////////////////////////////////////////////////////
             
    cout << "Read_Binary_File: Disassembly_WINPE!" << "\n";

    temp = 0;

    file_des = open(input_file.c_str(), O_RDONLY);
    if (file_des < 0)                                         // if we can't open the file
    { return RET_ERR_FILE_INPUT; }    

    fstat(file_des, &buf);

    input_file_size = buf.st_size;                            // file size

    file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, file_des, 0);
    if (file_data == MAP_FAILED)
    { close(file_des); return RET_ERR_FILE_INPUT; }    
                                     
    idh = (IMAGE_DOS_HEADER *) file_data;             // the DOS_HEADER starts at the beginning of the file

    t = idh->e_lfanew;
    pif = t + IMAGE_NT_HEADERS_LENGTH;

    inh = (IMAGE_NT_HEADERS * ) ((char *) file_data + t); // read in the header and optional header
    Optional_Header_Data(inh);                        // sections, offset to code, imports

    range_offset_low = range_offset_high = Imports_Offset_Raw = imp_sec = 0;

    // stock sections info
    for (t = 0; t < nSections; t++) 
    {
     ish = (IMAGE_SECTION_HEADER * ) ((char *) file_data + pif);
     pif += IMAGE_SECTION_HEADER_LENGTH;

     vsh.push_back(*ish);

     if (   (ish->VirtualAddress <= Imports_Offset)
         && (ish->VirtualAddress + ish->SizeOfRawData >= Imports_Offset))
     { Imports_Offset_Raw =                          // pos in file of imports section   
         ish->PointerToRawData + (Imports_Offset - ish->VirtualAddress); 
       imp_sec = t; 
     }
    }

    ///////////////////// read the bytes /////////////////////////////////////////////
    for (t = 0; t < nSections; t++) 
    {
     Section_Data(t, vsh[t].Characteristics, &ty);         

     if (   (ty == NODE_TYPE_DATA)
         && (opt->show_data_sections == false)                     // user options
         && (   (opt->show_import_sections == false)
             || ((opt->show_import_sections == true)
                 && (t != imp_sec))
            )
        )
     { continue; }

     cout << "ok ... " << t << "\n";

     pif = vsh[t].PointerToRawData;                                // start of next section

     if (pif == 0)                                                 // no actual bytes in the executable   
     { continue; }         

     pos = vsh[t].SizeOfRawData;                                   //  size of this section
     adr = vsh[t].VirtualAddress;                                  //  location when loaded into memory

     opcodes_mp.Reset_Available();                                 // every section its own poollist
     opcodes_mp.Set_Associated_Value(adr + Load_address);
     opcodes_mp.Set_Pool_Size(pos);

     if ((range_offset_low == 0) || (adr + Load_address < range_offset_low))
     { range_offset_low = adr + Load_address;}

     s.offset = adr + Load_address;
     s.size   = pos;
     s.name   = vsh[t].Name;                                  
     v_s.push_back(s);                                        

     while (pos)
     {
       if (pos >= mbpi)
       { temp = mbpi; }
       else
         temp = pos;

       section_data = ((char *) file_data + pif);     

       Initialize(&dn);

       dn.file_offset = pif;
       dn.memory_offset = adr + Load_address;
       dn.n_used = temp;

       dn.type = ty;
       dn.section = t;
       dn.opcode = (char*) opcodes_mp.Use_Pool(temp);
       for (i=0; i < temp; i++)
       { *(dn.opcode + i) = (char) *(section_data + i); }

       dn.status = NODE_STATUS_UNEXPLORED;

       l_dn.push_back(dn);

       pos -= temp;
       pif += temp;
       adr += temp;
     }

    if (range_offset_high < adr + Load_address)
     { range_offset_high = adr + Load_address - 1;}

    cout << "l_dn size after section " << t << " : " << l_dn.size() << "\n";
    }

    ///////////////////////// stock imports info /////////////////////////////////////////////////
    imp_it = l_i.begin();
    ioior = Imports_Offset - Imports_Offset_Raw;

    iid  = (IMAGE_IMPORT_DESCRIPTOR * ) ((char *) file_data + Imports_Offset_Raw);  // first IMAGE_IMPORT_DESCRIPTOR

    iid_empty = false;
    while (!iid_empty)
    {
      iid_empty = true;
      imp_t  = (char *) iid;
      for (i = 0; ((i < IMAGE_IMPORT_DESCRIPTOR_LENGTH) && (iid_empty == true)); i++)
      {                                                         // last iid will be filled with 0-bytes 
       if (*imp_t != '\0') 
       { iid_empty = false; }          

       imp_t++;
      }

      if (iid_empty == true)
      { continue; }
      
      imp.library = ((char *) file_data + iid->Name - ioior);   // name of dll that provides the imported function
      
      itd  = (IMAGE_THUNK_DATA *) ((char*) file_data + (int) iid->u.OriginalFirstThunk - ioior);

      itd_empty = false;
      imp.memory_offset = Load_address + (int) iid->FirstThunk; // FirstThunk is what gets called in the exe

      while (!itd_empty)
      {                                                         // last itd will be filled with 0-bytes 
       itd_empty = true;
       imp_t  = (char*) (itd);
       for (i = 0; ((i < IMAGE_THUNK_DATA_LENGTH) && (itd_empty == true)); i++)
       {
        if (*imp_t != '\0') 
           { itd_empty = false; }           
        imp_t++;
       }

       if (itd_empty == true)
       { continue; }

       iibn = (IMAGE_IMPORT_BY_NAME * ) ((char*) ((int) (file_data) + (int) (itd->u1.AddressOfData) - ioior));

       imp.name = iibn->Name;                                    // name of imported function
       l_i.push_back(imp);                                       // add new import to the list   
       imp_it++;
       
       o =                                                       // location in exe of name of import
           Load_address                                            // load address for exe
         + (int) (itd->u1.AddressOfData)                           // 'virtual' position of name in file
         + 2                                                       // leading short
         ;
       it = Get_Disassembly_Node_From_Offset(o, false);          // contains the imported function's name

       i = imp.name.size();  
       if ((i != 0)  && (it != l_dn.end()))
       {
        Isolate_l_dn(it, o, i, NODE_STATUS_AUTO);                // put the name in its own node

        it = Get_Disassembly_Node_From_Offset(o, true);          // select the node with only the name

        if (it != l_dn.end())                                    //  and update it
        {
         it->status = NODE_STATUS_AUTO;
         it->type = NODE_TYPE_DATA;

         Add_Reference(it, imp.memory_offset, true);             // references the location that will get called
         Update_Reference_For_Import(imp_it, imp.memory_offset); // add the Import info
        }

        it = Get_Disassembly_Node_From_Offset(imp.memory_offset, false);    
                                                                 // contains the imported function's data
        if (it != l_dn.end())
        {
          Isolate_l_dn(it, imp.memory_offset, IMAGE_THUNK_DATA_LENGTH, NODE_STATUS_AUTO); 
                                                                 // put the imported function in its own node
          it = Get_Disassembly_Node_From_Offset(imp.memory_offset, true);  
                                                                 // select the node pointing to the import data
          if (it != l_dn.end())                                  //  and update it
          {
           it->status = NODE_STATUS_AUTO;
           it->type = NODE_TYPE_DATA;
          }                          
        }                                           
       }

       imp.memory_offset += IMAGE_THUNK_DATA_LENGTH;
       itd++;
      }

      iid++;
    }

    cout << "imports finished " << "\n";

    close(file_des); 

    return RET_OK;
}

void
dis::Disassembly_WinPE::Optional_Header_Data(IMAGE_NT_HEADERS *inh)
{
    Load_address = inh->OptionalHeader.ImageBase;
    Offset_to_first_instruction =  inh->OptionalHeader.AddressOfEntryPoint;

    nSections = inh->FileHeader.NumberOfSections;

    Imports_Offset = inh->OptionalHeader.DataDirectory[1].VirtualAddress;
    Imports_Size = inh->OptionalHeader.DataDirectory[1].Size;

    v_to_explore.push_back(Offset_to_first_instruction + Load_address);
}


void
dis::Disassembly_WinPE::Section_Data(int section_number, int section_characteristics, int *section_type)
{
    if (section_characteristics & IMAGE_SCN_CNT_CODE )            // Section contains code
    { cout << "IMAGE_SCN_CNT_CODE in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) 
    { cout << "IMAGE_SCN_CNT_INITIALIZED_DATA in section " << section_number << "\n"; }
    
    if (section_characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) 
    { cout << "IMAGE_SCN_CNT_UNINITIALIZED_DATA in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_LNK_OTHER) 
    { cout << "IMAGE_SCN_LNK_OTHER in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_LNK_INFO) 
    { cout << "IMAGE_SCN_LNK_INFO in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_LNK_REMOVE) 
    { cout << "IMAGE_SCN_LNK_REMOVE in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_LNK_COMDAT) 
    { cout << "IMAGE_SCN_LNK_COMDAT in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_FARDATA) 
    { cout << "IMAGE_SCN_MEM_FARDATA in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_PURGEABLE) 
    { cout << "IMAGE_SCN_MEM_PURGEABLE in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_16BIT) 
    { cout << "IMAGE_SCN_MEM_16BIT in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_LOCKED) 
    { cout << "IMAGE_SCN_MEM_LOCKED in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_PRELOAD) 
    { cout << "IMAGE_SCN_MEM_PRELOAD in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_ALIGN_1BYTES) 
    { cout << "IMAGE_SCN_ALIGN_1BYTES in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_ALIGN_2BYTES) 
    { cout << "IMAGE_SCN_ALIGN_2BYTES in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_ALIGN_4BYTES) 
    { cout << "IMAGE_SCN_ALIGN_4BYTES in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_ALIGN_8BYTES) 
    { cout << "IMAGE_SCN_ALIGN_8BYTES in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_ALIGN_16BYTES) 
    { cout << "IMAGE_SCN_ALIGN_16BYTES in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_ALIGN_32BYTES) 
    { cout << "IMAGE_SCN_ALIGN_32BYTES in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_ALIGN_64BYTES) 
    { cout << "IMAGE_SCN_ALIGN_64BYTES in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_LNK_NRELOC_OVFL) 
    { cout << "IMAGE_SCN_LNK_NRELOC_OVFL in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_DISCARDABLE) 
    { cout << "IMAGE_SCN_MEM_DISCARDABLE in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_NOT_CACHED) 
    { cout << "IMAGE_SCN_MEM_NOT_CACHED in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_NOT_PAGED) 
    { cout << "IMAGE_SCN_MEM_NOT_PAGED in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_SHARED) 
    { cout << "IMAGE_SCN_MEM_SHARED in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_EXECUTE) 
    { cout << "IMAGE_SCN_MEM_EXECUTE in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_READ) 
    { cout << "IMAGE_SCN_MEM_READ in section " << section_number << "\n"; }

    if (section_characteristics & IMAGE_SCN_MEM_WRITE) 
    { cout << "IMAGE_SCN_MEM_WRITE in section " << section_number << "\n"; }

    //----------------------------------------------------------------------------------

    if (section_characteristics & IMAGE_SCN_CNT_CODE )            // Section contains code
    {
      *section_type = NODE_TYPE_CODE;
      cout << "Code in section " << section_number << " of " << input_file.c_str() << "\n";
    }
    else 
      *section_type = NODE_TYPE_DATA;                       
}

string
dis::Disassembly_WinPE::Get_Function_Type(int i)
{
   string                   s;

   ///////////////////////////////////
   
   if (i >= FUNCTION_TYPE_MAX)
   {
    if (i < FUNCTION_TYPE_MAX + FUNCTION_TYPE_INTEL_WINPE_MAX)
    { s = function_type_intel_winpe[i - FUNCTION_TYPE_MAX]; }                            
   }
   else {s = dis::Disassembly::Get_Function_Type(i);}

   return s;
}


int
dis::Disassembly_WinPE::Get_Function_Type(string *s)
{
    int                     i, ft;      // index, function type

    ////////////////////////////////////////////////////:

    ft = dis::Disassembly::Get_Function_Type(s);

    if (ft != FUNCTION_TYPE_UNKNOWN) 
        { return ft; }

    for (i = 0; i < FUNCTION_TYPE_INTEL_WINPE_MAX; i++)
    {
     if (*s == function_type_intel_winpe[i])
     { return i + FUNCTION_TYPE_MAX; }
    }                                         

    return FUNCTION_TYPE_UNKNOWN;
}                               

dis::Routine* 
dis::Disassembly_WinPE::Get_Routine_From_Name(const char *file_name, const char *routine_name)
{
    dis::Routine           *r;          // routine

    int                     l;          // length

    ///////////////////////////////////////////////////////

    r = dis::Disassembly::Get_Routine_From_Name(file_name, routine_name);

    // some Windows functions also exist with an 'A' (Ansi) or 'W' (Wide, for Unicode) suffix
    if (r == 0) 
    {
     l = strlen(routine_name);

     if (strrchr(routine_name, 'A') == (routine_name + l - 1))
     {
      string temp(routine_name, l - 1);

      r = dis::Disassembly::Get_Routine_From_Name(file_name, temp.c_str());    
     }                                                                   

     if (r == 0) 
     {
      l = strlen(routine_name);

      if (strrchr(routine_name, 'W') == (routine_name + l - 1))
      {
       string temp(routine_name, l - 1);

       r = dis::Disassembly::Get_Routine_From_Name(file_name, temp.c_str());    
      }                                                                   
     }
    }



    return r;
}



