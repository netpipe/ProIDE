/*
 * Disassembly_intel_raw.cpp: Routines for disassembling unknown intel file 
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

using namespace std;

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


#include "disassembly_intel_raw.hh"

 
dis::Disassembly_Intel_Raw::Disassembly_Intel_Raw(
        string* file_name, 
        dis::Main_Gui *mg,
        dis::Disassembly_Options *options) 
        :
        Disassembly_Intel(file_name, mg, options)  
{
    address_bitness = operand_bitness = DISASSEMBLY_BITNESS_32;

    disassembly_type = DISASSEMBLY_TYPE_INTEL_RAW;

    cout << "Constructor Disassembly_Intel_Raw(filename, mg, opt)" << "\n";
}

dis::Disassembly_Intel_Raw::Disassembly_Intel_Raw(
        string* file_name, 
        dis::Disassembly_Options *options) 
        :
        Disassembly_Intel(file_name, options)  
{
    address_bitness = operand_bitness = DISASSEMBLY_BITNESS_32;

    disassembly_type = DISASSEMBLY_TYPE_INTEL_RAW;

    cout << "Constructor Disassembly_Intel_Raw(filename, opt)" << "\n";
}

dis::Disassembly_Intel_Raw::~Disassembly_Intel_Raw()
{
    cout << "Destructor Disassembly_WinPE" << "\n";
}

void  
dis::Disassembly_Intel_Raw::Fill_Disassembly_Node(int pif, int n_read)
{
   Disassembly_Node       dn;
   int                    i;                       // index

   ////////////////////////////////////////////////////////////////////////////////////////////
    Initialize(&dn);
   
    dn.file_offset = pif;
    dn.memory_offset = pif;
    dn.n_used = n_read;

    dn.type = NODE_TYPE_CODE;
    dn.opcode = (char*) opcodes_mp.Use_Pool(n_read);
    
    for (i=0; i < n_read; i++)
        { *(dn.opcode + i) = raw_data[i]; }

    dn.status = NODE_STATUS_UNEXPLORED;

    l_dn.push_back(dn);
}

int  dis::Disassembly_Intel_Raw::Phase_1a_File()
{
    int                    i,                       // index
                           pif;                     // position in file
    char                   ch;                      // temp value

    int                    fl,                      // length of input file
                           file_des;                // file descriptor
    struct stat            buf;


    //////////////////////////////////////////////////////////////////////////////////////////


    cout << "Read_Binary_File: Disassembly_Intel_Raw!" << "\n";

    file_des = open(input_file.c_str(), O_RDONLY);
    if (file_des < 0)                                         // if we can't open the file
    { return RET_ERR_FILE_INPUT; }    

    fstat(file_des, &buf);
    input_file_size = fl = buf.st_size;                      // file size

    close(file_des); 

    ifstream bf(input_file.c_str());
    if (!bf)                                                 // if we can't open the file
    {
     return RET_ERR_FILE_INPUT;       
    }                         

    pif = i = 0;
    v_to_explore.push_back(0);

    opcodes_mp.Set_Associated_Value(0);
    opcodes_mp.Set_Pool_Size(fl);

    while (bf.get(ch))
     {
       raw_data[i] = ch;

       if (i + 1 >= DEF_BYTES_PER_NODE)
       {
        Fill_Disassembly_Node(pif, i + 1);
           
        pif += i + 1;
        i = 0;
       }
       else
       {
        i++;
       }
     }

    if (i > 0)
    {
     Fill_Disassembly_Node(pif, i);
    }

    range_offset_low = 0;
    range_offset_high = pif + i - 1;

    //if ( opt->delay_gui_init == false)
    //{ Gui_Command(GUI_COMMAND_INIT); }
    

    return RET_OK;
}





