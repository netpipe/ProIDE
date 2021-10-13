/*
 * utilities: Utilities class
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#include "utilities.hh"


util::Utilities::Utilities() 
{
    cout << "Constructor Utilities()" << "\n";
}

util::Utilities::~Utilities() 
{
    cout << "Destructor Utilities()" << "\n";
}


string
util::Utilities::int_to_string(int i)
{
 int    m;              // modulus
 bool   n;              // negative

 /////////////////////////////////////////////////////:

 if (i == 0)
 { temp_string = "0"; }

 else 
 {
  temp_string.clear();
  
  n = (i < 0);

  while (i != 0)
  {
     m = i % 10;               
     i = i / 10;

     switch (m)
     {
     case  0: {temp_string.insert(0, "0"); break;}

     case  1:
     case -1: {temp_string.insert(0, "1"); break;}
     
     case  2: 
     case -2: {temp_string.insert(0, "2"); break;}
     
     case  3: 
     case -3: {temp_string.insert(0, "3"); break;} 
     
     case  4: 
     case -4: {temp_string.insert(0, "4"); break;} 
     
     case  5: 
     case -5: {temp_string.insert(0, "5"); break;}  
     
     case  6: 
     case -6: {temp_string.insert(0, "6"); break;} 
     
     case  7: 
     case -7: {temp_string.insert(0, "7"); break;}  
     
     case  8: 
     case -8: {temp_string.insert(0, "8"); break;}  
     
     case  9: 
     case -9:{temp_string.insert(0, "9"); break;}
     }
  }

  if (n == true)
  { temp_string.insert(0, "-"); }

 }

 return temp_string;
}


string
util::Utilities::int_to_hexstring(int i)
{
 int    m;             // modulus
  
 //////////////////////////////////////////////////////

 temp_string = "0x";

 if (i == 0)
 { temp_string.insert(2, "00");}

 else 
 {
   while (i != 0)
   {
     m = i % 16;               
     i = i / 16;

     switch (m)
     {
     case 0: {temp_string.insert(2, "0"); break;}
     case 1: {temp_string.insert(2, "1"); break;}
     case 2: {temp_string.insert(2, "2"); break;}
     case 3: {temp_string.insert(2, "3"); break;} 
     case 4: {temp_string.insert(2, "4"); break;} 
     case 5: {temp_string.insert(2, "5"); break;}  
     case 6: {temp_string.insert(2, "6"); break;} 
     case 7: {temp_string.insert(2, "7"); break;}  
     case 8: {temp_string.insert(2, "8"); break;}  
     case 9: {temp_string.insert(2, "9"); break;}
     case 10: {temp_string.insert(2, "A"); break;}
     case 11: {temp_string.insert(2, "B"); break;}
     case 12: {temp_string.insert(2, "C"); break;}
     case 13: {temp_string.insert(2, "D"); break;}
     case 14: {temp_string.insert(2, "E"); break;}
     case 15: {temp_string.insert(2, "F"); break;}
     }
   }
 }

 return temp_string;
}

string
util::Utilities::int_to_hexstring_option(int i, int int_hex_option)
{   
 // int_hex_option : 0 = hex ; 1 = dec; 2 = hex + dec

 if (int_hex_option > 0)
 { 
   temp_string_2 = int_to_string(i);
   if (int_hex_option > 1)
   { temp_string_2.append(" / "); }
 }
 else 
 { temp_string_2.clear(); }

 if (int_hex_option != 1)
 { temp_string_2.append(int_to_hexstring(i)); }

 return temp_string_2;
}

string
util::Utilities::byte_to_hexstring(int i)
{
 int j;

 //////////////////////////////////////////////////////

 if (i == 0)
 { temp_string = "00"; }

 else 
 {
  temp_string.clear();

  j = i / 16;
  switch (j)
     {
       case 0: {temp_string += "0"; break;} 
       case 1: {temp_string += "1"; break;}
       case 2: {temp_string += "2"; break;} 
       case 3: {temp_string += "3"; break;}
       case 4: {temp_string += "4"; break;} 
       case 5: {temp_string += "5"; break;}
       case 6: {temp_string += "6"; break;}
       case 7: {temp_string += "7"; break;}
       case 8: {temp_string += "8"; break;} 
       case 9: {temp_string += "9"; break;}
       case 10: {temp_string += "A"; break;} 
       case 11: {temp_string += "B"; break;}
       case 12: {temp_string += "C"; break;} 
       case 13: {temp_string += "D"; break;}
       case 14: {temp_string += "E"; break;} 
       case 15: {temp_string += "F"; break;}
     }

  i = i - (16 * j);
  switch (i)
     {
       case 0: {temp_string += "0"; break;} 
       case 1: {temp_string += "1"; break;}
       case 2: {temp_string += "2"; break;} 
       case 3: {temp_string += "3"; break;}
       case 4: {temp_string += "4"; break;} 
       case 5: {temp_string += "5"; break;}
       case 6: {temp_string += "6"; break;}
       case 7: {temp_string += "7"; break;}
       case 8: {temp_string += "8"; break;} 
       case 9: {temp_string += "9"; break;}
       case 10: {temp_string += "A"; break;} 
       case 11: {temp_string += "B"; break;}
       case 12: {temp_string += "C"; break;} 
       case 13: {temp_string += "D"; break;}
       case 14: {temp_string += "E"; break;} 
       case 15: {temp_string += "F"; break;}
     }           
 }

 return temp_string;
}

bool        
util::Utilities::char_is_printable(char c)
{
    return
       (    ((c >= 32) && (c <= 57))                   // '0' - '9'
         || ((c >= 65) && (c <= 126))                  // 'A' - 'z'
       );
}

string
util::Utilities::time_to_string()
{
 time_t              now;

 string         temp;

 /////////////////////////////////////////////////////:

 time (&now);
 
 temp = ctime (&now);                               // has an annoying \n ...
 temp.replace(temp.find("\n"), 1, "");

 return temp;

}

int  
util::Utilities::hexstring_to_int(string *hex)
{
 int    i, j, l;           // temp, length

 bool   valid           = true,
        first_was_zero  = true,
        is_zero;

 //////////////////////////////////////////////////////

 i = j = 0;

 l = hex->size();

 while (  (hex->compare(i, 1," ") == 0)
       && (l > 0))
 {  i++; l--; }

 while (valid && (l > 0))
 {
  j *= 16;

  is_zero = false;

  if      (hex->compare(i, 1,"0") == 0) {is_zero = true; } 
  else if (hex->compare(i, 1,"1") == 0) {j += 1; }
  else if (hex->compare(i, 1,"2") == 0) {j += 2; } 
  else if (hex->compare(i, 1,"3") == 0) {j += 3; }
  else if (hex->compare(i, 1,"4") == 0) {j += 4; } 
  else if (hex->compare(i, 1,"5") == 0) {j += 5; }
  else if (hex->compare(i, 1,"6") == 0) {j += 6; }
  else if (hex->compare(i, 1,"7") == 0) {j += 7; }
  else if (hex->compare(i, 1,"8") == 0) {j += 8; } 
  else if (hex->compare(i, 1,"9") == 0) {j += 9; }
  
  else if (hex->compare(i, 1,"A") == 0) {j += 10; } 
  else if (hex->compare(i, 1,"B") == 0) {j += 11; }
  else if (hex->compare(i, 1,"C") == 0) {j += 12; } 
  else if (hex->compare(i, 1,"D") == 0) {j += 13; }
  else if (hex->compare(i, 1,"E") == 0) {j += 14; } 
  else if (hex->compare(i, 1,"F") == 0) {j += 15; }
  
  else if (hex->compare(i, 1,"a") == 0) {j += 10; } 
  else if (hex->compare(i, 1,"b") == 0) {j += 11; }
  else if (hex->compare(i, 1,"c") == 0) {j += 12; } 
  else if (hex->compare(i, 1,"d") == 0) {j += 13; }
  else if (hex->compare(i, 1,"e") == 0) {j += 14; } 
  else if (hex->compare(i, 1,"f") == 0) {j += 15; }

  else if (   (hex->compare(i, 1,"x") == 0)
           || (hex->compare(i, 1,"X") == 0))
        {
          if (   (first_was_zero == false)                           // allow for leading (0)x
              || (i > 2))
          {valid = false;}
          else
          {is_zero = true; } 
        }
  else {valid = false;}

  if ((is_zero == false) && (first_was_zero == false))
  {first_was_zero = false;}

  i++; l--;
 }

 if (valid)
 { return j;}
 else
 { return (RET_ERR_GENERAL);}
}

int 
util::Utilities::string_to_vector_char(string *input_string, std::vector<char> *v_c, bool hexa_decimal)
{
  char          t,                              // temp string
                ch;                             // char

  int           p,                           // index, position
                l, n, hti;                      // length, counter, hex_to_int

  string        s;                              // temp_string

  bool          stop;

  ////////////////////////////////////////////////////////////////

  v_c->clear();

  l = input_string->size();
  p = 0;
  
  stop = false;
  while (!stop)
  {
   t = (*input_string)[p];

   if (t == ' ')                    // eliminate leading spaces
   { 
    l--; p++; 
    if (l == 0)
    { stop = true; }
   }
   else
   { stop = true; }
  }

             
  if (    (l > 2)                   // eliminate leading '0x'
       && (t == '0') 
       && (   ((*input_string)[p + 1] == 'x')
           || ((*input_string)[p + 1] == 'X')
          )
     )
   {l = l - 2; p = p + 2; }

 // int i=0;
n = 0;
  s.clear();
  
  stop = false;

  while (!stop)
  {
   l--; p++;

   if (l < 0)
   { stop = true; }
   else
   {
    t = (*input_string)[p - 1];

    if (hexa_decimal)
    {
     if (t != ' ')
     {
      n++;
      s += t;

      if (n >= 2)
      {
       hti = hexstring_to_int(&s);        
     
       if (hti == RET_ERR_GENERAL)
       { return RET_ERR_GENERAL; }
       else
       {
        n = 0;
        s.clear();

        ch = hti;
        v_c->push_back(ch);
       }
      }
     }  
    }
    else
    {v_c->push_back(t); }
   }
  }    

  if (hexa_decimal && (n > 0))
  { return RET_ERR_GENERAL; }

  return RET_OK;
}

int  
util::Utilities::parse_string(string *s, vector<string> *v)
{
  int            i, l;    // index, length, counter
  bool           stop;
  char           c;
  string         t;          // temp

  //////////////////////////////////////////////////////////

  l = s->size();

  v->clear();

  stop = false; i = 0; t.clear(); 

  while (!stop)
  {
   if (i >= l)
     { stop = true; }
   else      
   {
    c = (*s)[i]; 

    if ((c == UTILITIES_TAB) || (c == ' '))
    { 
     if (t.size() > 0)
     { 
       v->push_back(t);
       t.clear();
     }
     while (((c == UTILITIES_TAB) || (c == ' ')) && (!stop))
     { 
      i++; 
      if (i >= l) { stop = true; } 
      else {c = (*s)[i]; }
     }
    }
    else 
    { 
     t += c; 
     i++;
    }
   }
  }

  if (t.size() > 0)
  { v->push_back(t); }

  return v->size();
}

string
util::Utilities::replace_all_occurrences_in_string(string s, char *c1, char *c2, int l1, int l2)
{
  int               l, p, pt, lt;  // length, pos, pos temp, length temp

  bool              finished, same;

  const char       *t, *t1, *t2;

  string            result, temp, replacement;

  //////////////////////////////////////////////////////////////////////
  ///  replace all occurrences of c1 in s by c2                      ///
  //////////////////////////////////////////////////////////////////////

  if (s.find(c1) == string::npos)
  { return s; }

  replacement.clear(); lt = 0; t2 = c2;
  while (lt < l2)
     { replacement += *t2; lt++; t2++;}    

  p = 0 ; l = s.size(); finished = false;

  t = s.c_str();

  while (!finished)
  {
   if (*t == *c1)                       // possible match
   {
    t2 = t;
    temp = *t; lt = 1; t1 = c1 + 1; t++; p++;
    pt = p; 

    same = true;
    while ((lt < l1) && (same))         // evaluate if match is complete
    {
     if (*t1 != *t) { same = false; }    
     else           { t++; t1++; lt++; temp += *t; p++;}    
    }

    if (same)                          
    {                                   // yes: replace c1 by c2 in result
      result += replacement; 
    }   
    else 
    { 
      result += *t2;                   // no: just copy s to result             
      p = pt; t = t2;                     // conservative approach: don't try to optimize, 
                                          // just advance search position by 1
      t++;
    }
   }    
   else
   { result += *t; p++; t++;}

   finished = (p >= l);
  }                      

  return result;
}

int
util::Utilities::str_i_cmp(const char *c1, const char *c2, int l1, int l2)
{
  int               i;  // index

  const char       *p1, *p2;

  ////////////////////////////////////////////////

  if (l1 != l2) {return l1; }   // arbitrary

  i = 0;
  p1 = c1;
  p2 = c2;

  while (true)
  {
   i++;

   if (i > l1) { return 0;}

   if (p1 == 0)
   {
    if (p2 == 0) { return 0; }

    return i;
   }
   else if (p2 == 0)
   { return i; }

   if (toupper(*p1) != toupper(*p2))
   { return i; }

   p1++; 
   p2++;
  }     
}

string
util::Utilities::get_executable_path(char *executable_name)
{
    string                  s, r;           // string, result

   ///////////////////////////////////////////////////////
   ///copied from http://autopackage.org/docs/binreloc 
   ///////////////////////////////////////////////////////

    if (executable_name == 0)
    { return ""; }                                                     

    ifstream map_file("/proc/self/maps");

    if (!map_file)
    { return ""; }                                                     

    while (getline(map_file, s))
    {
     if (   (s.find(" r-xp ") == string::npos)
         || (s.find('/') == string::npos)
         || (s.find(executable_name) == string::npos))
     { continue; }
     // first compliant line will be supposed to contain executable path
     
     // drop everything before first '/'
     r = s.substr(s.find('/'), s.size());  

     // Get rid of the newline 
     if (r.find('\n') != string::npos)
      { r = r.substr(0, r.size() - 1); }
      
     // Get rid of "(deleted)" 
     if (r.find(" (deleted)") != string::npos)
     { r = r.substr(0, r.find(" (deleted)")); }

     // Get rid of executable name
     r = r.substr(0, r.rfind('/')); 

     return r;
    }

    return "";
}
