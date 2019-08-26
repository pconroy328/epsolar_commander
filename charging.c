/*
 * 
 *     { .command = "HVD",     .fargs = FLOATARG,      .f = setHighVoltageDisconnect },
    { .command = "CLV",     .fargs = FLOATARG,      .f = setChargingLimitVoltage },
    { .command = "OVR",     .fargs = FLOATARG,      .f = setOverVoltageReconnect },
    { .command = "EV",      .fargs = FLOATARG,      .f = setEqualizationVoltage },
    { .command = "BV",      .fargs = FLOATARG,      .f = setBoostVoltage },
    { .command = "FV",      .fargs = FLOATARG,      .f = setFloatVoltage },
    { .command = "BRV",     .fargs = FLOATARG,      .f = setBoostReconnectVoltage },
    { .command = "LVR",     .fargs = FLOATARG,      .f = setLowVoltageReconnect },

 */
#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include "ncurses-menu.h"

extern int      getIntParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const int maxVal, const int minVal);
extern  double  getFloatParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const double minVal, const double maxVal);




static    char aMenu[][100] = { 
    { "Equalization Voltage" },
    { "Boost Voltage" },
    { "Float Voltage" },
    { "Equalization Duration" },
    { "Boost Duration" },
    
    { "Return" },};

#define NUM_MENU_ITEMS ( (sizeof( aMenu ) / 100 ) )


//-- ---------------------------------------------------------------------------
int doChargingMenu (int topLeft_Y, int topLeft_X, int menuMinWidth)
{
    char    aStr[ 80 ];
    int     selection = -1;

    do {
        selection = print_menu ( topLeft_Y + 3, 
                            topLeft_X + 20, 
                            NUM_MENU_ITEMS, 
                            menuMinWidth,
                            "Charging Parameters", aMenu, 1 );
        
        
        switch (selection) {

            case    1:  
                {
                    double  dVal = getFloatParam( 
                                "Enter Equalization Voltage: ",
                                "Aka Absorbtion Voltage (last 20%)",
                                "Error: Valid voltages are 9.0V to 17.0V", 
                                "Sending Equalization Voltage to the solar charge controller",
                                9.0, 17.0 );
                    //
                    //  send battery type MQTT message
                }
                break;
                
            case 2:
                {
                    double  dVal = getFloatParam( 
                                "Enter Boost Voltage: ",
                                "Aka Bulk Voltage (first 80%)",
                                "Error: Valid voltages are 9.0V to 17.0V", 
                                "Sending Boost Voltage to the solar charge controller",
                                9.0, 17.0 );
                    break;
                }
            
            case 3:
                {
                    double  dVal = getFloatParam( 
                                "Enter Float Voltage: ",
                                "Keeps a charged battery at 100%",
                                "Error: Valid voltages are 9.0V to 17.0V", 
                                "Sending Float Voltage to the solar charge controller",
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

