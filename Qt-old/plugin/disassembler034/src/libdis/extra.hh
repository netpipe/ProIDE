/*
 * extra: all kinds of non specific information about an executable file
 *          
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef EXTRA_HH
#define EXTRA_HH

namespace dis {
    struct Extra;
}                                         

struct dis::Extra
{   
    char                           *description;            // the info itself

    dis::Extra                     *next;                   // for storing several records

    dis::Extra                     *next_level;             // for storing several hierarchical records
};



#endif
