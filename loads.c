/*
 * 
    { .command = "WTL1",    .fargs = HHMMARG,       .f = setWorkingTimeLength1 },
    { .command = "WTL2",    .fargs = HHMMARG,       .f = setWorkingTimeLength2 },
    
    { .command = "SLON",    .fargs = HHMMARG,       .f = setLengthOfNight },
    
    { .command = "LCM",     .fargs = INTARG,        .f = setLoadControllingModes },
    { .command = "TONT1",   .fargs = HHMMSSARG,     .f = setTurnOnTiming1 },
    { .command = "TOFFT1",  .fargs = HHMMSSARG,     .f = setTurnOffTiming1 },
    { .command = "TONT2",   .fargs = HHMMSSARG,     .f = setTurnOnTiming2 },
    { .command = "TOFFT2",  .fargs = HHMMSSARG,     .f = setTurnOffTiming2 },
    
    { .command = "CDON",    .fargs = NOARG,         .f = setChargingDeviceOn },
    { .command = "CDOFF",   .fargs = NOARG,         .f = setChargingDeviceOff },
    { .command = "LDON",    .fargs = NOARG,         .f = setLoadDeviceOn },
    { .command = "LDOFF",   .fargs = NOARG,         .f = setLoadDeviceOff },
    { .command = "FLDON",   .fargs = NOARG,         .f = forceLoadOn },
    { .command = "FLDOFF",  .fargs = NOARG,         .f = forceLoadOff },

 */
#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include "ncurses-menu.h"

extern  int     getIntParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const int maxVal, const int minVal);
extern  double  getFloatParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const double minVal, const double maxVal);
extern  char    *getHHMMSSParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const char *minVal, const char *maxVal);




static    char aMenu[][100] = { 
    { "Set Load Mode" },
    { "T1 Turn On" },
    { "T1 Turn Off" },
    { "T2 Turn On" },
    { "T2 Turn Off" },
    { "Turn Load On" },
    { "Turn Load Off" },
  
    { "Return" },};

#define NUM_MENU_ITEMS ( (sizeof( aMenu ) / 100 ) )

//-- ---------------------------------------------------------------------------
static  void    showCurrentLoadSettings (int topLeft_Y, int topLeft_X, int menuMinWidth)
{
    mvaddstr( topLeft_Y + 3, topLeft_X + 30, "Current Settings" );
    refresh();
}

//-- ---------------------------------------------------------------------------
int doLoadMenu (int topLeft_Y, int topLeft_X, int menuMinWidth)
{
    char    aStr[ 80 ];
    int     selection = -1;

    do {
        showCurrentLoadSettings( topLeft_Y, topLeft_X, menuMinWidth );
        
        selection = print_menu ( topLeft_Y + 2, 
                            topLeft_X + 20, 
                            NUM_MENU_ITEMS, 
                            menuMinWidth,
                            "Load Settings", aMenu, 1 );
        
        
        switch (selection) {

            case    1:  
                {               /* 000H Manual Control 0001H Light ON/OFF 0002H Light ON+ Timer 0003H Time Control */
                    int  iVal = getIntParam( 
                                "Enter Load Control Mode: ",
                                "Modes are: (0) Manual (1) Dusk Dawn, (2) Dusk +  Timer, (3) Timer On Off",
                                "Error: Valid control modes are 0 to 3", 
                                "Sending Load Control Mode to the solar charge controller",
                                0, 3 );
                    //
                    //  send battery type MQTT message
                }
                break;
                
            case 2:
                {
                    char *tVal = getHHMMSSParam( 
                                "Enter Timer 1 Turn On Time (HHMMSS): ",
                                "Timer 1 will be turned On at this time",
                                "Error: Valid times are 00000 to 235959", 
                                "Sending Timer 1 Turn On Time to the solar charge controller",
                                "000000", "235959" );
                    break;
                }
            
            case 3:
                {
                    double  dVal = getFloatParam( 
                                "Enter Timer 1 Turn On Time (HHMMSS): ",
                                "Timer 1 will be turned On at this time",
                                "Error: Valid times are 00000 to 235959", 
                                "Sending Timer 1 Turn On Time to the solar charge controller",
                                9.0, 17.0 );
                    break;
                }
            

            case 4:
                {
                    int iVal = getIntParam( 
                                "Enter Equalization Time (minutes): ",
                                "Minutes Equalization is applied (usually 120)",
                                "Error: Valid durations are 0 to 180", 
                                "Sending Equalization Duration to the solar charge controller",
                                0, 180 );
                    break;
                }

            case 5:
                {
                    int iVal = getIntParam( 
                                "Enter Boost Time (minutes): ",
                                "Minutes Boost is applied (usually 120)",
                                "Error: Valid durations are 10 to 180", 
                                "Sending Boost Duration to the solar charge controller",
                                10, 180 );
                    break;
                }


            default:
                break;
        }
    } while (selection != NUM_MENU_ITEMS);
}

