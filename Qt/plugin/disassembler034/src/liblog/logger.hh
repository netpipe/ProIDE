/*
 * utilger.hh: messages
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef LOGGER_HH
#define LOGGER_HH
                 
#include <fstream>                 
#include <string>
#include <time.h>

#include "../gui_commands.hh"

namespace util {
    class Logger;
}   

class util::Logger
{
public:
    
    Logger();
    Logger(std::string Program_Name);

    virtual            ~Logger(); 	

    void                Set_Prefix(std::string  prefix_string);     // each logged record will begin with this string
    void                Add_To_Log(std::string  log_string);        // add text to the log
    void                Add_To_Log(int          log_int);           // add control information to the log

private:                                

    void                Init_File(std::string Program_Name);   
                        // search a file name for the messages of this execution

    std::string         File_Name;

    std::string         prefix;

    std::ofstream       of;

    int                 log_counter;
};


#endif
