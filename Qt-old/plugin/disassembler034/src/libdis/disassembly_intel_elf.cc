/*
 * Disassembly_intel_elf.cpp: Routines for disassembling an elf file
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

#include "disassembly_intel_elf.hh"
#include <cstring>
 
dis::Disassembly_Elf::Disassembly_Elf(
        string* file_name, 
        dis::Main_Gui *mg,
        dis::Disassembly_Options *options) 
        :
        Disassembly_Intel(file_name, mg, options)  
{
    address_bitness = operand_bitness = DISASSEMBLY_BITNESS_32;

    known_imports_file_name = 
        u.get_executable_path("disassembler") + 
        "/../share/disass/disassembly_elf/imports"; 

    disassembly_type = DISASSEMBLY_TYPE_INTEL_ELF;

    cout << "Constructor Disassembly_Elf(filename, mg, opt)" << "\n";
}

dis::Disassembly_Elf::Disassembly_Elf(
        string* file_name, 
        dis::Disassembly_Options *options) 
        :
        Disassembly_Intel(file_name, options)  
{
    address_bitness = operand_bitness = DISASSEMBLY_BITNESS_32;

    known_imports_file_name = 
        u.get_executable_path("disassembler") + 
        "/../share/disass/disassembly_elf/imports"; 

    disassembly_type = DISASSEMBLY_TYPE_INTEL_ELF;

    cout << "Constructor Disassembly_Elf(filename, opt)" << "\n";
}

dis::Disassembly_Elf::~Disassembly_Elf()
{
    cout << "Destructor Disassembly_Elf" << "\n";
}

int
dis::Disassembly_Elf::Phase_1a_File()
{
    char                               *section_data,            // buffer for the section bytes
                                       *section_names,           // where section names start
                                       *symbol_names,            // where symbol names start
                          //             *dynamic_functions,     // where dynamic function names start
                                       *s;                       // temp values
    void*                               file_data;               // for memory mapping the input file
    int                                 file_des;                // file descriptor
    struct stat                         buf;

    Disassembly_Node                    dn;
    Section                             section;

    string                              section_info;
    Extra                              *extra;

    ELF32_HDR                          *eh;                      // program header
    ELF32_SHDR                         *shdr;                    // section header
    ELF32_SYM                          *sym;                     // symbol table entry

    vector <ELF32_SHDR>                 vsh;                     // section headers
    vector <string>                     vsn;                     // section names

    vector <dis::Relocation>            vsr;                     // relocation entries

    Elf32_Off	                        shoff,                   // sections header table offset
                                        pif;                     // position in  file   
    Elf32_Half	                        shentsize,               // 1 section header's size
                                        shstrndx;                // section with string indexes
    Elf32_Half                      	nSections,               // number of section headers
                              
                        sec_bss,        sec_comment,             // sections
                        sec_data1,      sec_data,
                        sec_debug,      sec_dynamic,
                        sec_dynstr,     sec_dynsym,
                        sec_fini,       sec_got,
                        sec_hash,       sec_init,
                        sec_interp,     sec_line,
                        sec_note,       sec_plt,
                                        sec_rodata1,
                        sec_rodata,     sec_shstrtab,
                        sec_strtab,     sec_symtab,
                        sec_text,                        
                                        nSymbols,                // number of symbol table entries   
                                        t;                       // temp value
    Elf32_Word                      	sym_size,                // symbol size
                                        sec_type;                // section type

    int                                 ty,                      // type of data in section
                                        pos,                     // position
                                        adr,                     // address

                                        i, 
                                        temp;                    // temp values

    list<Variable>::iterator            var_it;
    list<Import>::iterator              imp_it;

    
    //////////////////////////////////////////////////////////////////////////////////////////
             
    cout << "Read_Binary_File: Disassembly_ELF!" << "\n";

    extra = &e;

    file_des = open(input_file.c_str(), O_RDONLY);
    if (file_des < 0)                                         // if we can't open the file
    { return RET_ERR_FILE_INPUT; }    

    fstat(file_des, &buf);

    input_file_size = buf.st_size;                            // file size

    file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, file_des, 0);
    if (file_data == MAP_FAILED)
    { close(file_des); return RET_ERR_FILE_INPUT; }    
                                     
    eh = (ELF32_HDR *) file_data;                             // the ELF HEADER starts at the beginning of the file

    pif = shoff = eh->e_shoff;
    shentsize = eh->e_shentsize;
    nSections = eh->e_shnum;

    shstrndx  = eh->e_shstrndx;
    shdr = (ELF32_SHDR * ) ((char *) file_data + shoff + ( shentsize * shstrndx));
                                                              // string table section
    section_names = ((char *) file_data + shdr->sh_offset);   // string table offset

    v_to_explore.push_back(eh->e_entry);                      // first instruction in file

    shdr = (ELF32_SHDR * ) ((char *) file_data + shoff);      // move to section headers table

    range_offset_low = range_offset_high = Imports_Offset_Raw = 
    sec_bss = sec_comment = sec_data1 = sec_data = sec_debug = sec_dynamic =
    sec_dynstr = sec_dynsym = sec_fini = sec_got = sec_hash  = sec_init   =
    sec_interp = sec_line = sec_note  = sec_plt  = sec_rodata1 = sec_rodata =
    sec_shstrtab = sec_strtab = sec_symtab = sec_text = 0;

    // stock sections info
    for (t = 0; t < nSections; t++) 
    {
     shdr = (ELF32_SHDR * ) ((char *) file_data + pif);
     pif += ELF32_SHDR_LENGTH;
     
     vsh.push_back(*shdr); 
     
     if (shdr->sh_name == 0)
     { vsn.push_back("null"); s = "null"; }
     else 
     {
      s = section_names + shdr->sh_name;                           
      vsn.push_back(s); 
      
           if (strncmp(s, ".bss", 4) == 0)          { sec_bss      = t; }
      else if (strncmp(s, ".comment", 8) == 0)      { sec_comment  = t; }
      else if (strncmp(s, ".data1", 6) == 0)        { sec_data1    = t; }
      else if (strncmp(s, ".data", 5) == 0)         { sec_data     = t; }
      else if (strncmp(s, ".debug", 6) == 0)        { sec_debug    = t; }
      else if (strncmp(s, ".dynamic", 8) == 0)      { sec_dynamic  = t; }
      else if (strncmp(s, ".dynstr", 7) == 0)       { sec_dynstr   = t; }
      else if (strncmp(s, ".dynsym", 7) == 0)       { sec_dynsym   = t; }
      else if (strncmp(s, ".fini", 5) == 0)         { sec_fini     = t; }
      else if (strncmp(s, ".got", 4) == 0)          { sec_got      = t; }
      else if (strncmp(s, ".hash", 5) == 0)         { sec_hash     = t; }
      else if (strncmp(s, ".init", 5) == 0)         { sec_init     = t; }
      else if (strncmp(s, ".interp", 7) == 0)       { sec_interp   = t; }
      else if (strncmp(s, ".line", 5) == 0)         { sec_line     = t; }
      else if (strncmp(s, ".note", 5) == 0)         { sec_note     = t; }
      else if (strncmp(s, ".plt", 4) == 0)          { sec_plt      = t; }
      else if (strncmp(s, ".hash", 5) == 0)         { sec_hash     = t; }
      else if (strncmp(s, ".rodata1", 8) == 0)      { sec_rodata1  = t; }
      else if (strncmp(s, ".rodata", 7) == 0)       { sec_rodata   = t; }
      else if (strncmp(s, ".shstrtab", 9) == 0)     { sec_shstrtab = t; }
      else if (strncmp(s, ".strtab", 7) == 0)       { sec_strtab   = t; }
      else if (strncmp(s, ".symtab", 7) == 0)       { sec_symtab   = t; }
      else if (strncmp(s, ".text", 5) == 0)         { sec_text     = t; }
     }

     section.name   = s;
     section.offset = shdr->sh_offset;                                  
     section.size   = shdr->sh_size;                                        
     v_s.push_back(section); 

     // copy section info as 'extra info'
     /////////////////////////////////////////////////////////////////////////////////
     section_info = s;
     section_info += ":  size ";
     section_info += u.int_to_hexstring(shdr->sh_size);
     section_info += " *  ";
     section_info += u.int_to_hexstring(shdr->sh_offset);
     section_info += " - ";
     section_info += u.int_to_hexstring(shdr->sh_offset + shdr->sh_size);
     section_info += " *  ";
     section_info += u.int_to_hexstring(shdr->sh_addr);
     section_info += " - ";
     section_info += u.int_to_hexstring(shdr->sh_addr + shdr->sh_size);

     Add_Extra_Info_Level_2(DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_SECTIONS, (char *) section_info.c_str(), extra);

     if (strncmp(s, DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_NOTE, 
                 strlen(DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_NOTE)
                 ) == 0)       
     { Extra_Info(DISASSEMBLY_INTEL_ELF_EXTRA_NOTE, shdr, (char *) file_data, extra); }
     else if (strncmp(s, DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_INTERP, 
                 strlen(DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_INTERP)
                 ) == 0)       
     { Extra_Info(DISASSEMBLY_INTEL_ELF_EXTRA_INTERP, shdr, (char *) file_data, extra); }
     /////////////////////////////////////////////////////////////////////////////////
    }

    // first, just load the file 

    ///////////////////// read the bytes /////////////////////////////////////////////
    for (t = 0; t < vsh.size(); t++) 
    {
     Section_Data(t, &(vsh[t]), &ty); 

     if ((   (ty == NODE_TYPE_DATA)
          && (opt->show_data_sections == false)                     // user options
         )
         ||
         (vsh[t].sh_addr == 0)
        )
     { continue; }

     cout << "ok ... " << t << "\n";

     pif = vsh[t].sh_offset;                                       // start of next section
     
     if (pif == 0)                                                 // no actual bytes in the executable   
     { continue; }         

     pos = vsh[t].sh_size;                                         //  size of this section
     adr = vsh[t].sh_addr;                                         //  location when loaded into memory
     sec_type = vsh[t].sh_type;                                 

     opcodes_mp.Reset_Available();                                 // every section its own poollist
     opcodes_mp.Set_Associated_Value(adr);
     opcodes_mp.Set_Pool_Size(pos);

     if ((range_offset_low == 0) || (adr < range_offset_low))
     { range_offset_low = adr;}

     while (pos)
     {
       if (pos >= mbpi)
       { temp = mbpi; }
       else
         temp = pos;

       section_data = ((char *) file_data + pif);     

       Initialize(&dn);

       dn.file_offset = pif;
       dn.memory_offset = adr;
       dn.n_used = temp;
       dn.section = t;

       dn.type = ty;
       dn.opcode = (char*) opcodes_mp.Use_Pool(temp);

       if (sec_type == SHT_NOBITS)                                  // section occupies no space in file
       { for (i=0; i < temp; i++) { *(dn.opcode + i) = 0; } }
       else 
       {
        for (i=0; i < temp; i++)
        { *(dn.opcode + i) = (char) *(section_data + i); }
       }

       dn.status = NODE_STATUS_UNEXPLORED;

       l_dn.push_back(dn);

       pos -= temp;
       pif += temp;
       adr += temp;
     }

    if (range_offset_high < adr)
     { range_offset_high = adr - 1;}

    cout << "l_dn size after section " << t << " : " << l_dn.size() << "\n";
    }

    // next, try and find info about functions from other libraries

    ///////////////////////// dynamic linking symbol table info //////////////////////////////////
    if (sec_dynsym > 0) 
    {
     temp = vsh[sec_dynsym].sh_link;                        // dynamic linking string table
     symbol_names = ((char *) file_data + vsh[temp].sh_offset);                 // symbol names are here

     sym = (ELF32_SYM * ) ((char *) file_data + vsh[sec_dynsym].sh_offset);     // symbol table is here
     sym_size = vsh[sec_dynsym].sh_entsize;               
     nSymbols = vsh[sec_dynsym].sh_size / sym_size;               

     for (i = 0; i < nSymbols; i++)
     {
      Section_Symbol(symbol_names, sym, extra, true);

      sym++; 
     }
    }

    // next, treat all other known symbols
    
    ///////////////////////// symbol table info /////////////////////////////////////////////////
    if ((sec_symtab > 0) && (sec_strtab > 0))
    {
     symbol_names = ((char *) file_data + vsh[sec_strtab].sh_offset);           // symbol names are here

     sym = (ELF32_SYM * ) ((char *) file_data + vsh[sec_symtab].sh_offset);     // symbol table is here
     sym_size = vsh[sec_symtab].sh_entsize;               
     nSymbols = vsh[sec_symtab].sh_size / sym_size;               

     for (i = 0; i < nSymbols; i++)
     {
      Section_Symbol(symbol_names, sym, extra, false);

      sym++; 
     }
    }

    close(file_des); 

    return RET_OK;
}

void
dis::Disassembly_Elf::Extra_Info(int section_type, ELF32_SHDR *shdr, char *file_data, dis::Extra *extra)
{   
    char                                 temp[4];           // temp value
    
    uint                                 i, j,              // indexes
                                         nl,                // name length
                                         d, dl;             // descriptor, descriptor length

    char                                *name, *desc;       // name , descriptor

    string                               s;

    bool                                 stop;

    //////////////////////////////////////////////////////////////////////////////////////////

    file_data = (char *) file_data + shdr->sh_offset;

    switch (section_type)
      {
       case DISASSEMBLY_INTEL_ELF_EXTRA_INTERP: 
       { 
        name = (char *) file_data;                   
        nl = strlen(name);

        if (nl <= shdr->sh_size)
        {
         s.assign(name, nl);

         Add_Extra_Info_Level_3(
              DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_SECTIONS, 
              DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_INTERP, 
              (char *) s.c_str(), 
              extra);
        }
        break;
       }

       case DISASSEMBLY_INTEL_ELF_EXTRA_NOTE: 
       { 
        i = 0;

        stop = false;
        while (!stop)
        {
         temp[0] = *((char*) (file_data));     temp[1] = *((char*) (file_data + 1)); 
         temp[2] = *((char*) (file_data + 2)); temp[3] = *((char*) (file_data + 3)); 
         nl = (*(uint*) (&temp));               
         temp[0] = *((char*) (file_data + 4)); temp[1] = *((char*) (file_data + 5)); 
         temp[2] = *((char*) (file_data + 6)); temp[3] = *((char*) (file_data + 7)); 
         dl = (*(uint*) (&temp));               
         
         if (nl > 0)
         {
          name = (char *) file_data + 12;
          s.assign(name, nl);
          s.insert(strlen(name), " = ");

          nl = nl & 4;

          if (dl > 0)
          {
           j = dl;
           desc = (char *) file_data + 12 + nl;

           while (j > 0)
           {
            temp[0] = *((char*) (desc));     temp[1] = *((char*) (desc + 1)); 
            temp[2] = *((char*) (desc + 2)); temp[3] = *((char*) (desc + 3)); 
            desc = (char *) desc + 4;

            d = (*(uint*) (&temp));               
            
            s.insert(s.size() - 1, u.int_to_string(d));
            
            j-= 4;

            if (j > 0)
            { s.insert(s.size() - 1, ","); }
           }                                                     
          }
                
          Add_Extra_Info_Level_3(
              DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_SECTIONS, 
              DISASSEMBLY_INTEL_ELF_EXTRA_TEXT_NOTE, 
              (char *) s.c_str(), 
              extra);
         }                                 

         i += 12 + nl + dl;
         file_data = (char *) file_data + i;

         if (i >= shdr->sh_size) {stop = true; }
        }      

        
        break;
       }
      } 
}

void
dis::Disassembly_Elf::Section_Data(int section_number,  ELF32_SHDR *section, int *result_type)
{
    Elf32_Word              section_type, section_flags;

    ////////////////////////////////////////////////////

    section_flags = section->sh_flags; 
    section_type  = section->sh_type;

    switch (section_type)
      {
       case SHT_NULL: 
          { cout << "SHT_NULL in section " << section_number << "\n"; break; }
       case SHT_PROGBITS: 
          { cout << "SHT_PROGBITS in section " << section_number << "\n"; break; }
       case SHT_SYMTAB: 
          { cout << "SHT_SYMTAB in section " << section_number << "\n"; break; }
       case SHT_STRTAB: 
          { cout << "SHT_STRTAB in section " << section_number << "\n"; break; }
       case SHT_RELA: 
          { cout << "SHT_RELA in section " << section_number << "\n"; break; }
       case SHT_HASH: 
          { cout << "SHT_HASH in section " << section_number << "\n"; break; }
       case SHT_DYNAMIC: 
          { cout << "SHT_DYNAMIC in section " << section_number << "\n"; break; }
       case SHT_NOTE: 
          { cout << "SHT_NOTE in section " << section_number << "\n"; break; }
       case SHT_NOBITS: 
          { cout << "SHT_NOBITS in section " << section_number << "\n"; break; }
       case SHT_REL: 
          { cout << "SHT_REL in section " << section_number << "\n"; break; }
       case SHT_SHLIB: 
          { cout << "SHT_SHLIB in section " << section_number << "\n"; break; }
       case SHT_DYNSYM: 
          { cout << "SHT_DYNSYM in section " << section_number << "\n"; break; }

       case SHT_NUM: 
          { cout << "SHT_NUM in section " << section_number << "\n"; break; }
       case SHT_LOPROC: 
          { cout << "SHT_LOPROC in section " << section_number << "\n"; break; }
       case SHT_HIPROC: 
          { cout << "SHT_HIPROC in section " << section_number << "\n"; break; }
       case SHT_LOUSER: 
          { cout << "SHT_LOUSER in section " << section_number << "\n"; break; }
       case SHT_HIUSER: 
          { cout << "SHT_HIUSER in section " << section_number << "\n"; break; }
      }

    if (section_flags & SHF_WRITE )           
    { cout << "SHF_WRITE in section " << section_number << "\n"; }

    if (section_flags & SHF_ALLOC) 
    { cout << "SHF_ALLOC in section " << section_number << "\n"; }
    
    if (section_flags & SHF_EXECINSTR) 
    { cout << "SHF_EXECINSTR in section " << section_number << "\n"; }

    if (section_flags & SHF_MASKPROC) 
    { cout << "SHF_MASKPROC in section " << section_number << "\n"; }
    
    //----------------------------------------------------------------------------------

    if (    (section_type == SHT_PROGBITS ) 
         && (section_flags & SHF_ALLOC) 
         && (section_flags & SHF_EXECINSTR) 
       )                                                // Section contains code
    {
      *result_type = NODE_TYPE_CODE;
      cout << "Code in section " << section_number << " of " << input_file.c_str() << "\n";
    }
    else 
      *result_type = NODE_TYPE_DATA;                       
}

void
dis::Disassembly_Elf::Section_Symbol(char *symbol_names, ELF32_SYM *sym, Extra *extra, bool import_function)
{
 int                                 i, temp, // temp values
                                     o;          // offset

 unsigned char                       st_i;                    // symbol type info

 bool                                sym_variable,            // this symbol is a variable
                                     sym_function;            // this symbol is a function

 char                               *n;                       // name

 Disassembly_Node                    dn;   

 Import                              imp;
 Variable                            var;

 Api                                 a;
 Routine                            *r;
 Function                            f;

 list<Disassembly_Node>::iterator    it, it_ref;

 list<Variable>::iterator            var_it;
 list<Import>::iterator              imp_it;
 list<Function>::iterator            f_it;

 list<dis::Reference>::iterator      ref_it; 

 /////////////////////////////////////////////////////////////////////////
 
 st_i = sym->st_info;

 sym_variable = sym_function = false;

 o    = sym->st_value;                                   // symbol offset
 n    = symbol_names + sym->st_name;                     // symbol name
 temp = sym->st_size;                                    // symbol size

 if      (ELF_ST_TYPE(st_i ) == STT_OBJECT) { sym_variable = true;} // symbol is a variable
 else if (ELF_ST_TYPE(st_i) == STT_FUNC)    { sym_function = true;} // symbol is a function

 else if (ELF_ST_TYPE(st_i) == STT_FILE)                            // symbol is a source file
      { Add_Extra_Info_Level_2("Source Files", n , extra);
        return;
      } 
 else { return; }

 it = Get_Disassembly_Node_From_Offset(o, false);     // contains the symbol

 if (   ((temp > 0) || (sym_function == true))        // first test irrelevant for a function 
      && (it != l_dn.end())
      && (it->status == NODE_STATUS_UNEXPLORED)
      && (   (sym_function == true) && (it->type == NODE_TYPE_CODE)
          || (sym_variable == true) && (it->type == NODE_TYPE_DATA)
         )
    )
 { 
  if (sym_function) {temp = 1;}                       // size is irrelevant for function

  Isolate_l_dn(it, o, temp, NODE_STATUS_AUTO);        // put the symbol in its own node
 

  it = Get_Disassembly_Node_From_Offset(o, true);     // select the node with only the symbol
  if (it == l_dn.end())  { return; }

  if (sym_variable)
  {                                                                                          
   it->status = NODE_STATUS_AUTO;
   it->type = NODE_TYPE_DATA;

   it->label = (char *) mp.Ensure_Minimum_Allocation(it->label, strlen(n) + 1);
   strcpy(it->label, n);      

   Add_Reference(it, o, true);                        // references the location that will get called

   Initialize(&var);
   var.name = n;
   var.memory_offset = o;
   l_v.push_back(var);                                // add new variable to the list   

     
   var_it = l_v.end();
   var_it--;

   Update_Reference_For_Variable(var_it, o);          // add the data info    
  }       
  else if (sym_function)
  {
   // disassemble just 1 instruction
   Convert_Opcodes_From_Offset(it, 1);     

   ref_it = it->ref_out;

   if ((import_function) && (ref_it != l_r.end()))           // the node containing the function refers to an other address
   {
    temp = ref_it->memory_offset;
    it_ref = Get_Disassembly_Node_From_Offset(temp, false);  // node containing the referenced address

    if (it_ref != l_dn.end())
    {
     Isolate_l_dn(it_ref, temp, address_bitness, NODE_STATUS_AUTO); // put the referenced address in its own node   
    }
    else 
    {
     Initialize (&dn);

     dn.file_offset = 0;
     dn.memory_offset = temp;
     dn.type = NODE_TYPE_DATA;

     dn.status = NODE_STATUS_AUTO;
     dn.n_used = address_bitness;   

     dn.opcode = (char*) opcodes_mp.Use_Pool(address_bitness);
     for (i= 0; i < address_bitness; i++) { *(dn.opcode + i) = 0; } 

     l_dn.insert(it_ref, dn);
    }

   it->status = NODE_STATUS_AUTO;
   it->type = NODE_TYPE_CODE;

   //it->label = (char *) mp.Ensure_Minimum_Allocation(it->label, strlen(n) + 1);
   //strcpy(it->label, n);      

   Add_Reference(it, temp, true);                     // references the location that will get called

   Initialize(&imp);

   imp.library = "?";                                 // name of library that provides the imported function
   imp.name = n;                                      // name of imported function
   imp.memory_offset = temp;                          // address of imported function
   l_i.push_back(imp);                                // add new import to the list   

   imp_it = l_i.end();
   imp_it--;

   Update_Reference_For_Import(imp_it, imp.memory_offset); // add the Import info

   /*
   l = imp.library.size() + imp.name.size() + 2;
   it->label = (char *) mp.Ensure_Minimum_Allocation(it->label, l);
    {
    strncpy(it->label, imp.library.c_str(), imp.library.size());
    strncat(it->label, " ", 1);
    strncat(it->label, imp.name.c_str(), imp.name.size());
    }
   */

   }  
   else if (!import_function)                         // this location is itself a function
   {
    Initialize(&a);                                   // first, add it to the list of known functions

    r = (Routine*) mp.Use_Pool(sizeof(Routine));
    Initialize(r);

    temp = strlen(n);
    r->name = (char*) mp.Use_Pool(temp + 1);
    strncpy(r->name, n, temp + 1);

    a.file_name = input_file;
    a.routine = r;

    Add_Routine(&a, r);

    Add_Reference(it, o, true);                       // then make sure there is a Reference to it

    Initialize(&f);
    f.memory_offset = o;
    f.name = n;
    l_f.push_back(f);                                     // add new function to the list   

    f_it = l_f.end();
    f_it--;

    Update_Reference_For_Function(f_it, o); 
   }
  }  
 }
}


