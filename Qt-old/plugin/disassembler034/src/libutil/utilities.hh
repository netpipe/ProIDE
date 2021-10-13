/*
 * utilities.hh: type conversions
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef UTILITIES_HH
#define UTILITIES_HH

#define UTIL_NEW_LINE   "\n"

using namespace std;

#include <iostream>                 
#include <fstream>                 

#include <string>
#include <vector>


#include "../return_codes.hh"
#include "utilities_defs.hh"

namespace util {
    class Utilities;
}    
                                            

class util::Utilities
{
public:
    
    Utilities();

    virtual ~Utilities(); 	

    string       int_to_string(int i);
    string       int_to_hexstring(int i);
    string       int_to_hexstring_option(int i, int int_hex_option);
    string       byte_to_hexstring(int i);

    int          hexstring_to_int(string *hex);

    int          string_to_vector_char(string *input_string, std::vector<char> *v_c, bool hexa_decimal);

    string       time_to_string();

    bool         char_is_printable(char c);

    int          parse_string(string *s, vector <string> *v);
    string       replace_all_occurrences_in_string(string s, char *c1, char *c2, int l1, int l2);

    int          str_i_cmp(const char *c1, const char *c2, int l1, int l2);    // case insensitive compare

    string       get_executable_path(char *executable_name);


private:

    string       temp_string, temp_string_2;        // temp values


};


#endif
