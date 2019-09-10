/*
 */

#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <libmqttrv.h>
#include <log4c.h>
#include <libepsolar.h>
#include <pthread.h>
#include "epsolar_commander.h"

extern  int     MaxRows;
extern  int     MaxCols;
static  WINDOW  *panel;


// -----------------------------------------------------------------------------
static
void    paintDeviceData ()
{
    int beginRow = 1;
    int beginCol = 3;
    HfloatAddTextField( panel, beginRow++, beginCol, "1. Over Temperature               ", deviceOverTemperature, 1, 3 );
    HfloatAddTextField( panel, beginRow++, beginCol, "2. Recovery Temperature           ", deviceRecoveryTemperature, 1, 3 );
    HfloatAddTextField( panel, beginRow++, beginCol, "   Current Device Temperature     ", deviceTemp, 1, 3 );
    
    beginRow += 1;
    HfloatAddTextField( panel, beginRow++, beginCol, "3. Battery Upper Limit Temperature", batteryUpperLimitTemperature, 1, 3 );
    HfloatAddTextField( panel, beginRow++, beginCol, "4. Battery Lower Limit Temperature", batteryLowerLimitTemperature, 1, 3 );
    HfloatAddTextField( panel, beginRow++, beginCol, "   Current Battery Temperature    ", batteryTemp, 1, 3 );
    
    beginRow += 1;
    HaddTextField( panel, beginRow++, beginCol, "5. Controller Time", controllerClock );
    HaddTextField( panel, beginRow++, beginCol, "   Computer   Time", computerClock );
    
    beginRow += 1;
    HintAddTextField( panel, beginRow++, beginCol, "6. Data Refresh Time", refreshContollerDataTime, 1, 2 );
    wrefresh( panel );
}

// -----------------------------------------------------------------------------
void    showDevicePanel()
{
    setActivePanel( DEVICE_PANEL );

    int     startY = 0;
    int     startX = 0;
    int     nRows = MaxRows - 1;
    int     nCols = MaxCols;

    panel = grouping( &panel, startY, startX, nRows, nCols, "Solar Charge Controller" );
    paintDeviceData();
}

// -----------------------------------------------------------------------------
void    paintDevicePanelData()
{
    paintDeviceData();
}

// -----------------------------------------------------------------------------
void    clearDevicePanel()
{
    werase( panel );
    delwin( panel );
    werase( stdscr );
    refresh();
}


// -----------------------------------------------------------------------------
void    editDevicePanel ()
{
    // modal... oh well...
    int done = FALSE;
    
    char    buffer[ 10 ];
   
    while (!done) {
        memset( buffer, '\0', sizeof buffer );
        getEditMenuSelection( buffer, sizeof buffer );
        
        if (!isdigit( buffer[ 0 ] )) {
            break;
        }        
    }
    
    if (done) {
        int value = atoi( buffer );
        Logger_LogInfo( "About to edit menu selection [%d]\n", value );
    }
}