/*
 * command_line.cc: class for recognizing command line options
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#include "command_line.hh"

util::Command_Line::Command_Line(int argc, char **argv) 
{
    int           i;    // index

    //////////////////////////////////////////////////////
    
    std::cout << "Constructor Command_Line()" << "\n";

    n_arguments = argc;

    arguments.clear();
    for (i = 1; i < argc; i++)
    { arguments += argv[i]; arguments += " "; }

    Parse_Arguments();
}

util::Command_Line::~Command_Line() 
{
    std::cout << "Destructor Command_Line()" << "\n";
}

void
util::Command_Line::Initialize(Option *o)
{
    o->prefix.clear();
    o->value.clear();
    o->meaning.clear();
}

void
util::Command_Line::Parse_Arguments() 
{
  int            i, l,    // index, length, counter
                 state;

  bool           stop;
  char           c;

  Option         o;       // temp

  std::string    t;       // temp

  //////////////////////////////////////////////////////////

  l = arguments.size();

  v_specified.clear();

  stop = false; i = 0; Initialize(&o); state = COMMAND_LINE_STATE_NONE;

  // eliminate leading characters
  while (!stop)
  {
   c = arguments[i]; 

   if ((c != UTILITIES_TAB) && (c != UTILITIES_ARG_DELIMITER))
   { stop = true; }
   else 
   { i++; }
  }
  stop = false;

  // parse the options 
  // each new option begins with a '-' (UTILITIES_ARG_PREFIX)
  while (!stop)
  {
   if (i >= l)
     {
      stop = true;
      if (state == COMMAND_LINE_STATE_VALUE)      // if we are treating the value
      { 
       o.value = t;              // t accumulates the characters
       v_specified.push_back(o);         // v_specified accumulates Options
      }
     }
   else      
   {
    c = arguments[i]; 

    if (c == UTILITIES_ARG_PREFIX)                  // '-'
    { 
     if (state == COMMAND_LINE_STATE_VALUE)      // if we are treating the value
     { 
      o.value = t;              // t accumulates the characters
      v_specified.push_back(o);         // v_specified accumulates Options

      t.clear();                                          
     }

     state = COMMAND_LINE_STATE_PREFIX;
     t += c; 
    }

    else if (c == UTILITIES_ARG_DELIMITER)          // ' '
    { 
     if (state == COMMAND_LINE_STATE_PREFIX)     // if we are treating the prefix
     { 
      o.prefix = t;             // t accumulates the characters
      t.clear();                                          
     }

     state = COMMAND_LINE_STATE_VALUE;
    }

    else 
    { 
     t += c; 
    }

    i++; 
   }
  }
}

void                             
util::Command_Line::Add_Specified_Option(util::Option *o)
{
  v_specified.push_back(*o);         // v_specified accumulates Options
}


int 
util::Command_Line::Match_Options()
{
  bool                              stop;

  uint                              n, e;

  Option                           *o1, *o2;

  Combined_Option                  *c_o;

  std::vector <Option>              v2;          // temp copy of specified options in v_specified

  std::vector <Option>::iterator    it1, it2;

  std::vector <Combined_Option>::iterator  it;

  std::string                       s;           // temp value


  /////////////////////////////////////////////////////////////////////////
  /// check what options are allowed, and determine which were specified //
  /////////////////////////////////////////////////////////////////////////

  o1 = o2 = 0;

  v2 = v_specified;

  for (it1 = v_legal.begin(); it1 != v_legal.end(); it1++)
  {                                                      // try and fill in every possible option
   o1 = &(*it1);
   s = o1->prefix;

   stop = false;                                                                                    
   it2 = v2.begin(); 

   while (stop == false)                                // search allowed option in specified option
   {
    if (it2 == v2.end() )
    { stop = true; }
    else 
    {
     o2 = &(*it2);
     if (s == o2->prefix)
     {
      stop = true;
      it1->value = o2->value;
      v2.erase(it2);
     }                     
    }

    it2++;
   }
  }

  // legal options have been removed from v2, so everything remaining is illegal
  v_illegal = v2;

  if (v_illegal.size() > 0)
  {
   Show_Illegal_Options();
   Show_Usage();
   return RET_ERR_OPTION;        
  }


  ///////////////////////////////////////////////////////
  /// now check for specific combinations of options  ///
  ///////////////////////////////////////////////////////

  
  it = v_combinations.begin(); 
  for (it = v_combinations.begin(); it != v_combinations.end(); it++)
  {
   c_o = &(*it);
                                                     
   // if this option was specified, it should be accompanied by specific other options
   o1 = Option_Specified(c_o->lead);
   if (o1 != 0)
   {
    if ((c_o->value == "") || (c_o->value == o1->value))    // any or correct combination to be matched
    {
     for (n = 0; n < c_o->v_comb.size(); n++)       // number of compulsory options
     {
      o2 = Option_Specified(c_o->v_comb[n]);

      if ( (o2 == 0) && (c_o->type_of_combination == COMMAND_LINE_COMBINATION_COMPULSORY)) 
       { e = 1; }
      else if ( (o2 == 0) || 
                ((o2 != 0) && (o2->value == "") &&
                 (c_o->type_of_combination == COMMAND_LINE_COMBINATION_COMPULSORY_WITH_VALUE))) 
       { e = 2; }
      else if ( (o2 != 0) && (c_o->type_of_combination == COMMAND_LINE_COMBINATION_ILLEGAL)) 
       { e = 3; }
      else { e = 0; }

      if (e > 0)
      {
       if (e == 1)
       {
        std::cout << "Fatal error: "
                  << "Option " << c_o->lead << " " << c_o->value
                  << " was specified, and this makes option " 
                  << c_o->v_comb[n] << " compulsory, but it is missing.\n"
                  << "\n"; 
       }
       if (e == 2)
       {
        std::cout << "Fatal error: "
                  << "Option " << c_o->lead << " " << c_o->value
                  << " was specified, and this makes specifying a value for option " 
                  << c_o->v_comb[n] << " compulsory, but it is missing.\n"
                  << "\n"; 
       }
       else if (e == 3)
       {
        std::cout << "Fatal error: "
                  << "Option " << c_o->v_comb[n] 
                  << " was specified, but is illegal with option " 
                  << c_o->lead << " " << c_o->value << "\n"; 
       }

       Show_Usage();
       return RET_ERR_OPTION;        
      }                              
     }
    }
   }

  }                                                                    

  return RET_OK; 
}

void
util::Command_Line::Add_Legal_Option(std::string s_prefix, std::string s_meaning)
{

  Option            o;

  //////////////////////////
  /// add a legal option  //
  //////////////////////////

  Initialize(&o);

  o.prefix = s_prefix;
  o.meaning = s_meaning;

  v_legal.push_back(o);
}

void
util::Command_Line::Add_Combination(int type_of_combination, std::string option, std::string value,
                                    std::vector<std::string> *v_comb)
{
  Combined_Option            c_o;

  ////////////////////////////////////////////////////////////////////
  /// check what combinations of options are allowed or compulsory  //
  ////////////////////////////////////////////////////////////////////

  c_o.type_of_combination = type_of_combination;

  c_o.lead                = option;

  c_o.value               = value;

  c_o.v_comb              = *v_comb;

  v_combinations.push_back(c_o);
}

util::Option*
util::Command_Line::Option_Specified(std::string   option)
{
  std::vector <Option>::iterator    it;

  /////////////////////////////////////////////////
  /// check whether a given option was specified //
  /////////////////////////////////////////////////

  for (it = v_specified.begin(); it != v_specified.end(); it++)
  {                                                      
   if (it->prefix == option) {return &(*it); }
  }

  return 0;
}

void
util::Command_Line::Show_Illegal_Options()
{
   util::Option                     *o;

   std::vector <Option>::iterator    it;

   std::string                       s;           // temp value

   ////////////////////////////////////////////////////////////

   for (it = v_illegal.begin(); it != v_illegal.end(); it++)
   {
    o = &(*it);

    s = "illegal option : " + o->prefix;

    std::cout << s << std::endl;   
   }                  
}

bool
util::Command_Line::Get_Specified_Option(std::string *requested_prefix, util::Option *o )
{
   bool              stop, found;

   std::vector <Option>::iterator    it;
  
   std::string                       s;           // temp value


   /////////////////////////////////////////////////////////////////////////
   /// see if a given option was specified                                //
   /////////////////////////////////////////////////////////////////////////

   stop = found = false;

   it = v_specified.begin();

   while (stop == false)
   {      
    if (it == v_specified.end())
    { stop = true; }

    else 
    {
     *o = (*it);

     if (o->prefix == *requested_prefix)
     { found = stop = true; }
    }

    it++;
   }     

   return found;
}



void
util::Command_Line::Show_Usage()
{

   util::Option                     *o;   

   std::string                       s;           // temp value

   std::vector <Option>::iterator    it;

   ////////////////////////////////////////////////////////////

   s = "usage : ";
   std::cout << s << std::endl;   

   for (it = v_legal.begin(); it != v_legal.end(); it++)
   {
    o = &(*it);

    s = o->prefix + " : " + o->meaning;

    std::cout << s << std::endl;   
   }                  
}



