/*
 */
#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include "ncurses-menu.h"

extern int getIntParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const int maxVal, const int minVal);



static    char batteryMenu[][100] = { 
    { "Type" },
    { "Capacity" },
    { "Temp Comp Coeff" },
    { "Return" },};

#define NUM_MENU_ITEMS ( (sizeof( batteryMenu ) / 100 ) )



// ----------------------------------------------------------------------------
static  int isValidBatteryType (const int typeValue)
{
    if (typeValue >= 1 && typeValue <= 4)
        return TRUE;
    
    return FALSE;
}

// ----------------------------------------------------------------------------
static  int isValidBatteryCapacity (const int capacityValue)
{
    if (capacityValue >= 1)
        return TRUE;
    
    return FALSE;
}

// ----------------------------------------------------------------------------
int doBatteryMenu (int topLeft_Y, int topLeft_X, int menuMinWidth)
{
    char    aStr[ 80 ];
    int     selection = -1;

    do {
        selection = print_menu ( topLeft_Y + 3, 
                            topLeft_X + 20, 
                            NUM_MENU_ITEMS, 
                            menuMinWidth,
                            "Battery Parameters", batteryMenu, 1 );
        
        
        switch (selection) {

            case    1:  
                {
                    int batteryType = getIntParam( 
                                "Enter Battery Type: ",
                                "Valid Types are: (1) Sealed Lead Acid, (2) Gel, (3) Flooded, (0) User Defined",
                                "Error: Valid Battery Types are 0 .. 3", 
                                "Sending Battery Type to the solar charge controller",
                                0, 3 );
                    //
                    //  send battery type MQTT message
                }
                break;
                
            case 2:
                {
                    int batteryCapacity = getIntParam( 
                                "Enter Rated Battery Capacity (AH): ",
                                "This is the Rated Capacity of the Battery in Amp Hours (AH)",
                                "Error: Valid Battery Capacities are numbers greater than 0", 
                                "Sending Battery Capacity to the solar charge controller",
                                1, 1000 );
                }
                break;
            default:
                break;
        }
    } while (selection != NUM_MENU_ITEMS);

}