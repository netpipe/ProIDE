/*
 * memory_pool: for organising memory allocations
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef MEMORY_POOL_HH
#define MEMORY_POOL_HH

#define RET_MEM_OK  0
#define RET_MEM_ERR 1
#define DEFAULT_POOL_SIZE 5000


#include <iostream>
#include <vector>     

#include "../return_codes.hh"
           
namespace mem {
	class   Memory_Pool;
    struct  Memory_Node;
}                                         

struct mem::Memory_Node     
{                                         
    char    *       memory_offset;          // offset of first byte in memory
    long            associated_value;       // each area of memory has its own associated value
    long            size;                   // each area of memory can have its own size
};


                                          
class mem::Memory_Pool      
{
                                           
 public:
    Memory_Pool();
    Memory_Pool(long Pool_Size);
 	
    ~Memory_Pool();     

    void * Use_Pool(long size);                               // use [size] bytes of memory of pool

    void * Ensure_Minimum_Allocation(char *allocated, uint minimum);

    void   Set_Pool_Size(int size);

    void   Set_Associated_Value(int associated);    

    void   Reset_Available();

    bool   Search_Memory_Pool(bool direction, bool wrap, long start_value, 
                              std::vector<char> *search_byte, long * offset_found);
                           // search the memory pool for the string, and indicate where it was found

    void   Clear();
         
protected:

    long            available;                                // available memory, in bytes
    long            pool_size;                                // size of new allocations
    long            associated_value;                         // each area of memory has its own associated value
    
    
 private:
	
 	std::vector <Memory_Node>   pool_list;                    // list of allocated memory
    char                       *current;
    long                        search_previous_offset,
                                search_previous_start ;
    bool                        search_previous_direction;  
};



#endif
