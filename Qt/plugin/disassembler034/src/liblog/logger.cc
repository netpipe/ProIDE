/*
 * logger: Logger class
 *
 *                  writes (debug) messages (to a file)
 *
 * Author:
 *   Danny Van Elsen 
 * 
 **/

#include "logger.hh"


util::Logger::Logger(std::string Program_Name) 
{
    Init_File(Program_Name);
}

util::Logger::Logger() 
{
    Init_File("debug");
}

util::Logger::~Logger() 
{
    of.close();
}


void
util::Logger::Init_File(std::string Program_Name)
{
 time_t              now;

 std::string         temp;

 /////////////////////////////////////////////////////:

 log_counter = 0;

 prefix = "";

 time (&now);
 
 temp = ctime (&now);                               // has an annoying \n ...
 temp.replace(temp.find("\n"), 1, "");

 File_Name = Program_Name + "." + temp + ".log";

 of.open(File_Name.c_str());
 
 of << ctime (&now) << "\n";
}

void  
util::Logger::Set_Prefix(std::string  prefix_string)
{
 prefix = prefix_string;
}

void 
util::Logger::Add_To_Log(int log_int)
{
  time_t              now;

  /////////////////////////////////////////////////////:

  of << log_counter++;

  switch  (log_int)
  {
  case LOG_COMMAND_TIME:
   {
    time (&now);
    of << ctime (&now) << "\n";
    break;
   }
  case LOG_COMMAND_SEP1:    
   {
    of << "-----------------------------------------\n";
    break;
   }
  case LOG_COMMAND_SEP2:    
   {
    of << "*****************************************\n";
    break;
   }
  case LOG_COMMAND_SEP3:    
   {
    of << "/////////////////////////////////////////\n";
    break;
   }
  case LOG_COMMAND_SEP4:    
   {
    of << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
    break;
   }
  case LOG_COMMAND_SEP5:    
   {
    of << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n";
    break;
   }
  case LOG_COMMAND_SEP6:    
   {
    of << "+++++++++++++++++++++++++++++++++++++++++\n";
    break;
   }
  case LOG_COMMAND_SEPN:    
   {
    of << "\n";
    break;
   }
  }

  of.flush();
}

void 
util::Logger::Add_To_Log(std::string log_string)
{
 of << prefix << log_string << "\n";

 of.flush();
}

