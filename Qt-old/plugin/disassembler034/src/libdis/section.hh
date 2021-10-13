/*
 * section: information about the section in which a disassembly_node remains
 *          
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef SECTION_HH
#define SECTION_HH
     
namespace dis {
    struct Section;
}                                         

struct dis::Section
{   
    std::string                     name;                   // name of section

    long                            offset,                 // starting address
                                    size;                   // size
};



#endif
