/*
 * utility program that reads system headers and creates a description of imported functions
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "read_imports.hh"

int File_Select_Headers(const struct direct   *entry);                  
int File_Select_Idts(const struct direct   *entry);                  
                 
int main(int argc, char **argv)
{  
   int                              return_code;

   prep::Read_Windows_Imports       rwi;                

   ////////////////////////////////////////////////////

   return_code = rwi.Init();

   if (return_code == RET_OK)
   {
    rwi.Load_Idts();
    rwi.Load_Functions();        

    rwi.Write_Statistics();
   }

   return 0;
}

prep::Read_Windows_Imports::Read_Windows_Imports() 
{
    std::cout << "Constructor Read_Windows_Imports()" << "\n";

    n_types = longest_type = 0;
}

prep::Read_Windows_Imports::~Read_Windows_Imports()
{
    std::cout << "Destructor Read_Windows_Imports" << "\n";

    intel_winpe_file.close();
    intel_winpe_stats.close();
}

int
prep::Read_Windows_Imports::Init()
{
   intel_winpe_file.open("intel_winpe_imports");
   intel_winpe_stats.open("intel_winpe_imports_stats");

   if (   (intel_winpe_file.is_open())
       && (intel_winpe_stats.is_open())
      )
   { return RET_OK; }
   else 
   { return RET_ERR_GENERAL; }
}


void
prep::Read_Windows_Imports::Load_Idts()
{
    int i;                  // index

    char *c;                // temp value

    struct direct **idts;

    //////////////////////////////////////////

    count = scandir(READ_IMPORT_DIR, &idts, &File_Select_Idts, alphasort);             
    std::cout << "count .idt = " << count << "\n";

    i = 0;
    while (i < count)
    {
     c = idts[i]->d_name;

     if (c)
     { Load_Idts_From_File(idts[i]->d_name); }

     free(idts[i]);
     i++;
    }

    free(idts);
}

void
prep::Read_Windows_Imports::Load_Functions()
{
    int i;                  // index

    char *c;                // temp value

    struct direct **headers;

    //////////////////////////////////////////

    count = scandir(READ_IMPORT_DIR, &headers, &File_Select_Headers, alphasort );    
    std::cout << "count .h = " << count << "\n";

    i = 0;
    while (i < count)
    {
     c = headers[i]->d_name;

     if (c)
     { Load_Functions_From_File(headers[i]->d_name); }

     free(headers[i]);
     i++;
    }

    free(headers);
}

void
prep::Read_Windows_Imports::Load_Functions_From_File(std::string file_name)
{
  int                        previous_state, state, 
                             n_typedef, n_interface;

  bool                       struct_found;

  std::string                line, t;
  std::ifstream              input_file;
  std::vector<std::string>   lines, typedef_lines;

  ////////////////////////////////

  std::cout << file_name << "\n";

  struct_found = false;
  previous_state = state = READ_IMPORT_STATE_INIT;
  n_typedef = n_interface = 0;

  t = READ_IMPORT_DIR;
  t += '/' + file_name;

  input_file.open(t.c_str());

  if (input_file.is_open())
  {
    while (! input_file.eof() )
    {
     getline (input_file, line);

     //debug
     //{ cout << line << "\n";}

     line = u.replace_all_occurrences_in_string(line, ";", " ; ", 1, 3);
     line = u.replace_all_occurrences_in_string(line, "(", " ( ", 1, 3);
     line = u.replace_all_occurrences_in_string(line, ")", " ) ", 1, 3);
     line = u.replace_all_occurrences_in_string(line, ",", " , ", 1, 3);
     line = u.replace_all_occurrences_in_string(line, "\r", "", 1, 0);
     line = u.replace_all_occurrences_in_string(line, "\n", "", 1, 0);
     line = u.replace_all_occurrences_in_string(line, "\f", "", 1, 0);

     if (strstr(line.c_str(), "/*"))
     {
      if (!strstr(line.c_str(), "*/"))
      { 
       previous_state = state; 
       state = READ_IMPORT_STATE_COMMENT; 
       line.replace(line.find("/*"), line.size(), ""); 
      }
      else 
      { line.replace(line.begin() + line.find("/*"), line.begin() + line.find("*/") + 2, ""); }
     }
     else if (state == READ_IMPORT_STATE_COMMENT)
     {                          
      if (strstr(line.c_str(), "*/"))
      {
       state = previous_state;                         
       line.replace(line.begin(), line.begin() + line.find("*/") + 2, ""); 
      }
      else 
      { line.clear(); }   
     }

     if (line.find("//") != string::npos)
     { line.replace(line.find("//"), line.size(), ""); }

     if (   (line.size() > 0) 
         && (line.find_first_not_of(" ") != string::npos)
         && (line !=  "\n")
         && (line !=  "\r")
         && (!strstr(line.c_str(), "DEFINE_"))
         && (!strstr(line.c_str(), "#define"))
         && (!strstr(line.c_str(), "#undef"))
         && (!strstr(line.c_str(), "#ifdef"))
         && (!strstr(line.c_str(), "#endif"))
         && (!strstr(line.c_str(), "#if"))
         && (!strstr(line.c_str(), "#ifndef"))
         && (!strstr(line.c_str(), "#include"))
         && (!strstr(line.c_str(), "#else"))
         && (!strstr(line.c_str(), "#pragma"))
         && (state != READ_IMPORT_STATE_IGNORE)
        )
     {
      switch (state)
      {
       case READ_IMPORT_STATE_INIT:                            
       {                          
        if (strstr(line.c_str(), "typedef"))
          {
           if (!strstr(line.c_str(), ";"))
           { 
             if (state != READ_IMPORT_STATE_TYPEDEF)
             { n_typedef = 0; }

             state = READ_IMPORT_STATE_TYPEDEF;
             typedef_lines.push_back(line);

             struct_found = (strstr(line.c_str(), "struct"));
           }      
           if (strstr(line.c_str(), "{"))
           { n_typedef++; }      
          }
        
        else if ((strstr(line.c_str(), "extern") && (strstr(line.c_str(), "C"))))
          { state = READ_IMPORT_STATE_EXTERN_C; }

        else if (   (strstr(line.c_str(), "interface"))
                 || (strstr(line.c_str(), "INTERFACE"))
                )
          {
           if (!strstr(line.c_str(), ";"))
           { 
             if (state != READ_IMPORT_STATE_INTERFACE)
             { n_interface = 0; }

             state = READ_IMPORT_STATE_INTERFACE;
           }      
           if (strstr(line.c_str(), "{"))
           { n_interface++; }      
          }
        else if (    (strstr(line.c_str(), ";"))
                  && (!strstr(line.c_str(), ")"))
                )
          { continue; }
        else 
          {
           state = READ_IMPORT_STATE_FUNCTION; 
           lines.push_back(line);

           if (strstr(line.c_str(), ";"))
           {
               state = READ_IMPORT_STATE_INIT; 

               Add_Import(&lines);
               lines.clear();
           }
          }     
        break;
       }
       case READ_IMPORT_STATE_TYPEDEF:                            
       {                         
        if (strstr(line.c_str(), "}"))
           { n_typedef--; }      
        if (strstr(line.c_str(), "{"))
           { n_typedef++; }      
        if (strstr(line.c_str(), "struct"))
           { struct_found = true; }

        typedef_lines.push_back(line);

        if ((strstr(line.c_str(), ";")) && (n_typedef <= 0))
        {
         state = READ_IMPORT_STATE_INIT;

         if (struct_found == true)
         { Add_Typedef(&typedef_lines); }
         
         typedef_lines.clear();
        }

        break;
       }
       case READ_IMPORT_STATE_FUNCTION:                            
       {        
        if (strstr(line.c_str(), "{"))
        { state = READ_IMPORT_STATE_FUNCTION_CODE; }
        else 
        {
         lines.push_back(line);

         if (strstr(line.c_str(), ";"))
         {
          state = READ_IMPORT_STATE_INIT; 

          Add_Import(&lines);
          lines.clear();
         }
        }

        break;
       }
       case READ_IMPORT_STATE_FUNCTION_CODE:                            
       {        
        if (strstr(line.c_str(), "}"))
        { state = READ_IMPORT_STATE_INIT; 

          Add_Import(&lines);
          lines.clear();
        }

       break;
      }
       case READ_IMPORT_STATE_EXTERN_C:                            
       {                          
        if (   (strstr(line.c_str(), ";"))
            || (strstr(line.c_str(), "endif"))
           )
        { state = READ_IMPORT_STATE_INIT; }

        break;
       }
      }
     }
     else
     {
      if (strstr(line.c_str(), "\\"))
      { state = READ_IMPORT_STATE_IGNORE; }

      else if (state == READ_IMPORT_STATE_IGNORE)
       { state = READ_IMPORT_STATE_INIT; }

      else if (state == READ_IMPORT_STATE_EXTERN_C)
       { if (   (strstr(line.c_str(), ";"))
             || (strstr(line.c_str(), "endif"))
            )
         { state = READ_IMPORT_STATE_INIT; }
       }
     }
    }
    input_file.close();
  }       
}

void
prep::Read_Windows_Imports::Load_Idts_From_File(std::string file_name)
{
  std::string                line, t;
  std::ifstream              input_file;

  std::string                dll_name;

  std::vector<std::string>   function_lines;

  pair <string, vector<string> >  p_function_names;  

  ////////////////////////////////

  std::cout << file_name << "\n";

  t = READ_IMPORT_DIR;
  t += '/' + file_name;

  input_file.open(t.c_str());

  if (input_file.is_open())
  {
    while (! input_file.eof() )
    {
     getline (input_file, line);

     //std::cout << line  << "\n";

     if (line.find("\r") != string::npos)
      { line.replace(line.find("\r"), 1, "", 0); }

     if (   (line.find("0 Name=") != string::npos)
         && (line.find(".dll") != string::npos)
        )
     {
      line.replace(line.begin(), line.begin() + line.find("=") + 1, ""); 

      line.replace(line.begin() + line.find(".dll") + 4, line.end(), ""); 

      dll_name = line;
     }
     else if (line.find("Name=") != string::npos)
     {
      line.replace(line.begin(), line.begin() + line.find("=") + 1, ""); 

      function_lines.push_back(line);
     }                              
    }

    input_file.close();

    p_function_names.first  = dll_name;
    p_function_names.second = function_lines;

    v_p_function_names.push_back(p_function_names);  
  }       
}


void
prep::Read_Windows_Imports::Add_Import(std::vector<std::string> *import_strings)
{
  int                       i, j,               // indexes 
                            s,                  //  size
                            n_brackets         // number of brackets found
                            //, s_vpt
                                    ;           

  uint                      l;

  bool                      stop, 
                            //found, 
                            argument;

  std::string               t, a,              // type, argument
                            s1, s2, s3,        // temp values
                            fn, ft, ln;        // function name, function type

  dis::Parameter            p;

  vector<std::string>       total, line, td;

  vector<std::string>::iterator 
                            it, it2;

  vector<dis::Parameter>    v_p;

 /////////////////////////////////////////////////////

 s = import_strings->size();

 for (i = 0; i < s; i++)
 {
  u.parse_string(&(import_strings->at(i)), &line);

  copy(line.begin(), line.end(), back_inserter(total));
 }
 // total now contains all elements of one function


 /*
 // first, try and replace typedefs by their true meaning
 it = total.begin();         
 stop = (it == total.end());
 i = 0;

 while (stop == false)
 {
  i++; 
  found = false; j = 0;
  s_vpt = v_p_typedef.size();

  s2 = *it;
  s3 = "*" + s2;

  while ((j < s_vpt) && (!found))
  {
   s1 = v_p_typedef[j].first;

   if (   (s1 == s2)
       || (s1 == s3)
      )
   {
    // found a typedef to be replaced
    found = true;

    it2 = it; it2++; total.erase(it); it2--;

    td = v_p_typedef[j].second;
    total.insert(it2, td.begin(), td.end());

    it = total.begin();

    i += td.size() - 1;
    for (s = 0; s < i; s++)
    { it++; }     
   }

   j++;
  }

  it ++;
  stop = (it == total.end());
 }
 */

 i = total.size() - 1;
 stop = false;

 while ((i > 0) && (stop == false))
 {
  if (total[i].find(")") != string::npos) {stop = true; }
  else { i-- ; }
 }

 // if stop is true, then we have found the last argument of a function
 if (stop)
 {
  stop = false; argument = true;
  t = a = "";
  n_brackets = 0;

  // so first work backwards toward the first argument,
  // stocking all arguments in v_p
  while ((i > 0) && (stop == false))
  {           
   if (total[i].find("(") != string::npos) 
   {
    n_brackets --;

    if (n_brackets <= 0)
    { stop = true; }
   }
   else if (total[i].find(")") != string::npos) 
   { n_brackets ++; }  

   if ((   (total[i].find(",") != string::npos)
        || ((   (total[i].find("(") != string::npos)
             //|| (total[i].find(")") != string::npos)
            )
            &&
            (n_brackets <= 0)
           )
        )
       &&
       (a != "")
      )
   {
    p.name = 0; p.type = 0; p.type_name = 0; p.next = 0;

    p.name = (char*) mp.Use_Pool(a.size() + 1);
    strcpy(p.name, a.c_str());
    
    if (t == "") { t = a; }

    p.type_name = (char*) mp.Use_Pool(t.size() + 1);
    strcpy(p.type_name, t.c_str());

    v_p.push_back(p);

    a = t = "";
    argument = true;
   }

   if  (   (total[i].find(",")  == string::npos)
        && (total[i].find("\r") == string::npos)
        && (total[i].find(";") == string::npos)
        && (n_brackets <= 1)
        && (total[i].find(")") == string::npos)
        //&& (total[i].find("(") == string::npos)
       )
   {
    if (total[i] == "*")         { a += total[i]; }
    else if (argument)           { a = total[i] + a; }
    else                         { t = total[i] + t; }

    argument = false;
   }
   else if (n_brackets > 1)
   {
    if (argument)                { a = total[i] + a; }
    else                         { t = total[i] + t; }

    if (total[i].find(")") == string::npos)
    {
     argument = false;    
    }                 
   }

   i--;                         
  }                    
  
  if (stop)
  {
   // next, determine the function's name
   fn = total[i];

   // and finally the function's return type
   for (j = 0; j < i; j++)
   {
    if (   (total[j] != "WINUSERAPI")
        && (total[j] != "WINAPIV")
        && (total[j] != "WINAPI")
       )
    { 
     ft += total[j];
     l_ftypes.push_back(ft);

     //ft += " ";
    }  
   }
  
   // now try and find the library's name from the idt files
   ln = Find_Dll_Name(fn);

   // and write out the definition of the function
   intel_winpe_file << ln       << "\t"
                    << fn       << "\t"
                    << ft       << "\t"
                    << "-"      << "\t"
                    << "input"  << "\t";

   for (l = v_p.size(); l > 0; l--)
   {
    p = v_p[l - 1];

    intel_winpe_file << p.type_name << "\t"
                     << p.name << "\t";
   }

   intel_winpe_file << UTIL_NEW_LINE;
  }                
 }       
}

void
prep::Read_Windows_Imports::Add_Typedef(std::vector<std::string> *t_strings)
{
  int                            i, j,           // indexes 
                                 s;              //  size

  bool                           stop;

  vector<std::string>            total, line, names, definitions;

  vector<dis::Parameter>         v_p;

  pair <string, vector<string> > p_typedef;  

 /////////////////////////////////////////////////////

 s = t_strings->size();

 for (i = 0; i < s; i++)
 {
  u.parse_string(&(t_strings->at(i)), &line);

  copy(line.begin(), line.end(), back_inserter(total));
 }
 // total now contains all elements of one typedef

 i = total.size() - 1;
 stop = false;

 while ((i > 0) && (stop == false))
 {
  if (total[i].find("}") != string::npos) {stop = true; }

  else
  // collect the typedef names
  {
   if (   (total[i].find(";") == string::npos)
       && (total[i].find("\r") == string::npos)
       && (total[i].find(",") == string::npos)
      )
   { names.push_back(total[i]); }

   i-- ; 
  }

  total.pop_back();
 }

 // if stop is true, then we have found the last bracket of a typedef
 if (stop)
 {
  j = i;

  i =  0;
  stop = false; 

  // so first work backwards toward the first bracket, and remember all 
  // stocking all arguments in v_p
  while ((i < j) && (stop == false))
  {           
   if (total[0].find("{") != string::npos) {stop = true; }
   i++; 

   total.erase(total.begin());
  }     
  
  if (stop)
  {          
   s = names.size();

   for (i = 0; i < s; i++)
   {
    p_typedef.first = names[i]; 
    p_typedef.second = total; 

    v_p_typedef.push_back(p_typedef);
   }
  }                
 }       
}

std::string
prep::Read_Windows_Imports::Find_Dll_Name(std::string function_name)
{
 int                                                        l1, l2, l3;    // length, length, length

 bool                                                       found;

 std::string                                                s;

 std::vector<string>::iterator                              it2;

 std::vector<pair <string, vector<string> > >::iterator     it;

 std::vector<string>                                        v_s;

 /////////////////////////////////////////////////////////////////////////


 s = "_" + function_name; 

 l1 = function_name.size();

 l3 = s.size();

 found = false;

 it = v_p_function_names.begin();  

 while (    (!found)
         && (it != v_p_function_names.end())
       )
 {
  v_s = it->second;

  it2 = v_s.begin();

  while (   (it2 != v_s.end())
         && (!found)
        )
  {
   l2 = it2->size();

   found = Compare_String_To_Function_Name(it2->c_str(), function_name.c_str(), l2, l1, false); 
   if (found) { return it->first; }   

   else 
   {   
    found = Compare_String_To_Function_Name(it2->c_str(), s.c_str(), l2, l3, false); 
    if (found) { return it->first; }   
   }

   it2++;
  }                 

  it++;
 }

 if ((   (strncmp(function_name.c_str(), "I", 1) == 0)   
     && (   (function_name.find("Proxy") != string::npos)
         || (function_name.find("Stub") != string::npos)
         )
     )                        
     || (function_name.find("to_xmit") != string::npos)
     || (function_name.find("from_xmit") != string::npos)
     || (function_name.find("free_inst") != string::npos)
     || (function_name.find("free_xmit") != string::npos)
     || (function_name.find("midl") != string::npos)
     || (function_name.find("NdrSH") != string::npos)
     || (function_name.find("NdrStub") != string::npos)
     || (function_name.find("I_Ns") != string::npos)
     || (function_name.find("CStdStub") != string::npos)
    )
 { return "GENERIC.dll"; }

 else if (function_name.find("TSPI_") != string::npos)
 { return "UNIMDM.dll"; }

 else if (function_name.find("VDM") != string::npos)
 { return "VDMDBG.dll"; }        

 else if (function_name.find("Printer") != string::npos)
 { return "SPOOL.dll"; }        

 else if (function_name.find("Wlx") != string::npos)
 { return "GINA.dll"; }        

 return "unknown.dll";   
}


bool
prep::Read_Windows_Imports::Compare_String_To_Function_Name(
                        const char *function_name, const char *input_string, int l1, int l2, bool inverted)
{
 int                                r;          // result

 ////////////////////////////////////////////////////////

 if (l1 < l2) 
 {
  if (inverted) { return false; }
  else          { return Compare_String_To_Function_Name(input_string, function_name, l2, l1, true); }
 }

 r = u.str_i_cmp(function_name, input_string, l2, l2); 

 if (
     (   (r == 0)
      && (   (l1 == l2)                                        // exact match
          || (strncmp( function_name + l2, "@", 1) == 0)       // input_string = function name + @ 
          || (   (l1 == l2 + 1)
              && (   (strncmp( function_name + l2, "A", 1) == 0)   // input_string = function name + A 
                  || (strncmp( function_name + l2, "W", 1) == 0)   // input_string = function name + W
                 )
             )
         )
     )                                

     ||                               

     (   (r == l2)
      && (   (strncmp( input_string + l2 - 1, "A", 1) == 0)   // input_string + @ = function name - A
          || (strncmp( input_string + l2 - 1, "W", 1) == 0)   // input_string + @ = function name - W
         )
      && (l1 >= l2 + 1)
      && (strncmp( function_name + l2, "@", 1) == 0)      
     )                                     

     ||                               

     (   (r == l2)
      && (l1 == l2)                                           // only a difference in last letter A/W
      && (   (strncmp( input_string + l2 - 1, "A", 1) == 0)   // input_string - A/W = function name - A/W
          || (strncmp( input_string + l2 - 1, "W", 1) == 0)   
         )
      && (   (strncmp( function_name + l2 - 1, "A", 1) == 0)   
          || (strncmp( function_name + l2 - 1, "W", 1) == 0)   
         )
     )                                     
    )                                                                                 
 { return true; }
 else
 { return false; }
}


void 
prep::Read_Windows_Imports::Write_Statistics()
{                               
 std::list <string>::iterator       it;

 ///////////////////////////////

 l_ftypes.sort();       
 l_ftypes.unique();

 it = l_ftypes.begin();

 while (it != l_ftypes.end())
 {
  intel_winpe_stats << "\"" 
                    << it->c_str()
                    << "\"," 
                    << UTIL_NEW_LINE;    

  if (it->size() > longest_type)
   { longest_type = it->size(); }

  it ++;
 }

 intel_winpe_stats << l_ftypes.size() << UTIL_NEW_LINE;
 intel_winpe_stats << longest_type << UTIL_NEW_LINE;
}

int
File_Select_Headers(const struct direct   *entry)
{   
    if (   (strcmp(entry->d_name, ".") == 0)
        || (strcmp(entry->d_name, "..") == 0)
        || (!strstr(entry->d_name, ".h"))  
       )                              
        return 0;
	else
        return 1;
}

int
File_Select_Idts(const struct direct   *entry)
{   
    if (   (strcmp(entry->d_name, ".") == 0)
        || (strcmp(entry->d_name, "..") == 0)
        || (!strstr(entry->d_name, ".idt"))  
       )                              
        return 0;
	else
        return 1;
}



