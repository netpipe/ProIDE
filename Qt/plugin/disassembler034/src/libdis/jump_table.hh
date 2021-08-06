/*
 * jump_table: avoiding jump tables to be interpreted as code 
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef JUMP_TABLE_HH
#define JUMP_TABLE_HH

#define JUMP_TABLE_NOT_USED (int) (-1)

namespace dis {
    struct Jump_Table_Request;
    struct Jump_Table;
}  

struct dis::Jump_Table_Request                               // used during analysis
{

    int                            jump_to;                  //  offset of first byte in jump table
    int                            jump_from;                //  offset of reference to jump table
};

struct dis::Jump_Table
{
    int                            jump_to;                  //  destination of jump table
};



#endif
