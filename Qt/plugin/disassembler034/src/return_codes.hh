/*
 * return_codes.hh: all posible return codes
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef RET_CODES_HH
#define RET_CODES_HH

#define RET_OK                    0

#define RET_ERR_GENERAL           -1

#define RET_ERR_UNKNOWN_FILETYPE  1
#define RET_ERR_FILE_INPUT        2
#define RET_ERR_OPCODE            3
#define RET_ERR_LDN               4                    // ldn = List Disassembly Nodes
#define RET_ERR_CANCEL            5 
#define RET_ERR_OPTION            6 
#define RET_ERR_MEMORY            7 
                                    
#define DEBUG_ALL                 0                                           
#define DEBUG_ONE                 1
#define DEBUG_FILE                2
#define DEBUG_DISPLAY             3
                                         


#endif
