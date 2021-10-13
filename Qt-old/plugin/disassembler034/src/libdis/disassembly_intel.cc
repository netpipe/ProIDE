/*
 * disassembly_intel: disassembling intel opcodes 
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

using namespace std;

#include <stdio.h>

#include "disassembly_intel.hh"
#include <cstring>
#include <algorithm>


 
dis::Disassembly_Intel::Disassembly_Intel()
{
    cout << "Constructor Disassembly_Intel()" << "\n";

    mbpi = DEF_BYTES_PER_NODE;

    disassembly_type = DISASSEMBLY_TYPE_INTEL;
}

dis::Disassembly_Intel::Disassembly_Intel (
        string* file_name, 
        dis::Main_Gui *mg,
        dis::Disassembly_Options *options) 
        :
        Disassembly(file_name, mg, options)     

{
    cout << "Constructor Disassembly_Intel(filename, mg, opt)" << "\n";

    mbpi = DEF_BYTES_PER_NODE;

    disassembly_type = DISASSEMBLY_TYPE_INTEL;
}

dis::Disassembly_Intel::Disassembly_Intel (
        string* file_name, 
        dis::Disassembly_Options *options) 
        :
        Disassembly(file_name, options)     

{
    cout << "Constructor Disassembly_Intel(filename, opt)" << "\n";

    mbpi = DEF_BYTES_PER_NODE;

    disassembly_type = DISASSEMBLY_TYPE_INTEL;
}


dis::Disassembly_Intel::~Disassembly_Intel()
{
    cout << "Destructor Disassembly_Intel" << "\n";
}
                                         
int
dis::Disassembly_Intel::Convert_Opcodes(std::vector<int> &v)
// convert  all opcodes to assembler
{
    cout << "Convert Opcodes:  Disassembly_Intel" << "\n";

    int                              offset;  // offset of opcode                    

    list<Disassembly_Node>::iterator it;      // for finding the next to be looked at Disassembly_Node
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
   
    it = l_dn.begin(); 

    while (v.size() > 0)
    {
     offset = v.back();
     v.pop_back();

     // find the Disassembly_Node that contains the offset
     it = Get_Disassembly_Node_From_Offset(offset, false);

     if (   (it != l_dn.end())
         && (it->status == NODE_STATUS_UNEXPLORED)
         && (it->type == NODE_TYPE_CODE))
     {
         // Split the node if it contains leading opcodes 
         if (it->memory_offset < offset)           // target byte is not at beginning of Disassembly_Node 
         { 
             Split_l_dn(it, offset - it->memory_offset, true);   

             it++;
         }
     
         // disassemble all the code we can be sure of after this offset
         Convert_Opcodes_From_Offset(it, 0);        
     }                                        

     Convert_Jump_Tables();
    }

    return RET_OK;
}

int
dis::Disassembly_Intel::Convert_SIB_Byte(list<Disassembly_Node>::iterator it,
                                        int position, RegMemPart *rmp)
{
  short              ss, index, base;            // SIB byte
  short              mod;                        // MOD/RM byte
  Displacement       *d;
  int                bytes_used = 0;
  char               opcode[4];

  //////////////////////////////////////////////////////////////////////////////////
    
  if (Get_Byte_From_Disassembly_Node(it, position + 1, &opcode[1]) == RET_ERR_OPCODE)
     {
       return 0; 
     }              
  ss      = (opcode[1] & 0xC0) >> 6;                     // 0xC0 = 11000000b  
  index   = (opcode[1] & 0x38) >> 3;                     // 0x38 = 00111000b  
  base    =  opcode[1] & 0x7;                            // 0x7  = 00000111b  

  rmp->abs   = false;
    
  d = rmp->displ;
  if (d == 0) { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); rmp->displ = d;}  
  Initialize(d);

  if (base != index)
    {
      d->reg2 = index + 1;
      switch (ss)
      { 
       {case 1: d->mul2 = 2; break;}
       {case 2: d->mul2 = 4; break;}
       {case 3: d->mul2 = 8; break;}
      }
    }
  else if ((base != 4) && (base != 5)) // esp, ebp
    {
    switch (ss)
     { 
      {case 0: d->mul = 2; break;}
      {case 1: d->mul = 3; break;}
      {case 2: d->mul = 5; break;}
      {case 3: d->mul = 9; break;}
     }
    }
    
  switch (base)
  {
  case 0:                       // [eax]
  case 1:                       // [ecx]
  case 2:                       // [edx]
  case 3:                       // [ebx]
  case 4:                       // [esp]
  case 6:                       // [esi]
  case 7:                       // [edi]
   {  
    rmp->reg32 = base + 1;
    bytes_used = 1;
    break;
   }
  case 5:                       // [ebp ...]
   {
    if (Get_Byte_From_Disassembly_Node(it, position, &opcode[0]) == RET_ERR_OPCODE)
      { return 0; }       

    mod = (opcode[0] & 0xC0) >> 6;      // 0xC0 = 11000000b  
    switch (mod)
    {
     case 1: 
     case 2:
     { 
      rmp->reg32 = 6;                   // ebp
      bytes_used = 1;
      break;
     }
     case 0:                            // special case: only one register  + disp32
     { 
      rmp->reg32 = d->reg2;
      d->reg2 = 0;
      d->mul = d->mul2;
      d->mul2 = 0;

      for (bytes_used =0; bytes_used < 4; bytes_used++)    
      { if (Get_Byte_From_Disassembly_Node(it, position + 2 + bytes_used, &opcode[bytes_used]) == RET_ERR_OPCODE)
         { return RET_ERR_OPCODE; }
      }          
      d->add = *(int*) (&opcode);               
      bytes_used++;
      break;
     }
    }
   }
  } 

  return bytes_used;
}

int
dis::Disassembly_Intel::Convert_Address_16(int bitn, list<Disassembly_Node>::iterator it,
                                        int position,
                                        int use_reg_opc,
                                        int *refer,
                                        Instruction *instr)
{
  int           bytes_used = 0;
  int           i;                          // index
  char          opcode;  
  char          ref[4];
  short         mod, reg_opc, rm;           // Mod, Reg/Opcode, R/M
  Displacement *d;
  RegMemPart *rmp, *rmp2;                   // parts 1 and 2 of instr
  RegMemPart t_rmp;                         // temp value

  //////////////////////////////////////////////////////////////////////////////////

  d = 0;

  if (Get_Byte_From_Disassembly_Node(it, position, &opcode) == RET_ERR_OPCODE)
      { return 0; }                                  
  mod     = (opcode & 0xC0) >> 6;                     // 0xC0 = 11000000b  
  reg_opc = (opcode & 0x38) >> 3;                     // 0x38 = 00111000b  
  rm      = opcode & 0x7;                             // 0x7  = 00000111b  
  
  rmp  = &(instr->part1);
  rmp2 = &(instr->part2);
  
  if ((rm < 5) && (mod == 0)
   || (mod == 1)
   || (mod == 2))
  {
   d = rmp->displ;
   if (d == 0)
    { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); Initialize(d); rmp->displ = d;}  
  }                   
  
  if (use_reg_opc > 0)              // if use_reg_opc == 0, this part of the opcode isn't used
  {                      
   rmp2->abs   = true;
   switch (bitn)
   {
   case DISASSEMBLY_BITNESS_32: 
    { rmp2->reg32 = reg_opc + 1; break;}
   case DISASSEMBLY_BITNESS_16: 
    { rmp2->reg16 = reg_opc + 1; break;}
   case DISASSEMBLY_BITNESS_08: 
    { rmp2->reg08 = reg_opc + 1; break;}
   }
  }

  switch (mod)
  {
     case 0:
     {
          rmp->abs    = false;
          bytes_used = 1;

          switch (rm)
          {
          case  0:                         //  [bx + si]
            {
             rmp->reg16  = 4;              // bx
             d->reg2     = 7;              // si 
             break;
            }
          case  1:                          //  [bx + di]
            {
             rmp->reg16  = 4;              // bx
             d->reg2     = 8;              // di 
             break;
            }                                  
          case  2:                          //  [bp + si]
            {
             rmp->reg16  = 6;              // bp
             d->reg2     = 7;              // si 
             break;
            }     
          case  3:                         //  [bp + di]
            {
             rmp->reg16  = 6;              // bp
             d->reg2     = 8;              // di 
             break;
            }               
          case  4:                         //  [si]
            {
             rmp->reg16  = 7;              // si
             break;
            }               
          case  5:                         //  [di]
            {
             rmp->reg16  = 8;              // di
             break;
            }               
          case  6:                         // disp16
            {
             for (i=0; i < 2; i++)    
             {
                 if (Get_Byte_From_Disassembly_Node(it, position + 1 + i, &ref[i]) == RET_ERR_OPCODE)
                 { return RET_ERR_OPCODE; }
             }          
             rmp->imm = (*(int*) (&ref) & 0xFFFF);               

             bytes_used = 3;
             break;
            }
          case  7:                         //  [bx]
           {
             rmp->reg16  = 5;              // bx
             break;
           }       
          }
          break;
     }

     case 1:
     {
          if (Get_Byte_From_Disassembly_Node(it, position + 1, &ref[0]) == RET_ERR_OPCODE)
                 { return RET_ERR_OPCODE; }                           
          i = (*(int*) (&ref) & 0xFF);    // first byte only   
          if (i > 127)
          { d->add = 0 - (256 - i); }
          else { d->add = i; }
          
          rmp->abs    = false;
          bytes_used  = 2;

          switch (rm)
          {
          case  0:                         //  [bx + si + disp8]
            {
             rmp->reg16  = 4;              // bx
             d->reg2     = 7;              // si 
             break;
            }
          case  1:                          //  [bx + di + disp8]
            {
             rmp->reg16  = 4;              // bx
             d->reg2     = 8;              // di 
             break;
            }                                  
          case  2:                          //  [bp + si + disp8]
            {
             rmp->reg16  = 6;              // bp
             d->reg2     = 7;              // si 
             break;
            }     
          case  3:                         //  [bp + di + disp8]
            {
             rmp->reg16  = 6;              // bp
             d->reg2     = 8;              // di 
             break;
            }               
          case  4:                         //  [si + disp8]
            {
             rmp->reg16  = 7;              // si
             break;
            }               
          case  5:                         //  [di + disp8]
            {
             rmp->reg16  = 8;              // di
             break;
            }               
          case  6:                         // [bp + disp8]
            {
             rmp->reg16  = 6;              // bp
             break;
            }
          case  7:                         //  [bx + disp8]
           {
             rmp->reg16  = 4;              // bx
             break;
           }       
          }
          break;
     }

     case 2:
     {
          for (i=0; i < 2; i++)    
             {
                 if (Get_Byte_From_Disassembly_Node(it, position + 1 + i, &ref[i]) == RET_ERR_OPCODE)
                 { return RET_ERR_OPCODE; }
             }          
          d->add = (*(int*) (&ref) & 0xFFFF);               
                          
          rmp->abs    = false;
          bytes_used  = 3;

          switch (rm)
          {
          case  0:                         //  [bx + si + disp16]
            {
             rmp->reg16  = 4;              // bx
             d->reg2     = 7;              // si 
             break;
            }
          case  1:                          //  [bx + di + disp16]
            {
             rmp->reg16  = 4;              // bx
             d->reg2     = 8;              // di 
             break;
            }                                  
          case  2:                          //  [bp + si + disp16]
            {
             rmp->reg16  = 6;              // bp
             d->reg2     = 7;              // si 
             break;
            }     
          case  3:                         //  [bp + di + disp16]
            {
             rmp->reg16  = 6;              // bp
             d->reg2     = 8;              // di 
             break;
            }               
          case  4:                         //  [si + disp16]
            {
             rmp->reg16  = 7;              // si
             break;
            }               
          case  5:                         //  [di + disp16]
            {
             rmp->reg16  = 8;              // di
             break;
            }               
          case  6:                         // [bp + disp16]
            {
             rmp->reg16  = 6;              // bp
             break;
            }
          case  7:                         //  [bx + disp16]
           {
             rmp->reg16  = 4;              // bx
             break;
           }               
          }
          break;
     }
     case 3:
     {
       rmp->abs    = true;
       bytes_used = 1;

       switch (bitn)
       {
       case DISASSEMBLY_BITNESS_32: 
           { rmp->reg32 = reg_opc + 1; break;}
       case DISASSEMBLY_BITNESS_16:    
           { rmp->reg16 = reg_opc + 1; break;}
       case DISASSEMBLY_BITNESS_08: 
           { rmp->reg08 = reg_opc + 1; break;}
       }  
       break;
     } 
  }

  if (use_reg_opc == 1)              // indicating that part 1 must contain a register
  { t_rmp = *rmp; *rmp  = *rmp2; *rmp2 = t_rmp; }                       

  return bytes_used;       
}

int
dis::Disassembly_Intel::Convert_Address_32(int bitn, list<Disassembly_Node>::iterator it,
                                        int position,
                                        int use_reg_opc,
                                        int *refer,
                                        Instruction *instr)
{
  int               bytes_used = 0;
  int               i;                     // index
  char              opcode;  
  char              ref[4];
  short             mod, reg_opc, rm;      // Mod, Reg/Opcode, R/M
  RegMemPart        t_rmp;                 // temp value
  RegMemPart        *rmp, *rmp2;           // parts 1 and 2 of instr
  Displacement      *d;
    
  //////////////////////////////////////////////////////////////////////////////////
  
  if (Get_Byte_From_Disassembly_Node(it, position, &opcode) == RET_ERR_OPCODE)
      {
        return 0; 
      }                                  

  mod     = (opcode & 0xC0) >> 6;                     // 0xC0 = 11000000b  
  reg_opc = (opcode & 0x38) >> 3;                     // 0x38 = 00111000b  
  rm      = opcode & 0x7;                             // 0x7  = 00000111b  
  
  rmp  = &(instr->part1);
  rmp2 = &(instr->part2);

  if (use_reg_opc > 0)
  {                   
   rmp2->abs   = true;
   switch (bitn)
   {
   case DISASSEMBLY_BITNESS_32: 
    { rmp2->reg32 = reg_opc + 1; break;}
   case DISASSEMBLY_BITNESS_16: 
    { rmp2->reg16 = reg_opc + 1; break;}
   case DISASSEMBLY_BITNESS_08: 
    { rmp2->reg08 = reg_opc + 1; break;}
   }
  }
       
  switch (mod)
  {
     case 0:
       {
          switch (rm)
          {
          case  0:                         //  [eax]
          case  1:                         //  [ecx]
          case  2:                         //  [edx]
          case  3:                         //  [ebx]
          case  6:                         //  [esi]
          case  7:                         //  [edi]
           {
             rmp->reg32  = rm + 1;
             rmp->abs    = false;
             bytes_used = 1;
             break;
           }
          case  4:                         //  [--][--]
           {
             bytes_used = Convert_SIB_Byte(it, position, rmp) + 1;
             break;                   
           }
          case  5:                         //  disp32
           {
             for (i=0; i < 4; i++)    
             {
                 if (Get_Byte_From_Disassembly_Node(it, position + 1 + i, &ref[i]) == RET_ERR_OPCODE)
                 { return RET_ERR_OPCODE; }
             }          
             rmp->imm = *(int*) (&ref);               
             rmp->abs = false;               

             bytes_used = 5;
             break;
           }
          }
          break;
       }
      case 1:
       {
         switch (rm)
         {
         case  0:                         //  [eax]
         case  1:                         //  [ecx]
         case  2:                         //  [edx]
         case  3:                         //  [ebx]
         case  5:                         //  [esp]
         case  6:                         //  [esi]
         case  7:                         //  [edi]
          {
            if (rm == 5)
            { rmp->reg32 = 6;}              // ebp     
            else
            { rmp->reg32  = rm + 1;}

            rmp->abs    = false;

            if (Get_Byte_From_Disassembly_Node(it, position + 1 , &ref[0]) == RET_ERR_OPCODE)
                { return RET_ERR_OPCODE; }
            i = (*(int*) (&ref) & 0xFF); // first byte only              

            d = rmp->displ;
            if (d == 0)
            { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); Initialize (d); rmp->displ = d;}  

            if (i > 127)
            { d->add = 0 - (256 - i); }
            else { d->add = i; }

            bytes_used = 2;
            break;
          }
         case  4:                         //  disp8 [--][--]
          {
            bytes_used = Convert_SIB_Byte(it, position, rmp);

            if (Get_Byte_From_Disassembly_Node(it, position + bytes_used + 1 , &ref[0]) == RET_ERR_OPCODE)
                { return RET_ERR_OPCODE; }
            i = (*(int*) (&ref) & 0xFF); // first byte only              
            
            d = rmp->displ;
            if (d == 0)
            { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); Initialize(d); rmp->displ = d;}  

            if (i > 127)
            { d->add = 0 - (256 - i); }
            else { d->add = i; }

            bytes_used += 2;
            break;
          }
         }
         break;
      }
     case 2:
     {
      switch (rm)
      {
      case  0:                         //  [eax]
      case  1:                         //  [ecx]
      case  2:                         //  [edx]
      case  3:                         //  [ebx]
      case  5:                         //  [esp]
      case  6:                         //  [esi]
      case  7:                         //  [edi]
       {
         if (rm == 5)
          { rmp->reg32 = 6;}              // ebp     
         else
          { rmp->reg32  = rm + 1;}

         rmp->abs    = false;

         for (i=0; i < 4; i++)    
         {
             if (Get_Byte_From_Disassembly_Node(it, position + 1 + i, &ref[i]) == RET_ERR_OPCODE)
             { return RET_ERR_OPCODE; }
         }                             
         
         d = rmp->displ;
         if (d == 0)
         { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); Initialize(d); rmp->displ = d;}  
         
         d->add = *(int*) (&ref);               

         bytes_used = 5;
         break;
       }
      case  4:                         //  disp32[--][--]
       {
         bytes_used = Convert_SIB_Byte(it, position, rmp);

         for (i=0; i < 4; i++)    
         {
             if (Get_Byte_From_Disassembly_Node(it, position + bytes_used + 1 + i, &ref[i]) == RET_ERR_OPCODE)
             { return RET_ERR_OPCODE; }
         }                             
         
         d = rmp->displ;
         if (d == 0)
         { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); Initialize(d); rmp->displ = d;}  

         d->add = *(int*) (&ref);               

         bytes_used += 5;
         break;
       }
      }
      break;
    }
    case 3:
    {
     rmp->abs    = true;
     bytes_used = 1;

     switch (bitn)
     {
     case DISASSEMBLY_BITNESS_32: 
       { rmp->reg32 = rm + 1; break;}
     case DISASSEMBLY_BITNESS_16: 
       { rmp->reg16 = rm + 1; break;}
     case DISASSEMBLY_BITNESS_08: 
       { rmp->reg08 = rm + 1; break;}
     }
    }
    break;
  };       

  if (use_reg_opc == 1)              // indicating that part 1 must contain a register
  { t_rmp = *rmp; *rmp  = *rmp2; *rmp2 = t_rmp; }                       
                                  
  i = rmp->imm;
  if (rmp2->imm > i) {i = rmp2->imm;}
  if (rmp->displ) 
   { if (rmp->displ->add > i) {i = rmp->displ->add;}
     if (rmp->displ->add2 > i) {i = rmp->displ->add2;}
  }
  if (rmp2->displ) 
   { if (rmp2->displ->add > i) {i = rmp2->displ->add;}
     if (rmp2->displ->add2 > i) {i = rmp2->displ->add2;}
  }
  if (i > 0) {*refer = i;}

  return bytes_used;       
}

int
dis::Disassembly_Intel::Convert_Opcodes_CoProcessor(list<Disassembly_Node>::iterator it, char init_byte,
                                           int *ref, int *target, int position, int *n_bytes_pushed,
                                           int bitn_a, int *bitn_o, Instruction *instr,
                                           bool wait_flag)
{
    char   opcode,                      // next opcode to disassemble
           param[DISASSEMBLY_MAX_INSTRUCTION_LENGTH] // extensions to the opcode
           ;

    int        reg,                          // register field of opcode
               result,                       // return code
               n_bytes_used,                 // number of bytes in this instruction
               i                             // index = temp value
               ;

    RegMemPart //*rmp3,
               *rmp, *rmp2;          // parts 1, 2 and 3 of instr



    ////////////////////////////////////////////////////////////////////////////////////////
    /// returns number of bytes used by the instruction                                   //
    ////////////////////////////////////////////////////////////////////////////////////////

    n_bytes_used = 0;


    result = Get_Byte_From_Disassembly_Node(it, position + 1, &opcode);  
    if (result == RET_ERR_OPCODE) { return 0; }                             

    for (i=0; i < DISASSEMBLY_MAX_INSTRUCTION_LENGTH; i++) { param[i] = '\000';}

    rmp  = &(instr->part1);
    rmp2 = &(instr->part2);

    instr->coprocessor = true;        

    switch (init_byte) 
    {                                               
    case 0xD8:      
      {
       ////////////////////////////////////  D8 /////////////////////////////////////////////////
         
              
       switch (opcode & 0xFF) 
       {                                               
       case 0xC0:                                 // fadd
       case 0xC1:      
       case 0xC2:      
       case 0xC3:      
       case 0xC4:      
       case 0xC5:      
       case 0xC6:      
       case 0xC7:      

       case 0xC8:                                 // fmul
       case 0xC9:      
       case 0xCA:      
       case 0xCB:      
       case 0xCC:      
       case 0xCD:      
       case 0xCE:      
       case 0xCF:      

       case 0xD0:                                 // fcom
       case 0xD1:      
       case 0xD2:      
       case 0xD3:      
       case 0xD4:      
       case 0xD5:      
       case 0xD6:      
       case 0xD7:      

       case 0xD8:                                 // fcomp
       case 0xD9:      
       case 0xDA:      
       case 0xDB:      
       case 0xDC:      
       case 0xDD:      
       case 0xDE:      
       case 0xDF:      
       
       case 0xE0:                                 // fsub
       case 0xE1:      
       case 0xE2:      
       case 0xE3:      
       case 0xE4:      
       case 0xE5:      
       case 0xE6:      
       case 0xE7:      
       
       case 0xE8:                                 // fsubr
       case 0xE9:      
       case 0xEA:      
       case 0xEB:      
       case 0xEC:      
       case 0xED:      
       case 0xEE:      
       case 0xEF:      

       case 0xF0:                                 // fdiv
       case 0xF1:      
       case 0xF2:      
       case 0xF3:      
       case 0xF4:      
       case 0xF5:      
       case 0xF6:      
       case 0xF7:      

       case 0xF8:                                 // fdivr
       case 0xF9:      
       case 0xFA:      
       case 0xFB:      
       case 0xFC:      
       case 0xFD:      
       case 0xFE:      
       case 0xFF:      

         {
          rmp->fp_reg = 1;

          i = (opcode & 0xFF); 
          if ((i >= 0xC0) && (i <= 0xC7))
           { rmp2->fp_reg = i - 0xC0 + 1; }
          else if ((i >= 0xC8) && (i <= 0xCF))
           { rmp2->fp_reg = i - 0xC8 + 1; }
          else if ((i >= 0xD0) && (i <= 0xD7))
           { rmp2->fp_reg = i - 0xD0 + 1; }
          else if ((i >= 0xD8) && (i <= 0xDF))
           { rmp2->fp_reg = i - 0xD8 + 1; }
          else if ((i >= 0xE0) && (i <= 0xE7))
           { rmp2->fp_reg = i - 0xE0 + 1; }
          else if ((i >= 0xE8) && (i <= 0xEF))
           { rmp2->fp_reg = i - 0xE8 + 1; }
          else if ((i >= 0xF0) && (i <= 0xF7))
           { rmp2->fp_reg = i - 0xF0 + 1; }
          else if ((i >= 0xF8) && (i <= 0xFF))
           { rmp2->fp_reg = i - 0xF8 + 1; }

          instr->mnemonic = (char*) &(intel_mnemonic_D8[opcode & 0xFF]);        
          
          rmp->used = true;
          rmp2->used = true;

          n_bytes_used = 2;

          break;
         }
           
       default:
         { 
             reg = (opcode & 0x38) >> 3;                     // 0x38 = 111000b
             switch (reg) 
             {                                               
             case 0:                        // fadd
             case 1:                        // fmul
             case 2:                        // fcom
             case 3:                        // fcomp
             case 4:                        // fsub
             case 5:                        // fsubr
             case 6:                        // fdiv
             case 7:                        // fdivr
               {
                instr->mnemonic = (char*) &(intel_mnemonic_D8_[reg]);  // mnemonic
          
                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                *rmp2 = *rmp;
                Initialize(rmp);
                rmp->fp_reg = 1;

                rmp->used = true;
                rmp2->used = true;

                n_bytes_used += 1;

                break;
               }

             default:                                                             
               {
                cout << "unknown opcode after byte " << init_byte << " = " << hex << (opcode & 0xFF)
                     << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";    
                n_bytes_used = 0;
               }
             }
         }      
       }

       break;

       ////////////////////////////////////  D8 /////////////////////////////////////////////////
      }

    case 0xD9:      
      {
       ////////////////////////////////////  D9 /////////////////////////////////////////////////
          
       switch (opcode & 0xFF) 
       {                                               
       case 0xC0:      // fld
       case 0xC1:      
       case 0xC2:      
       case 0xC3:      
       case 0xC4:      
       case 0xC5:      
       case 0xC6:      
       case 0xC7:      
         {
          i = (opcode & 0xFF); 
          if ((i >= 0xC0) && (i <= 0xC7))
                { rmp->fp_reg = i - 0xC0 + 1; }

          instr->mnemonic = (char*) &(intel_mnemonic_D9[opcode & 0xFF]);        

          rmp->used = true;

          n_bytes_used = 2;

          break;
          }

       case 0xC8:      // fxch
       case 0xC9:      
       case 0xCA:      
       case 0xCB:      
       case 0xCC:      
       case 0xCD:      
       case 0xCE:      
       case 0xCF:      
         {
          i = (opcode & 0xFF); 
          if ((i >= 0xC8) && (i <= 0xCF))
                { rmp->fp_reg = i - 0xC8 + 1; }

          instr->mnemonic = (char*) &(intel_mnemonic_D9[opcode & 0xFF]);        

          rmp->used = true;

          n_bytes_used = 2;

          break;
          }

       case 0xD0:       // fnop
       case 0xE0:       // fchs
       case 0xE1:       // fabs
       case 0xE4:       // ftst
       case 0xE5:       // fxam
       case 0xE8:       // fld1
       case 0xE9:       // fldl2t
       case 0xEA:       // fldl2e
       case 0xEB:       // fldpi
       case 0xEC:       // fldlg2
       case 0xED:       // fldln2
       case 0xEE:       // fldz
       case 0xF0:       // f2xm1
       case 0xF1:       // fyl2x
       case 0xF2:       // fptan
       case 0xF3:       // fpatan
       case 0xF4:       // fxtract
       case 0xF5:       // fprem1
       case 0xF6:       // fdecstp
       case 0xF7:       // fincstp
       case 0xF8:       // fprem
       case 0xF9:       // fyl2xp1
       case 0xFA:       // fsqrt
       case 0xFB:       // fsincos
       case 0xFC:       // frndint
       case 0xFD:       // fscale
       case 0xFE:       // fsin
       case 0xFF:       // fcos   
         {
          instr->mnemonic = (char*) &(intel_mnemonic_D9[opcode & 0xFF]);        
          n_bytes_used = 2;

          break;
         }
           
       default:
         { 
             reg = (opcode & 0x38) >> 3;                     // 0x38 = 111000b
             switch (reg) 
             {                                               
             case 0:                        // fld
             case 2:                        // fst
             case 3:                        // fstp
             case 4:                        // fldenv
             case 5:                        // fldcw
               {
                instr->mnemonic = (char*) &(intel_mnemonic_D9_[reg]);  // mnemonic
          
                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                rmp->used = true;

                n_bytes_used += 1;

                break;
               }

             case 6:      
               {
                if (wait_flag == true)
                    {instr->mnemonic = (char*) &(intel_mnemonic_D9_6_[0]);}  // fstenv
                else
                    {instr->mnemonic = (char*) &(intel_mnemonic_D9_6_[1]);}  // fnstenv

                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                rmp->used = true;
                
                n_bytes_used += 1;

                break;
               }
           
             case 7:      
             {
              if (wait_flag == true)
                  {instr->mnemonic = (char*) &(intel_mnemonic_D9_7_[0]);}  // fstcw
              else
                  {instr->mnemonic = (char*) &(intel_mnemonic_D9_7_[1]);}  // fnstcw

              switch (bitn_a)                                               // part 1 
              {
              case DISASSEMBLY_BITNESS_32: 
                  { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
              case DISASSEMBLY_BITNESS_16: 
                  { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
              }                                               

              rmp->used = true;

              n_bytes_used += 1;

              break;
             }

             default:                                                             
               {
                cout << "unknown opcode after byte " << init_byte << " = " << hex << (opcode & 0xFF)
                     << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";    
                n_bytes_used = 0;
               }
             }
         }      
       }

       break;

       ////////////////////////////////////  D9 /////////////////////////////////////////////////
      }
    
    case 0xDA:      
      {
       ////////////////////////////////////  DA /////////////////////////////////////////////////
          
       switch (opcode & 0xFF) 
       {                                               
       
       case 0xE9:      // fucompp
          {
           instr->mnemonic = (char*) &(intel_mnemonic_DA[opcode & 0xFF]);        
           n_bytes_used = 2;

           break;
          }

       default:
         { 
             reg = (opcode & 0x38) >> 3;                     // 0x38 = 111000b
             switch (reg) 
             {                                               
             case 0:                        // fiadd
             case 1:                        // fimul
             case 2:                        // ficom
             case 3:                        // ficomp
             case 4:                        // fisub
             case 5:                        // fisubr
             case 6:                        // fidiv
             case 7:                        // fidivr
               {
                instr->mnemonic = (char*) &(intel_mnemonic_DA_[reg]);  // mnemonic
          
                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                rmp->used = true;

                n_bytes_used += 1;

                break;
               }

             default:                                                             
               {
                cout << "unknown opcode after byte " << init_byte << " = " << hex << (opcode & 0xFF)
                     << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";    
                n_bytes_used = 0;
               }
             }
         }      
       }

       break;

       ////////////////////////////////////  DA /////////////////////////////////////////////////
      }
    
    case 0xDB:      
      {
       ////////////////////////////////////  DB /////////////////////////////////////////////////
          
       switch (opcode & 0xFF) 
       {                                               
       case 0xE0:      
         {
          if (wait_flag == true)
           {instr->mnemonic = (char*) &(intel_mnemonic_DB_E0[0]);}  // feni
          else
           {instr->mnemonic = (char*) &(intel_mnemonic_DB_E0[1]);}  // fneni

          n_bytes_used = 2;

          break;
         }
           
       case 0xE1:      
         {
          if (wait_flag == true)
           {instr->mnemonic = (char*) &(intel_mnemonic_DB_E1[0]);}  // fdisi
          else
           {instr->mnemonic = (char*) &(intel_mnemonic_DB_E1[1]);}  // fndisi

          n_bytes_used = 2;

          break;
         }
           
       case 0xE2:      
         {
          if (wait_flag == true)
           {instr->mnemonic = (char*) &(intel_mnemonic_DB_E2[0]);}  // fclex
          else
           {instr->mnemonic = (char*) &(intel_mnemonic_DB_E2[1]);}  // fnclex

          n_bytes_used = 2;

          break;
         }
           
       case 0xE3:      
         {
          if (wait_flag == true)
           {instr->mnemonic = (char*) &(intel_mnemonic_DB_E3[0]);}  // finit
          else
           {instr->mnemonic = (char*) &(intel_mnemonic_DB_E3[1]);}  // fninit

          n_bytes_used = 2;

          break;
         }
       case 0xE4:                                                   // fsetpm
         {
          instr->mnemonic = (char*) &(intel_mnemonic_DB[opcode & 0xFF]);        
                    
          n_bytes_used = 2;

          break;
         }

       case 0xE8:                                 // fucomi
       case 0xE9:      
       case 0xEA:      
       case 0xEB:      
       case 0xEC:      
       case 0xED:      
       case 0xEE:      
       case 0xEF:      

         {
          rmp->fp_reg = 1;

          i = (opcode & 0xFF); 
          if ((i >= 0xE8) && (i <= 0xEF))
           { rmp2->fp_reg = i - 0xE8 + 1; }

          instr->mnemonic = (char*) &(intel_mnemonic_DB[opcode & 0xFF]);        
          
          rmp->used = true;
          rmp2->used = true;

          n_bytes_used = 2;

          break;
         }


       default:
         { 
              reg = (opcode & 0x38) >> 3;                     // 0x38 = 111000b
              switch (reg) 
              {                                               
              case 0:                        // fild
              case 2:                        // fist
              case 3:                        // fistp
              case 5:                        // fld
              case 7:                        // fstp
               {
                instr->mnemonic = (char*) &(intel_mnemonic_DB_[reg]);  // mnemonic
          
                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                rmp->used = true;

                n_bytes_used += 1;

                if ((reg == 5) || (reg == 7))
                { *bitn_o = DISASSEMBLY_BITNESS_80; }

                break;
               }

              default:                                                             
               {
                cout << "unknown opcode after byte " << init_byte << " = " << hex << (opcode & 0xFF)
                     << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";    
                n_bytes_used = 0;
               }
              }
         }      
       }

       break;

       ////////////////////////////////////  DB /////////////////////////////////////////////////
      }
    
    case 0xDC:      
        {
           ////////////////////////////////////  DC /////////////////////////////////////////////////

           switch (opcode & 0xFF) 
           {                                               
           case 0xC0:      // fadd
           case 0xC1:      
           case 0xC2:      
           case 0xC3:      
           case 0xC4:      
           case 0xC5:      
           case 0xC6:      
           case 0xC7:      

           case 0xC8:      // fmul
           case 0xC9:      
           case 0xCA:      
           case 0xCB:      
           case 0xCC:      
           case 0xCD:      
           case 0xCE:      
           case 0xCF:      
           
           case 0xE0:      // fsubr
           case 0xE1:      
           case 0xE2:      
           case 0xE3:      
           case 0xE4:      
           case 0xE5:      
           case 0xE6:      
           case 0xE7:      
           
           case 0xE8:      // fsub
           case 0xE9:      
           case 0xEA:      
           case 0xEB:      
           case 0xEC:      
           case 0xED:      
           case 0xEE:      
           case 0xEF:      

           case 0xF0:      // fdivr
           case 0xF1:      
           case 0xF2:      
           case 0xF3:      
           case 0xF4:      
           case 0xF5:      
           case 0xF6:      
           case 0xF7:      
           
           case 0xF8:      // fdiv
           case 0xF9:      
           case 0xFA:      
           case 0xFB:      
           case 0xFC:      
           case 0xFD:      
           case 0xFE:      
           case 0xFF:      
             {
              rmp2->fp_reg = 1;

              i = (opcode & 0xFF); 
              if ((i >= 0xC0) && (i <= 0xC7))
                { rmp->fp_reg = i - 0xC0 + 1; }
              if ((i >= 0xC8) && (i <= 0xCF))
                { rmp->fp_reg = i - 0xC8 + 1; }
              else if ((i >= 0xE0) && (i <= 0xE7))
                { rmp->fp_reg = i - 0xE0 + 1; }
              else if ((i >= 0xE8) && (i <= 0xEF))
                { rmp->fp_reg = i - 0xE8 + 1; }
              else if ((i >= 0xF0) && (i <= 0xF7))
                { rmp->fp_reg = i - 0xF0 + 1; }
              else if ((i >= 0xF8) && (i <= 0xFF))
                { rmp->fp_reg = i - 0xF8 + 1; }

              instr->mnemonic = (char*) &(intel_mnemonic_DC[opcode & 0xFF]);        

              rmp->used = true;
              rmp2->used = true;

              n_bytes_used = 2;

              break;
             }

           default:
             { 
              reg = (opcode & 0x38) >> 3;                     // 0x38 = 111000b
              switch (reg) 
              {                                               
              case 0:                        // fadd
              case 1:                        // fmul
              case 2:                        // fcom
              case 3:                        // fcomp
              case 4:                        // fsub
              case 5:                        // fsubr
              case 6:                        // fdiv
              case 7:                        // fdivr
               {
                instr->mnemonic = (char*) &(intel_mnemonic_DC_[reg]);  // mnemonic
          
                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                rmp->used = true;
                rmp2->used = true;

                *rmp2 = *rmp;
                Initialize(rmp);
                rmp->fp_reg = 1;
                rmp->used = true;
                *bitn_o = DISASSEMBLY_BITNESS_64;

                n_bytes_used += 1;

                break;
               }

              default:                                                             
               {
                cout << "unknown opcode after byte " << init_byte << " = " << hex << (opcode & 0xFF)
                     << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";    
                n_bytes_used = 0;
               }
              }
             }      
           }

           break;

           ////////////////////////////////////  DC /////////////////////////////////////////////////
          }
    
    case 0xDD:      
        {
           ////////////////////////////////////  DD /////////////////////////////////////////////////

           switch (opcode & 0xFF) 
           {                                               
           case 0xC0:      // ffree
           case 0xC1:      
           case 0xC2:      
           case 0xC3:      
           case 0xC4:      
           case 0xC5:      
           case 0xC6:      
           case 0xC7:      
           
           case 0xD0:      // fst
           case 0xD1:      
           case 0xD2:      
           case 0xD3:      
           case 0xD4:      
           case 0xD5:      
           case 0xD6:      
           case 0xD7:      
           
           case 0xD8:      // fstp
           case 0xD9:      
           case 0xDA:      
           case 0xDB:      
           case 0xDC:      
           case 0xDD:      
           case 0xDE:      
           case 0xDF:      
           
           case 0xE0:      // fucom
           case 0xE1:      
           case 0xE2:      
           case 0xE3:      
           case 0xE4:      
           case 0xE5:      
           case 0xE6:      
           case 0xE7:      
           
           case 0xE8:      // fucomp
           case 0xE9:      
           case 0xEA:      
           case 0xEB:      
           case 0xEC:      
           case 0xED:      
           case 0xEE:      
           case 0xEF:               
           {
              i = (opcode & 0xFF); 
              if ((i >= 0xC0) && (i <= 0xC7))
                { rmp->fp_reg = i - 0xC0 + 1; }
              else if ((i >= 0xD0) && (i <= 0xD7))
                { rmp->fp_reg = i - 0xD0 + 1; }
              else if ((i >= 0xD8) && (i <= 0xDF))
                { rmp->fp_reg = i - 0xD8 + 1; }
              else if ((i >= 0xE0) && (i <= 0xE7))
                { rmp->fp_reg = i - 0xE0 + 1; }
              else if ((i >= 0xE8) && (i <= 0xEF))
                { rmp->fp_reg = i - 0xDE + 1; }

              instr->mnemonic = (char*) &(intel_mnemonic_DD[opcode & 0xFF]);        

              rmp->used = true;

              n_bytes_used = 2;

              break;
             }

           default:
             { 
              reg = (opcode & 0x38) >> 3;                     // 0x38 = 111000b
              switch (reg) 
              {                                                 
              case 0:                        // fld
              case 2:                        // fst
              case 3:                        // fstp
              case 4:                        // frstor

               {
                instr->mnemonic = (char*) &(intel_mnemonic_DD_[reg]);  // mnemonic
          
                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                rmp->used = true;
                
                n_bytes_used += 1;

                if (reg != 4)
                 { *bitn_o = DISASSEMBLY_BITNESS_64;}

                break;
               }

              case 6:      
               {
                if (wait_flag == true)
                    {instr->mnemonic = (char*) &(intel_mnemonic_DD_6_[0]);}  // fsave
                else
                    {instr->mnemonic = (char*) &(intel_mnemonic_DD_6_[1]);}  // fnsave

                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                rmp->used = true;
                
                n_bytes_used += 1;

                break;
               }
           
              case 7:      
               {
                if (wait_flag == true)
                    {instr->mnemonic = (char*) &(intel_mnemonic_DD_7_[0]);}  // fstsw
                else
                    {instr->mnemonic = (char*) &(intel_mnemonic_DD_7_[1]);}  // fnstsw

                switch (bitn_a)                                               // part 1 
                {
                case DISASSEMBLY_BITNESS_32: 
                    { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                case DISASSEMBLY_BITNESS_16: 
                    { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                }                                               

                rmp->used = true;
                
                n_bytes_used += 1;

                break;
               }                                                                     
           
              default:                                                             
               {
                cout << "unknown opcode after byte " << init_byte << " = " << hex << (opcode & 0xFF)
                     << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";    
                n_bytes_used = 0;
               }
              }
             }      
           }

           break;

           ////////////////////////////////////  DD /////////////////////////////////////////////////
        }
    
    case 0xDE:      
        {
           ////////////////////////////////////  DE /////////////////////////////////////////////////

           switch (opcode & 0xFF) 
           {                                               
           case 0xC0:      // faddp
           case 0xC1:      
           case 0xC2:      
           case 0xC3:      
           case 0xC4:      
           case 0xC5:      
           case 0xC6:      
           case 0xC7:      
             
           case 0xC8:      // fmulp
           case 0xC9:      
           case 0xCA:      
           case 0xCB:      
           case 0xCC:      
           case 0xCD:      
           case 0xCE:      
           case 0xCF:      
           
           case 0xE0:      // fsubrp
           case 0xE1:      
           case 0xE2:      
           case 0xE3:      
           case 0xE4:      
           case 0xE5:      
           case 0xE6:      
           case 0xE7:      

           case 0xE8:      // fsubp
           case 0xE9:      
           case 0xEA:      
           case 0xEB:      
           case 0xEC:      
           case 0xED:      
           case 0xEE:      
           case 0xEF:      

           case 0xF0:      // fdivrp
           case 0xF1:      
           case 0xF2:      
           case 0xF3:      
           case 0xF4:      
           case 0xF5:      
           case 0xF6:      
           case 0xF7:      

           case 0xF8:      // fdivp
           case 0xF9:      
           case 0xFA:      
           case 0xFB:      
           case 0xFC:      
           case 0xFD:      
           case 0xFE:      
           case 0xFF:      
           {
              rmp2->fp_reg = 1;
              
              i = (opcode & 0xFF); 
              if ((i >= 0xC0) && (i <= 0xC7))
                { rmp->fp_reg = i - 0xC0 + 1; }
              if ((i >= 0xC8) && (i <= 0xCF))
                { rmp->fp_reg = i - 0xC8 + 1; }
              else if ((i >= 0xE0) && (i <= 0xE7))
                { rmp->fp_reg = i - 0xE0 + 1; }
              else if ((i >= 0xE8) && (i <= 0xEF))
                { rmp->fp_reg = i - 0xE8 + 1; }
              else if ((i >= 0xF0) && (i <= 0xF7))
                { rmp->fp_reg = i - 0xF0 + 1; }
              else if ((i >= 0xF8) && (i <= 0xFF))
                { rmp->fp_reg = i - 0xF8 + 1; }

              instr->mnemonic = (char*) &(intel_mnemonic_DE[opcode & 0xFF]);        

              rmp->used = true;
              rmp2->used = true;

              n_bytes_used = 2;

              break;
             }

           case 0xD9:      
             {
              instr->mnemonic = (char*) &(intel_mnemonic_DE[opcode & 0xFF]);    // fcompp      

              n_bytes_used = 2;

              break;
             }

           default:
             { 
                 reg = (opcode & 0x38) >> 3;                     // 0x38 = 111000b
                 switch (reg) 
                 {                                               
                 case 0:                        // fiadd
                 case 1:                        // fimul
                 case 2:                        // ficom
                 case 3:                        // ficomp
                 case 4:                        // fisub
                 case 5:                        // fisubr
                 case 6:                        // fidiv
                 case 7:                        // fidivr
                   {
                    instr->mnemonic = (char*) &(intel_mnemonic_DE_[reg]);  // mnemonic

                    switch (bitn_a)                                               // part 1 
                    {
                    case DISASSEMBLY_BITNESS_32: 
                        { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                    case DISASSEMBLY_BITNESS_16: 
                        { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
                    }                                               

                    rmp->used = true;
                    *bitn_o = DISASSEMBLY_BITNESS_16;

                    n_bytes_used += 1;

                    break;
                   }

                 default:                                                             
                   {
                    cout << "unknown opcode after byte " << init_byte << " = " << hex << (opcode & 0xFF)
                         << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";    
                    n_bytes_used = 0;
                   }
                 }
             }      
           }

           break;

           ////////////////////////////////////  DE /////////////////////////////////////////////////
        }
    
    case 0xDF:      
        {
           ////////////////////////////////////  DF /////////////////////////////////////////////////

           switch (opcode & 0xFF) 
           {                                               
               
           case 0xC0:      // ffreep
           case 0xC1:      
           case 0xC2:      
           case 0xC3:      
           case 0xC4:      
           case 0xC5:      
           case 0xC6:      
           case 0xC7:      
           
             {
                  i = (opcode & 0xFF); 
                  if ((i >= 0xC0) && (i <= 0xC7))
                    { rmp->fp_reg = i - 0xC0 + 1; }

                  instr->mnemonic = (char*) &(intel_mnemonic_DF[opcode & 0xFF]);        

                  rmp->used = true;

                  n_bytes_used = 2;

                  break;
             }
           
           case 0xE0:      
               {
                if (wait_flag == true)
                    {instr->mnemonic = (char*) &(intel_mnemonic_DF_E0_[0]);}  // fstsw ax
                else
                    {instr->mnemonic = (char*) &(intel_mnemonic_DF_E0_[1]);}  // fnstsw ax

                rmp->reg16 = 1;                                 

                rmp->used = true;
                
                n_bytes_used = 2;

                break;
               }                                                                     

           case 0xE8:                                 // fucomip
           case 0xE9:      
           case 0xEA:      
           case 0xEB:      
           case 0xEC:      
           case 0xED:      
           case 0xEE:      
           case 0xEF:      

              {
               rmp->fp_reg = 1;

               i = (opcode & 0xFF); 
               if ((i >= 0xE8) && (i <= 0xEF))
               { rmp2->fp_reg = i - 0xE8 + 1; }

               instr->mnemonic = (char*) &(intel_mnemonic_DB[opcode & 0xFF]);        

               rmp->used = true;
               rmp2->used = true;

               n_bytes_used = 2;

               break;
              }
           
           default:
            { 
             reg = (opcode & 0x38) >> 3;                     // 0x38 = 111000b
                
             instr->mnemonic = (char*) &(intel_mnemonic_DF_[reg]);        
           
             switch (bitn_a)                                               // part 1 
             {
                 case DISASSEMBLY_BITNESS_32: 
                              { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
                 case DISASSEMBLY_BITNESS_16: 
                              { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
             }                                               
             
             switch (reg) 
             {
             case 0:             // fild
             case 2:             // fist
             case 3:             // fistp
               {
                rmp->used = true;
                *bitn_o = DISASSEMBLY_BITNESS_16;

                n_bytes_used += 1;

                break;
               }
             case 4:             // fbld
             case 6:             // fbstp
               {
                rmp->used = true;
                *bitn_o = DISASSEMBLY_BITNESS_80;

                n_bytes_used += 1;

                break;
               }
             case 5:             // fild
             case 7:             // fistp
               {
                rmp->used = true;
                *bitn_o = DISASSEMBLY_BITNESS_64;

                n_bytes_used += 1;

                break;
               }

             default:                                                             
               {
                cout << "unknown opcode after byte " << init_byte << " = " << hex << (opcode & 0xFF)
                               << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";    
                n_bytes_used = 0;
               }
            }
           }

           break;
          }

          break;
          ////////////////////////////////////  DF /////////////////////////////////////////////////
       }

    default:
         { 
           cout << "unknown init_byte for coprocessor instruction " << hex << init_byte <<
                   " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
           n_bytes_used = 0;
         }
    }       


    return n_bytes_used;
}


int
dis::Disassembly_Intel::Convert_Opcodes_0F(list<Disassembly_Node>::iterator it, 
                                           int *ref, int *target, int position, int *n_bytes_pushed,
                                           int bitn_a, int *bitn_o, int *type_call,
                                           Instruction *instr)
{
        char   opcode,                      // next opcode to disassemble
               param[DISASSEMBLY_MAX_INSTRUCTION_LENGTH]; // extensions to the opcode

        int    reg,                         // register field of opcode
               result,                      // return code
               n_bytes_used,                // number of bytes in this instruction
               i                            // indexes
               ;

        RegMemPart *rmp, *rmp2, *rmp3;      // parts 1, 2 and 3 of instr
        Displacement *d;
    
       ////////////////////////////////////////////////////////////////////////////////////////
       /// returns number of bytes used by the instruction                                   //
       ////////////////////////////////////////////////////////////////////////////////////////

       n_bytes_used = 0;

       for (i=0; i < DISASSEMBLY_MAX_INSTRUCTION_LENGTH; i++) { param[i] = '\000';}
              
       rmp  = &(instr->part1);
       rmp2 = &(instr->part2);

       result = Get_Byte_From_Disassembly_Node(it, position, &opcode);  
       if (result == RET_ERR_OPCODE)
       { return 0; }                                  

       switch (opcode & 0xFF) 
       {                                               
       case 0x00:      
          {
          if (Get_Byte_From_Disassembly_Node(it, position + 1, &param[0]) == RET_ERR_OPCODE)
          { return RET_ERR_OPCODE;}
              
          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                       // mnemonic = 'sldt r/m 16'
          case 1:                                                       // mnemonic = 'str  r/m 16'
          case 2:                                                       // mnemonic = 'lldt r/m 16'
          case 3:                                                       // mnemonic = 'ltr  r/m 16'
          case 4:                                                       // mnemonic = 'verr r/m 16'
          case 5:                                                       // mnemonic = 'verw r/m 16'

            *bitn_o = DISASSEMBLY_BITNESS_16;

            instr->mnemonic = (char*) &(intel_mnemonic_0F_00[reg]);        

            switch (bitn_a)                                               // part 1 
            {
            case DISASSEMBLY_BITNESS_32: 
                { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
            case DISASSEMBLY_BITNESS_16: 
                { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
            }                                               

            rmp->used = true;
            rmp2->used = false;
            n_bytes_used += 1;

            break;

          default:
            { 
              cout << "unknown reg/opcode digit for 0x0F opcode " << hex << (opcode & 0xFF) << " : " << reg <<
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              n_bytes_used = 0;
            }
          }       

          break;
          }   

       case 0x01:      
          {
          if (Get_Byte_From_Disassembly_Node(it, position + 1, &param[0]) == RET_ERR_OPCODE)
          { return RET_ERR_OPCODE;}
              
          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 6:                                                       // 'lmsw r/m 16'
            *bitn_o = DISASSEMBLY_BITNESS_16;

          case 0:                                                       // 'sgdt m 16/32'
          case 1:                                                       // 'sidt m 16/32'
          case 2:                                                       // 'lgdt m 16/32'
          case 3:                                                       // 'lidt m 16/32'
          case 4:                                                       // 'smsw m 16/32'
          case 7:                                                       // mnemonic = 'invlpg r/m 32'
            {
             instr->mnemonic = (char*) &(intel_mnemonic_0F_01[reg]);        
             switch (bitn_a)                                               // part 1 
             {
             case DISASSEMBLY_BITNESS_32: 
                { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
             case DISASSEMBLY_BITNESS_16: 
                { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
             }                                                     
          

             rmp->used = true;
             rmp2->used = false;
             n_bytes_used += 1;

             break;
            }

          default:
            { 
              cout << "unknown reg/opcode digit for 0x0F opcode " << hex << (opcode & 0xFF) << " : " << reg <<
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              n_bytes_used = 0;
            }
          }       

          break;
          }   

       case 0x02:      // lar  r16/32, r/m 16/32
       case 0x03:      // lar  r16/32, r/m 16/32
       case 0xA3:      // bt   r16/32, r/m 16/32
       case 0xAB:      // bts  r16/32, r/m 16/32
       case 0xAF:      // imul r16/32, r/m 16/32
       case 0xB2:      // lss  r16/32, r/m 16/32
       case 0xB3:      // btr  r16   , r/m 16/32
       case 0xB4:      // lfs  r16/32, r/m 16/32
       case 0xB5:      // lgs  r16/32, r/m 16/32
       case 0xBB:      // btc  r16/32, r/m 16/32
       case 0xBC:      // bsf  r16/32, r/m 16/32
       case 0xBD:      // bsr  r16/32, r/m 16/32
          {
            instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

            switch (bitn_a)                                    // part 1 + 2
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 1, ref, instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 1, ref, instr); break; }
            } 
                         
            rmp->used = true;
            rmp2->used = true;
            n_bytes_used += 1;

            break;
          }   

       case 0x06:                                    // clts
       case 0x08:                                    // invd
       case 0x09:                                    // wbinvd
       case 0x30:                                    // wrmsr
       case 0xA2:                                    // cpuid
       case 0xAA:                                    // rsm
          {
           instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic
           rmp->used = false;
           rmp2->used = false;
           n_bytes_used = 1;
           break;
          }                                            
       
       case 0x0E:                                    // femms
          {
           instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic
           instr->coprocessor = true;
           rmp->used = false;
           rmp2->used = false;
           n_bytes_used = 1;
           break;
          }                                            
       
       case 0x20:                   // mov r32, CRx
       case 0x21:                   // mov r32, DRx
       case 0x24:                   // mov r32, TRx
          {
          instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

          if (Get_Byte_From_Disassembly_Node(it, position + 1, &param[0]) == RET_ERR_OPCODE)
          { return RET_ERR_OPCODE;}

          *bitn_o =  DISASSEMBLY_BITNESS_32; 
              
          switch (bitn_a)                                               // part 1 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
          }                                                     
          
          d = rmp2->displ;
          if (d == 0)
          { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); rmp2->displ = d;}  
          Initialize(d);

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (opcode & 0xFF)
          {
           case 0x20 : {d->contr_reg = reg + 1; break;}
           case 0x21 : {d->debug_reg = reg + 1; break;}
           case 0x24 : {d->test_reg = reg + 1; break;}
          }

          rmp->used = true;
          rmp2->used = true;
          n_bytes_used += 1;

          break;
          }
       
       case 0x22:                   // mov CRx, r32
       case 0x23:                   // mov DRx, r32
       case 0x26:                   // mov TRx, r32
          {
          instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

          if (Get_Byte_From_Disassembly_Node(it, position + 1, &param[0]) == RET_ERR_OPCODE)
          { return RET_ERR_OPCODE;}

          *bitn_o =  DISASSEMBLY_BITNESS_32; 
              
          switch (bitn_a)                                               // part 2 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
          }                                                     
          rmp2->reg32 = rmp->reg32;

          
          Initialize(rmp);                                   // part 1
          d = rmp->displ;
          if (d == 0)
          { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); rmp->displ = d;}  
          Initialize(d);

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (opcode & 0xFF)
          {
           case 0x22 : {d->contr_reg = reg + 1; break;}
           case 0x23 : {d->debug_reg = reg + 1; break;}
           case 0x26 : {d->debug_reg = reg + 1; break;}
          }

          rmp->used = true;
          rmp2->used = true;
          n_bytes_used += 1;

          break;
          }   

       case 0x32:                                      // rdmsr
          {
            instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic
            n_bytes_used = 1;

            rmp->used = false;
            rmp2->used = false;
            break;
          }

       case 0x80:     // jo  rel 16/32
       case 0x81:     // jno rel 16/32
       case 0x82:     // jb  rel 16/32
       case 0x83:     // jae rel 16/32
       case 0x84:     // jz  rel 16/32
       case 0x85:     // jne rel 16/32
       case 0x86:     // jbe rel 16/32
       case 0x87:     // ja  rel 16/32
       case 0x88:     // js  rel 16/32
       case 0x89:     // jns rel 16/32
       case 0x8A:     // jp  rel 16/32
       case 0x8B:     // jnp rel 16/32
       case 0x8C:     // jl  rel 16/32
       case 0x8D:     // jge rel 16/32
       case 0x8E:     // jle rel 16/32
       case 0x8F:     // jg  rel 16/32
        {
          for (i=0; i < *bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
          {
            if (Get_Byte_From_Disassembly_Node(it, position + i + 1, &param[i]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          }

          instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic
          
          n_bytes_used = *bitn_o + 1;

          *target =  (*(int*) &param) + it->memory_offset + n_bytes_used + 1;               
          instr->part1.imm = *target;                   // part1
          instr->part1.abs = true;
                                                                       
          rmp->used = true;
          rmp2->used = false;                   
          *type_call = INSTRUCTION_CALL_JUMP_COND;    

          break;
        }        

       case 0x90:      // seto  r/m 8
       case 0x91:      // setno r/m 8
       case 0x92:      // setb  r/m 8
       case 0x93:      // setae r/m 8
       case 0x94:      // sete  r/m 8
       case 0x95:      // setnz r/m 8
       case 0x96:      // setbe r/m 8
       case 0x97:      // seta  r/m 8
       case 0x98:      // sets  r/m 8
       case 0x99:      // setns r/m 8
       case 0x9A:      // setp  r/m 8
       case 0x9B:      // setpo r/m 8
       case 0x9C:      // setl  r/m 8
       case 0x9D:      // setnl r/m 8
       case 0x9E:      // setng r/m 8
       case 0x9F:      // setg  r/m 8
          {
            instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

            *bitn_o = DISASSEMBLY_BITNESS_08; 

            switch (bitn_a)                                    // part 1 + 2
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
            } 
                         
            rmp->used = true;
            rmp2->used = false;
            n_bytes_used += 1;

            break;
          }   

       case 0xA0:                                             // push fs
       case 0xA1:                                             // pop fs
       case 0xA8:                                             // push gs
       case 0xA9:                                             // pop gs
        {
          instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

          d = instr->part1.displ;                                     // part 1
          if (d == 0)
           { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); instr->part1.displ = d;}  
          Initialize(d);

          switch (opcode & 0xFF)
          {
           case 0xA0: d->seg_reg = 5; *n_bytes_pushed =  *bitn_o; break;
           case 0xA1: d->seg_reg = 5; *n_bytes_pushed = -*bitn_o; break;
           case 0xA8: d->seg_reg = 6; *n_bytes_pushed =  *bitn_o; break;
           case 0xA9: d->seg_reg = 6; *n_bytes_pushed = -*bitn_o; break;
          }
          instr->part1.abs = true;

          rmp->used = true;
          rmp2->used = false;
          
          n_bytes_used = 1;
          break;
        }

       case 0xA4:                                             // shld r/m 16/32, r16/32, imm8
       case 0xAC:                                             // shrd r/m 16/32, r16/32, imm8
        {
         instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

         rmp3 = (RegMemPart*) mp.Use_Pool(sizeof(RegMemPart));
         instr->part3 = rmp3;
         Initialize(rmp3);

         switch (bitn_a)                                    // part 1 + 2
         {
          case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 2, ref, instr); break; }
          case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 2, ref, instr); break; }
         }

         if (Get_Byte_From_Disassembly_Node(it, position + 1 + n_bytes_used, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}
         *ref = *(int*) &param;                             // part3
         rmp3->imm = *ref;           
                         
         rmp->used  = true;
         rmp2->used = true;
         rmp3->used = true;                                     

         n_bytes_used += 2;
         break;
        }

       case 0xA5:                                             // shld r/m 16/32, r16/32, cl
       case 0xAD:                                             // shrd r/m 16/32, r16/32, cl
        {
         instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

         rmp3 = (RegMemPart*) mp.Use_Pool(sizeof(RegMemPart));
         instr->part3 = rmp3;
         Initialize(rmp3);

         switch (bitn_a)                                    // part 1 + 2
         {
          case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 2, ref, instr); break; }
          case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 2, ref, instr); break; }
         }

         rmp3->reg08 = 2;                                   // part3
                         
         rmp->used  = true;
         rmp2->used = true;
         rmp3->used = true;                                     

         n_bytes_used += 1;
         break;
        }

       case 0xAE:        
        {
          if (Get_Byte_From_Disassembly_Node(it, position + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}
          
          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                       // fxsave
          case 1:                                                       // fxrstor

            instr->mnemonic = (char*) &(intel_mnemonic_0F_AE[reg]);        

            switch (bitn_a)                                    // part 1 + 2
            {
             case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 2, ref, instr); break; }
             case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 2, ref, instr); break; }
            }      

            instr->coprocessor = true;
            rmp->used = true;
            n_bytes_used += 1;

            break;

          default:
            { 
              cout << "unknown reg/opcode digit for 0x0F opcode " << hex << (opcode & 0xFF) << " : " << reg <<
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              n_bytes_used = 0;
            }
          }       

          break;           
        }                                            
       
       case 0xB0:                                             // cmpxchg  r/m8, r8
       case 0xB1:                                             // cmpxchg  r/m16/32, r16/32
       case 0xC0:                                             // xadd     r/m8, r8
       case 0xC1:                                             // xadd     r/m16/32, r16/32
          {
           instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

           if (   ((opcode & 0xFF) == 0xB0)
               || ((opcode & 0xFF) == 0xC0))
           { *bitn_o = DISASSEMBLY_BITNESS_08; }

           switch (bitn_a)                                    // part 1 + 2
           {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 2, ref, instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 2, ref, instr); break; }
           }      

           rmp->used = true;
           rmp2->used = true;
           n_bytes_used += 1;   
           break;
          }                                                           

       case 0xB6:                   // movzx r16/32,r/m 8
       case 0xB7:                   // movzx r32,r/m 16
       case 0xBE:                   // movsx r16/32,r/m 8
       case 0xBF:                   // movsx r32,r/m 16
          {
          instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

          switch (bitn_a)                                               // part 1 + 2
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 1, ref, instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 1, ref, instr); break; }
          }                                                     
          
          if ((opcode & 0xFF == 0xB7) || (opcode & 0xFF == 0xBF))
          { *bitn_o = DISASSEMBLY_BITNESS_16;}
          else
          { *bitn_o = DISASSEMBLY_BITNESS_08;}

          rmp->used = true;
          rmp2->used = true;
          n_bytes_used += 1;

          break;
          }   

       case 0xBA:      
          {
          if (Get_Byte_From_Disassembly_Node(it, position + 1, &param[0]) == RET_ERR_OPCODE)
          { return RET_ERR_OPCODE;}
              
          *bitn_o = DISASSEMBLY_BITNESS_08;

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 4:                                                       // mnemonic = 'bt r/m16/32, imm8'
          case 5:                                                       // mnemonic = 'bts r/m16/32, imm8'
          case 6:                                                       // mnemonic = 'btr r/m16/32, imm8'
          case 7:                                                       // mnemonic = 'btc r/m16/32, imm8'
            instr->mnemonic = (char*) &(intel_mnemonic_0F_BA[reg]);        

            switch (bitn_a)                                               // part 1 
            {
             case DISASSEMBLY_BITNESS_32: 
                 { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
             case DISASSEMBLY_BITNESS_16: 
                 { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
            }                                                     
          
            if (Get_Byte_From_Disassembly_Node(it, position + 2, &param[0]) == RET_ERR_OPCODE)
             { return RET_ERR_OPCODE;}

            instr->part2.imm = *(int*) &param;                    // part2
            instr->part2.abs = true;

            rmp->used = true;
            rmp2->used = true;
            n_bytes_used += 2;

            break;

          default:
            { 
              cout << "unknown reg/opcode digit for 0x0F opcode " << hex << (opcode & 0xFF) << " : " << reg <<
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              n_bytes_used = 0;
            }
          }       

          break;
          }   

       case 0xC7:                                             // cmpxchg8b r/m64
          {
           instr->mnemonic = (char *) mp.Use_Pool(10);         // mnemonic

           strcpy(instr->mnemonic, "CMPXCHG8B"); 

           switch (bitn_a)                                    // part 1
           {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
           }      

           rmp->used = true;
           rmp2->used = false;
           n_bytes_used += 1;   
           break;
          }                                                           

       case 0xC8:      // bswap r16/32
          {
            instr->mnemonic = (char*) &(intel_mnemonic_0F[(opcode & 0xFF) ]);  // mnemonic

            switch (bitn_a)                                    // part 1 + 2
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(*bitn_o, it, position + 1, 0, ref, instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(*bitn_o, it, position + 1, 0, ref, instr); break; }
            } 
                         
            rmp->used = true;
            rmp2->used = false;
            n_bytes_used += 1;

            break;
          }   

       default:
          {
           cout << "unknown opcode after byte 0x0F = " << hex << (opcode & 0xFF) << "-" << (param[0] & 0xFF) << 
                   " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
           n_bytes_used = 0;
          }
       }

       return n_bytes_used;
}


int
dis::Disassembly_Intel::Convert_Opcodes_From_Offset(list<Disassembly_Node>::iterator it, int n_opcodes)
{
    bool   finished,                    // done with instructions we can be sure of?
           wait_flag,                   // does this instruction follow a WAIT?
           disassembled;                // has this instruction been disassembled?

    char   opcode,                      // next opcode to disassemble
           param[DISASSEMBLY_MAX_INSTRUCTION_LENGTH]; // extensions to the opcode

    int    ref, ref2,                   // reference from this instruction
           target,                      // reference to code offset
           reg,                         // register field of opcode
           //rm,                          // r/m      field of opcode
           result,                      // return code
           bitn_a, bitn_o,              // bitness for address and operands
          
           n_bytes_used,                // number of bytes in this instruction
           n_bytes_pushed,              // number of bytes pushed on the stack in this instruction
           type_call,                   // whether or not this instruction calls a routine

           prefix,                      // prefixes to an instruction
           continuation,                // number of bytes to be prefixed to current opcode
           segment_override,            // overriding the default ds

           n_converted,                 // number of converted opcodes

           i, j;                        // indexes

    char            *t_char;            // temp value
    Displacement    *d;                 // temp value

    Instruction     instr;

    Call            *c;

    ///////////////////////////////////////////////////////////////////////////////////////////////

    bitn_a = address_bitness;
    bitn_o = operand_bitness;

    finished = wait_flag = false;
    disassembled = true;
    prefix = continuation = segment_override = n_converted = 0;
            
    while (!finished)
    {

      i = j = ref = ref2 = target = n_bytes_used = n_bytes_pushed = 0;
      type_call = INSTRUCTION_CALL_NONE;
      for (i=0; i < DISASSEMBLY_MAX_INSTRUCTION_LENGTH; i++) { param[i] = '\000';}
      disassembled = true;

      Initialize (&instr);

      result = Get_Byte_From_Disassembly_Node(it, continuation, &opcode);  
      if (result == RET_ERR_OPCODE)
      {
        return RET_ERR_OPCODE; 
      }                                  
            
      switch (opcode & 0xFF) 
      {
      case 0x00:     // align ... or add r/m8,r8
        {
         if (((it->memory_offset % 4) != 0) && (continuation == 0))
         {
           if (Get_Byte_From_Disassembly_Node(it, 1, &param[0]) == RET_ERR_OPCODE)
                { return RET_ERR_OPCODE;}

           if (*(int*) &param == 0)     // just adding 0 to something doesn't make sense, so we're probably 
           {                            // dealing with padding ...
            while ( (*(int*) &param == 0)  &&
                    (n_bytes_used < mbpi - 1))
            {
              if (Get_Byte_From_Disassembly_Node(it, n_bytes_used + 2, &param[0]) == RET_ERR_OPCODE)
                     { return RET_ERR_OPCODE;}
              n_bytes_used++;
            }
           }
         }                   

         if (n_bytes_used == 0)      // we didn't find two 0x00s in a row 
         {                           // so we're going for: add r/m8,r8
          bitn_o = DISASSEMBLY_BITNESS_08;

          switch (bitn_a)                                               // part 1 + 2
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 2, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 2, &ref, &instr); break; }
          }                                                     

          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic
          instr.part1.used = true;
          instr.part2.used = true;
         }
         else                        // we found probable padding
         {
          instr.mnemonic = (char *) mp.Use_Pool(8);  // mnemonic
          strcpy(instr.mnemonic , "ALIGN 4"); 

          instr.part1.used = false;
          instr.part2.used = false;
         }
         
         n_bytes_used++;
         break;
        }                

      case 0x01:                                      // add  r/m16/32, r16/32
      case 0x09:                                      // or   r/m16/32, r16/32
      case 0x11:                                      // adc  r/m16/32, r16/32
      case 0x19:                                      // sbb  r/m16/32, r16/32
      case 0x21:                                      // and  r/m16/32, r16/32
      case 0x29:                                      // sub  r/m16/32, r16/32
      case 0x31:                                      // xor  r/m16/32, r16/32
      case 0x39:                                      // cmp  r/m16/32, r16/32
      case 0x85:                                      // test r/m16/32, r16/32
      case 0x89:                                      // mov  r/m16/32, r16/32
          {
           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

           switch (bitn_a)                                    // part 1 + 2
           {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 2, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 2, &ref, &instr); break; }
           }      

           instr.part1.used = true;
           instr.part2.used = true;
           n_bytes_used += 1;   
           break;
          }                                                           

      case 0x02:                                      // add r8, r/m8
      case 0x0A:                                      // or  r8, r/m8
      case 0x12:                                      // adc r8, r/m8
      case 0x1A:                                      // sbb r8, r/m8
      case 0x22:                                      // and r8, r/m8
      case 0x2A:                                      // sub r8, r/m8
      case 0x32:                                      // xor r8, r/m8
      case 0x3A:                                      // cmp r8, r/m8
      case 0x8A:                                      // mov r8, r/m8
          {
           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

           bitn_o = DISASSEMBLY_BITNESS_08;

           switch (bitn_a)                                    // part 1 + 2
           {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(DISASSEMBLY_BITNESS_08, it, continuation + 1, 1, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(DISASSEMBLY_BITNESS_08, it, continuation + 1, 1, &ref, &instr); break; }
           }      

           instr.part1.used = true;
           instr.part2.used = true;
           n_bytes_used += 1;                                         
           break;
          }                                                           
      
      case 0x03:                                      // add   r16/32, r/m 16/32
      case 0x0B:                                      // or    r16/32, r/m 16/32
      case 0x13:                                      // adc   r16/32, r/m 16/32
      case 0x1B:                                      // sbb   r16/32, r/m 16/32
      case 0x23:                                      // and   r16/32, r/m 16/32
      case 0x2B:                                      // sub   r16/32, r/m 16/32
      case 0x33:                                      // xor   r16/32, r/m 16/32
      case 0x3B:                                      // cmp   r16/32, r/m 16/32
      case 0x62:                                      // bound r16/32, r/m 16/32
      case 0x87:                                      // xchg  r16/32, r/m 16/32
      case 0x8B:                                      // mov   r16/32, r/m 16/32
          {
           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

           switch (bitn_a)                                    // part 1 + 2
           {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 1, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 1, &ref, &instr); break; }
           }      

           instr.part1.used = true;
           instr.part2.used = true;
           n_bytes_used += 1;   
           break;
          }                                                           

      case 0x04:                                    // add  al, imm8
      case 0x0C:                                    // or   al, imm8
      case 0x14:                                    // adc  al, imm8
      case 0x24:                                    // and  al, imm8
      case 0x2C:                                    // sub  al, imm8
      case 0x34:                                    // xor  al, imm8
      case 0x3C:                                    // cmp  al, imm8
      case 0xE4:                                    // in   al, imm8
      case 0xA8:                                    // test al, imm8
        {
              if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
              { return RET_ERR_OPCODE;}

              bitn_o = DISASSEMBLY_BITNESS_08;
              instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic
                
              instr.part1.reg08 = 1;                                        // part1
              instr.part1.abs = true;                                               

              j =  *(int*) &param;               
              instr.part2.imm = j;                                          // part2
              instr.part2.abs = true;

              instr.part1.used = true;
              instr.part2.used = true;
              n_bytes_used = 2;
              break;
        }                

      case 0x05:                                      // add  (e)ax, imm16/32
      case 0x0D:                                      // or   (e)ax, imm16/32
      case 0x15:                                      // adc  (e)ax, imm
      case 0x25:                                      // and  (e)ax, imm 16/32
      case 0x2D:                                      // sub  (e)ax, imm 16/32
      case 0x35:                                      // xor  (e)ax, imm 16/32
      case 0x3D:                                      // cmp  (e)ax, imm 16/32
      case 0xA9:                                      // test (e)ax, imm 16/32
         {
         instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

         for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
            {
            result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
            if (result == RET_ERR_OPCODE)
            {
               return RET_ERR_OPCODE; 
            }
          }
          
          ref =  *(int*) &param;               
          
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

          switch (bitn_o)                                   // part1
          {
          case DISASSEMBLY_BITNESS_32: 
            { instr.part1.reg32 = 1; break; }
          case DISASSEMBLY_BITNESS_16: 
            { instr.part1.reg16 = 1; break; }
          }
          instr.part1.abs = true;

          instr.part2.imm = ref;                         // part2
          instr.part2.abs = true; 

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used = bitn_o + 1;
         break;
        }                  

      case 0x06:                                      // push es
      case 0x07:                                      // pop es
      case 0x0E:                                      // push cs
      case 0x16:                                      // push ss
      case 0x17:                                      // pop ss 
      case 0x1E:                                      // push ds
      case 0x1F:                                      // pop ds

          {
           instr.mnemonic = (char*) &(intel_mnemonic[opcode & 0xFF]); // mnemonic
           
           d = instr.part1.displ;                                     // part 1
           if (d == 0)
           { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); instr.part1.displ = d;}  
           Initialize(d);

           switch (opcode & 0xFF)
           {
           case 0x06: d->seg_reg = 1; n_bytes_pushed  =  bitn_o; break;
           case 0x07: d->seg_reg = 1; n_bytes_pushed  = -bitn_o; break;
           case 0x0E: d->seg_reg = 2; n_bytes_pushed  =  bitn_o; break;
           case 0x16: d->seg_reg = 3; n_bytes_pushed  =  bitn_o; break;
           case 0x17: d->seg_reg = 3; n_bytes_pushed  = -bitn_o; break;
           case 0x1E: d->seg_reg = 4; n_bytes_pushed  =  bitn_o; break;
           case 0x1F: d->seg_reg = 4; n_bytes_pushed  = -bitn_o; break;
           }
           instr.part1.abs = true;

           instr.part1.used = true;
           instr.part2.used = false;
           
           n_bytes_used = 1;

           break;
          } 
      
      case 0x08:                                      // or   r/m8, r8
      case 0x10:                                      // adc  r/m8, r8
      case 0x18:                                      // sbb  r/m8, r8
      case 0x20:                                      // and  r/m 8, r8
      case 0x28:                                      // sub  r/m 8, r8
      case 0x30:                                      // xor  r/m 8, r8
      case 0x38:                                      // cmp  r/m 8, r8
      case 0x84:                                      // test r/m 8, r8
      case 0x86:                                      // xchg r/m 8, r8
      case 0x88:                                      // mov  r/m 8, r8
          {
           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

           bitn_o = DISASSEMBLY_BITNESS_08;

           switch (bitn_a)                                    // part 1 + 2
           {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(DISASSEMBLY_BITNESS_08, it, continuation + 1, 2, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(DISASSEMBLY_BITNESS_08, it, continuation + 1, 2, &ref, &instr); break; }
           }      

           instr.part1.used = true;
           instr.part2.used = true;
           n_bytes_used += 1;                                         
           break;
          }                                                 
          
      case 0x0F:                                      // multibyte instruction
        {
          n_bytes_used = Convert_Opcodes_0F(it, &ref, &target, continuation + 1, &n_bytes_pushed,
                                            bitn_a, &bitn_o, &type_call, &instr);

          if (n_bytes_used > 0)
          { n_bytes_used++; }          
          else
          { finished = true; disassembled = false;}          

          break;
        }

      case 0x1C:                                      // sbb  imm8    
      case 0x6A:                                      // push imm8    
      case 0xCD:                                      // int  imm8
      {
       result = Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]);   
       if (result == RET_ERR_OPCODE)
         { return RET_ERR_OPCODE; }

       bitn_o = DISASSEMBLY_BITNESS_08;

       j = (opcode & 0xFF);
       instr.mnemonic = (char*) &(intel_mnemonic[j]);  // mnemonic

       j =  *(int*) &param;               
       instr.part1.abs = true;
       instr.part1.imm = j;               

       n_bytes_pushed = 1;

       instr.part1.used = true;
       instr.part2.used = false;
       n_bytes_used = 2;
       break;
      } 

      case 0x1D:                                      // sbb  imm 16/32    
      case 0x68:                                      // push imm 16/32    
      {
       for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
       {
         result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
         if (result == RET_ERR_OPCODE)
         { return RET_ERR_OPCODE; }
       }
       ref =  *(int*) &param;               

       j = (opcode & 0xFF);
       instr.mnemonic = (char*) &(intel_mnemonic[j]);  // mnemonic
       switch (j)
       {
       case 0x68:
         { n_bytes_pushed  =  bitn_o; break; }
       }        

       instr.part1.abs = true;
       instr.part1.imm = ref;   

       instr.part1.used = true;
       instr.part2.used = false;
       n_bytes_used = bitn_o + 1;
       break;
      } 

      case 0x26:                                      // ES segment override
          {
              continuation++;
              disassembled = false;
              segment_override = 1;
              break;
          }                                                 

      case 0x27:                                      // daa
      case 0x2F:                                      // das
      case 0x37:                                      // aaa
      case 0x3F:                                      // aas
      case 0x6C:                                      // insb
      case 0x6E:                                      // outsb
      case 0x90:                                      // nop
      case 0x9E:                                      // sahf
      case 0x9F:                                      // lahf
      case 0xCC:                                      // int3
      case 0xCE:                                      // into
      case 0xD7:                                      // xlatb
      case 0xF4:                                      // hlt
      case 0xF5:                                      // cmc
      case 0xF8:                                      // clc
      case 0xF9:                                      // stc
      case 0xFA:                                      // cli
      case 0xFB:                                      // sti
      case 0xFC:                                      // cld
      case 0xFD:                                      // std
          {
              instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic
              n_bytes_used = 1;

              instr.part1.used = false;
              instr.part2.used = false;
              break;
          }

      case 0x2E:                                      // CS segment override
          {
              continuation++;
              disassembled = false;
              segment_override = 2;
              break;
          }                                                                  
                
      case 0x36:                                      // SS segment override
          {
              continuation++;
              disassembled = false;
              segment_override = 3;
              break;
          }                                                                  

      case 0x3E:                                      // DS segment override
          {
              continuation++;
              disassembled = false;
              segment_override = 4;
              break;
          }

      
      case 0x40:                                      // inc EAX
      case 0x41:                                      // inc ECX
      case 0x42:                                      // inc EDX 
      case 0x43:                                      // inc EBX
      case 0x44:                                      // inc ESP 
      case 0x45:                                      // inc EBP 
      case 0x46:                                      // inc ESI
      case 0x47:                                      // inc EDI    

      case 0x48:                                      // dec EAX
      case 0x49:                                      // dec ECX
      case 0x4A:                                      // dec EDX 
      case 0x4B:                                      // dec EBX
      case 0x4C:                                      // dec ESP 
      case 0x4D:                                      // dec EBP 
      case 0x4E:                                      // dec ESI
      case 0x4F:                                      // dec EDI    

      case 0x50:                                      // push EAX
      case 0x51:                                      // push ECX
      case 0x52:                                      // push EDX 
      case 0x53:                                      // push EBX
      case 0x54:                                      // push ESP 
      case 0x55:                                      // push EBP 
      case 0x56:                                      // push ESI
      case 0x57:                                      // push EDI    

      case 0x58:                                      // pop EAX
      case 0x59:                                      // pop ECX
      case 0x5A:                                      // pop EDX 
      case 0x5B:                                      // pop EBX
      case 0x5C:                                      // pop ESP 
      case 0x5D:                                      // pop EBP 
      case 0x5E:                                      // pop ESI
      case 0x5F:                                      // pop EDI    
          {
           j = (opcode & 0xFF);
           instr.mnemonic = (char*) &(intel_mnemonic[j]);  // mnemonic

           if (opcode > 0x57)
           {  j = j - 0x58; n_bytes_pushed  = -bitn_o; }
           else if (opcode > 0x4F)
           {  j = j - 0x50; n_bytes_pushed  = bitn_o; }
           else if (opcode > 0x47)
           {  j = j - 0x48; }
           else if (opcode > 0x3F)
           {  j = j - 0x40; }     

           switch (bitn_o)                            // part1
           {
           case DISASSEMBLY_BITNESS_32: 
             { instr.part1.reg32 = j + 1; break; }
           case DISASSEMBLY_BITNESS_16: 
             { instr.part1.reg16 = j + 1; break; }
           }

           instr.part1.abs = true;

           instr.part1.used = true;
           instr.part2.used = false;
           n_bytes_used = 1;

           break;
          } 
      
      case 0x60:                                      // pusha
          {
            switch (bitn_o)
            {                                                    // mnemonic
            case DISASSEMBLY_BITNESS_16:
                {instr.mnemonic = (char*) &(intel_mnemonic_60[0]); break;}   // pushaw
            case DISASSEMBLY_BITNESS_32:
                {instr.mnemonic = (char*) &(intel_mnemonic_60[1]); break;}   // pushad
            }                                                  
              
            n_bytes_used = 1;
            n_bytes_pushed = 8 * bitn_o;

            instr.part1.used = false;
            instr.part2.used = false;
            break;
           }

      case 0x61:                                      // popa
          {
            switch (bitn_o)
            {                                                    // mnemonic
            case DISASSEMBLY_BITNESS_16:
                {instr.mnemonic = (char*) &(intel_mnemonic_61[0]); break;}   // popaw
            case DISASSEMBLY_BITNESS_32:
                {instr.mnemonic = (char*) &(intel_mnemonic_61[1]); break;}   // popad
            }                                                  
              
            n_bytes_used = 1;
            n_bytes_pushed = -(8 * bitn_o);

            instr.part1.used = false;
            instr.part2.used = false;
            break;
           }

      case 0x63:                                      // arpl  r/m16, r16
          {
           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

           switch (bitn_a)                                    // part 1 + 2
           {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(DISASSEMBLY_BITNESS_16, it, continuation + 1, 2, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(DISASSEMBLY_BITNESS_16, it, continuation + 1, 2, &ref, &instr); break; }
           }      

           n_bytes_used += 1;   

           instr.part1.used = true;
           instr.part2.used = true;
           break;
          }                                                           

      case 0x64:                                      // FS segment override
          {
              continuation++;
              disassembled = false;
              segment_override = 5;
              break;
          }

      case 0x65:                                      // GS segment override
          {
              continuation++;
              disassembled = false;
              segment_override = 6;
              break;
          }

      case 0x66:                                      // operand size prefix
          {
              continuation++;;
              disassembled = false;
              break;
          }
      
      case 0x67:                                      // address size prefix
          {
              continuation++;
              disassembled = false;
              break;
          }

      case 0x69:                                      // imul r16/32,  r/m 16/32, imm16/32
      case 0x6B:                                      // imul r16/32,  r/m 16/32, imm8
          {
           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

           switch (bitn_a)                                    // part 1 + 2
           {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 1, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 1, &ref, &instr); break; }
           }      

           param[0] = param[1] = param[2] = param[3] = '\000';
           switch (opcode)
           {
           case 0x69:
              {
                  for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
                  {
                      result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1 + n_bytes_used, &param[i]);   
                      if (result == RET_ERR_OPCODE)
                      { return RET_ERR_OPCODE; }
                  }
                  
                  n_bytes_used += 1 + bitn_o;   
                  break;
              }
           case 0x6B:
              {
                  result = Get_Byte_From_Disassembly_Node(it, continuation + 1 + n_bytes_used, &param[0]);   
                  if (result == RET_ERR_OPCODE)
                  { return RET_ERR_OPCODE; }
                  n_bytes_used += 2;   
              }
           }        

           j =  *(int*) &param;               
           instr.part3 = (RegMemPart*) mp.Use_Pool(sizeof(RegMemPart));
           Initialize(instr.part3);
           instr.part3->imm = j;                                                                      
           
           instr.part1.used = true;
           instr.part2.used = true;
           instr.part3->used = true;

           break;
          }                                                           

      case 0x6D:                                      
          {
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_6D[0]); break;}   // insw 
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_6D[1]); break;}   // insd
           }                                                            

           instr.part1.used = false;
           instr.part2.used = false;
           n_bytes_used = 1;                                    

           break;
          }

      case 0x6F: 
          {
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_6F[0]); break;}   // outsw 
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_6F[1]); break;}   // outsd
           }                                                            

           instr.part1.used = false;
           instr.part2.used = false;
           n_bytes_used = 1;                                    

           break;
          }

      case 0x70:     // jo  relative mem_offset 8
      case 0x71:     // jno relative mem_offset 8
      case 0x72:     // jb  relative mem_offset 8
      case 0x73:     // jnb relative mem_offset 8
      case 0x74:     // jz  relative mem_offset 8
      case 0x75:     // jne relative mem_offset 8
      case 0x76:     // jbe relative mem_offset 8
      case 0x77:     // ja  relative mem_offset 8
      case 0x78:     // js  relative mem_offset 8
      case 0x79:     // jns relative mem_offset 8
      case 0x7A:     // ja  relative mem_offset 8
      case 0x7B:     // ja  relative mem_offset 8
      case 0x7C:     // jl  relative mem_offset 8
      case 0x7D:     // jge relative mem_offset 8
      case 0x7E:     // jle relative mem_offset 8
      case 0x7F:     // jg  relative mem_offset 8
      case 0xE0:     // loopne relative mem_offset 8
      case 0xE1:     // loope  relative mem_offset 8
      case 0xE2:     // loop   relative mem_offset 8
        { 
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          bitn_o = DISASSEMBLY_BITNESS_08;
          n_bytes_used = 2;

          j = *(int*) &param;               
          if (j > 127)
          { target = 0 - (256 - j); }
          else { target = j; }
          target += it->memory_offset + n_bytes_used ;       
          
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic
          
          instr.part1.imm = target;                             // part1
          instr.part1.abs = true;

          instr.part1.used = true;
          instr.part2.used = false;
          break;
        }
      
      case 0x80:     // r/m 8 , imm8
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          bitn_o = DISASSEMBLY_BITNESS_08;

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                       // mnemonic = 'add'
          case 1:                                                       // mnemonic = 'or'
          case 2:                                                       // mnemonic = 'adc'
          case 3:                                                       // mnemonic = 'sbb'
          case 4:                                                       // mnemonic = 'and'
          case 5:                                                       // mnemonic = 'sub'
          case 6:                                                       // mnemonic = 'xor'
          case 7:                                                       // mnemonic = 'cmp'
            instr.mnemonic = (char*) &(intel_mnemonic_80[reg]);        
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg <<
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }       

          switch (bitn_a)                                               // part 1 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }                                                     
          
          if (Get_Byte_From_Disassembly_Node(it, continuation + n_bytes_used + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}
          j =  *(int*) &param;               
          instr.part2.imm = j;                                          // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 2;
          break;
        }

      case 0x81:     // r/m 16/32 , imm16/32
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                       // mnemonic = 'add'
          case 1:                                                       // mnemonic = 'or'
          case 2:                                                       // mnemonic = 'adc'
          case 3:                                                       // mnemonic = 'sbb'
          case 4:                                                       // mnemonic = 'and'
          case 5:                                                       // mnemonic = 'sub'
          case 6:                                                       // mnemonic = 'xor'
          case 7:                                                       // mnemonic = 'cmp'
            instr.mnemonic = (char*) &(intel_mnemonic_81[reg]);        
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg <<
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }

          switch (bitn_a)                                               // part 1 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }                                                     
          
          for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
          {
            if (Get_Byte_From_Disassembly_Node(it, continuation + i + 1 + n_bytes_used, &param[i]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          }

          j =  *(int*) &param;               
          instr.part2.imm = j;                                          // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += bitn_o + 1;
          break;
        }
      
      case 0x83:     // r/m 16/32 , imm8
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                       // mnemonic = 'add'
          case 1:                                                       // mnemonic = 'or'
          case 2:                                                       // mnemonic = 'adc'
          case 3:                                                       // mnemonic = 'sbb'
          case 4:                                                       // mnemonic = 'and'
          case 5:                                                       // mnemonic = 'sub'
          case 6:                                                       // mnemonic = 'xor'
          case 7:                                                       // mnemonic = 'cmp'
            instr.mnemonic = (char*) &(intel_mnemonic_83[reg]);        
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg <<
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }

          switch (bitn_a)                                               // part 1 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }                                                     
          
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1 + n_bytes_used, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          j =  *(int*) &param;               
          instr.part2.imm = j;                                          // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 2;
          break;
        }                                                                     
      
      case 0x8C:     // mov r/m 16/32 , seg_reg
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

          switch (bitn_a)                                               // part 1 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }                                                            

          d = instr.part2.displ;                                     // part 2
          if (d == 0)
          { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); instr.part2.displ = d;}  
          Initialize(d);

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          d->seg_reg = reg + 1;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 1;
          break;
        }
      
      case 0x8D:     // lea r16/32, m 16/32
      case 0xC4:     // les r16/32, m 16/32
      case 0xC5:     // lds r16/32, m 16/32


        {
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

          switch (bitn_a)                                               // part 1 + 2
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 1, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 1, &ref, &instr); break; }
          }                                                     

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 1;
          
          break;
        }
      
      case 0x8E:     // mov seg_reg, r/m 16/32 
        {
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          switch (bitn_a)                                               // part 2 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }                                   

          instr.part2 = instr.part1;

          Initialize(&(instr.part1));                       // part1
                                  
          d = instr.part1.displ;                                    
          if (d == 0)
           { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); instr.part1.displ = d;}  
          Initialize(d);

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          d->seg_reg = reg + 1;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 1;
          break;
        }
      
      case 0x8F:     // m 16/32 
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                       // mnemonic = 'pop'
            instr.mnemonic = (char*) &(intel_mnemonic_8F[reg]);        
            n_bytes_pushed  = -bitn_o; 
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg <<
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }

          switch (bitn_a)                                               // part 1 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }                                                     
          
          instr.part1.used = true;
          instr.part2.used = false;
          n_bytes_used += 1;
          break;
        }                                                                     
      
      case 0x91:                                  // xchg (e)ax, cx   
      case 0x92:                                  // xchg (e)ax, dx   
      case 0x93:                                  // xchg (e)ax, bx   
      case 0x94:                                  // xchg (e)ax, sp   
      case 0x95:                                  // xchg (e)ax, bp   
      case 0x96:                                  // xchg (e)ax, si   
      case 0x97:                                  // xchg (e)ax, di
          {
           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

           j = (opcode & 0xFF);

           switch (bitn_o)                            // part1
           {
           case DISASSEMBLY_BITNESS_32: 
             { instr.part1.reg32 = 1; instr.part2.reg32 = j - 0x90; break; }
           case DISASSEMBLY_BITNESS_16: 
             { instr.part1.reg16 = 1; instr.part2.reg16 = j - 0x90; break; }
           }

           instr.part1.abs = true;
           instr.part2.abs = true;

           instr.part1.used = true;
           instr.part2.used = true;

           n_bytes_used = 1;   
           break;
          }
                
      case 0x98:                                      
          {
           n_bytes_used = 1;                                    
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_98[0]); break;}   // cbw
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_98[1]); break;}   // cwde
           }                      

           instr.part1.used = false;
           instr.part2.used = false;
           break;
          }
                
      case 0x99:                                      
          {
           n_bytes_used = 1;                                    
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_99[0]); break;}   // cwd 
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_99[1]); break;}   // cdq
           }                        

           instr.part1.used = false;
           instr.part2.used = false;
           break;
          }
                
      case 0x9A:     // call ptr16/32:ptr16/32
        { 
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

          d = instr.part1.displ;                                        // part 1
          if (d == 0)
           { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); instr.part1.displ = d;}  
          Initialize(d);

          for (i=0; i < bitn_a; i++)                   
             {
               result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
               if (result == RET_ERR_OPCODE)
               { return RET_ERR_OPCODE; }
             }
           j =  *(int*) &param;      
           d->seg_offset = j;

          for (i=0; i < bitn_a; i++)                    
              {
                result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1 + bitn_a, &param[i]);   
                if (result == RET_ERR_OPCODE)
                { return RET_ERR_OPCODE; }
              }
          j =  *(int*) &param;      
          instr.part1.imm = j;
            
          instr.part1.abs = true;

          instr.part1.used = true;
          instr.part2.used = false;
          n_bytes_used = 1 + bitn_a + bitn_o;
          type_call = INSTRUCTION_CALL_ROUTINE;
          
          break;
        }
      
      case 0x9B:                                      // wait
          {
              instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic
              n_bytes_used = 1;

              instr.part1.used = false;
              instr.part2.used = false;
              break;
          }

      case 0x9C:                                      // pushf
          {
            switch (bitn_o)
            {                                                    // mnemonic
            case DISASSEMBLY_BITNESS_16:
                {instr.mnemonic = (char*) &(intel_mnemonic_9C[0]); break;}   // pushfw
            case DISASSEMBLY_BITNESS_32:
                {instr.mnemonic = (char*) &(intel_mnemonic_9C[1]); break;}   // pushfd
            }                                                  
              
            n_bytes_used = 1;
            n_bytes_pushed = bitn_o;

            instr.part1.used = false;
            instr.part2.used = false;
            break;
           }

      case 0x9D:                                      // popf
          {
            switch (bitn_o)
            {                                                    // mnemonic
            case DISASSEMBLY_BITNESS_16:
                {instr.mnemonic = (char*) &(intel_mnemonic_9D[0]); break;}   // popfw
            case DISASSEMBLY_BITNESS_32:
                {instr.mnemonic = (char*) &(intel_mnemonic_9D[1]); break;}   // popfd
            }                                                  
              
            n_bytes_used = 1;
            n_bytes_pushed = -(bitn_o);

            instr.part1.used = false;
            instr.part2.used = false;
            break;
           }

      case 0xA0:     // mov al, mem_offset
          bitn_o = DISASSEMBLY_BITNESS_08;
      case 0xA1:     // mov (e)ax, mem_offset
        {
          for (i=0; i < bitn_a; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
            {
            result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
            if (result == RET_ERR_OPCODE)
            {
               return RET_ERR_OPCODE; 
            }
          }
          n_bytes_used = bitn_a + 1;
          ref =  *(int*) &param;               
          
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

          switch (bitn_o)                                // part1
          {
          case DISASSEMBLY_BITNESS_32: 
            { instr.part1.reg32 = 1; break; }
          case DISASSEMBLY_BITNESS_16: 
            { instr.part1.reg16 = 1; break; }
          case DISASSEMBLY_BITNESS_08: 
            { instr.part1.reg08 = 1; break; }                   
          }
          instr.part1.abs = true;

          instr.part2.imm = ref;                         // part2
          instr.part2.abs = false;

          instr.part1.used = true;
          instr.part2.used = true;
          break;
        }

      case 0xA2:     // mov mem_offset 8, al
          bitn_o = DISASSEMBLY_BITNESS_08;
      case 0xA3:     // mov mem_offset, (e)ax
        {
          for (i=0; i < bitn_a; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
            {
            result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
            if (result == RET_ERR_OPCODE)
            {
               return RET_ERR_OPCODE; 
            }
          }
          n_bytes_used = bitn_a + 1;
          ref =  *(int*) &param;               
          
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

          switch (bitn_o)                                // part2
          {
          case DISASSEMBLY_BITNESS_32: 
            { instr.part2.reg32 = 1; break; }
          case DISASSEMBLY_BITNESS_16: 
            { instr.part2.reg16 = 1; break; }
          case DISASSEMBLY_BITNESS_08: 
            { instr.part2.reg08 = 1; break; }                   
          }
          instr.part2.abs = true;

          instr.part1.imm = ref;                         // part1
          instr.part1.abs = false;

          instr.part1.used = true;
          instr.part2.used = true;
          break;
        }

      case 0xA4:                                      // MOVSB
      case 0xAA:                                      // STOSB
      case 0xAC:                                      // LODSB
      case 0xAE:                                      // SCASB
          {
           n_bytes_used = 1;                                                        
           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic

           instr.part1.used = false;
           instr.part2.used = false;
           break;
          }

      case 0xA5:                                      // MOVSW/D
          {
           n_bytes_used = 1;                                    
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_A5[0]); break;} 
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_A5[1]); break;} 
           }                       

           instr.part1.used = false;
           instr.part2.used = false;
           break;
          }

      case 0xA6:     // cmps m8,m8
      case 0xA7:     // cmps m16/32,m16/32
        {
          if ((opcode & 0xFF) == 0xA6)
          { bitn_o = DISASSEMBLY_BITNESS_08; }

          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

          switch (bitn_a)                                   // part 1 + 2 
          {
          case DISASSEMBLY_BITNESS_32: 
            {
            instr.part1.reg32 = 7; 
            instr.part2.reg32 = 8; 
            break;
            }
          case DISASSEMBLY_BITNESS_16: 
            {
            instr.part1.reg16 = 7; 
            instr.part2.reg16 = 8; 
            break;
            }
          }

          for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
          {
            result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
            if (result == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          }
          
          ref = *(int*) &param;               
          instr.part1.imm = ref;
          instr.part1.abs = false;             

          for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
          {
            result = Get_Byte_From_Disassembly_Node(it, continuation + i + bitn_o + 1, &param[i]);   
            if (result == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          }
                    
          ref2 = *(int*) &param;               
          instr.part2.imm = ref2;               
          instr.part2.abs = false;  

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used = 1 + bitn_o + bitn_o;

          break;
        }          

      case 0xAB:                                      // STOSW/D
          {
           n_bytes_used = 1;                                    
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_AB[0]); break;} 
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_AB[1]); break;} 
           }                       

           instr.part1.used = false;
           instr.part2.used = false;
           break;
          }

      case 0xAD:                                      // LODSD
          {
           n_bytes_used = 1;                                    
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_AD[0]); break;} 
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_AD[1]); break;} 
           }                       

           instr.part1.used = false;
           instr.part2.used = false;
           break;
          }

      case 0xAF:                                      // LODSD
          {
           n_bytes_used = 1;                                    
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_AF[0]); break;} 
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_AF[1]); break;} 
           }                       

           instr.part1.used = false;
           instr.part2.used = false;
           break;
          }

      case 0xB0:     // mov al, imm
      case 0xB1:     // mov cl, imm
      case 0xB2:     // mov dl, imm
      case 0xB3:     // mov bl, imm
      case 0xB4:     // mov ah, imm
      case 0xB5:     // mov ch, imm
      case 0xB6:     // mov dh, imm
      case 0xB7:     // mov bh, imm
        {
          result = Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]);   
          if (result == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          bitn_o = DISASSEMBLY_BITNESS_08;
          
          ref =  *(int*) &param;               
          
          j = (opcode & 0xFF);
          instr.mnemonic = (char*) &(intel_mnemonic[j]);  // mnemonic

          instr.part2.imm = ref;                          // part1
          instr.part2.abs = true;

          j = j - 0xB0;
          instr.part1.reg08 = j + 1; 
          instr.part1.abs = true;
          
          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used = 2;
          break;
        }

      case 0xB8:     // mov (e)ax, imm
      case 0xB9:     // mov (e)cx, imm
      case 0xBA:     // mov (e)dx, imm
      case 0xBB:     // mov (e)bx, imm
      case 0xBC:     // mov (e)sp, imm
      case 0xBD:     // mov (e)bp, imm
      case 0xBE:     // mov (e)si, imm
      case 0xBF:     // mov (e)di, imm
        {
          for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
          {
            result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
            if (result == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          }
          
          ref =  *(int*) &param;               
          
          j = (opcode & 0xFF);
          instr.mnemonic = (char*) &(intel_mnemonic[j]);  // mnemonic

          instr.part2.imm = ref;                          // part1
          instr.part2.abs = true;

          j = j - 0xB8;
          switch (bitn_o)                                   // part2
          {
          case DISASSEMBLY_BITNESS_32: 
            { instr.part1.reg32 = j + 1; break; }
          case DISASSEMBLY_BITNESS_16: 
            { instr.part1.reg16 = j + 1; break; }
          }
          instr.part1.abs = true;
          
          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used = bitn_o + 1;
          break;
        }

      case 0xC0:     // r/m 8, imm8
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          bitn_o = DISASSEMBLY_BITNESS_08;

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                   // mnemonic = 'rol'
          case 1:                                                   // mnemonic = 'ror'
          case 2:                                                   // mnemonic = 'rcl'
          case 3:                                                   // mnemonic = 'rcr'
          case 4:                                                   // mnemonic = 'sal'
          case 5:                                                   // mnemonic = 'shr'
          case 7:                                                   // mnemonic = 'sar'

            instr.mnemonic = (char*) &(intel_mnemonic_D0[reg]);     
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg << 
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                 

          switch (bitn_a)                                    // part 1
          {
          case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }      

          if (Get_Byte_From_Disassembly_Node(it, continuation + 1 + n_bytes_used, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          j =  *(int*) &param;               

          instr.part2.imm = j;                         // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 2;

          break;
        }                                                          

      case 0xC1:     // s[h/a][l/r] r16/32 /m, imm8
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                   // mnemonic = 'rol'
          case 1:                                                   // mnemonic = 'ror'
          case 2:                                                   // mnemonic = 'rcl'
          case 3:                                                   // mnemonic = 'rcr'
          case 4:                                                   // mnemonic = 'shl'
          case 5:                                                   // mnemonic = 'shr'
          case 7:                                                   // mnemonic = 'sar'
            instr.mnemonic = (char*) &(intel_mnemonic_C1[reg]);     
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg << 
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                 

          switch (bitn_a)                                    // part 1
          {
          case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }      

          if (Get_Byte_From_Disassembly_Node(it, continuation + 1 + n_bytes_used, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}


          j =  *(int*) &param;               
          instr.part2.imm = j;                         // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 2;

          break;
        }                                                          

      case 0xC2:     // ret imm 16
      case 0xCA:     // ret imm 16
        {
          for (i=0; i < 2; i++)     
          {
            if (Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          }

          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic
          
          j =  (*(int*) &param);               
          instr.part1.imm = j;                   // part1
          instr.part1.abs = true;

          n_bytes_pushed = -j;
          instr.part1.used = true;
          instr.part2.used = false;
          n_bytes_used = 3;

          finished = true;
                                                                       
          break;
        }                            

      case 0xC3:                                    // ret
      case 0xC9:                                    // leave
      case 0xCB:                                    // ret
      case 0xCF:                                    // iret
          {   
              instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic
              instr.part1.used = false;
              instr.part2.used = false;
              n_bytes_used = 1;
              finished = true;

              break;
          } 
            
      case 0xC6:     // mov r/m 8 , imm8
        {
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

          bitn_o = DISASSEMBLY_BITNESS_08;

          switch (bitn_a)                                               // part 1 
          {
          case DISASSEMBLY_BITNESS_32: 
            { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
            { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }                                                     
          
          if (Get_Byte_From_Disassembly_Node(it, continuation + n_bytes_used + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}
          j =  *(int*) &param;               
          instr.part2.imm = j;                                          // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 2;
          break;
        }

      case 0xC7:           // mov r/m 16/32, imm 16/32
          {
            instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

            switch (bitn_a)                                               // part 1 
            {
            case DISASSEMBLY_BITNESS_32: 
              { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
              { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            }                                                     

            for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
            {
                if (Get_Byte_From_Disassembly_Node(it, continuation + i + 1 + n_bytes_used, &param[i]) == RET_ERR_OPCODE)
                { return RET_ERR_OPCODE; }
            }

            j =  *(int*) &param;               
            instr.part2.imm = j;                                          // part2
            instr.part2.abs = true;

            instr.part1.used = true;
            instr.part2.used = true;
            n_bytes_used += bitn_o + 1;
            break;
          }                                                                     

      case 0xC8:     // enter imm 16, imm8
        {
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

          for (i=0; i < 2; i++)     
          {
            if (Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          }

          j =  (*(int*) &param);               
          instr.part1.imm = j;                   // part1
          instr.part1.abs = true;
          param[0] = param[1] = '\000';

          if (Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          j =  (*(int*) &param);               
          instr.part2.imm = j;                   // part2
          instr.part2.abs = true;                        

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used = 4;                                                             
          
          break;
        }                            

      case 0xD0:     // r/m 8, 1
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          bitn_o = DISASSEMBLY_BITNESS_08;

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                   // mnemonic = 'rol'
          case 1:                                                   // mnemonic = 'ror'
          case 2:                                                   // mnemonic = 'rcl'
          case 3:                                                   // mnemonic = 'rcr'
          case 4:                                                   // mnemonic = 'sal'
          case 5:                                                   // mnemonic = 'shr'
          case 7:                                                   // mnemonic = 'shr'

            instr.mnemonic = (char*) &(intel_mnemonic_D0[reg]);     
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg << 
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                 

          switch (bitn_a)                                    // part 1
          {
          case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }      

          instr.part2.imm = 1;                         // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 1;

          break;
        }                                                          

      case 0xD1:     // r/m 16/32, 1
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                   // mnemonic = 'rol'
          case 1:                                                   // mnemonic = 'ror'
          case 2:                                                   // mnemonic = 'rcl'
          case 3:                                                   // mnemonic = 'rcr'
          case 4:                                                   // mnemonic = 'shl'
          case 5:                                                   // mnemonic = 'shr'
          case 7:                                                   // mnemonic = 'sar'
            instr.mnemonic = (char*) &(intel_mnemonic_D1[reg]);     
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg << 
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                 

          switch (bitn_a)                                    // part 1
          {
          case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }      

          instr.part2.imm = 1;                         // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 1;

          break;
        }                                                          

      case 0xD2:     // r/m 8, cl
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          bitn_o = DISASSEMBLY_BITNESS_08;

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                   // mnemonic = 'rol'
          case 1:                                                   // mnemonic = 'ror'
          case 2:                                                   // mnemonic = 'rcl'
          case 3:                                                   // mnemonic = 'rcr'
          case 4:                                                   // mnemonic = 'shl'
          case 5:                                                   // mnemonic = 'shr'
          case 7:                                                   // mnemonic = 'sar'

            instr.mnemonic = (char*) &(intel_mnemonic_D0[reg]);     
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg << 
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                 

          switch (bitn_a)                                    // part 1
          {
          case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }      

          instr.part2.reg08 = 2;                         // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 1;

          break;
        }                                                          

      case 0xD3:     // r/m 16/32, cl
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE;}

          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                   // mnemonic = 'rol'
          case 1:                                                   // mnemonic = 'ror'
          case 2:                                                   // mnemonic = 'rcl'
          case 3:                                                   // mnemonic = 'rcr'
          case 4:                                                   // mnemonic = 'shl'
          case 5:                                                   // mnemonic = 'shr'
          case 7:                                                   // mnemonic = 'sar'

            instr.mnemonic = (char*) &(intel_mnemonic_D0[reg]);     
            break;

          default:
            { 
              cout << "unknown reg/opcode digit for opcode" << hex << (opcode & 0xFF) << " : " << reg << 
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                 

          switch (bitn_a)                                    // part 1
          {
          case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
          }      

          instr.part2.reg08 = 2;                         // part2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used += 1;

          break;
        }                                                          

      case 0xD4: 
        {
         result = Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]);   
            
         if (result == RET_ERR_OPCODE)
            {
               return RET_ERR_OPCODE; 
            }
           
         j = (param[0] & 0xFF);
         switch (j)
         {
          case 0xA:                                         // D4 0A = AAM
            {
              n_bytes_used = 2;

              instr.mnemonic = (char*) mp.Use_Pool(4); 
              strcpy(instr.mnemonic, "AAM");
            }                   
          default:
            {
              cout << "unknown param[0] for opcode = " << hex << (opcode & 0xFF) << " : " << (param[0] & 0xFF) << 
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              finished = true; 
              disassembled = false;
            } 
                    
         }

         instr.part1.used = false;
         instr.part2.used = false;

         break;
        }                                                                

      case 0xD5: 
        {
         result = Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]);   
            
         if (result == RET_ERR_OPCODE)
            {
               return RET_ERR_OPCODE; 
            }
           
         j = (param[0] & 0xFF);
         switch (j)
         {
          case 0xA:                                         // D5 0A = AAD
            {
              n_bytes_used = 2;

              instr.mnemonic = (char*) mp.Use_Pool(4); 
              strcpy(instr.mnemonic, "AAD");
            }                   
          default:
            {
              cout << "unknown param[0] for opcode = " << hex << (opcode & 0xFF) << " : " << (param[0] & 0xFF) << 
                      " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              finished = true; 
              disassembled = false;
            } 
         }

        instr.part1.used = false;
        instr.part2.used = false;
        break;
        }                                                                
      
      case 0xD8:                                    // co processor instructions
      case 0xD9:  
      case 0xDA:  
      case 0xDB:  
      case 0xDC:  
      case 0xDD:  
      case 0xDE:  
      case 0xDF:  
          {
            n_bytes_used = Convert_Opcodes_CoProcessor(it, (opcode & 0xFF), &ref, &target, continuation, 
                                                       &n_bytes_pushed, bitn_a, &bitn_o, &instr, wait_flag);

            if (n_bytes_used == 0)
            { finished = true; disassembled = false;}          

            break;
          }

      case 0xE3:                                      
          {
           switch (bitn_o)
           {
           case DISASSEMBLY_BITNESS_16:
            { instr.mnemonic = (char*) &(intel_mnemonic_E3[0]); break;}   // jcx
           case DISASSEMBLY_BITNESS_32:
            { instr.mnemonic = (char*) &(intel_mnemonic_E3[1]); break;}   // jecx
           }                                                            
          
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          n_bytes_used = 2;

          j = *(int*) &param;               
          if (j > 127)
          { target = 0 - (256 - j); }
          else { target = j; }
          target += it->memory_offset + n_bytes_used ;       
          
          instr.part1.imm = target;                             // part1
          instr.part1.abs = true;

          instr.part1.used = true;
          instr.part2.used = false;
          break;                       
          }
      
      case 0xE5:                                    // in  (e)ax, imm8
        {
              if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
              { return RET_ERR_OPCODE;}

              instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic
                
              switch (bitn_o)                                    // part 1
              {
              case DISASSEMBLY_BITNESS_32: 
               { instr.part1.reg32 = 1; break; }
              case DISASSEMBLY_BITNESS_16: 
               { instr.part1.reg16 = 1; break; }
              }                   
              
              instr.part1.abs = true;                                               

              j =  *(int*) &param;               
              instr.part2.imm = j;                                          // part2
              instr.part2.abs = true;

              instr.part1.used = true;
              instr.part2.used = true;
              n_bytes_used = 2;
              break;
        }                

      case 0xE6:                                    // out  imm8, al
      case 0xE7:                                    // out  imm8, (e)ax
        {
              if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
              { return RET_ERR_OPCODE;}

              switch (opcode & 0xFF)
              {
              case 0xE6: 
               { bitn_o = DISASSEMBLY_BITNESS_08; 
                 instr.part2.reg08 = 1; 
                 break;
               }
              case 0xE7: 
               { if (bitn_o == DISASSEMBLY_BITNESS_16)
                      {instr.part2.reg16 = 1;}
                 else {instr.part2.reg32 = 1;}
                 break;
               }
              }
                  
              instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic
              
              instr.part2.abs = true;                                               

              j =  *(int*) &param;               
              instr.part1.imm = j;                                          // part1
              instr.part1.abs = true;

              instr.part1.used = true;
              instr.part2.used = true;
              n_bytes_used = 2;
              break;
        }                

      case 0xE8:     // call rel 16/32
      case 0xE9:     // jmp rel 16/32
        {
          for (i=0; i < bitn_o; i++)                     //  DISASSEMBLY_BITNESS_32 = 4
          {
            if (Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          }

          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic
          n_bytes_used += bitn_o + 1;

          target =  (*(int*) &param) + it->memory_offset + n_bytes_used;               
          instr.part1.imm = target;                   // part1
          instr.part1.abs = true;

          instr.part1.used = true;
          instr.part2.used = false;

          if ((opcode & 0xFF) == 0xE8)
          { type_call = INSTRUCTION_CALL_ROUTINE;}
          else
          { type_call = INSTRUCTION_CALL_JUMP;}

          break;
        }                            

      case 0xEA:     // jmp ptr 16:16/32
         {
           d = instr.part1.displ;                                     // part 1
           if (d == 0)
           { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); instr.part1.displ = d;}  
           Initialize(d);

           for (i=0; i < 2; i++)                               //  part1 ptr16:
            {
               result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
               if (result == RET_ERR_OPCODE)
               { return RET_ERR_OPCODE; }
            }
           j =  *(int*) &param;      
           d->seg_offset = j;
            
           switch (bitn_a)                                    // part 1 ptr16/32
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 3, 0, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 3, 0, &ref, &instr); break; }
            }      

           instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic

           instr.part1.used = true;
           instr.part2.used = false;
           n_bytes_used += 3;
           break;
         }                            

      case 0xEB:     // jmp rel 8
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF) ]);  // mnemonic
          n_bytes_used += 2;

          j =  (*(int*) &param);
          if (j > 127)
          { target = 0 - (256 - j); }
          else { target = j; }

          target += it->memory_offset + n_bytes_used;               

          instr.part1.imm = target;                   // part1
          instr.part1.abs = true;

          instr.part1.used = true;
          instr.part2.used = false;

          break;
        }

      case 0xEC:                                    // in  al, dx
          bitn_o = DISASSEMBLY_BITNESS_08; 
      case 0xED:                                    // in  (e)ax, dx
        {
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic
                
          switch (bitn_o)                                    // part 1
              {
              case DISASSEMBLY_BITNESS_32: 
               { instr.part1.reg32 = 3; break; }
              case DISASSEMBLY_BITNESS_16: 
               { instr.part1.reg16 = 3; break; }
              case DISASSEMBLY_BITNESS_08: 
               { instr.part1.reg08 = 3; break; }
              }                   
              
          instr.part1.abs = true;                                               

          instr.part2.reg16 = 1;                        // part 2
          instr.part2.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used = 1;
          break;
        }                

      case 0xEE:                                    // out dx, al
          bitn_o = DISASSEMBLY_BITNESS_08; 
      case 0xEF:                                    // out dx, (e)ax  
         {
          instr.mnemonic = (char*) &(intel_mnemonic[(opcode & 0xFF)]);  // mnemonic
                
          switch (bitn_o)                               // part 2
              {
              case DISASSEMBLY_BITNESS_32: 
               { instr.part2.reg32 = 3; break; }
              case DISASSEMBLY_BITNESS_16: 
               { instr.part2.reg16 = 3; break; }
              case DISASSEMBLY_BITNESS_08: 
               { instr.part2.reg08 = 3; break; }
              }                   
          instr.part2.abs = true;                                               

          instr.part1.reg16 = 1;                        // part 1
          instr.part1.abs = true;

          instr.part1.used = true;
          instr.part2.used = true;
          n_bytes_used = 1;
          break;
        }                

      case 0xF0:                                      // lock prefix
      case 0xF2:                                      // repne prefix
      case 0xF3:                                      // rep prefix
          {
              continuation++;
              disassembled = false;
              break;
          }                                                        
      
      case 0xF6:     // ... r/m 8
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          bitn_o = DISASSEMBLY_BITNESS_08;
          
          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                              // mnemonic = 'test'
          case 2:                                              // mnemonic = 'not'
          case 3:                                              // mnemonic = 'neg'
          case 4:                                              // mnemonic = 'mul'
          case 5:                                              // mnemonic = 'imul'
          case 6:                                              // mnemonic = 'div'
          case 7:                                              // mnemonic = 'idiv'
            {
            instr.mnemonic = (char*) &(intel_mnemonic_F6[reg]);     
            
            switch (bitn_a)                                    // part 1
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            }      

            if (reg == 0)                                      // takes an additional immediate byte
            {
             if (Get_Byte_From_Disassembly_Node(it, continuation + n_bytes_used + 1, &param[0]) == RET_ERR_OPCODE)    
             { return RET_ERR_OPCODE; }

             ref =  *(int*) &param;               
             instr.part2.imm = ref;                            // part 2
             instr.part2.abs = true;

             n_bytes_used ++;
            }

            break;
            }

          default:
            { 
              cout << "unknown reg/opcode digit for opcode " << hex << (opcode & 0xFF) << "," << (*(int*) &param) << " : "
                   << reg << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                           

          instr.part1.used = true;
          instr.part2.used = (reg == 0);
          n_bytes_used += 1;                         
          break;
        }

      case 0xF7:     // ... r/m 16/32
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          
          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                           // mnemonic = 'test'
          case 2:                                           // mnemonic = 'not'
          case 3:                                           // mnemonic = 'neg'
          case 4:                                           // mnemonic = 'mul'
          case 5:                                           // mnemonic = 'imul'
          case 6:                                           // mnemonic = 'div'
          case 7:                                           // mnemonic = 'idiv'
            {
            instr.mnemonic = (char*) &(intel_mnemonic_F7[reg]);     
            
            switch (bitn_a)                                 // part 1
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            }      

            if (reg == 0)                                    // takes bitn_o additional immediate bytes
            {
             for (i=0; i < bitn_o; i++)     
             {
              if (Get_Byte_From_Disassembly_Node(it, continuation + i + 1 + n_bytes_used, &param[i]) == RET_ERR_OPCODE)
              { return RET_ERR_OPCODE; }
             }

             ref =  (*(int*) &param);               
             instr.part2.imm = ref;                         // part 2
             instr.part2.abs = true;

             n_bytes_used += bitn_o;
            }

            break;
            }

          default:
            { 
              cout << "unknown reg/opcode digit for opcode " << hex << (opcode & 0xFF) << "," << (*(int*) &param) << " : "
                   << reg << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                           

          instr.part1.used = true;
          instr.part2.used = (reg == 0);
          n_bytes_used += 1;                         
          break;
        }
        
      case 0xFE:     // ... r/m 8
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }

          bitn_o = DISASSEMBLY_BITNESS_08;
          
          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                   // mnemonic = 'inc'
          case 1:                                                   // mnemonic = 'dec'
            {
            instr.mnemonic = (char*) &(intel_mnemonic_FE[reg]);     
            
            switch (bitn_a)                                    // part 1
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            }      

            break;
            }

          default:
            { 
              cout << "unknown reg/opcode digit for opcode " << hex << (opcode & 0xFF) << "," << (*(int*) &param) << " : "
                   << reg << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                           

          instr.part1.used = true;
          instr.part2.used = false;
          n_bytes_used += 1;                         
          break;
        }

      case 0xFF:     // ... r/m 16/32
        {
          if (Get_Byte_From_Disassembly_Node(it, continuation + 1, &param[0]) == RET_ERR_OPCODE)
            { return RET_ERR_OPCODE; }
          
          reg = (param[0] & 0x38) >> 3;                     // 0x38 = 111000b  
          switch (reg)                                      // mnemonic
          {
          case 0:                                                   // mnemonic = 'inc'
          case 1:                                                   // mnemonic = 'dec'
          case 2:                                                   // mnemonic = 'call'
          case 6:                                                   // mnemonic = 'push'
            {
            instr.mnemonic = (char*) &(intel_mnemonic_FF[reg]);     
            
            switch (bitn_a)                                    // part 1
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            }

            if (reg == 6)
            { n_bytes_pushed  =  bitn_o; }

            break;
            }

          case 3:                                              // mnemonic = 'call m16:m16/32'
          case 5:                                              // mnemonic = 'jmp  m16:m16/32'
            {
            instr.mnemonic = (char*) &(intel_mnemonic_FF[reg]);     

            d = instr.part1.displ;                                     // part 1
            if (d == 0)
            { d = (Displacement*) mp.Use_Pool(sizeof(Displacement)); instr.part1.displ = d;}  
            Initialize(d);

            for (i=0; i < 2; i++)                              //  part1 m16:
            {
               result = Get_Byte_From_Disassembly_Node(it, continuation + i + 1, &param[i]);   
               if (result == RET_ERR_OPCODE)
               { return RET_ERR_OPCODE; }
            }
            j =  *(int*) &param;      
            d->seg_offset = j;
            
            switch (bitn_a)                                    // part 2 m16/32
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 3, 0, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 3, 0, &ref, &instr); break; }
            }      

            n_bytes_used += 3;

            break;
            }   
          
          case 4:                                                   // mnemonic = 'jmp'
            {
            instr.mnemonic = (char*) &(intel_mnemonic_FF[reg]);     
            
            switch (bitn_a)                                    // part 1
            {
            case DISASSEMBLY_BITNESS_32: 
               { n_bytes_used = Convert_Address_32(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            case DISASSEMBLY_BITNESS_16: 
               { n_bytes_used = Convert_Address_16(bitn_o, it, continuation + 1, 0, &ref, &instr); break; }
            }      

            d = instr.part1.displ;
            if (d != 0)
            { 
             if (d->seg_override == 0)               
             {
              d->seg_override = 4;                
             }
            }

            target = ref;
            break;
            }   
          
          default:
            { 
              cout << "unknown reg/opcode digit for opcode " << hex << (opcode & 0xFF) << "," << (*(int*) &param) << " : "
                   << reg << " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               
              disassembled = false;
              finished = true; 
            }
          }                                                                           

          switch (reg)
          {
          case 2: 
          case 3: {type_call = INSTRUCTION_CALL_ROUTINE; break;}
          case 4: 
          case 5: {type_call = INSTRUCTION_CALL_JUMP;    break;}
          }

          instr.part1.used = true;
          instr.part2.used = false;
          n_bytes_used += 1;                         
          break;
        }
        
      default:
        {
         cout << "unknown opcode = " << hex << (opcode & 0xFF) << "-" << (param[0] & 0xFF) << 
                 " at offset " << it->file_offset << " , " << it->memory_offset << "\n";               

         finished = true; 
         disassembled = false;
        }
      }  


      // put the disassembled instruction in its own Disassembly_Node
      if (disassembled == true)
      {  
         n_converted++;
         if ((n_converted >= n_opcodes) && (n_opcodes > 0))
         { v_to_explore.push_back(it->memory_offset + n_bytes_used); finished = true; }

         if (continuation > 0)
         { n_bytes_used += continuation; }


         // put instruction in its own node
         if ((n_bytes_used > 0) && (n_bytes_used < it->n_used))
         { 
             Split_l_dn(it, n_bytes_used, true);
         }                       


         // complete instruction
         if (prefix != 0)
         {
          t_char = (char*) mp.Use_Pool(strlen(instr.mnemonic) + DISASSEMBLY_MAX_MNEMONIC_LENGTH + 2);

          switch (prefix)
          {
          case 0xF0:
          case 0xF2:
          case 0xF3:
           {  
              strcpy(t_char, (char*) &(intel_mnemonic[(prefix & 0xFF)])); 
              break;
           }
          }         
          strcat(t_char, " ");
          strcat(t_char, instr.mnemonic);
          instr.mnemonic = t_char;
         }

         if (segment_override > 0)
         {                                  // try and find the part of the instruction that
          if (   (instr.part1.reg32 > 0)    //  does not use a register,and therefor has to have 
              || (instr.part1.reg16 > 0)    //  its segment overridden
              || (instr.part1.reg08 > 0))
               { d = instr.part2.displ; j = 2;}
          else { d = instr.part1.displ; j = 1;}

          if (d == 0)
          { 
            d = (Displacement*) mp.Use_Pool(sizeof(Displacement));
            if (j == 1) { instr.part1.displ = d;}               
            else        { instr.part2.displ = d;}               
          }  
          Initialize(d);

          d->seg_override = segment_override;
         }
         
         if (type_call == INSTRUCTION_CALL_NONE)
          { it->instruction.call = 0;}
         else 
          {
           if (it->instruction.call == 0)
           { 
            c = (Call*) mp.Use_Pool(sizeof(Call));
            instr.call = c;               
           }  
           else 
           { instr.call = it->instruction.call; }
          
           Initialize(instr.call);
           instr.call->type_of_call = type_call;
          }
         instr.n_pushed = n_bytes_pushed;                                            
         instr.operand_size = bitn_o;
         it->instruction = instr;

         it->status = NODE_STATUS_AUTO;
         it->type = NODE_TYPE_CODE;
         it->n_used = n_bytes_used;

         continuation = 0; 
         continuation = 0;
         segment_override = 0;

         // immediate can always be an offset
         if (instr.part1.imm != 0)
         {
           Add_Reference(it, instr.part1.imm, false); 
           Add_Uncertain_Address_To_Explore(instr.part1.imm); 
         }
         if (instr.part2.imm != 0)
         {
           Add_Reference(it, instr.part2.imm, false); 
           Add_Uncertain_Address_To_Explore(instr.part2.imm); 
         }
         if ((instr.part3) && (instr.part1.imm != 0))
         {
           Add_Reference(it, instr.part3->imm, false); 
           Add_Uncertain_Address_To_Explore(instr.part3->imm); 
         }                                               

         // update statistics
         if (target)
         {
           // we have found a reference to code
           Add_Reference(it, target, true); 

           Add_Code_To_Explore(it, target);
         }
         else 
         {
           if (ref)
           // we have found a reference to data
           { 
               Isolate_l_dn(
                 Get_Disassembly_Node_From_Offset(ref, false), 
                 ref, bitn_o, NODE_STATUS_UNEXPLORED);
             
             Add_Reference(it, ref, true);
           }
           //if (ref2)
           //{ Add_Reference(it, ref2, true); }
         }
                                                                 
         if (finished)
         {
           Add_Uncertain_Address_To_Explore(it->memory_offset + n_bytes_used); 
         }

         it++;

         if (   (it->type != NODE_TYPE_CODE)
             || (it->status == NODE_STATUS_AUTO))
         { finished = true; }

      }       
      
      if (((opcode & 0xFF) != 0x66) && ((opcode & 0xFF)!= 0x67))
      {
       bitn_o = operand_bitness;    
       bitn_a = address_bitness;              

       if (
          ( (opcode & 0xFF) != 0xF0 ) && 
          ( (opcode & 0xFF) != 0xF2 ) &&
          ( (opcode & 0xFF) != 0xF3 )
         )
        { 
          prefix = 0;
          
          wait_flag = ((opcode & 0xFF) == 0x9B);
        }
       else
        { prefix = (opcode & 0xFF);}  
      }
      
      else if ((opcode & 0xFF) == 0x66) 
      { bitn_o = operand_bitness >> 1; }
      
      else //if (opcode == 0x67) 
      { bitn_a = address_bitness >> 1; }                              
    }

    return it->memory_offset;
}

int  dis::Disassembly_Intel::Phase_2a_Naive()
{
 int i;

 ////////////////////////////////////////////////////////////

 cout << "Naive_Approach: Disassembly_Intel!" << "\n";

 // start of executable has already been found in Phase_1_File(),
 // so  let's just start decoding
 i = Convert_Opcodes(v_to_explore);   

 return i;
}

int 
dis::Disassembly_Intel::Phase_2b_Platform_Specific()
{
 cout << "Phase 2 Platform_Specific: Disassembly_Intel!" << "\n";

 int i = 0;

 ///////////////////////////////////////////////////////

 // Phase_2a_Naive has begun a list of uncertain addresses,
 //  now add the stack frames
 Find_Stack_Frames();        
 

 while (   (v_to_explore_uncertain.size() > 0)
        || (v_to_explore.size() > 0))

 {     
   i = Convert_Opcodes(v_to_explore_uncertain);
   
   if (i == RET_OK)
   {
     i = Convert_Opcodes(v_to_explore);    
   }

 }  

 return i;
}

void  
dis::Disassembly_Intel::Find_Stack_Frames()
{

 list<Disassembly_Node>::iterator   it, it_sf;

 int                                i, j,       // indexes
                                    n,          // n_used
                                    current;    // current byte

 char                               *c;         // temp

 int                               sf[3] =     // stack frame
                                    {
                                     0x55,       // push ebp
                                     0x8B, 0xEC  // mov  ebp,esp
                                    };


 /////////////////////////////////////////////////////////////////
 /// intel stack frames usually indicate the start of code, so ///
 /// let's add these bytes to the list of uncertain nodes...   ///
 /////////////////////////////////////////////////////////////////

 it = l_dn.begin();
 i = j = 0 ;

 while (it != l_dn.end())
 {
   if ((it->status == NODE_STATUS_UNEXPLORED)
    && (it->type == NODE_TYPE_CODE))
   {
    n = it->n_used;
    c = it->opcode;

    for (current = 0; current < n; current++)    
    {
     if (*c == sf[i])
     {
      switch (i)
      {
       case 0: 
          { j = current; it_sf = it; i++; break;}
       case 1: 
          { i++; break;}
       case 2: 
       { 
        v_to_explore_uncertain.push_back(it_sf->memory_offset + j); 
        i = 0;
        break;
       }    
      }
     }
     else
     { i = 0; }

     c++;
    }
   }

   it++;
 }
}

void  
dis::Disassembly_Intel::Translate_Mnemonic(Instruction *i, string *str_instr)
{
    if  (!i->mnemonic)
    { 
     if (i->jump_table.jump_to == JUMP_TABLE_NOT_USED)
     {
        *str_instr  = "***???***"; return; 
     }
     
     switch (address_bitness)     
     {
          case DISASSEMBLY_BITNESS_32: 
            { *str_instr  = "dd loc_"; break; }
          case DISASSEMBLY_BITNESS_16: 
            { *str_instr  = "dw loc_"; break; }
          case DISASSEMBLY_BITNESS_08: 
            { *str_instr  = "db loc_"; break; }
     }                              

     *str_instr += u.int_to_hexstring(i->jump_table.jump_to); 
    }

    else
    {
     *str_instr  = i->mnemonic;
    }               
}


void  
dis::Disassembly_Intel::Translate_RegMemPart(RegMemPart *rmp, int use_override, string *str_instr)
{
    int             i,                  // immediate value
                    t,                  // temp value
                    r8, r16, r32, fpr;  // registers
    bool            a, ok;              // abs
    Displacement    *d;

    ////////////////////////////////////////////////////////////

      r8  = rmp->reg08;
      r16 = rmp->reg16;
      r32 = rmp->reg32;
      fpr = rmp->fp_reg;
      i   = rmp->imm;
      a   = rmp->abs;
      d   = rmp->displ; 

      ok = false;

      if (d)                                         // segment override
      {
       if (d->seg_override > 0)
       {
        *str_instr += regseg[d->seg_override];
        *str_instr += ":";
       }                  
      }

      if (a != true)
        {                      
          if (use_override > 0) 
          {
            *str_instr  += override[use_override];
            *str_instr  += " ";
          }

          *str_instr  += "[";  
        }

      if (r8 != 0)                                   // 8 bit register
      {
        *str_instr  += reg08[r8];
        ok = true;
      }
      
      if ((r16 != 0) && (ok == false))               // 16 bit register
      {
        *str_instr  += reg16[r16];
        ok = true;
      }

      if ((r32 != 0) && (ok == false))               // 32 bit register
      {
        *str_instr  += reg32[r32];
        ok = true;
      }

      if ((fpr != 0) && (ok == false))               // floating point registers
      {
        *str_instr  += regfpu[fpr - 1];
        ok = true;
      }                          

      if ((d != 0) && (d->seg_reg > 0))              // segment register
      {
        *str_instr += regseg[d->seg_reg];
        ok = true;
      }

      if (d)                                         // funny registers
      {
       if (d->contr_reg > 0)
       {
        *str_instr += regcontr[d->contr_reg];
        ok = true;
       }                  
       else if (d->debug_reg > 0)
       {
        *str_instr += regdebug[d->debug_reg];
        ok = true;
       }                  
       else if (d->test_reg > 0)
       {
        *str_instr += regtest[d->test_reg];
        ok = true;
       }                  
      }

      if (d && ok)
      {
       if (d->mul != 0)                              // mul to reg1
       {
         *str_instr += " * ";  
         *str_instr  += u.int_to_string(d->mul);
       }

       if (d->add != 0)                              // add to reg1
       {
         t = d->add;
         if (t > 0){*str_instr += " + ";}
         else {t = t * -1; *str_instr += " - ";}
         *str_instr  += u.int_to_string(t);
       }

       if (d->reg2 != 0)                             // register 2
       {
         *str_instr += " + ";  
         if (r8 != 0)
         { *str_instr  += reg08[d->reg2]; }
         else if (r16 != 0)
         { *str_instr  += reg16[d->reg2]; }
         else if (r32 != 0)
         { *str_instr  += reg32[d->reg2]; }
       }

       if (d->mul2 != 0)                              // mul to reg2
       {
         *str_instr += " * ";  
         *str_instr  += u.int_to_string(d->mul2);
       }
       
       if (d->add2 != 0)                              // add to reg2
       {
         t = d->add2;
         if (t > 0){*str_instr += " + ";}
         else {t = t * -1; *str_instr += " - ";}
         *str_instr  += u.int_to_string(t);
       }                       
      }                        

      if (ok == false)                               // immediate 
      {
          if  (a == true) 
          { *str_instr  += u.int_to_string(i);}
          else
          { *str_instr  += u.int_to_hexstring(i);}
          ok = true;
      }

      if (a != true)
      { *str_instr  += "]"; }
}

int 
dis::Disassembly_Intel::Use_Override(Instruction *i, int part)
{
  int           u;      // use override
  RegMemPart   *rmp;

  ///////////////////////////////////////////////////

  u = 0;

  switch (part)
  {
  case 1:
    rmp = &(i->part2);
    if ((   (rmp->imm > 0) 
         && (rmp->abs == true) 
         && (rmp->reg08 == 0) 
         && (rmp->reg16 == 0) 
         && (rmp->reg32 == 0))
        ||
        (   (i->operand_size != operand_bitness)
         && (i->part1.abs == false))) 
    {u = i->operand_size;}
    break;

  case 2:
    rmp = &(i->part1);
    if ((   (rmp->imm > 0) 
         && (rmp->abs == true) 
         && (rmp->reg08 == 0) 
         && (rmp->reg16 == 0) 
         && (rmp->reg32 == 0))
        ||
        (   (i->operand_size != operand_bitness)
         && (i->part2.abs == false))) 
    {u = i->operand_size;}
    break;
  }

  return u;
}


void
dis::Disassembly_Intel::Callback_Translate_Instruction(Instruction *instruction, string *str_instr)
{
    int             o;              // override operand_size
    
    ////////////////////////////////////////////////////////////

    o = instruction->operand_size;

    // mnemonic
    Translate_Mnemonic(instruction, str_instr);

    // import
    if (   (instruction->call)
        && (instruction->call->name))
    { *str_instr  += " "; *str_instr  += instruction->call->name; return; }

    // part 1
    if (instruction->part1.used == false) { return; }      

    *str_instr  += " ";                                 
    o = Use_Override(instruction, 1);
    Translate_RegMemPart(&(instruction->part1), o, str_instr); 
    
    // part 2
    if (instruction->part2.used == false) { return; }      
    
    *str_instr  += ", ";                                   
    o = Use_Override(instruction, 2);
    Translate_RegMemPart(&(instruction->part2), o, str_instr);

    // part 3
    if (   (instruction->part3 == 0) 
        || (instruction->part3->used == false)) { return; }
    *str_instr  += ", "; 
    Translate_RegMemPart(instruction->part3, o, str_instr);
}


