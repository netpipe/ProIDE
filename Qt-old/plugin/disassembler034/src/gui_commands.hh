/*
 * gui_commands.hh: all posible gui commands
 *                                        
 * Author:
 *   Danny Van Elsen 
 *                   
 **/

#ifndef GUI_COMMANDS_HH
#define GUI_COMMANDS_HH

#define GUI_MARGIN_LEFT      10                               
#define GUI_MARGIN_RIGHT     20
#define GUI_MARGIN_BOTTOM    3
#define GUI_MARGIN_TOP       10
#define GUI_MARGIN_DIVIDER   1.5
#define GUI_COLUMN_2         3.75
                          
#define GUI_COUNTER_MAX         150
#define GUI_LABEL_MIN           2
#define GUI_LABEL_MAX           20
#define GUI_MAX_BYTES_PER_ROW   8
                                            
#define GUI_COMMAND_INIT            1
#define GUI_COMMAND_UPDATE          2
#define GUI_COMMAND_DELETE          3
#define GUI_COMMAND_REFRESH         4
#define GUI_COMMAND_STATUSBAR_POP   5
#define GUI_COMMAND_STATUSBAR_PUSH  6

#define LOG_COMMAND_TIME          0
#define LOG_COMMAND_SEP1          1
#define LOG_COMMAND_SEP2          2
#define LOG_COMMAND_SEP3          3
#define LOG_COMMAND_SEP4          4
#define LOG_COMMAND_SEP5          5
#define LOG_COMMAND_SEP6          6
#define LOG_COMMAND_SEPN          99


#endif
