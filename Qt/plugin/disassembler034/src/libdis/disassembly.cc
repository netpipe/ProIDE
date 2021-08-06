/*
 * Disassembly.cpp: Routines for analysing a binary file
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

using namespace std;

#include "disassembly.hh" 
#include <cstring>
#include <algorithm>

dis::Disassembly::Disassembly() 
{
    cout << "Constructor Disassembly()" << "\n";
}

dis::Disassembly::Disassembly(string* file_name, 
                              dis::Main_Gui *mg,
                              dis::Disassembly_Options *options)

#ifdef LOGGING                
     : 
     debug_log("Disassembler")
#endif                        
{                  
    input_file = *file_name;

    gui_mg = mg;

    opt = options;

    Phase_0_Init();

    cout << "Constructor Disassembly (filename, mg, options)" << "\n";
}

dis::Disassembly::Disassembly(string* file_name, 
                              dis::Disassembly_Options *options) 

#ifdef LOGGING                
     : 
     debug_log("Disassembler")
#endif                        
{                  
    input_file = *file_name;

    opt = options;

    Phase_0_Init();

    cout << "Constructor Disassembly (filename, options)" << "\n";
}


dis::Disassembly::~Disassembly()
{
    cout << "Destructor Disassembly" << "\n";
}

int 
dis::Disassembly::Perform( )
{
 int i; 
         
 cout << "Perform: Disassembly!" << "\n";

 Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "reading input file...");
 i = Phase_1a_File();
 Gui_Command(GUI_COMMAND_STATUSBAR_POP);

 if (i != RET_OK)
 {   return i;    }

 completed_phase = DISASSEMBLY_PHASE_1A;

 cout << "Perform: Disassembly!!" << "\n";

 /*
 //velsd: more trouble than it's worth... gtk isn't thread safe:
 //http://www.gtkmm.org/gtkmm2/docs/FAQ/html/index.html#id2805622

velsd: however, also see:
http://libsigcx.sourceforge.net/docs/sigcx_starting.html
                                 
 i = Perform_Background();

 if (i != RET_OK)
 {   return i;    }
 
 //velsd : let's simply put the whole analysis in the main process instead...
 */
 Phases_In_Thread();
  
 cout << "Perform: Disassembly!!!" << "\n";


 cout << u.time_to_string() << "\n";

 //if (gui_init == false)
 //{ Gui_Command(GUI_COMMAND_INIT); }

 Update_Short_Cut_List(true); 

 return RET_OK;
}

void
dis::Disassembly::Phase_0_Init()
{
    disassembly_type = DISASSEMBLY_TYPE_GENERIC;

    input_file_size = 0;                        

    completed_phase = DISASSEMBLY_PHASE_NONE;

    type_of_gui = opt->type_of_gui;

    phases_completed = gui_init = false;

    bookmark_ldn = l_dn.begin();
    bookmark_lsc = l_sc.begin();
    bookmark_lr  = l_r.begin();

    debug_counter = 0;

    l_dn_modified_counter = 0;

    known_imports_file_name = ""; 

    Initialize(&e);                         
}


int  
dis::Disassembly::Phase_1a_File( )
{
 cout << "Phase_1a_File: Disassembly!" << "\n";

 return RET_OK;
}

int
dis::Disassembly::Phase_1b_Imports( )
{
    string                  s, t, lf;             // string, temp, last file

    vector<string>          v;

    uint                    i, j;                 // indexes  

    bool                    stop, found;

    vector<Api>::iterator   it_a; 

    Api                     a;

    Routine                *r, *prev_r;           // routine, previous routine

    Parameter              *p, *prev_p;           // parameter, previous parameter


    ///////////////////////////////////////////////////////////////////////

    cout << "Phase_1b_Imports: Disassembly! from " << known_imports_file_name.c_str() << "\n";

    if (known_imports_file_name.c_str() == "")
    {
     cout << "Disassembly: no imports file specified!" << "\n";
     return RET_ERR_FILE_INPUT;
    }                                                     

    ifstream imports_file(known_imports_file_name.c_str());

    if (!imports_file)
    {
     cout << "Disassembly: no imports file found!" << "\n";
     return RET_ERR_FILE_INPUT;
    }    

    prev_r = 0;

    while (getline(imports_file, s))
    {
     u.parse_string(&s, &v);

     if (v.size() < 3 )                    // file, routine, type 
     { continue; }

     t = v[0]; v.erase(v.begin());         // name of file

     if (t != lf)                          // if new file 
     {
      lf = t;

      i = 0;          
      found = false;  
      j = v_api.size();
      stop = (j == 0);
      it_a = v_api.begin();

      while (stop == false)
      {
       if (it_a->file_name == t)
       { found= true; stop = true;}
       else
       {
        i++; it_a ++;
        if ((i >= j) || (it_a->file_name > t))
        { stop = true;}
       }
      } 

      if (!found)
      {
       Initialize(&a); 
       a.file_name = t;
       it_a = v_api.insert(it_a, a);

       prev_r = 0;
      }
      else 
      {
       prev_r = it_a->routine; 
       while (prev_r != 0) {prev_r = prev_r->next; }        
      }
     } 

     r = (Routine*) mp.Use_Pool(sizeof(Routine)); 
     Initialize(r);

     t = v[0]; v.erase(v.begin());         // name of routine
     r->name = (char*) mp.Use_Pool(t.size() + 1);  
     strcpy(r->name, t.c_str());        

     stop = false;
     prev_p = 0;
     while ((v.size() > 0) && (stop != true))  // output parameters first
     {                                    
      t = v[0]; v.erase(v.begin());        

      if (t == "input") {stop = true; }
      else if (t != "void")
      {
       p = (Parameter*) mp.Use_Pool(sizeof(Parameter));  
       Initialize(p);

       if (prev_p == 0)
       { r->output = p; }
       else 
       { prev_p->next = p; }
       prev_p = p; 

       p->type = Get_Function_Type(&t);
       if (p->type == FUNCTION_TYPE_UNKNOWN)
       {
        p->type_name = (char*) mp.Use_Pool(t.size() + 1);   // copy the name of unknown types...    
        strcpy(p->type_name, t.c_str());
       }

       t = v[0]; v.erase(v.begin());         // next parameter name          
       if (t != "-")
       {
        p->name = (char*) mp.Use_Pool(t.size() + 1);
        strcpy(p->name, t.c_str());                                           
       }
      }
      else // "void"
      if (v.size() > 0) {v.erase(v.begin());}   // delete corresponding "-"
     }

     // input is next
     prev_p = 0;    
     while (v.size() > 0)
     {
      p = (Parameter*) mp.Use_Pool(sizeof(Parameter));  
      Initialize(p);

      if (prev_p == 0)
      { r->input = p; }
      else 
      { prev_p->next = p; }
      prev_p = p; 

      t = v[0]; v.erase(v.begin());         // next parameter type          
      p->type = Get_Function_Type(&t);
      if (p->type == FUNCTION_TYPE_UNKNOWN)
      {
       p->type_name = (char*) mp.Use_Pool(t.size() + 1);  // copy the name of unknown types...        
       strcpy(p->type_name, t.c_str());
      }

      if (v.size() == 0) { continue; }                   // shouldn't happen = missing parameter or name ...

      t = v[0]; v.erase(v.begin());         // next parameter name          
      p->name = (char*) mp.Use_Pool(t.size() + 1);
      strcpy(p->name, t.c_str());                                           

     }                                 

     if (prev_r == 0) {it_a->routine = r; }
     else {prev_r->next = r; }
     prev_r = r;
    }
     
    cout << "imports finished, found " << v_api.size() << " imports " << "\n";

    return RET_OK;
}


void *Start_Phase_2(void *_disassembly)
{

  cout << "Start_Phase_2!" << "\n";

  dis::Disassembly *d = (dis::Disassembly *) _disassembly;
  
  cout << "Start_Phase_2!!" << "\n";
  
  d->Phases_In_Thread();

  return 0;
}


void 
dis::Disassembly::Phases_In_Thread( )
{
 cout << "Phases_In_Thread: Disassembly!" << "\n";

 // for faster navigation in l_dn
 Update_Short_Cut_List(true);                                                  


 //////////////////////////////////////////////////////////////////////////////
 // phase 1 collects static data
 //////////////////////////////////////////////////////////////////////////////

 Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "reading known imports");
 Phase_1b_Imports();                                              
 Gui_Command(GUI_COMMAND_STATUSBAR_POP);

 completed_phase = DISASSEMBLY_PHASE_1B;


 //////////////////////////////////////////////////////////////////////////////
 // phase 2 decodes the opcodes
 //////////////////////////////////////////////////////////////////////////////

 Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "performing brute force approach...");
 Phase_2a_Naive();
 Gui_Command(GUI_COMMAND_STATUSBAR_POP);

 completed_phase = DISASSEMBLY_PHASE_2A;

 cout << u.time_to_string() << " , " << l_dn.size() << " , " << l_sc.size() << "\n";

 Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "performing platform specific approach...");
 Phase_2b_Platform_Specific();
 Gui_Command(GUI_COMMAND_STATUSBAR_POP);

 completed_phase = DISASSEMBLY_PHASE_2B;

 cout << u.time_to_string() << " , " << l_dn.size() << " , " << l_sc.size() << "\n";


 //////////////////////////////////////////////////////////////////////////////
 // phase 3 manages the resulting disassembly nodes 
 //////////////////////////////////////////////////////////////////////////////

 if (completed_phase < DISASSEMBLY_PHASE_3A)
 {
  Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "reviewing data section...");
  Phase_3a_Review_Data();
  Gui_Command(GUI_COMMAND_STATUSBAR_POP);

  completed_phase = DISASSEMBLY_PHASE_3A;

  cout << u.time_to_string() << " , " << l_dn.size() << " , " << l_sc.size() << "\n";
 }                                                                                   

 if (completed_phase < DISASSEMBLY_PHASE_3B)
 {
  Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "reviewing references to code and data...");
  Phase_3b_Review_References();
  Gui_Command(GUI_COMMAND_STATUSBAR_POP);

  completed_phase = DISASSEMBLY_PHASE_3B;

  cout << u.time_to_string() << " , " << l_dn.size() << " , " << l_sc.size() << "\n";
 }

 if (completed_phase < DISASSEMBLY_PHASE_3C)
 {
  Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "reviewing imported functions...");
  Phase_3c_Review_Imports();
  Gui_Command(GUI_COMMAND_STATUSBAR_POP);

  completed_phase = DISASSEMBLY_PHASE_3C;

  cout << u.time_to_string() << " , " << l_dn.size() << " , " << l_sc.size() << "\n";
 }

 if (completed_phase < DISASSEMBLY_PHASE_3D)
 {
  Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "reviewing functions...");
  Phase_3d_Review_Functions();
  Gui_Command(GUI_COMMAND_STATUSBAR_POP);

  completed_phase = DISASSEMBLY_PHASE_3D;

  cout << "End Phases_In_Thread = " 
       << u.time_to_string() << " , " << l_dn.size() << " , " << l_sc.size() << "\n";
 }

 if (completed_phase < DISASSEMBLY_PHASE_3E)
 {
  Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "reviewing variables...");
  Phase_3e_Review_Variables();
  Gui_Command(GUI_COMMAND_STATUSBAR_POP);

  completed_phase = DISASSEMBLY_PHASE_3E;

  cout << "End Phases_In_Thread = " 
       << u.time_to_string() << " , " << l_dn.size() << " , " << l_sc.size() << "\n";
 }

 if (completed_phase < DISASSEMBLY_PHASE_3F)
 {
  Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "reviewing parameters to known functions...");
  Phase_3f_Review_Parameters();
  Gui_Command(GUI_COMMAND_STATUSBAR_POP);

  completed_phase = DISASSEMBLY_PHASE_3F;

  cout << "End Phases_In_Thread = " 
       << u.time_to_string() << " , " << l_dn.size() << " , " << l_sc.size() << "\n";
 }                                                                                   

 phases_completed = true;
}                                                  

int 
dis::Disassembly::Phase_2a_Naive()
{
 cout << "Phase 2 Naive: Disassembly!" << "\n";

 return RET_OK;
}

int 
dis::Disassembly::Phase_2b_Platform_Specific()
{
 cout << "Phase 2 Platform_Specific: Disassembly!" << "\n";

 return RET_OK;
}

int 
dis::Disassembly::Phase_3a_Review_Data()
{
 list<Disassembly_Node>::iterator   it;

 Next_String                        ns;      // info about next printable string

 int                                i;       // index   

 ///////////////////////////////////////////////////////////////////////////
 ///  try and collect strings in the data sections  ////////////////////////
 ///////////////////////////////////////////////////////////////////////////

 cout << "Phase_3a_Review_Data" << "\n";

 if (opt->collect_strings != true)
 { return RET_ERR_OPTION; }

 it = l_dn.begin(); 
      
 while (it != l_dn.end())
 {
  if (   (it->type == NODE_TYPE_DATA)
      && (it->status == NODE_STATUS_UNEXPLORED))
  {
    ns = Get_Next_Printable_String(it, 0);

    while ((it->n_used <= ns.offset) && (it != l_dn.end()))
    { ns.offset -= it->n_used; it++;}

    if (ns.length >= GUI_LABEL_MIN)
    {                     
     Isolate_l_dn(it , it->memory_offset + ns.offset, ns.length, NODE_STATUS_UNEXPLORED);
    } 
    else 
    {                     
     i =
         // ns.offset + ns.length + 
         ns.offset_next - it->memory_offset
         // - 1
         ;

     if (i < it->n_used)
     {
      Isolate_l_dn(it , it->memory_offset + i , it->n_used - i
                   // + 1
                   , NODE_STATUS_UNEXPLORED);         
     }                    
    }               

    if (it->memory_offset + it->n_used >= ns.offset_next) 
     { it++; }
    else
    {
     while (   (it != l_dn.end())
            && (it->n_used + it->memory_offset < ns.offset_next)) 
     { it++;}
    }                                                     
  }
  else
  { it++ ; }              
 }

 return RET_OK;                                                
}

int 
dis::Disassembly::Phase_3b_Review_References()
{
 list<Reference>::iterator          it_r;

 list<Disassembly_Node>::iterator   it_n;

 bool                               remove;  // whether or not to remove this reference
 
 int                                i,       // index
                                    s;       // size

 ////////////////////////////////////////////////

 cout << "Phase 3 Review References: Disassembly!" << "\n";

 it_r = l_r.begin();

 while (it_r != l_r.end())
 {
  s = it_r->ref_out.size();     // how many nodes refer to this location?

  // first check if memory location really exists...
  remove = ((it_r->memory_offset < range_offset_low) || 
            (it_r->memory_offset > range_offset_high));  

  // then see if we can find the Disassembly_Node for this memory location
  {it_n = Get_Disassembly_Node_From_Offset(it_r->memory_offset, true);}

  it_r->it_dn = it_n;
  
  if (it_n != l_dn.end())
  {
      it_n->ref_in = it_r;
  }                                        
    
  // and with this update the references 'out'
  for (i = 0; i < s; i++)
  {
    it_n = Get_Disassembly_Node_From_Offset(it_r->ref_out[i], true);

    if (it_n == l_dn.end()) 
      { continue; }

    if (remove)
    { it_n->ref_out = l_r.end();}
  }

  it_r++;                        
 }                                                       

 cout << "Phase 3 Review References: Disassembly : done!" << "\n";
   
 return RET_OK;                                                
}

int 
dis::Disassembly::Phase_3c_Review_Imports()
{
 list<Import>::iterator           it_i;
 
 list<Reference>::iterator        it_r;
 
 list<Disassembly_Node>::iterator it_n;

 int                              i , n;    // index, number of referencing nodes

 //////////////////////////////////////////////////////////////////////////
 //  l_i is the list of imported functions we found in the executable    //
 //  (in Phase_1a_File); run through this list, comparing each           //
 //  entry with a list of known system routines for this platform        //
 //                                                                      //
 // if found: update this node with Update_Disassembly_Node_For_Import   //
 //////////////////////////////////////////////////////////////////////////

 cout << "Phase 3 Review Imports: Disassembly!" << "\n";

 it_i = l_i.begin();

 while (it_i != l_i.end())
 {
  it_i->routine = Get_Routine_From_Name( it_i->library.c_str(), it_i->name.c_str() );

  it_r = Get_Reference_From_Offset(it_i->memory_offset, true);

  if (it_r != l_r.end())
  {
   n = it_r->ref_out.size();                       // nodes that call this function

   for (i = 0; i < n; i++)
   {
    it_n = Get_Disassembly_Node_From_Offset(it_r->ref_out[i], true);       

    if (it_n == l_dn.end())
    { continue; }

    Update_Disassembly_Node_For_Import(it_i, it_n, true);
   }
  }

  it_i++;
 }

 cout << "Phase 3 Review Imports: Disassembly : done!" << "\n";

 return RET_OK;                                                
}

int
dis::Disassembly::Phase_3d_Review_Functions()
{
 list<Function>::iterator         it_f;

 list<Reference>::iterator        it_r;

 list<Disassembly_Node>::iterator it_n;

 int                              i , n;    // index, number of referencing nodes

 //////////////////////////////////////////////////////////////////////////
 //  l_f is the list of functions we found in the executable             //
 //  (in Phase_1a_File); run through this list, updating each            //
 //  node that references the function                                   //
 //////////////////////////////////////////////////////////////////////////

 cout << "Phase 3 Review Functions: Disassembly!" << "\n";

 it_f = l_f.begin();


 while (it_f!= l_f.end())
 {
  it_r = Get_Reference_From_Offset(it_f->memory_offset, true);

  if (it_r != l_r.end())
  {
   n = it_r->ref_out.size();                 // nodes that call this function

   for (i = 0; i < n; i++)
   {
    it_n = Get_Disassembly_Node_From_Offset(it_r->ref_out[i], true);       

    if (it_n == l_dn.end())
    { continue; }

    Update_Disassembly_Node_For_Function(it_f, it_n);
   }
  }

  it_f++;
 }

 cout << "Phase 3 Review Imports: Disassembly : done!" << "\n";

 return RET_OK;                                                
}

int
dis::Disassembly::Phase_3e_Review_Variables()
{

 return RET_OK;                                                
}      

int 
dis::Disassembly::Phase_3f_Review_Parameters()
{
 int                              i, n,    // index, number of referencing nodes
                                  d, l;    // delimiter, length

 list<Reference>::iterator        it_r;

 list<Disassembly_Node>::iterator it_n;

 string                           s; 

 Parameter                       *p;

 Call                            *c;

 Routine                         *r;  

 Instruction                      instr;  

 bool                             stop; 

 ////////////////////////////////////////////////////////////

 cout << "Phase 3 Review Parameters: Disassembly!" << "\n";
 
 it_r = l_r.begin();

 while (it_r != l_r.end())
 {
  // first, find parameters to imports
  if (it_r->type == REFERENCE_TYPE_IMPORT)
  {
   n = it_r->ref_out.size();

   for (i = 0; i < n; i++)
   {
    it_n = Get_Disassembly_Node_From_Offset(it_r->ref_out[i], true);       

    if (it_n == l_dn.end()) { continue; }

    c = it_n->instruction.call;
    if (c == 0)             { continue; }

    r = c->routine; 
    if (r == 0)             { continue; }

    p = r->input;
    if (p == 0)             { continue; }

    stop = false;
    while (stop == false)
    {
     it_n--;                                        
     if (it_n == l_dn.begin()) {stop = true; break; }

     instr = it_n->instruction;
     d = instr.delimiter;
     if (d == INSTRUCTION_DELIMITER_BEGIN) {stop = true;}
     else if (   (d == INSTRUCTION_DELIMITER_END)
              || (instr.call != 0)) {stop = true; break;}

     if (instr.n_pushed > 0)
     {
      if ((p->type_name == 0) && (p->type > 0))         // type of parameter was recognised
      {
       s = Get_Function_Type(p->type);
       l = strlen(p->name) + 1 + s.size() + 1;
       it_n->label = (char*) mp.Use_Pool(l);       
       strcpy(it_n->label, p->name);
       strcat(it_n->label, " ");
       strcat(it_n->label, s.c_str());

       /*
       // if a string type was pushed, then collect its bytes in 1 node
       
       if (   (it->type == NODE_TYPE_DATA)
           && (it->status == NODE_STATUS_UNEXPLORED))
       {
         ns = Get_Next_Printable_String(it, 0);

         while ((it->n_used <= ns.offset) && (it != l_dn.end()))
         { ns.offset -= it->n_used; it++;}

         if (ns.length >= GUI_LABEL_MIN)
         {                     
          Isolate_l_dn(it , it->memory_offset + ns.offset, ns.length, NODE_STATUS_UNEXPLORED);
         } 
         else 
         {                     
          i = ns.offset + ns.length + ns.offset_next - it->memory_offset - 1;

          if (i < it->n_used)
          {
           Isolate_l_dn(it , it->memory_offset + i , it->n_used - i + 1, NODE_STATUS_UNEXPLORED);         
          }                    
         }                     
       */                  


      }
      else 
      {
       l = strlen(p->name) + 1 + strlen(p->type_name) + 1;
       it_n->label = (char*) mp.Use_Pool(l);       
       strcpy(it_n->label, p->name);
       strcat(it_n->label, " ");
       strcat(it_n->label, p->type_name);
      }

      p = p->next;
      if (p == 0) {stop = true; }
     }
    }                                                  
   }
  }

  it_r++;
 }

 cout << "Phase 3 Review Parameters: Disassembly : done!" << "\n";

 return RET_OK;
}                 

void
dis::Disassembly::Add_Extra_Info_Level_2(char *title1, char *extra_info, Extra *extra)
{                                                   
    int                         l;      // length

    bool                        stop;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////  adds a level 2 line to a perhaps already existing level 1 line ////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////



    while ((extra->next) && (extra->description) && (strcmp(extra->description, title1) != 0))
    {extra = extra->next;}

    l = strlen(title1);

    if (extra->description == 0)
    {
     extra->description = (char *) mp.Use_Pool(l + 1);
     strncpy(extra->description, title1, l + 1);    
    }
    else if (strcmp(extra->description, title1) != 0)
    {
     extra->next = (Extra *) mp.Use_Pool(sizeof(Extra));
     extra = extra->next;
     Initialize(extra);
    }

    if (extra->next_level == 0)
    {
     extra->next_level = (Extra *) mp.Use_Pool(sizeof(Extra));
     extra = extra->next_level;
     Initialize(extra);
    }
    else 
    {
     extra = extra->next_level;

     stop = false;
     while (!stop)
     { 
      if ((extra->description) && (strcmp(extra->description, extra_info) == 0))
      { return; }

      if (extra->next != 0)
      { extra = extra->next; }
      else
      { stop = true; }
     }

     extra->next = (Extra *) mp.Use_Pool(sizeof(Extra));
     extra = extra->next;
     Initialize(extra);
    }

    l = strlen(extra_info);

    extra->description = (char *) mp.Use_Pool(l + 1);
    strncpy(extra->description, extra_info, l + 1);     
}

void
dis::Disassembly::Add_Extra_Info_Level_3(char *title1, char *title2, char *extra_info, Extra *extra)
{                                                   
    int                         l;      // length

    bool                        stop;

    //////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////  adds a level 3 line to an already existing level 2 line ////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    while ((extra->next) && (extra->description) && (strcmp(extra->description, title1) != 0))
    {extra = extra->next;}

    l  = strlen(title1);

    if (   (extra->description == 0)
        || (strcmp(extra->description, title1) != 0)
        || (extra->next_level == 0)
       )
    { return; }                                      // node not found, so abort

    extra = extra->next_level;

    l  = strlen(title2);
    stop = false;

    while (!stop)
    { 
      if ((extra->description) && (strncmp(extra->description, title2, l) == 0))
      { stop = true; }
      else 
      {
       if (extra->next != 0) { extra = extra->next; }
       else                  { return; }             // node not found, so abort
      }
    }

    extra->next_level = (Extra *) mp.Use_Pool(sizeof(Extra));
    extra = extra->next_level;
    Initialize(extra);

    l = strlen(extra_info);

    extra->description = (char *) mp.Use_Pool(l + 1);
    strncpy(extra->description, extra_info, l + 1);     
}

int 
dis::Disassembly::Convert_Opcodes(std::vector<int> &v)
{
 cout << "Convert_Opcodes: Disassembly!" << "\n";

 return RET_OK;
}                 

void
dis::Disassembly::Convert_Jump_Tables()
{

 int        jump,                       // offset to possible jump table
            target,                     // next entry of the jump table
            previous_target,            // previous entry of the jump table
            redo,                       // where the disassembly will rebegin
            i;                          // index

 char       param[DISASSEMBLY_MAX_INSTRUCTION_LENGTH];  // temp value

 string     temp;                       // temp value

 list<Disassembly_Node>::iterator   it_j, it_t;

 bool       undone,                     // had to undo a previous disassembly
            jt;                         // found a jump table

 Jump_Table_Request jtr;

 /////////////////////////////////////////////////////

 undone = false;

 while (v_jump_table.size() > 0)
 {
  jtr = v_jump_table.back();
  v_jump_table.pop_back();

  jump = jtr.jump_to;

  previous_target = 0;

  it_t = it_j = Get_Disassembly_Node_From_Offset(jump, false);

  if (it_j == l_dn.end())
  { continue; }

  undone = (it_j->status != NODE_STATUS_UNEXPLORED);

  Undo_From_Offset(it_j->memory_offset, jtr.jump_from, DISASSEMBLY_COND_JUMP_TABLE);

  i = it_j->memory_offset;
  Isolate_l_dn(it_j, i, jump - i, NODE_STATUS_UNEXPLORED);

  if (jump > i)
  { it_j ++; }
  
  redo = it_j->memory_offset;
  jt = true;

  while (jt)
  {                           
   for (i=0; i < address_bitness; i++)                  //  DISASSEMBLY_BITNESS_32 = 4
   {
    if (Get_Byte_From_Disassembly_Node(it_j, i, &param[i]) == RET_ERR_OPCODE)
        { jt = false; }
   }

   Isolate_l_dn(it_j, it_j->memory_offset, address_bitness, NODE_STATUS_UNEXPLORED);

   target = (*(int*) &param);

   if (jt)
   { 
    i = (target - previous_target);
    jt = (    (target >= range_offset_low)             // target is legal adress
           && (target <= range_offset_high))
           && (   (i == target)                        // target is next in a series of adresses
               || (i == 0)
               || ((i < 0) && (i > DISASSEMBLY_MAX_DIFF_JUMP_TABLE * (-1)))
               || ((i > 0) && (i < DISASSEMBLY_MAX_DIFF_JUMP_TABLE)))
           ;  
   }

   //////////////////////////////////////////////////////////////////////////////////////////

   if (jt)                                                 // define the jump table entry
   {
     it_j->status = NODE_STATUS_AUTO;    
     it_j->instruction.jump_table.jump_to = target;

     temp = "jump_table_" +  u.int_to_hexstring(it_j->memory_offset); 

     it_j->label = (char *) mp.Ensure_Minimum_Allocation(it_j->label, temp.size());

     strcpy(it_j->label, temp.c_str());      

     it_j->instruction.jump_table.jump_to = target;

     Add_Reference(it_j, target, true);

   //////////////////////////////////////////////////////////////////////////////////////////   
                                 
     target = it_j->memory_offset + it_j->n_used;

     it_j++;
     redo = it_j->memory_offset;                          // jump table must be continuous
     jt = (target == redo);
   }
  } 

  if (undone)
  { v_to_explore.push_back(redo); }       
 }                                        
}

int
dis::Disassembly::Undo_From_Offset(int o, int max, int condition)
{
  list<Disassembly_Node>::iterator      it_o;

  int                                   current, next;      // offsets of the it_o

  ///////////////////////////////////////////////////////
  ///  start from offset o,  and undo all disassembly ///
  ///   up to offset max                              ///
  ///  until condition is met                         ///
  ///   condition can be:                             ///
  ///       0: no condition                           ///
  ///       1: clear for jump_tables                  ///
  ///////////////////////////////////////////////////////


  current = 0;

  if (max < o)
  { max = range_offset_high; }

  it_o = Get_Disassembly_Node_From_Offset(o, false);

  if (it_o == l_dn.end())
  { return 0; }

  while (it_o != l_dn.end())
  {
    next = it_o->memory_offset;

    if (
        (   (current != 0)                                       // gap between two offsets
         && (current != next))
        ||
        (it_o->status == NODE_STATUS_UNEXPLORED)                 // nothing to undo
        ||
        (it_o->memory_offset >= max)                             // max offset to undo reached
        ||
        (   (condition == DISASSEMBLY_COND_JUMP_TABLE )
         && (it_o->instruction.jump_table.jump_to != JUMP_TABLE_NOT_USED)) // node already known as jumptable
       )
    { return it_o->memory_offset; }

    current = it_o->memory_offset + it_o->n_used;

    it_o->status = NODE_STATUS_UNEXPLORED;                       // undo disassembly
    it_o->comment = 0;
    it_o->label = 0;
    Initialize(&(it_o->instruction));
    it_o->ref_in = it_o->ref_out = l_r.end();   

    it_o++;
  }

  return 0;
}

/*
int   
dis::Disassembly::Perform_Background( )
{
 cout << "Perform_Background: Disassembly!" << "\n";

 
 return pthread_create( &thr_dis, 0, Start_Phase_2, this);

 }

*/

bool
dis::Disassembly::Isolate_l_dn(list<Disassembly_Node>::iterator it, int pos, int length, int status)
{
    int                     i, j;                             // indexes

    ///////////////////////////////////////////////////////////////////////////
    //// for when we know where a Disassembly_Node begins              ////////
    //// within an existing iterator, and we want to isolate it into   ////////
    //// its own node                                                  ////////
    ///////////////////////////////////////////////////////////////////////////
    //// ensure that existing iterator 'it' is cut off at position     ////////
    //// 'pos' and that from there on the next iterator will contain   ////////
    ////  the next 'length' bytes                                      ////////
    ///////////////////////////////////////////////////////////////////////////


    if (it->status != NODE_STATUS_UNEXPLORED)
    {return false;}

    pos = pos - it->memory_offset;
    i = it->n_used;

    if ((pos < 0) || (pos > i - 1) || (length <= 0) ||
        (i == 0) || (it == l_dn.end()) || (it->status != NODE_STATUS_UNEXPLORED))
    { return false; }                                        

    if (pos > 0) 
    { 
     Split_l_dn(it, pos, true); 
     it++;
    }
    
    i = it->n_used;

    if (length < i)
    { Split_l_dn(it, length, true); }
    else
    {
      i = length - i;

      for (j = 0; j < i; j++)
      { 
          if (Get_Byte_From_Disassembly_Node(it, it->n_used, 0) != RET_OK)
          { return false; }

//          it->n_used++;
      }                      
    }

    if (status != NODE_STATUS_UNEXPLORED)
     { it->status = status; }

    return true;
}

void
dis::Disassembly::Split_l_dn(list<Disassembly_Node>::iterator it, int diff, bool keep_status)
{
    Disassembly_Node dn;    // the second part after the split

    ////////////////////////////////////////////////////////////////////
    //// splits the Disassembly_Node at the position indicated  ////////
    ////////////////////////////////////////////////////////////////////

    Initialize (&dn);

    dn.file_offset = it->file_offset + diff;
    dn.memory_offset = it->memory_offset + diff;
    dn.type = it->type;
    dn.section = it->section;

    if (keep_status == true)
    { dn.status = it->status; }
    else
    { dn.status = NODE_STATUS_UNEXPLORED;}

    dn.opcode = it->opcode + diff; 
    dn.n_used = it->n_used - diff;   

    it->n_used = diff;                           
    it ++;

    l_dn.insert(it, dn);

    l_dn_modified_counter++;

    Update_Short_Cut_List(false);
}

void
dis::Disassembly::Update_Short_Cut_List(bool force_update)                 
{
    int                                 i, j,   // indexes
                                        s;      // size

    Short_Cut                           sh;         

    list<Disassembly_Node>::iterator    it;     // for iterating l_dn

    //////////////////////////////////////////////////////////////////////
    //  read the l_dn and update indexed list of entries                //
    //////////////////////////////////////////////////////////////////////

    if ((!force_update) && (l_dn_modified_counter < DISASSEMBLY_MODIFIED_COUNTER_MAX))
    { return ;}

    l_dn_modified_counter = 0;

    s = l_dn.size() - 1;
    j = (s / DISASSEMBLY_SHORT_CUT_MAX) + 1;
    if (j < DISASSEMBLY_SHORT_CUT_MIN_SIZE) {j = DISASSEMBLY_SHORT_CUT_MIN_SIZE; }

    i = 0;          
    l_sc.clear();
    it = l_dn.begin();

    while (i < s)                      // build a list of DISASSEMBLY_SHORT_CUT_MAX indexes to entries in l_dn
    {
     i++;                              

     if (   (i % j == 1)
         || (i >= s))                  // always include last l_dn, for easy end of list comparison
     {                                               
      Initialize(&sh);

      sh.file_offset   = it->file_offset;
      sh.memory_offset = it->memory_offset;
      sh.n_used        = it->n_used;
      sh.row_number    = i - 1;
      sh.it_s          = it;

      l_sc.push_back(sh);

      it->bookmarked = true;               
     }
     else
     {
      it->bookmarked = false;        
     }

     it++;
    }     

    bookmark_lsc = l_sc.begin();

    cout << "Update_Short_Cut_List: l_sc.size() = " << l_sc.size() << "\n";
}

int   
dis::Disassembly::Get_Byte_From_Disassembly_Node(list<Disassembly_Node>::iterator it, short pos, char *byte)
{
    
    int                              i;
    list<Disassembly_Node>::iterator t;

    ///////////////////////////////////////////////////////////////////////////
    /// copies one byte to *byte; this byte can be at most one position  //////
    /// beyond the current iterator, in which case it is transferred     //////
    /// to this iterator ( and erased from the next one); the status of  //////
    /// the nodes involved has to be NODE_STATUS_UNEXPLORED              //////
    ///////////////////////////////////////////////////////////////////////////
    /// there is no actual copying of the transferred byte, however,     //////
    /// since the opcodes are realley in a contigous area of memory, and //////
    /// the nodes just contain a pointer into this memory area, and not  //////
    /// the actual opcode bytes                                          //////
    ///////////////////////////////////////////////////////////////////////////



    if (pos < it->n_used)                                             // simplest case 
    { 
      if (byte != 0)
       { strncpy(byte, &(it->opcode[pos]), 1); }
      return RET_OK;
    }

    i = it->memory_offset + it->n_used;
    t = it++;                                                          // it = t->next

    if ((it == l_dn.end()) ||
        (it->memory_offset != i) ||
        (it->status != NODE_STATUS_UNEXPLORED) ||
        (it->n_used == 0))                                             // no unused bytes available
    {
      return RET_ERR_OPCODE; 
    }               
                                                                       
    // first we transfer one byte from the next Disassembly_Node 
    t->n_used++;                                                                           

    // next, we update the next node
    it->n_used--;
    it->opcode++;
    it->file_offset++;
    it->memory_offset++;

    if (it->n_used == 0)
    { 
      Update_Ldn_For_Delete(&(*it));
      l_dn.erase(it); 
    }

    if (byte != 0)
    { strncpy(byte, &(t->opcode[pos]),1); }

    return RET_OK;
}

dis::Section*
dis::Disassembly::Get_Section_From_Offset(int pos)
{
    int                     i, n;                  // index, number of sections present in file

    ////////////////////////////////////////////////////////

    n = v_s.size();

    i = 0;

    while (i < n)
    {
     if (   (v_s[i].offset <= pos)
         && (v_s[i].size > (pos - v_s[i].offset))
        )
     { return &(v_s[i]); }
     else 
     { i++; }
    }        

    return 0;
}

list<dis::Disassembly_Node>::iterator                                   // find the DisassemblyNode
dis::Disassembly::Get_Disassembly_Node_From_Offset(int pos,             //  at this position
                                                   bool exact_match)    //  offset of node has to match pos exactly
{
    int                     b;                  // memory offset for bookmark

    ////////////////////////////////////////////////////////

    if ((pos < range_offset_low) || (pos > range_offset_high))
        {return l_dn.end();}

    if (l_sc.size() > 0)
    {
     if (bookmark_lsc == l_sc.end())
     { 
     bookmark_lsc--; }

     b = bookmark_lsc->memory_offset;

     // first, find the Short_Cut closest to the offset
     if (pos > b)                                                            // higher in memory
     {
       while (  (bookmark_lsc != l_sc.end())
             && (bookmark_lsc->memory_offset + bookmark_lsc->n_used <= pos))
       {
         bookmark_lsc++; 
       }      
     }
     else                                                                   // lower in memory
       while (  (bookmark_lsc->memory_offset > pos) 
             && (bookmark_lsc != l_sc.begin()))
       { 
         bookmark_lsc--; 
       }               
     if (bookmark_lsc == l_sc.end())
     { bookmark_lsc--; }

     bookmark_ldn = bookmark_lsc->it_s;
    }
    
    // then begin a narrower search for the correct node
    if (bookmark_ldn == l_dn.end())                     
    { bookmark_ldn = l_dn.begin(); }

    b = bookmark_ldn->memory_offset;

    // find the Disassembly_Node that contains the offset
    if (pos > b)                                                            // higher in memory
     {
       while (  (bookmark_ldn != l_dn.end())
             && (bookmark_ldn->memory_offset + bookmark_ldn->n_used <= pos))
       {
         bookmark_ldn++;
       }      
     }
    else                                                                   // lower in memory
       while (  (bookmark_ldn->memory_offset > pos) 
             && (bookmark_ldn != l_dn.begin()))
       { bookmark_ldn--; }               

    if (bookmark_ldn == l_dn.end())
    { return bookmark_ldn; }  

    if ((exact_match == true) && (bookmark_ldn->memory_offset != pos))
    { return l_dn.end(); }    

    { return bookmark_ldn; }
}

list<dis::Disassembly_Node>::iterator                                   // find the DisassemblyNode
dis::Disassembly::Get_Disassembly_Node_From_Row(uint row)                //  at this row number
{
    uint                     r;                  // row number for bookmark

    ////////////////////////////////////////////////////////

    if (row > l_dn.size()) 
        {return l_dn.end();}
                                                // velsd:
    if (l_dn_modified_counter > 0)              // this shouldn't happen often, since only the gui 
    { Update_Short_Cut_List(true); }            // will call this function... it would be a real performance 
                                                // buster if called during initial analysis

    if (l_sc.size() > 0)
    {
     if (bookmark_lsc == l_sc.end())
     { bookmark_lsc--; }

     r = bookmark_lsc->row_number;

     // first, find the Short_Cut closest to the offset
     if (row > r)                                                            // higher in memory
     {
       while (  (bookmark_lsc != l_sc.end())
             && (bookmark_lsc->row_number < row))
       {
         bookmark_lsc++; 
       }      
     }
     else                                                                   // lower in memory
       while (  (bookmark_lsc->row_number > row) 
             && (bookmark_lsc != l_sc.begin()))
       { 
         bookmark_lsc--; 
       }               
     if (bookmark_lsc == l_sc.end())
     { bookmark_lsc--; }

     bookmark_ldn = bookmark_lsc->it_s;
     r = bookmark_lsc->row_number;
    }
    else 
    {
     bookmark_ldn = l_dn.begin();
     r = 1;                     
    }
    
    // then begin a narrower search for the correct node

    // velsd: is this necessary?
    if (bookmark_ldn == l_dn.end())                     
    { bookmark_ldn = l_dn.begin(); r = 1;}

    // find the Disassembly_Node that contains the offset
    if (row > r)                                                            // higher in memory
     {
       while (  (bookmark_ldn != l_dn.end())
             && (row > r))
       {
         bookmark_ldn++; 
         r++;
       }      
     }
    else                                                                   // lower in memory
       while (  (row < r) 
             && (bookmark_ldn != l_dn.begin()))
       { 
        bookmark_ldn--; 
        r--;
       }               

    return bookmark_ldn; 
}

list<dis::Reference>::iterator                                           // find the Reference
dis::Disassembly::Get_Reference_From_Offset(int pos, bool exact_match)   //  for this position
{
    int                     b;                  // memory offset for bookmark

    ////////////////////////////////////////////////////////

    if ((pos < range_offset_low) || (pos > range_offset_high))
        {return l_r.end();}

    if (bookmark_lr == l_r.end())
    { bookmark_lr = l_r.begin(); }

    b = bookmark_lr->memory_offset;

    // find the Disassembly_Node that contains the offset
    if (pos > b)                                                            // higher in memory
     {
       while (  (bookmark_lr != l_r.end())
             && (bookmark_lr->memory_offset < pos))
       {
         bookmark_lr++;
       }      
     }
    else                                                                   // lower in memory
       while (  (bookmark_lr->memory_offset > pos) 
             && (bookmark_lr != l_r.begin()))
       { bookmark_lr--; }               

    if (   (bookmark_lr->memory_offset != pos) 
        && (exact_match == true))
    { bookmark_lr = l_r.end();}

    return bookmark_lr;
}

dis::Routine*                                                     // find the Routine
dis::Disassembly::Get_Routine_From_Name(const char *file_name,     //  for this name
                                        const char *routine_name)   
{
    int                             i, l1, l2, l3,      // index, lengths
                                    a;                  // number of apis 
    dis::Routine                   *r, *r2;                  

    ////////////////////////////////////////////////////////           
    
    i = 0; r = r2 = 0;

    a = v_api.size();

    l2 = strlen(routine_name);
    l3 = strlen(file_name);

    while (i < a)
    {
     r = v_api[i].routine;

     while ((r != 0) && (r2 == 0))
     {
       l1 = strlen(r->name);   

       if (u.str_i_cmp(r->name, routine_name, l1, l2) == 0)
       {
        l1 = v_api[i].file_name.size();

        if ((file_name == 0) || (u.str_i_cmp(v_api[i].file_name.c_str(), file_name, l1, l3) == 0))
        { return r; }                            // priority for match on routine + file name
        else 
        { r2 = r; r = r->next; }                 // routine name match alone
       }          
       else
       { r = r->next; }
     }

     i++; 
    }

    return r2;
}

list<dis::Import>::iterator
dis::Disassembly::Get_Import_From_Name(const char *function_name, const char *file_name)   //  for this name
{
    int                             l1, l2;                 // index, lengths
    list<dis::Import>::iterator     it_i;

    ////////////////////////////////////////////////////////

    
    it_i = l_i.begin(); 

    while (it_i != l_i.end())
    {
     l1 = it_i->name.size();
     l2 = strlen(function_name);

     if (u.str_i_cmp(it_i->name.c_str(), function_name, l1, l2) == 0)
     {
       l1 = it_i->library.size();
       l2 = strlen(file_name);

       if (u.str_i_cmp(it_i->library.c_str(), file_name, l1, l2) == 0)
       { return it_i;}
     }

     it_i++; 
    }

    return it_i;
}

list<dis::Variable>::iterator
dis::Disassembly::Get_Variable_From_Name(const char *variable_name)  
{
    int                             l1, l2;                 // index, lengths
    list<dis::Variable>::iterator     it_v;

    ////////////////////////////////////////////////////////
    
    it_v = l_v.begin(); 

    while (it_v != l_v.end())
    {
     l1 = it_v->name.size();
     l2 = strlen(variable_name);

     if (u.str_i_cmp(it_v->name.c_str(), variable_name, l1, l2) == 0)
     { return it_v;}

     it_v++; 
    }

    return it_v;
}

dis::Next_String   
dis::Disassembly::Get_Next_Printable_String(list<dis::Disassembly_Node>::iterator it, int start_from)
{
    Next_String     ns;

    int             i,       // index
                    n,       // next
                    t,       // total
                    o,       // offset   
                    p,       // printable
                    l;       // length

    short           s;       // section

    char           *c;

    bool            stop;    // stop_search

    //////////////////////////////////////////////////////////////////////////////
    //// start searching in iterator it, beginning from character start_from,   //
    ////  and  count how far away the next printable string is... and how       //
    ////  many non printables follow it                                         //
    //////////////////////////////////////////////////////////////////////////////

    o = it->memory_offset;
    s = it->section;

    l = it->n_used;
    n = it->memory_offset + l;

    ns.offset = ns.length = ns.length_following = ns.offset_next = 0;
    
    if (   (start_from >= l)
        || (it == l_dn.end()))
    { return ns; }       

    c = it->opcode;
    i = 0; 

    // begin from character start_from
    while (i < start_from)
    {
     i++; c++;
    }

    stop = false;
    l -= i;
    i = t = 0;
    while
     (!u.char_is_printable(*c))                              // skip non printables
    {
     i++; 
     c++; 

     if (i >= l)
     {
      it++;
      if ((it == l_dn.end()) || (n < it->memory_offset)  ||  (s != it->section))
      { ns.offset_next = n; return ns; }       
      else
      {
       c = it->opcode;
       l = it->n_used;
       n = it->memory_offset + l;
       t += i;
       i = 0;
      }
     }
    }                                                                             

    // t = first printable position
    t += i;      
    ns.offset = t;                                        // at least one byte is printable
    
    l -= i;
    i = t = 0;
    while
     (u.char_is_printable(*c)                             // skip non printables
      && (!stop))
    {
     i++; 
     c++; 

     if (i >= l)
     {
      it++;
      if ((it == l_dn.end()) || (n < it->memory_offset)  ||  (s != it->section))
      { stop = true; }       
      else
      {
       c = it->opcode;
       l = it->n_used;
       n = it->memory_offset + l;
       t += i;
       i = 0;
      }
     }
    }                              

    // t = last printable position
    t += i;                                                                                
    ns.length = t;                                       

    l -= i;
    i = t = p = 0;
    while (!stop)
    {
     i++; 
     c++; 

     if (u.char_is_printable(*c))
     {p++;}
     else
     {p = 0;}
     stop = (p >= GUI_LABEL_MIN);                       // count at most mbpi non printables

     if (i >= l)
     {
      it++;
      if ((it == l_dn.end()) || (n < it->memory_offset)  ||  (s != it->section))
      { stop = true; }       
      else
      {c = it->opcode; l = it->n_used; n = it->memory_offset + l; t += i; i = 0; }
     }
    }

    // where to begin searching next
    ns.length_following += i + t - p;                                                      

    ns.offset_next = 
        o                       // begin position
      + ns.offset               // first printable
      + ns.length               // number of printables after that
      + ns.length_following;    // number of non printables after that

    return ns;
}


void 
dis::Disassembly::Initialize()
{
    l_dn.clear(); 
    v_api.clear();
    l_r.clear(); 
    l_i.clear(); 
    v_to_explore.clear(); 
    v_to_explore_uncertain.clear(); 
    mp.Clear();
};         

void 
dis::Disassembly::Initialize(RegMemPart *r)
{
    r->reg08 = 0;
    r->reg16 = 0;
    r->reg32 = 0;
    r->fp_reg = 0;
    r->displ = 0;
    r->imm   = 0;
    r->abs   = true;
    r->used  = false;
};         

void 
dis::Disassembly::Initialize(Call *c)
{
    c->type_of_call = INSTRUCTION_CALL_NONE;
    
    c->name = 0;

    c->routine = 0;
};         

void 
dis::Disassembly::Initialize (Instruction *i)
{
    i->mnemonic  = 0;
    
    Initialize(&(i->part1));
    Initialize(&(i->part2));
    i->part3 = 0;

    i->operand_size = operand_bitness;
    i->jump_table.jump_to = JUMP_TABLE_NOT_USED;

    i->coprocessor = false;

    i->n_pushed = i->delimiter = 0;

    i->call = 0;
};         


void 
dis::Disassembly::Initialize(Displacement *d)
{   
    //d->imm  = 0; 
    d->mul  = 0;
    d->add  = 0;
    d->reg2 = 0;
    d->mul2 = 0;
    d->add2 = 0;
    d->seg_override = 0; 
    d->seg_offset   = 0;       
    d->seg_reg      = 0;       
    d->contr_reg    = 0;
    d->debug_reg    = 0;
    d->test_reg     = 0;
};         

void 
dis::Disassembly::Initialize(Reference *r)
{   
    r->memory_offset = 0;
    r->type = REFERENCE_TYPE_UNEXPLORED;

    r->label.clear();
    r->it_dn = l_dn.end();
    r->ref_out.clear();

    r->it_i = l_i.end();
    r->it_f = l_f.end();
    r->it_v = l_v.end();
};         

void 
dis::Disassembly::Initialize(Import *i)
{   
    i->memory_offset = 0;
    i->name.clear();
    i->library.clear();
    i->routine = 0;
};         

void 
dis::Disassembly::Initialize(Function *f)
{   
    f->memory_offset = 0;
    f->name.clear();
    f->routine = 0;
};         

void 
dis::Disassembly::Initialize(Variable *v)
{   
    v->memory_offset = 0;
    v->name.clear();
};         

void 
dis::Disassembly::Initialize(Extra *e)
{   
    e->description = 0;
    e->next        = 0;
    e->next_level  = 0;
};         


void 
dis::Disassembly::Initialize(Disassembly_Node *d)
{
    d->file_offset = d->memory_offset = 0;    

    d->type = d->status = d->n_used = 0;  

    d->section = 0;  
    
    d->opcode = d->comment = d->label = 0;
    
    d->ref_in = d->ref_out = l_r.end();     

    d->bookmarked = false;

    Initialize(&(d->instruction));
}

void 
dis::Disassembly::Initialize (Api *a)
{
    a->file_name = "";    

    a->routine = 0;
}

void 
dis::Disassembly::Initialize (Routine *r)
{
    r->name = 0;    

    r->input = 0;

    r->output = 0;

    r->next = 0;
}

void 
dis::Disassembly::Initialize (Short_Cut *s)
{
    s->file_offset = 0;

    s->memory_offset = 0;

    s->n_used = 0;

    s->it_s = l_dn.begin();
}

void 
dis::Disassembly::Initialize (Parameter *p)
{
    p->name = 0;    

    p->type = 0;

    p->type_name = 0;

    p->next = 0;
}               

string
dis::Disassembly::Get_Function_Type(int i)
{
   string           s;

   ///////////////////////////////////
   
   if (i < FUNCTION_TYPE_MAX)
   { s = function_type[i]; }

   return s;
}

int
dis::Disassembly::Get_Function_Type(string *s)
{
   int                  i;  // index

   //////////////////////////////////////

   for (i = 0; i < FUNCTION_TYPE_MAX; i++)
   {
    if (*s == function_type[i])
    { return i; }
   }

   return FUNCTION_TYPE_UNKNOWN;
}                                       

void  
dis::Disassembly::Translate_Mnemonic(Instruction *i, string *str_instr)
{
    cout << "Translate Mnemonic : Disassembly " << "\n";
}

void
dis::Disassembly::Callback_Translate_Instruction(Instruction *instruction, string *str_instr)
{
    cout << "Translate Instruction : Disassembly " << "\n";
}

void
dis::Disassembly::Callback_Translate_Section(uint section, string *str_section)
{
 if (section < v_s.size())
 { *str_section = v_s[section].name; }
}

void  
dis::Disassembly::Translate_RegMemPart(RegMemPart *rmp,  int use_override, string *str_instr)
{
    cout << "Translate RegMemPart : Disassembly " << "\n";
}

void 
dis::Disassembly::Callback_Translate_Opcodes(Disassembly_Node *dn, bool data, string *s, int max_bytes_per_row)
{
    char        *t;            // temp value
    short        i, n;         // index
    char         c;            // temp value     
    int          m;            // max


    /////////////////////////////////////////////////////////////
    
    t = dn->opcode;
    n = dn->n_used;

    m = 0;

    *s = "";

    while (n)
    {
     i = (*(int*) t) & 0xFF;           

     if (data == false)                     // instruction mode: hexedecimal bytes
     {
      *s += u.byte_to_hexstring(i);
      if (n > 1) {*s += " ";}
     }
     else                                   // data mode: readable characters
     {
      if (u.char_is_printable(*t))
      { c = *t; *s += c; }
      else
      { *s += '.'; }
     }

     t++;
     n--;

     m++;
     if ((m >= max_bytes_per_row) && (max_bytes_per_row > 0))
     {*s += UTIL_NEW_LINE; m = 0;}
    }
}                             

void 
dis::Disassembly::Add_Routine(Api *a, Routine *r)
{
   bool                         stop, found;
   
   int                          i, j;          // indexes

   vector<Api>::iterator        it_a;      

   Routine                     *rout, *prev_rout;

   /////////////////////////////////////////////

   found = false;  
   i = 0;

   j = v_api.size();
   stop = (j == 0);
   prev_rout = rout = 0;

   it_a = v_api.begin();

   while (stop == false)
   {
    if (it_a->file_name == a->file_name)
       { found= true; stop = true;}
    else
       {
        i++; it_a ++;
        if ((i >= j) || (it_a->file_name > a->file_name))
        { stop = true;}
       }
   } 

   if (!found)
   {
    it_a = v_api.insert(it_a, *a);
   }
   else 
   {
    rout = it_a->routine; 

    while (rout != 0)
    {prev_rout = rout; rout = rout->next; }        

    prev_rout->next = r;
   }
}

void 
dis::Disassembly::Add_Reference(list<Disassembly_Node>::iterator it_n, int ref, bool certain)
{
   bool                         found = false;
   
   Reference                    result;

   string                       s;

   list<Reference>::iterator    it_r;       

   std::vector <long>::iterator it_o;      

   ////////////////////////////////////////////////////////////

   //don't bother if it can't be a valid reference in the first place...
   if ((ref < range_offset_low) || (ref > range_offset_high))
   {return;}

   // first, try and and find an existing Reference
   it_r = Get_Reference_From_Offset(ref, false);

   found = ((it_r != l_r.end()) && (it_r->memory_offset == ref));

   if (!found)                          // create new Reference
   { 
     while (   (!found)
            && (it_r != l_r.end()))
     {
      if (it_r->memory_offset >= ref)
      { found = true; }
      else 
      { it_r++; }
     }

     Initialize (&result);    

     result.memory_offset = ref; 

     result.label += "loc ";
     result.label += u.int_to_hexstring(ref);

     if (it_n->memory_offset == ref)
     { result.it_dn = it_n; }
     else 
     { result.ref_out.push_back(it_n->memory_offset); }

     l_r.insert(it_r, result);
     it_r--;
   }
   else                                 // reuse existing Reference 
   {
    if (it_n->memory_offset != ref)
    {
     it_r->ref_out.push_back(it_n->memory_offset); 

     sort(it_r->ref_out.begin(), it_r->ref_out.end());
     it_o = unique(it_r->ref_out.begin(), it_r->ref_out.end());
     it_r->ref_out.erase(it_o, it_r->ref_out.end());
    

     if (it_r->it_i != l_i.end())        // if reference points to an import
     {
      s = it_r->it_i->library + ':' + it_r->it_i->name;

      it_n->label = (char *) mp.Ensure_Minimum_Allocation(it_n->label, s.size());
      
      strcpy(it_n->label, s.c_str());
     }
    }
   }

   if ((    (it_n->ref_out == l_r.end())
         || (certain == true))
       &&
       (it_n->memory_offset != ref)
      )
   {it_n->ref_out = it_r;} 
   /*
   else
   {  cout << "Add_Reference too late " << it_n->memory_offset << " ==> " << ref << "\n";}
   */
}

void 
dis::Disassembly::Update_Reference_For_Import(list<Import>::iterator it, int ref)
{
   bool             found = false;

   list<Reference>::iterator it_r;       
   /////////////////////////////////////////////////////////////////////

   //don't bother if it can't be a valid reference in the first place...
   if ((ref < range_offset_low) || (ref > range_offset_high))
   {return;}

   // try and and find an existing Reference
   it_r = l_r.begin();

   while (   (!found)
          && (it_r != l_r.end()))
   {
     if (it_r->memory_offset >= ref)
     { found = true; }
     else 
     { it_r++; }
   }

   if (found) 
     {found = it_r->memory_offset == ref;}                               
   
   if (!found)                          
   { return; }                          // reference must exist if we want to add an import to it
   
   it_r->it_i = it;
   it_r->label = "imported function: " + it->library + "." + it->name;
   it_r->type = REFERENCE_TYPE_IMPORT;
}

void
dis::Disassembly::Update_Reference_For_Function(list<Function>::iterator it, int ref)
{
   bool             found = false;

   list<Reference>::iterator it_r;       
   /////////////////////////////////////////////////////////////////////

   //don't bother if it can't be a valid reference in the first place...
   if ((ref < range_offset_low) || (ref > range_offset_high))
   {return;}

   // try and and find an existing Reference
   it_r = l_r.begin();

   while (   (!found)
          && (it_r != l_r.end()))
   {
     if (it_r->memory_offset >= ref)
     { found = true; }
     else 
     { it_r++; }
   }

   if (found) 
     {found = it_r->memory_offset == ref;}                               
   
   if (!found)                          
   { return; }                          // reference must exist if we want to add an import to it
   
   it_r->it_f = it;
   it_r->label = "function: " + it->name;
   it_r->type = REFERENCE_TYPE_FUNCTION;
}

void
dis::Disassembly::Update_Reference_For_Variable(list<Variable>::iterator it, int ref)
{
   bool             found = false;

   list<Reference>::iterator it_r;       
   /////////////////////////////////////////////////////////////////////
    
   //don't bother if it can't be a valid reference in the first place...
   if ((ref < range_offset_low) || (ref > range_offset_high))
   {return;}
    
   // try and and find an existing Reference
   it_r = l_r.begin();
    
   while (   (!found)
          && (it_r != l_r.end()))
    {
     if (it_r->memory_offset >= ref)
     { found = true; }
     else 
     { it_r++; }
    }
    
    if (found) 
     {found = it_r->memory_offset == ref;}                               
    
    if (!found)                          
    { return; }                          // reference must exist if we want to add an import to it
    
    it_r->it_v = it;
    it_r->label = "variable: " + it->name;
    it_r->type = REFERENCE_TYPE_VARIABLE;
}

void 
dis::Disassembly::Update_Disassembly_Node_For_Import(list<Import>::iterator it_i, 
                                                     list<Disassembly_Node>::iterator it_n,
                                                     bool force_call)
{
   Call                        *c;

   int                          i, n;               // indexes

   list<Reference>::iterator    it_r;

   /////////////////////////////////////////////////////////////////////

   if (it_n->instruction.call == 0)
   {
       if (!force_call) { return; }

       c = (Call*) mp.Use_Pool(sizeof(Call));
       Initialize(c);
       it_n->instruction.call = c;               
   }  
   else 
   { c = it_n->instruction.call; }

   if (c->routine == 0)
   { c->routine = it_i->routine; }
   

   c->name = (char*) mp.Use_Pool(it_i->name.size() + 1);
   strcpy(c->name, it_i->name.c_str());

   ///////////////////////////////////////////////////////////////////////
   // now pass on the import reference to all calling nodes             //
   ///////////////////////////////////////////////////////////////////////

   it_r = Get_Reference_From_Offset(it_n->memory_offset, true);

   if (it_r != l_r.end())
   {
    it_r->type = REFERENCE_TYPE_IMPORT;

    n = it_r->ref_out.size();

    for (i = 0; i < n; i++)
    {
     it_n = Get_Disassembly_Node_From_Offset(it_r->ref_out[i], true);       

     if (it_n == l_dn.end())
     { continue; }

     Update_Disassembly_Node_For_Import(it_i, it_n, false);
    }
   }
}

void 
dis::Disassembly::Update_Disassembly_Node_For_Function(list<Function>::iterator it_f, 
                                                     list<Disassembly_Node>::iterator it_n)
{
   Call                        *c;

   list<Reference>::iterator    it_r;

   /////////////////////////////////////////////////////////////////////

   if (it_n->instruction.call == 0)
   {
    c = (Call*) mp.Use_Pool(sizeof(Call));
    Initialize(c);
    it_n->instruction.call = c;               
   }  
   else 
   { c = it_n->instruction.call; }

   if (c->routine == 0)
   { c->routine = it_f->routine; }

   c->name = (char*) mp.Use_Pool(it_f->name.size() + 1);
   strcpy(c->name, it_f->name.c_str());
}

void 
dis::Disassembly::Update_Ldn_For_Delete (Disassembly_Node *d)
{   
    if (&(*bookmark_ldn) == d)
    { bookmark_ldn = l_dn.begin(); }

    if (d->bookmarked == true)
    {
      Update_Short_Cut_List(true);    
    }                             
}

void
dis::Disassembly::Add_Address_To_Explore(int adr)
{
    if  ((adr >= range_offset_low) && (adr <= range_offset_high))
     { v_to_explore.push_back(adr); }
}

void
dis::Disassembly::Add_Code_To_Explore(list<Disassembly_Node>::iterator it, int adr)
{
    RegMemPart          rmp;
    Displacement        *d;    

    list<Disassembly_Node>::iterator   it_t;

    bool                found_jump_table;
    Jump_Table_Request  jtr;                     

    //////////////////////////////////////////////////////////////////////

    rmp = it->instruction.part1;
    d = rmp.displ;

    found_jump_table =
    (
     (rmp.abs == false) 
     &&
     (d) 
     &&
     (   (rmp.reg08 > 0) 
      || (rmp.reg16 > 0) 
      || (rmp.reg32 > 0))
     &&
     (   (d->mul > 0)
      || (d->add > 0))
    ); 

    if (found_jump_table)
    {
      cout << "found jump table at " << it->memory_offset << " -> " << adr << "\n";

      it_t = Get_Disassembly_Node_From_Offset(adr, false);

      if (   (it_t != l_dn.end())
          && (it_t->instruction.jump_table.jump_to == JUMP_TABLE_NOT_USED))
      {
       Isolate_l_dn(it_t, adr, address_bitness, NODE_STATUS_AUTO);

       jtr.jump_from = it->memory_offset;
       jtr.jump_to   = adr;

       v_jump_table.push_back(jtr);   
      }
    }
    else
    { Add_Address_To_Explore(adr);}
}

void 
dis::Disassembly::Add_Uncertain_Address_To_Explore(int unc)
{
   if ((unc < range_offset_low) || (unc > range_offset_high))
   { return; }

   v_to_explore_uncertain.push_back(unc);
}

dis::Extra*
dis::Disassembly::Callback_Get_Extra()
{
 return &e;
}

void
dis::Disassembly::Callback_Get_Statistics(Statistics* s)
{
    s->max_row_offset = range_offset_high;
    s->n_rows = l_dn.size();
}

dis::Disassembly_Node *
dis::Disassembly::Callback_Get_nth_Row(uint n) 
{
  list<Disassembly_Node>::iterator it;

  /////////////////////////////////////

  it = Get_Disassembly_Node_From_Row(n);

  if (it == l_dn.end())
  { return 0; }
  else
  { return &(*it); }
}

int
dis::Disassembly::Callback_Get_Row_From_Offset(int pos)
{
  int                     b,                  // memory offset for bookmark
                          r;                  // row number 

  ////////////////////////////////////////////////////////

  if ((pos < range_offset_low) || (pos > range_offset_high))
    {return -1;}

  if (l_sc.size() > 0)
    {
     if (bookmark_lsc == l_sc.end())
     { 
     bookmark_lsc--; }

     b = bookmark_lsc->memory_offset;

     // first, find the Short_Cut closest to the offset
     if (pos > b)                                                            // higher in memory
     {
       while (  (bookmark_lsc != l_sc.end())
             && (bookmark_lsc->memory_offset + bookmark_lsc->n_used <= pos))
       {
         bookmark_lsc++; 
       }      
     }
     else                                                                   // lower in memory
       while (  (bookmark_lsc->memory_offset > pos) 
             && (bookmark_lsc != l_sc.begin()))
       { 
         bookmark_lsc--; 
       }               
     if (bookmark_lsc == l_sc.end())
     { bookmark_lsc--; }

     bookmark_ldn = bookmark_lsc->it_s;
     r = bookmark_lsc->row_number;
    }
  else 
    {
     bookmark_ldn = l_dn.begin();
     r = 1;                     
    }

    
  // then begin a narrower search for the correct node
  if (bookmark_ldn == l_dn.end())                     
  { bookmark_ldn = l_dn.begin(); r = 1;}

  b = bookmark_ldn->memory_offset;

  // find the Disassembly_Node that contains the offset
  if (pos > b)                                                            // higher in memory
     {
       while (  (bookmark_ldn != l_dn.end())
             && (bookmark_ldn->memory_offset + bookmark_ldn->n_used <= pos))
       { bookmark_ldn++; r++; }      
     }
  else                                                                   // lower in memory
       while (  (bookmark_ldn->memory_offset > pos) 
             && (bookmark_ldn != l_dn.begin()))
       { bookmark_ldn--; r--;}               

  if (bookmark_ldn == l_dn.end())
    { return -1; }  

  return r; 
}

bool
dis::Disassembly::Callback_Is_Valid_Reference(std::list<dis::Reference>::iterator *it_ref)
{
 return (*it_ref != l_r.end());
}

int
dis::Disassembly::Callback_Navigation_Search_Byte(int row, std::vector<char> *search_string,  
                                                  bool direction, bool wrap)
{
  long                                  offset_found, offset_start; 

  bool                                  found;

  list<dis::Disassembly_Node>::iterator it_dn;

  ///////////////////////////////////////////////////////////////

  cout << "Disassembly Navigation_Search_Byte!" << "\n";

  it_dn = Get_Disassembly_Node_From_Row(row);

  offset_start = it_dn->memory_offset;

  found = opcodes_mp.Search_Memory_Pool(direction, wrap, offset_start, search_string, &offset_found);

  if (found)
  { return Callback_Get_Row_From_Offset(offset_found); }
  else
  { return -1; }
}



dis::Disassembly_Node*
dis::Disassembly::Callback_Get_Next_Disassembly_Node_From_Offset(int pos)
{
  list<Disassembly_Node>::iterator it;

  /////////////////////////////////////

  it = Get_Disassembly_Node_From_Offset(pos, true);

  if (it == l_dn.end()) { return 0; }
  else
  { 
   it++;
   if (it == l_dn.end()) { return 0; }

   return &(*it);
  }                                     
}

void   
dis::Disassembly::Gui_Command(int gui_command, char *gui_text)
{
    int                                      i;
                                                      
    ////////////////////////////////////////////////////////////////

    if (type_of_gui == DISASSEMBLY_GUI_NONE) { return; }

#ifdef HAVE_GTK                               
    while(gui_mg->m->events_pending()) gui_mg->m->iteration();

    switch (gui_command)
    {
     case GUI_COMMAND_REFRESH:                            
     {

       while(gui_mg->m->events_pending()) gui_mg->m->iteration();
       break;
     }
    
     case GUI_COMMAND_STATUSBAR_PUSH:
     {
       i = gui_mg->sb_context_id;
       gui_mg->sb->push(gui_text, i);
       break;
     }

     case GUI_COMMAND_STATUSBAR_POP:
     { 
       i = gui_mg->sb_context_id;
       gui_mg->sb->pop(i);
       break;
     }
    }

    while(gui_mg->m->events_pending()) gui_mg->m->iteration();
#endif

}

void   
dis::Disassembly::Gui_Command(int gui_command)
{
    int                                      i;
                                                      
    ////////////////////////////////////////////////////////////////

    if (type_of_gui == DISASSEMBLY_GUI_NONE) { return; }

#ifdef HAVE_GTK                               
    while(gui_mg->m->events_pending()) gui_mg->m->iteration();

    switch (gui_command)
    {
     case GUI_COMMAND_REFRESH:                            
     {

       while(gui_mg->m->events_pending()) gui_mg->m->iteration();
       break;
     }
    
     case GUI_COMMAND_STATUSBAR_POP:
     { 
       i = gui_mg->sb_context_id;
       gui_mg->sb->pop(i);
       break;
     }
    }

    while(gui_mg->m->events_pending()) gui_mg->m->iteration();
#endif

}

void
dis::Disassembly::Callback_Save_Routine(ofstream& of, Routine *routine)
{    
  Parameter      *param;

  //////////////////////////////////////////////////////////

  if (routine->name) 
    {
     of << DISASSEMBLY_SEPARATOR_ROUTINE_NAME << "\n"; 
     of << routine->name << "\n"; 
    }

  if (routine->input) 
    {
     of << DISASSEMBLY_SEPARATOR_INPUT << "\n"; 
     param = routine->input;
     while (param)
     {
      of << DISASSEMBLY_SEPARATOR_PARAMETER << "\n"; 
      of << param->type << "\n";                                                               
      if (param->name) 
      {
       of << DISASSEMBLY_SEPARATOR_PARAMETER_NAME << "\n"; 
       of << param->name << "\n"; 
      }    
      if (param->type_name) 
      {
       of << DISASSEMBLY_SEPARATOR_PARAMETER_TYPE_NAME << "\n"; 
       of << param->type_name << "\n"; 
      }    
      param = param->next;
     }
    }

  if (routine->output) 
    {
     of << DISASSEMBLY_SEPARATOR_OUTPUT << "\n"; 
     param = routine->output;
     while (param)
     {
      of << DISASSEMBLY_SEPARATOR_PARAMETER << "\n"; 
      of << param->type << "\n";                                                               
      if (param->name) 
      {
       of << DISASSEMBLY_SEPARATOR_PARAMETER_NAME << "\n"; 
       of << param->name << "\n"; 
      }    
      if (param->type_name) 
      {
       of << DISASSEMBLY_SEPARATOR_PARAMETER_TYPE_NAME << "\n"; 
       of << param->type_name << "\n"; 
      }    
      param = param->next;
     }
    }
}   

void
dis::Disassembly::Callback_Save_Database(std::string *file_name)
{
    int             i, j, k;                 // indexes
    
    ofstream        of;                      // output_file
    string          File_Name;

    Instruction     instr;
    RegMemPart      rmp;
    Displacement   *displ;
    Jump_Table      jump_table;
    Call           *call;
    Reference      *ref;

    list<Disassembly_Node>::iterator    it_ldn;
    vector <Api>::iterator              it_api;              
    list <Reference>::iterator          it_r; 
    list <Import>::iterator             it_i;             
    list <Variable>::iterator           it_v;             
    vector <int>::iterator              it_vi;                

    //////////////////////////////////////////////////////////////

    Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "saving to file");

    jump_table.jump_to = JUMP_TABLE_NOT_USED;

    File_Name = *file_name;

    of.open(File_Name.c_str());

    //  header   ////////////////////////////////////////:

    of << DISASSEMBLY_SEPARATOR_RESULT << "\n"; 
    of << u.time_to_string() << "\n";          

    of << DISASSEMBLY_SEPARATOR_FILE << "\n"; 
    of << input_file << "\n";

    of << DISASSEMBLY_SEPARATOR_FILESIZE << "\n"; 
    of << input_file_size << "\n";

    of << DISASSEMBLY_SEPARATOR_DISASSEMBLY << "\n"; 
    of << disassembly_type << "\n";

    of << DISASSEMBLY_SEPARATOR_STATISTICS << "\n"; 
    of << completed_phase   << DISASSEMBLY_SEPARATOR_ITEM
       << range_offset_low  << DISASSEMBLY_SEPARATOR_ITEM
       << range_offset_high << "\n";

    // v_api  //////////////////////////////////////////////
    of << DISASSEMBLY_SEPARATOR_VAPI << "\n";        
    i = 0;                            
    for (it_api = v_api.begin(); it_api != v_api.end(); it_api++)
    {
     of << DISASSEMBLY_SEPARATOR_API << "\n";        
     i++;
     of << i  << DISASSEMBLY_SEPARATOR_ITEM << it_api->file_name << "\n"; 

     if (it_api->routine) 
     {
      of << DISASSEMBLY_SEPARATOR_API_ROUTINE << "\n"; 
      Callback_Save_Routine(of, it_api->routine);
     }
    } // DISASSEMBLY_SEPARATOR_VAPI

    // l_i  //////////////////////////////////////////////
    if (l_i.size() > 0)
    {
     of << DISASSEMBLY_SEPARATOR_LI << "\n";        
     i = 0;                            
     for (it_i = l_i.begin(); it_i != l_i.end(); it_i++)
     {
      of << DISASSEMBLY_SEPARATOR_IMPORT << "\n";        
      i++;
      of      << i                    << DISASSEMBLY_SEPARATOR_ITEM
              << it_i->memory_offset  << "\n";

      of << DISASSEMBLY_SEPARATOR_IMPORT_NAME << "\n"; 
      of << it_i->name                << "\n";

      of << DISASSEMBLY_SEPARATOR_LIB_NAME << "\n"; 
      of << it_i->library             << "\n";

      if (it_i->routine) 
      {
       of << DISASSEMBLY_SEPARATOR_IMPORT_ROUTINE << "\n"; 
       Callback_Save_Routine(of, it_i->routine);
      }                              
     } // DISASSEMBLY_SEPARATOR_LI
    }

    // l_v  //////////////////////////////////////////////
    if (l_v.size() > 0)
    {
     of << DISASSEMBLY_SEPARATOR_LV << "\n";        
     i = 0;                            
     for (it_v = l_v.begin(); it_v != l_v.end(); it_v++)
     {
      of << DISASSEMBLY_SEPARATOR_VARIABLE << "\n";        
      i++;
      of     << i                    << DISASSEMBLY_SEPARATOR_ITEM
             << it_v->memory_offset  << "\n";

      of << DISASSEMBLY_SEPARATOR_VARIABLE_NAME << "\n"; 
      of << it_v->name               << "\n";
     } // DISASSEMBLY_SEPARATOR_LV
    }                                                                       

    // l_r  //////////////////////////////////////////////
    if (l_r.size() > 0)
    {
     of << DISASSEMBLY_SEPARATOR_LR << "\n";        
     i = 0;                            
     for (it_r = l_r.begin(); it_r != l_r.end(); it_r++)
     {
      ref = &(*it_r);
      i++;
      of << DISASSEMBLY_SEPARATOR_REFERENCE_REF << "\n";        
      of      << i                    << DISASSEMBLY_SEPARATOR_ITEM
              << ref->memory_offset   << DISASSEMBLY_SEPARATOR_ITEM
              << ref->type            << DISASSEMBLY_SEPARATOR_ITEM << "\n";

      if (ref->label.size() > 0)
      {
       of << DISASSEMBLY_SEPARATOR_REFERENCE_STRING << "\n";
       of << ref->label          << "\n";
      }

      if (ref->it_dn != l_dn.end())
      {
       of << DISASSEMBLY_SEPARATOR_REFERENCE_IT_DN << "\n";
       it_ldn = ref->it_dn;
       of
              << it_ldn->file_offset            << DISASSEMBLY_SEPARATOR_ITEM
              << it_ldn->memory_offset          << "\n";
      }

      if (ref->it_i != l_i.end())
      {
       of << DISASSEMBLY_SEPARATOR_REFERENCE_IT_IMPORT << "\n";
       of 
         << ref->it_i->memory_offset << DISASSEMBLY_SEPARATOR_ITEM               
         << ref->it_i->name          << DISASSEMBLY_SEPARATOR_ITEM
         << ref->it_i->library                        << "\n";
      }                                       

      if (ref->it_v != l_v.end())
      {
       of << DISASSEMBLY_SEPARATOR_REFERENCE_IT_VARIABLE << "\n";
       of 
         << ref->it_v->memory_offset << DISASSEMBLY_SEPARATOR_ITEM               
         << ref->it_v->name          << DISASSEMBLY_SEPARATOR_ITEM
                                                         << "\n";
      }                                       

      j = ref->ref_out.size();
      if (j > 0)
      {
       of << DISASSEMBLY_SEPARATOR_REFERENCE_IT_OUT << "\n"; 
       of << j         << DISASSEMBLY_SEPARATOR_ITEM;
       for (k = 0; k < j; k++)
       { of << ref->ref_out[k]   << DISASSEMBLY_SEPARATOR_ITEM; }
       of << "\n";
       }
     }
    } // DISASSEMBLY_SEPARATOR_LR

    // v_to_explore //////////////////////////////////////
    of << DISASSEMBLY_SEPARATOR_TO_EXPLORE << "\n";        
    j = v_to_explore.size(); 

    if (j > 0)
    {
     for (i = 0; i < j; i++)
     { of      << v_to_explore[i]     << DISASSEMBLY_SEPARATOR_ITEM; }
     of  << "\n";
    } // DISASSEMBLY_SEPARATOR_TO_EXPLORE 

    // v_to_explore_uncertain ////////////////////////////
    of << DISASSEMBLY_SEPARATOR_TO_EXPLORE_UNCERTAIN << "\n";        
    j = v_to_explore_uncertain.size(); 

    if (j > 0)
    {
     for (i = 0; i < j; i++)
     { of      << v_to_explore_uncertain[i]     << DISASSEMBLY_SEPARATOR_ITEM; }
     of  << "\n";
    } // DISASSEMBLY_SEPARATOR_TO_EXPLORE_UNCERTAIN

    // l_dn   //////////////////////////////////////////////
    of << DISASSEMBLY_SEPARATOR_LDN << "\n";        
    i = 0;                            
    for (it_ldn = l_dn.begin(); it_ldn != l_dn.end(); it_ldn++)
    {
        of << DISASSEMBLY_SEPARATOR_NODE << "\n";        
        i++;
        of 
         << i                      << DISASSEMBLY_SEPARATOR_ITEM                     
         << it_ldn->file_offset    << DISASSEMBLY_SEPARATOR_ITEM                     
         << it_ldn->memory_offset  << DISASSEMBLY_SEPARATOR_ITEM                     
         << it_ldn->type           << DISASSEMBLY_SEPARATOR_ITEM                     
         << it_ldn->status         << DISASSEMBLY_SEPARATOR_ITEM                     
         << it_ldn->n_used         << DISASSEMBLY_SEPARATOR_ITEM;                     
        of << "\n";                                      

        for (j=0; j < it_ldn->n_used; j++)
        { of << u.int_to_hexstring((it_ldn->opcode[j]) & 0xff) << DISASSEMBLY_SEPARATOR_ITEM; }   
        of << "\n";        

        of << DISASSEMBLY_SEPARATOR_INSTRUCTION << "\n"; 
        instr = it_ldn->instruction;
        of 
         << instr.operand_size << DISASSEMBLY_SEPARATOR_ITEM                     
         << instr.coprocessor  << DISASSEMBLY_SEPARATOR_ITEM                     
         << instr.delimiter    << DISASSEMBLY_SEPARATOR_ITEM                     
         << instr.n_pushed     << DISASSEMBLY_SEPARATOR_ITEM;                    
        of << "\n";       

        if (instr.mnemonic)
        {
         of << DISASSEMBLY_SEPARATOR_MNEMONIC << "\n"; 
         of << instr.mnemonic << "\n"; 
        }

        of << DISASSEMBLY_SEPARATOR_REGMEMPART << "\n"; 
        rmp = instr.part1;
        of 
         << "1"                << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.reg08          << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.reg16          << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.reg32          << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.fp_reg         << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.abs            << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.used           << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.imm            << DISASSEMBLY_SEPARATOR_ITEM;                    
        of << "\n";

        if (rmp.displ)
        {
         of << DISASSEMBLY_SEPARATOR_DISPLACEMENT << "\n"; 
         displ = rmp.displ;
         of 
          << displ->mul         << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->add         << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->reg2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->mul2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->add2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_override << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_offset  << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_reg     << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->contr_reg   << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->debug_reg   << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->test_reg    << DISASSEMBLY_SEPARATOR_ITEM;                    
         of << "\n";                                                              
        }

        of << DISASSEMBLY_SEPARATOR_REGMEMPART << "\n"; 
        rmp = instr.part2;
        of 
         << "2"                << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.reg08          << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.reg16          << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.reg32          << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.fp_reg         << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.abs            << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.used           << DISASSEMBLY_SEPARATOR_ITEM                     
         << rmp.imm            << DISASSEMBLY_SEPARATOR_ITEM;                    
        of << "\n";

        if (rmp.displ)
        {
         of << DISASSEMBLY_SEPARATOR_DISPLACEMENT << "\n"; 
         displ = rmp.displ;
         of 
          << displ->mul         << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->add         << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->reg2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->mul2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->add2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_override << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_offset  << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_reg     << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->contr_reg   << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->debug_reg   << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->test_reg    << DISASSEMBLY_SEPARATOR_ITEM << "\n";                                                              
        }

        if (instr.part3)
        {
         of << DISASSEMBLY_SEPARATOR_REGMEMPART << "\n"; 
         rmp = *(instr.part3);
         of 
          << "3"                << DISASSEMBLY_SEPARATOR_ITEM                     
          << rmp.reg08          << DISASSEMBLY_SEPARATOR_ITEM                     
          << rmp.reg16          << DISASSEMBLY_SEPARATOR_ITEM                     
          << rmp.reg32          << DISASSEMBLY_SEPARATOR_ITEM                     
          << rmp.fp_reg         << DISASSEMBLY_SEPARATOR_ITEM                     
          << rmp.abs            << DISASSEMBLY_SEPARATOR_ITEM                     
          << rmp.used           << DISASSEMBLY_SEPARATOR_ITEM                     
          << rmp.imm            << DISASSEMBLY_SEPARATOR_ITEM << "\n";

         if (rmp.displ)
         {
          of << DISASSEMBLY_SEPARATOR_DISPLACEMENT << "\n"; 
          displ = rmp.displ;
          of 
          << displ->mul         << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->add         << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->reg2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->mul2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->add2        << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_override << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_offset  << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->seg_reg     << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->contr_reg   << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->debug_reg   << DISASSEMBLY_SEPARATOR_ITEM                     
          << displ->test_reg    << DISASSEMBLY_SEPARATOR_ITEM << "\n";                                                              
         }
        }

        if (jump_table.jump_to != JUMP_TABLE_NOT_USED)
        {
         of << DISASSEMBLY_SEPARATOR_JUMPTABLE << "\n"; 
         jump_table = instr.jump_table;
         of << jump_table.jump_to << "\n";                                                              
        }

        if (instr.call)
        {
         of << DISASSEMBLY_SEPARATOR_CALL << "\n"; 
         call = instr.call;
         of << call->type_of_call << "\n";                                                              
         if (call->name) 
         {
          of << DISASSEMBLY_SEPARATOR_CALL_NAME << "\n"; 
          of << call->name << "\n"; 
         }
         if (call->routine) 
         {
          of << DISASSEMBLY_SEPARATOR_ROUTINE << "\n"; 
          Callback_Save_Routine(of, call->routine);
         }
        }

        if (it_ldn->comment)
        {
         of << DISASSEMBLY_SEPARATOR_COMMENT << "\n"; 
         of << it_ldn->comment << "\n"; 
        }

        if (it_ldn->label)
        {
         of << DISASSEMBLY_SEPARATOR_LABEL << "\n"; 
         of << it_ldn->label << "\n"; 
        }

        if (it_ldn->ref_in != l_r.end())
        {
         of << DISASSEMBLY_SEPARATOR_REFERENCE_DN_IN << "\n"; 
         of << it_ldn->ref_in->memory_offset             << "\n";
        }

        if (it_ldn->ref_out != l_r.end())
        {
         of << DISASSEMBLY_SEPARATOR_REFERENCE_DN_OUT << "\n"; 
         of << it_ldn->ref_out->memory_offset             << "\n";
        }
    } // DISASSEMBLY_SEPARATOR_LDN 

    of << DISASSEMBLY_SEPARATOR_END << "\n";        

    of.close();

    Gui_Command(GUI_COMMAND_STATUSBAR_POP);
}

void
dis::Disassembly::Callback_Save_Listing(std::string *file_name)
{
    ofstream        of;                      // output_file
    string          File_Name;

    Instruction     instr;

    string          str_instr;   

    list<Disassembly_Node>::iterator    it_ldn;

    //////////////////////////////////////////////////////////////

    Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "saving to file");

    File_Name = *file_name;

    of.open(File_Name.c_str());

    //  header   ////////////////////////////////////////:

    of << DISASSEMBLY_SEPARATOR_RESULT << "\n"; 

    of << u.time_to_string() << "\n";          

    of << input_file << "\n";

    // l_dn   //////////////////////////////////////////////
    for (it_ldn = l_dn.begin(); it_ldn != l_dn.end(); it_ldn++)
    {
        instr = it_ldn->instruction;

        Callback_Translate_Instruction(&instr, &str_instr);   

        of 
            << it_ldn->memory_offset
            << " : "
            << str_instr
            << "\n";

    } // DISASSEMBLY_SEPARATOR_LDN 

    of.close();

    Gui_Command(GUI_COMMAND_STATUSBAR_POP);
}

int
dis::Disassembly::Callback_Open()
{
    int                 state, prev_state,
                        sub_state, rmp_state, 
                        routine_state,                  // state_machine
                        i, j;                           // indexes

    bool                stop, first,
                        new_state, 
                        api_state, import_state, var_state,
                        routine_found;

    char               *opcodes;                        // one memory region for all opcodes

    Call               *call;
    Displacement       *displ;        
    Parameter          *param, *prev_param;
    Routine            *routine, *prev_routine;

    Api                 api;
    Disassembly_Node    dn;
    Import              import;
    Variable            var;
    Instruction         instr; 
    Jump_Table          jump_table;            
    Reference           ref;
    RegMemPart          rmp;

    std::ifstream                          i_file;      // input_file
    std::string                            s;      
    std::vector<std::string>               v;
    list<dis::Disassembly_Node>::iterator  it_dn;  
    std::list<dis::Import>::iterator       it_i;
    std::list<dis::Reference>::iterator    it_r;

    //////////////////////////////////////////////////////////////


    Gui_Command(GUI_COMMAND_STATUSBAR_PUSH, "opening from file");

    // first abandon all previously collected data
    Initialize();

    // initialize temporary data
    Initialize(&dn);            Initialize(&ref);       Initialize(&import);
    Initialize(&var);           Initialize(&instr);    
    Initialize(&rmp);  
    displ = 0; call = 0; prev_routine = routine = 0; prev_param = param = 0;

    // then try and reload the analysis
    i_file.open(input_file.c_str());
    if (!i_file) { return RET_ERR_FILE_INPUT; }

    // initiate state machine
    import_state = 0;   api_state = 0;  routine_state = 0;  
    rmp_state = 0;      sub_state = 0;  prev_state    = 0;  
    var_state = 0;          state = 0; 

    opcodes = 0;

    stop = new_state = false;
    while ((getline(i_file, s)) && (!stop))
    {
     // recognize new state   
     if (s == DISASSEMBLY_SEPARATOR_FILE)
     { new_state = true; state = DISASSEMBLY_STATE_FILE; }

     else if (s == DISASSEMBLY_SEPARATOR_FILESIZE)
     { new_state = true; state = DISASSEMBLY_STATE_FILESIZE; }

     else if (s == DISASSEMBLY_SEPARATOR_STATISTICS)
     { new_state = true; state = DISASSEMBLY_STATE_STATISTICS; }

     else if (s == DISASSEMBLY_SEPARATOR_DISASSEMBLY)
     { new_state = true; state = DISASSEMBLY_STATE_DISASSEMBLY; }

     else if (s == DISASSEMBLY_SEPARATOR_LDN)
     { new_state = true; state = DISASSEMBLY_STATE_LDN; }

     else if (s == DISASSEMBLY_SEPARATOR_NODE)
     { new_state = true; api_state = false; state = DISASSEMBLY_STATE_NODE; }

     else if (s == DISASSEMBLY_SEPARATOR_INSTRUCTION)
     { state = DISASSEMBLY_STATE_INSTRUCTION; }      

     else if (s == DISASSEMBLY_SEPARATOR_MNEMONIC)
     { state = DISASSEMBLY_STATE_MNEMONIC; }      

     else if (s == DISASSEMBLY_SEPARATOR_REGMEMPART)
     { state = DISASSEMBLY_STATE_REGMEMPART; }      

     else if (s == DISASSEMBLY_SEPARATOR_DISPLACEMENT)
     { state = DISASSEMBLY_STATE_DISPLACEMENT; }      

     else if (s == DISASSEMBLY_SEPARATOR_JUMPTABLE)
     { state = DISASSEMBLY_STATE_JUMPTABLE; }      

     else if (s == DISASSEMBLY_SEPARATOR_CALL)
     { state = DISASSEMBLY_STATE_CALL; }      

     else if (s == DISASSEMBLY_SEPARATOR_CALL_NAME)
     { state = DISASSEMBLY_STATE_CALL_NAME; }      

     else if (s == DISASSEMBLY_SEPARATOR_ROUTINE)
     { state = DISASSEMBLY_STATE_ROUTINE; }      

     else if (s == DISASSEMBLY_SEPARATOR_ROUTINE_NAME)
     { state = DISASSEMBLY_STATE_ROUTINE_NAME; }      

     else if (s == DISASSEMBLY_SEPARATOR_TO_EXPLORE)
     { new_state = true; state = DISASSEMBLY_STATE_EXPLORE; }      

     else if (s == DISASSEMBLY_SEPARATOR_TO_EXPLORE_UNCERTAIN)
     { new_state = true; state = DISASSEMBLY_STATE_EXPLORE_UNCERTAIN; }             

     else if (s == DISASSEMBLY_SEPARATOR_IMPORT_NAME)
     { state = DISASSEMBLY_STATE_IMPORT_NAME; }  

     else if (s == DISASSEMBLY_SEPARATOR_LIB_NAME)
     { state = DISASSEMBLY_STATE_LIB_NAME; }                        

     else if (s == DISASSEMBLY_SEPARATOR_IMPORT)
     { new_state = true; state = DISASSEMBLY_STATE_IMPORT; }            

     else if (s == DISASSEMBLY_SEPARATOR_IMPORT_ROUTINE)
     { import_state = true; state = DISASSEMBLY_STATE_IMPORT_ROUTINE; } 

     else if (s == DISASSEMBLY_SEPARATOR_VARIABLE_NAME)
     { state = DISASSEMBLY_STATE_VARIABLE_NAME; }  

     else if (s == DISASSEMBLY_SEPARATOR_VARIABLE)
     { new_state = true; state = DISASSEMBLY_STATE_VARIABLE; }            

     else if (s == DISASSEMBLY_SEPARATOR_LI)
     { new_state = true; state = DISASSEMBLY_STATE_LI; }            

     else if (s == DISASSEMBLY_SEPARATOR_LR)
     { new_state = true; state = DISASSEMBLY_STATE_LR; }      

     else if (s == DISASSEMBLY_SEPARATOR_LV)
     { new_state = true; state = DISASSEMBLY_STATE_LV; }      

     else if (s == DISASSEMBLY_SEPARATOR_REFERENCE_REF)
     { new_state = true; state = DISASSEMBLY_STATE_REFERENCE_REF; }      

     else if (s == DISASSEMBLY_SEPARATOR_REFERENCE_DN_IN)
     { state = DISASSEMBLY_STATE_REFERENCE_DN_IN; }                  

     else if (s == DISASSEMBLY_SEPARATOR_REFERENCE_DN_OUT)
     { state = DISASSEMBLY_STATE_REFERENCE_DN_OUT; }                  

     else if (s == DISASSEMBLY_SEPARATOR_REFERENCE_IT_OUT)
     { state = DISASSEMBLY_STATE_REFERENCE_IT_OUT; }            

     else if (s == DISASSEMBLY_SEPARATOR_REFERENCE_IT_IMPORT)
     { state = DISASSEMBLY_STATE_REFERENCE_IT_IMPORT; }             

     else if (s == DISASSEMBLY_SEPARATOR_REFERENCE_IT_VARIABLE)
     { state = DISASSEMBLY_STATE_REFERENCE_IT_VARIABLE; }             

     else if (s == DISASSEMBLY_SEPARATOR_REFERENCE_IT_DN)
     { state = DISASSEMBLY_STATE_REFERENCE_IT_DN; } 

     else if (s == DISASSEMBLY_SEPARATOR_REFERENCE_STRING)
     { state = DISASSEMBLY_STATE_REFERENCE_STRING; } 

     else if (s == DISASSEMBLY_SEPARATOR_INPUT)
     {
      state = DISASSEMBLY_STATE_INPUT;
      routine_state = DISASSEMBLY_STATE_INPUT;
      prev_param = 0;
     }      

     else if (s == DISASSEMBLY_SEPARATOR_OUTPUT)
     {
      state = DISASSEMBLY_STATE_OUTPUT;
      routine_state = DISASSEMBLY_STATE_OUTPUT;
      prev_param = 0;
     }                                        

     else if (s == DISASSEMBLY_SEPARATOR_PARAMETER)
     { state = DISASSEMBLY_STATE_PARAMETER; }      

     else if (s == DISASSEMBLY_SEPARATOR_PARAMETER_NAME)
     { state = DISASSEMBLY_STATE_PARAMETER_NAME; }      

     else if (s == DISASSEMBLY_SEPARATOR_PARAMETER_TYPE_NAME)
     { state = DISASSEMBLY_STATE_PARAMETER_TYPE_NAME; }      

     else if (s == DISASSEMBLY_SEPARATOR_COMMENT)
     { state = DISASSEMBLY_STATE_COMMENT; }      

     else if (s == DISASSEMBLY_SEPARATOR_LABEL)
     { state = DISASSEMBLY_STATE_LABEL; }      

     else if (s == DISASSEMBLY_SEPARATOR_VAPI)
     { new_state = true; state = DISASSEMBLY_STATE_VAPI; }      

     else if (s == DISASSEMBLY_SEPARATOR_API)
     { new_state = true; api_state = true; state = DISASSEMBLY_STATE_API; }      

     else if (s == DISASSEMBLY_SEPARATOR_END)
     { new_state = true; stop = true; }

     // treat current state
     else
     {
      sub_state++;

      switch (state) 
         {                                               
          case DISASSEMBLY_STATE_FILE:            // name of disassembled file
            {
             input_file = s;
             break;
            }
          case DISASSEMBLY_STATE_FILESIZE:       // file size
            {
             i = atoi(s.c_str());
             if (i > 0) {input_file_size = i;}

             opcodes = (char*) mp.Use_Pool(input_file_size + 1); // will contain opcodes

             break;
            }
         case DISASSEMBLY_STATE_STATISTICS:      // statistics
           {
            u.parse_string(&s, &v);
            if (v.size() >= 3)
            {
             i = atoi(v[0].c_str()); completed_phase = i;
             i = atoi(v[1].c_str()); range_offset_low = i;
             i = atoi(v[2].c_str()); range_offset_high = i;

             break;
            }
           }
            
         case DISASSEMBLY_STATE_DISASSEMBLY:     // type of disassembly
           {
            i = atoi(s.c_str());
            if (i > 0) {disassembly_type = i;}
            break;
           }
          case DISASSEMBLY_STATE_NODE:            // Disassembly_Node 
            {
             switch (sub_state)
             {
              case 1:                               // general data
              {
               u.parse_string(&s, &v);
               if (v.size() >= 6)
               {
                i = atoi(v[1].c_str()); dn.file_offset = i;
                i = atoi(v[2].c_str()); dn.memory_offset = i;
                i = atoi(v[3].c_str()); dn.type = i;
                i = atoi(v[4].c_str()); dn.status = i;
                i = atoi(v[5].c_str()); dn.n_used = i;
               }
               break;
              }      
              case 2:                               // opcodes
              {
               u.parse_string(&s, &v);
               if (v.size() >= (uint) dn.n_used)
               {                                   
                if (opcodes == 0)           
                { dn.opcode = (char*) mp.Use_Pool(dn.n_used + 1); }
                else 
                { dn.opcode = opcodes; }

                i = 0;
                while (v.size() > 0)
                 {
                  j = u.hexstring_to_int(&(v[0]));      // alpha to hex
                  *opcodes = j & 0xff; 

                  opcodes++;
                  v.erase(v.begin());    
                 }                         
               }
               break;
              }            
             }
             break;
            }
          case DISASSEMBLY_STATE_INSTRUCTION:     // Instruction
            {
             Initialize(&instr);
             rmp_state = 0;
             u.parse_string(&s, &v);
             if (v.size() >= 4)
             {
              i = atoi(v[0].c_str()); instr.operand_size = i;
              i = atoi(v[1].c_str()); instr.coprocessor = i;
              i = atoi(v[2].c_str()); instr.delimiter = i;
              i = atoi(v[3].c_str()); instr.n_pushed = i;
             }
             break;
            }                                            
          case DISASSEMBLY_STATE_MNEMONIC:        // Mnemonic
            {
             instr.mnemonic = (char*) mp.Use_Pool(s.size() + 1);
             strcpy(instr.mnemonic, s.c_str());
             break;
            }                                            

          case DISASSEMBLY_STATE_REGMEMPART:      // One detail of an instruction
            {
             u.parse_string(&s, &v);
             if (v.size() >= 8)
             {
              Initialize(&rmp);          
              i = atoi(v[1].c_str()); rmp.reg08 = i;
              i = atoi(v[2].c_str()); rmp.reg16 = i;
              i = atoi(v[3].c_str()); rmp.reg32 = i;
              i = atoi(v[4].c_str()); rmp.fp_reg = i;
              i = atoi(v[5].c_str()); rmp.abs = i;
              i = atoi(v[6].c_str()); rmp.used = i;
              i = atoi(v[7].c_str()); rmp.imm = i;
              rmp.displ = 0;

              i = atoi(v[0].c_str()); 
              switch (i)
              {
               case 1:                
               { instr.part1 = rmp; rmp_state = 1; break; }          
               case 2:                
               { instr.part2 = rmp; rmp_state = 2; break; }          
               case 3: 
               { 
                instr.part3 = (RegMemPart*) mp.Use_Pool(sizeof(RegMemPart));
                rmp_state = 3;

                instr.part3->reg08 = rmp.reg08; instr.part3->reg16  = rmp.reg16; 
                instr.part3->reg32 = rmp.reg32; instr.part3->fp_reg = rmp.fp_reg; 
                instr.part3->abs   = rmp.abs;   instr.part3->used   = rmp.used; 
                instr.part3->imm   = rmp.imm;   instr.part3->displ  = rmp.displ; 
                break;
               }          
              }
             }
             break;
            }
          case DISASSEMBLY_STATE_DISPLACEMENT:      // One further detail of an instruction
            {
             switch (rmp_state)
              {
               case 1: 
               { instr.part1.displ = (Displacement*) mp.Use_Pool(sizeof(Displacement)); displ = instr.part1.displ; break; }          
               case 2:
               { instr.part2.displ = (Displacement*) mp.Use_Pool(sizeof(Displacement)); displ = instr.part2.displ; break; }          
               case 3:
                { if (instr.part3) 
                    {
                      instr.part3->displ = (Displacement*) mp.Use_Pool(sizeof(Displacement));
                      displ = instr.part3->displ;
                    }
                  break;
                }
              }

             u.parse_string(&s, &v);
             if (v.size() >= 11)
             {
              Initialize(&rmp);          
              i = atoi(v[0].c_str()); displ->mul = i;
              i = atoi(v[1].c_str()); displ->add = i;
              i = atoi(v[2].c_str()); displ->reg2 = i;
              i = atoi(v[3].c_str()); displ->mul2 = i;
              i = atoi(v[4].c_str()); displ->add2 = i;
              i = atoi(v[5].c_str()); displ->seg_override = i;
              i = atoi(v[6].c_str()); displ->seg_offset = i;
              i = atoi(v[7].c_str()); displ->seg_reg = i;
              i = atoi(v[8].c_str()); displ->contr_reg = i;
              i = atoi(v[9].c_str()); displ->debug_reg = i;
              i = atoi(v[10].c_str()); displ->test_reg = i;
             }
             break;
            }                                            
          case DISASSEMBLY_STATE_JUMPTABLE:      // One further detail of an instruction
            {
              i = atoi(s.c_str()); 
              jump_table.jump_to = i;
              instr.jump_table = jump_table;
              break;
            }                                            
          case DISASSEMBLY_STATE_CALL:           // One further detail of an instruction
            {
              instr.call = (Call*) mp.Use_Pool(sizeof(Call));
              Initialize(instr.call);
              i = atoi(s.c_str()); 
              instr.call->type_of_call = i;
              break;
            }                                            
          case DISASSEMBLY_STATE_CALL_NAME:      // One further detail of an instruction
            {
              instr.call->name = (char*) mp.Use_Pool(s.size() + 1); 
              strcpy(instr.call->name, s.c_str());
              break;
            }                                            
          case DISASSEMBLY_STATE_ROUTINE_NAME:   // One further detail of an instruction
            {
              routine_found = false;

              if (api_state == true)
              {
               api.routine = (Routine*) mp.Use_Pool(sizeof(Routine));
               routine = api.routine;
               Initialize(routine);
              }
              else 
              {
               routine = Get_Routine_From_Name(0, s.c_str());

               if (routine == 0)
               {
                if (import_state == true)
                {
                 import.routine = (Routine*) mp.Use_Pool(sizeof(Routine));
                 routine = import.routine;     
                }
                else 
                {
                 instr.call->routine = (Routine*) mp.Use_Pool(sizeof(Routine)); 
                 routine = instr.call->routine;    
                }
               }
               else 
               {
                routine_found = true;

                if (import_state == true)
                { import.routine = routine; }
                else 
                { instr.call->routine = routine; }
               }
              } 

              if (!routine_found)
              {
               routine->name = (char*) mp.Use_Pool(s.size() + 1);          
               strcpy(routine->name, s.c_str());
              }

              break;
            }                                            
          case DISASSEMBLY_STATE_PARAMETER:          // One further detail of an instruction
            {
             if (routine_state == DISASSEMBLY_STATE_ROUTINE_KNOWN) {break;}

             param = (Parameter*) mp.Use_Pool(sizeof(Parameter));
             Initialize(param);

             if (prev_param == 0)
             {
              if (routine_state == DISASSEMBLY_STATE_INPUT)
              { if (routine){routine->input = param; }}
              else if (routine_state == DISASSEMBLY_STATE_OUTPUT)
              { if (routine){routine->output = param; }}  
             }
             else
             { prev_param->next = param; }
             prev_param = param;

             i = atoi(s.c_str()); 
             param->type = i;

             break;
            }
          case DISASSEMBLY_STATE_PARAMETER_NAME:   // One further detail of an instruction
            {
              if (routine_state == DISASSEMBLY_STATE_ROUTINE_KNOWN) {break;}

              if (param) { strcpy(param->name, s.c_str()); }
              break;
            }                                            
          case DISASSEMBLY_STATE_PARAMETER_TYPE_NAME:   // One further detail of an instruction
            {
              if (routine_state == DISASSEMBLY_STATE_ROUTINE_KNOWN) {break;}

              if (param) { strcpy(param->type_name, s.c_str()); }
              break;
            }                                          
          case DISASSEMBLY_STATE_COMMENT:                // Comment
            {
             dn.comment = (char*) mp.Use_Pool(s.size() + 1);           
             strcpy(dn.comment, s.c_str());
             break;
            }                                          
          case DISASSEMBLY_STATE_LABEL:                  // One further detail of an instruction
            {
             dn.label = (char*) mp.Use_Pool(s.size() + 1);           
             strcpy(dn.label, s.c_str());
             break;
            }                                          
          case DISASSEMBLY_STATE_REFERENCE_DN_IN:       // the ref_in for a DisassemblyNode
            {
             j = atoi(s.c_str()); 
             it_r = Get_Reference_From_Offset(j, true);
             dn.ref_in = it_r;
             break;
            }                                          
         case DISASSEMBLY_STATE_REFERENCE_DN_OUT:       // the ref_out for a DisassemblyNode
           {
            j = atoi(s.c_str()); 
            it_r = Get_Reference_From_Offset(j, true);
            dn.ref_out = it_r;
            break;
           }                                          
          case DISASSEMBLY_STATE_API:                   // new routine
            {
             u.parse_string(&s, &v);
             if (v.size() >= 2)
             { api.file_name = v[1]; }
             break;
            }                
          case DISASSEMBLY_STATE_EXPLORE:               // complete list for v_to_explore
            {
             u.parse_string(&s, &v);
             i = v.size();
             while (i > 0)
             {
              i--;
              s = v[0]; 
              v.erase(v.begin()); 
              j = atoi(s.c_str()); 
              v_to_explore.push_back(j);
             }
             break;
            }                
          case DISASSEMBLY_STATE_EXPLORE_UNCERTAIN:     // complete list for v_to_explore_uncertain
            {
             u.parse_string(&s, &v);
             i = v.size();
             while (i > 0)
             {
              i--;
              s = v[0]; 
              v.erase(v.begin()); 
              j = atoi(s.c_str()); 
              v_to_explore_uncertain.push_back(j);
             }
             break;
            }                                                 
          case DISASSEMBLY_STATE_REFERENCE_IT_IMPORT: // reference Import
            {
             u.parse_string(&s, &v);
             if (v.size() >= 3)
             { 
              ref.it_i = Get_Import_From_Name(v[1].c_str(), v[2].c_str());
             }
             break;
            }                
          case DISASSEMBLY_STATE_REFERENCE_IT_VARIABLE: // reference Variable
            {
             u.parse_string(&s, &v);
             if (v.size() >= 2)
             { 
              ref.it_v = Get_Variable_From_Name(v[1].c_str());
             }
             break;
            }                
          case DISASSEMBLY_STATE_REFERENCE_STRING:   // reference label
            {
             ref.label = s;
             break;
            }                
          case DISASSEMBLY_STATE_REFERENCE_IT_DN:    // reference Disassembly Node
            {
             u.parse_string(&s, &v);
             if (v.size() >= 2)
             { 
              j = atoi(v[1].c_str());
              ref.it_dn = Get_Disassembly_Node_From_Offset(j, true);
             }
             break;
            }                
          case DISASSEMBLY_STATE_REFERENCE_REF:    // reference basic data
            {
             u.parse_string(&s, &v);
             i = v.size();
             if (i >= 3)
             {
              Initialize(&ref);

              j = atoi(v[1].c_str()); 
              ref.memory_offset = j;

              j = atoi(v[2].c_str()); 
              ref.type = j;
             }
             break;
            }                
          case DISASSEMBLY_STATE_REFERENCE_IT_OUT:    // reference out
            {
             u.parse_string(&s, &v);
             i = v.size();
             first = true;
             while (i > 0)
             {
              i--;
              s = v[0]; 
              v.erase(v.begin()); 
              j = atoi(s.c_str()); 

              if (first == false)                   // first entry is just the total number of entries
              { ref.ref_out.push_back(j); }
              
              first = false;
             }
             break;
            }                                                      
         case DISASSEMBLY_STATE_LIB_NAME:            // one import description
           {
            import.library = s;
            break;
           }                                         
         case DISASSEMBLY_STATE_IMPORT_NAME:          // one import description
          {
           import.name = s;
           break;
          }                        
         case DISASSEMBLY_STATE_VARIABLE_NAME:       // one variable description
           {
            var.name = s;
            break;
           }                        
         case DISASSEMBLY_STATE_IMPORT:              // one import description
            {
             u.parse_string(&s, &v);
             if (v.size() >= 2)
             { 
              j = atoi(v[1].c_str()); 
              import.memory_offset = j;
             }
             break;
            }                        
         case DISASSEMBLY_STATE_VARIABLE:            // one variable description
           {
            u.parse_string(&s, &v);
            if (v.size() >= 2)
            { 
             j = atoi(v[1].c_str()); 
             var.memory_offset = j;
            }
            break;
           }                        
         }
     }

     // treat previous state   
     if (new_state == true)
     {
      switch (prev_state) 
         {                                               
          case DISASSEMBLY_STATE_NODE:            // add Disassembly_Node to l_dn
            {
             dn.instruction = instr;
             l_dn.insert(l_dn.end(), dn);
             Initialize(&dn);
             break;
            }

          case DISASSEMBLY_STATE_REFERENCE_REF:   // add Reference to l_r
            {
             l_r.insert(l_r.end(), ref);
             Initialize(&ref);
             break;
            }

          case DISASSEMBLY_STATE_IMPORT:          // add Import to l_i
            {
             l_i.insert(l_i.end(), import);
             Initialize(&import);
             break;
            }

          case DISASSEMBLY_STATE_VARIABLE:        // add Variable to l_v
            {
             l_v.insert(l_v.end(), var);
             Initialize(&var);
             break;
            }
    
          case DISASSEMBLY_STATE_API:             // add Routine to v_api
            {
             v_api.push_back(api);
             Initialize(&api);  
             break;
            }
         }

      prev_state = state;

      sub_state = 0; 

      new_state = false;
     }
    }                                   

    i_file.close();

    /*
    Gui_Command(GUI_COMMAND_INIT);
    */

    Gui_Command(GUI_COMMAND_STATUSBAR_POP);

    return RET_OK; 
}

#ifdef LOGGING
void
dis::Disassembly::Debug_Show_It(int debug_command, list<Disassembly_Node>::iterator it)                
{
    int j;
    short t;
    ostringstream ss;

    //////////////////////////////////////////////////////////////////////////

    debug_log.Add_To_Log(debug_command);

    ss.setf(ios_base::hex, ios_base::basefield);  

    ss << "* n_used "        << it->n_used << "\n";        

    ss   << "offset "          << it->file_offset   
         << "* memory_offset " << it->memory_offset 
         << "* type "          << it->type
         << "* opcode "       ;
        
    for (j=0; (j < it->n_used); j++)
            {
             t = (it->opcode[j]) & 0xff;
             ss << "0x" << t << " ";
            }                  

    ss.setf(ios_base::dec, ios_base::basefield);  
    
    ss << "* status "       << it->status;

    ss  << "* instruction " ;
    if (it->instruction.mnemonic) {ss << it->instruction.mnemonic;}
        else {ss << " - ";}

    ss << "* comment " ;
    if (it->comment) {ss << it->comment;}
        else {ss << " - ";}

    ss << "* label ";
    if (it->label) {ss << it->label;}
        else {ss << " - ";}

    /*
    ss << " * ref_in ";
    if (it->ref_in != l_r.end()) {ss << it->ref_in->memory_offset;}
        else {ss << " - ";}
    
    ss << " * ref_out";
    if (it->ref_out != l_r.end()) {ss << it->ref_out->memory_offset;}
        else {ss << " - ";}
    */    

    debug_log.Add_To_Log(ss.str());
}

void
dis::Disassembly::Debug_Show_To_Explore(int debug_status)
{
    uint            i;
    int             j;
    ostringstream   ss;

    ////////////////////////////////////////////////////////////////
                                                  
    debug_log.Add_To_Log(LOG_COMMAND_SEP1);
    debug_log.Add_To_Log("Debug_Show_To_Explore, n = " + u.int_to_string(v_to_explore.size()));

    ss.setf(ios_base::hex, ios_base::basefield);  

    for (i = 0; i < v_to_explore.size(); i++)
    {
        j = v_to_explore[i];
        
        ss 
         << "vte "
         << i                  << " : "                     
         << j                  << " / " << v_to_explore.size();
        
        debug_log.Add_To_Log(ss.str());

        ss.str("");
    }                            

    debug_log.Add_To_Log(LOG_COMMAND_SEP1);
    debug_log.Add_To_Log("Debug_Show_To_Explore_Uncertain, n = " + u.int_to_string(v_to_explore_uncertain.size()));

    for (i = 0; i < v_to_explore_uncertain.size(); i++)
    {
        j = v_to_explore_uncertain[i];

        ss 
         << "vteu "
         << i                  << " : "                     
         << j                  << " / " << v_to_explore_uncertain.size();
        
        debug_log.Add_To_Log(ss.str());

        ss.str("");
    }                            

}

void
dis::Disassembly::Debug_Show_Ldn(int debug_ldn)
{
    list<Disassembly_Node>::iterator it;

    int             i, j;
    short           t;
    ostringstream   ss;

    ////////////////////////////////////////////////////////////////
                     
    debug_log.Add_To_Log(LOG_COMMAND_SEP1);
    debug_log.Add_To_Log("Debug_Show_Ldn, size = " + u.int_to_string(l_dn.size()));
    debug_log.Add_To_Log("Debug_Show_Ldn, ref  = " + u.int_to_hexstring(debug_ldn));

    ss.setf(ios_base::hex, ios_base::basefield);  

    i = 0;                                        

    for (it = l_dn.begin(); it != l_dn.end(); it++)
    {
      if (   (debug_ldn == 0)
          || (   ( debug_ldn - 100 < it->memory_offset)
              && ( debug_ldn + 100 > it->memory_offset)))
      {
        i++;
        ss 
         << i                  << ":"                     
         << "file offset "     << it->file_offset   
         << "* memory_offset " << it->memory_offset 
         << "* type "          << it->type
         << "* opcode "        << &(it->opcode)
         << "* ";

        for (j=0; j < it->n_used; j++)
        {
          t = (it->opcode[j]) & 0xff;
          ss << "0x" << t ;
          if (u.char_is_printable(it->opcode[j]))
          { ss << "(" << it->opcode[j] << ")"; }
          ss << " ";
        }   

        ss << "* n_used "       << it->n_used;

        ss << "* section "      << it->section;

        ss << "* status "       << it->status;

        ss << "* instruction " ;
        if (it->instruction.mnemonic) {ss << it->instruction.mnemonic;}
        else {ss << " - ";}

        ss << "* comment " ;
        if (it->comment) {ss << it->comment;}
        else {ss << " - ";}

        ss << "* label ";
        if (it->label) {ss << it->label;}
        else {ss << " - ";}

        ss << " * ref in";
        if (it->ref_in != l_r.end()) {ss << it->ref_in->memory_offset;}
        else {ss << " - ";}

        ss << " * ref out";
        if (it->ref_out != l_r.end()) {ss << it->ref_out->memory_offset;}
        else {ss << " - ";}

        debug_log.Add_To_Log(ss.str()); 
        ss.str("");
      }
    }                            
}

void
dis::Disassembly::Debug_Show_Vapi(int debug_status)
{
    vector<Api>::iterator   it; 

    Routine                *r; 

    Parameter              *p; 

    uint                    i;

    ostringstream ss;

    ////////////////////////////////////////////////////////////////

    ss.setf(ios_base::hex, ios_base::basefield);  

    i = 0;

    ss << "Debug_Show_Vapi" << " : " << v_api.size() << " = v_api.size " << "\n";  
    debug_log.Add_To_Log(ss.str());
    ss.str("");

    for (it = v_api.begin(); it != v_api.end(); it++)
    {
        i++;

        ss << i << " : " << it->file_name << "\n";  
        debug_log.Add_To_Log(ss.str());
        ss.str("");

        r = it->routine;

        while (r != 0)
        {
         ss << r->name << "\n";  

         ss << "input : " << "\n";  

         p = r->input;
         while (p != 0)
         {
             if (p->name != 0) { ss << "; name = " << p->name;}
             ss << "; type = " << p->type;
             if (p->type_name != 0) { ss << "; type_name = " << p->type_name;}
             ss << "\n";      

             p = p->next;    
         }

         ss << "output : " << "\n";  

         p = r->output;
         while (p != 0)
         {
           if (p->name != 0) { ss << "; name = " << p->name;}
           ss << "; type = " << p->type;
           if (p->type_name != 0) { ss << "; type_name = " << p->type_name;}
           ss << "\n";      

           p = p->next;    
         }

         debug_log.Add_To_Log(ss.str());
         ss.str("");

         r = r->next;
        }
    }                            

    ss << "End Debug_Show_Vapi" << "\n";  
    debug_log.Add_To_Log(ss.str());
    ss.str("");
}

void
dis::Disassembly::Debug_Show_Lr(int debug_status)
{
    list<Reference>::iterator it;

    uint i,j;
    long t;

    ostringstream ss;

    ////////////////////////////////////////////////////////////////

    ss.setf(ios_base::hex, ios_base::basefield);  

    i = 0;

    for (it = l_r.begin(); it != l_r.end(); it++)
    {
        i++;
        ss 
         << i                  << ":"                     
         << "* memory_offset " << it->memory_offset 
         << "* label "         << it->label << "\n";  
        
        ss << "ref_out: ";
        j = it->ref_out.size();
        for (j=0; (j < it->ref_out.size()); j++)
        {
          t = (it->ref_out[j]);
          ss << u.int_to_hexstring(t) << " ";
        }          

        debug_log.Add_To_Log(ss.str());
        ss.str("");
    }                            
}

void
dis::Disassembly::Debug_Show_Li(int debug_status)
{
    list<Import>::iterator it;

    uint i;

    ostringstream ss;

    ////////////////////////////////////////////////////////////////

    ss.setf(ios_base::hex, ios_base::basefield);  

    i = 0;

    for (it = l_i.begin(); it != l_i.end(); it++)
    {
        i++;
        ss 
         << i                  << ":"                     
         << "* memory_offset " << it->memory_offset 
         << "* library "       << it->library 
         << "* name "          << it->name << "\n";  
        
        debug_log.Add_To_Log(ss.str());
        ss.str("");
    }                            
}

#endif






