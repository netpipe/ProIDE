/*
 * command_line.hh: class for recognizing command line options
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef COMMAND_LINE_HH
#define COMMAND_LINE_HH

#define COMMAND_LINE_STATE_NONE     0
#define COMMAND_LINE_STATE_PREFIX   100
#define COMMAND_LINE_STATE_VALUE    200

#define COMMAND_LINE_COMBINATION_COMPULSORY             100
#define COMMAND_LINE_COMBINATION_COMPULSORY_WITH_VALUE  110
#define COMMAND_LINE_COMBINATION_ILLEGAL                200

#include <iostream>                 

#include <string>
#include <vector>                   

#include "utilities_defs.hh"                
#include "../return_codes.hh"

namespace util {
    class   Command_Line;
    struct  Option;
    struct  Combined_Option;
}    

struct util::Option                 // One command line argument: prefix + value
{
    std::string         prefix;     //  prefix announcing the option
    std::string         value;      //  value of the option
    std::string         meaning;    //  meaning of the option
};  

struct util::Combined_Option        // One command line argument can render other options illegal or compulsory
{
    int                       type_of_combination; // illegal / compulsory / ...

    std::string               lead;                //  lead option

    std::string               value;               //  lead value

    std::vector <std::string> v_comb;              //  combinations for this lead
};  


class util::Command_Line
{
public:
    
    Command_Line(int argc, char **argv);                  // constructor, passing the specified options
    virtual ~Command_Line(); 	

    void                              Add_Specified_Option(util::Option *o);
                                                          // add a specified option

    int                               Match_Options();    // check what options are allowed, determine which were specified,
                                                          // and collect invalid options

    void                              Add_Legal_Option(std::string s_prefix, std::string s_meaning);
                                                          // add a legal option, with its meaning


    void                              Add_Combination(int type_of_combination, std::string option, std::string value,
                                                      std::vector<std::string> *v_comb);
                                                          // specify something about combined options



    void                              Show_Illegal_Options();
                                                          // mention illegal options and show proper usage

    void                              Show_Usage();       // show proper usage

    bool                              Get_Specified_Option(std::string *requested_prefix, util::Option *o);
                                                          // see if a given option was specified

private:

    void                         Parse_Arguments();        // recognize which options were specified
    
    void                         Initialize(Option *o);    // Option o = 0

    Option*                      Option_Specified(std::string   option);
                                                           // was this option specified?

    int                          n_arguments;              // number of arguments
    std::string                  arguments;                // arguments

    std::vector <Option>         v_specified;              // vector of specified Options
    std::vector <Option>         v_legal;                  // vector of legal Options
    std::vector <Option>         v_illegal;                // vector of illegal Options

    std::vector<Combined_Option> v_combinations;           // vector of illegal / compulsory combinations of options 
    std::vector<Combined_Option> v_illegal_combinations;   // vector of illegal combinations of options 
    std::vector<Combined_Option> v_missing_combinations;   // vector of compulsory but missing combinations of options 

};


#endif
