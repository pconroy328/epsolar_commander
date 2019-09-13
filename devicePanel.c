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
#include "libepsolar.h"

extern  int     MaxRows;
extern  int     MaxCols;
static  WINDOW  *panel;

#define getContext      epsolarModbusGetContext

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

#define MIN_SELECTION   1
#define MAX_SELECTION   6

extern  int dialogGetInteger2(const char *title, const char *prompt, const int minVal, const int maxVal, const int defaultVal);

// -----------------------------------------------------------------------------
void    editDataRefreshValue()
{
    suspendUpdatingPanels();
    int refreshValue = dialogGetInteger( "Data Refresh Time", "Sets seconds between data updates\nDefault is 10 seconds", 1, 3600, 10 );
    
    refreshContollerDataTime = refreshValue;
    resumeUpdatingPanels();
    Logger_LogInfo( "Changing data refresh time to [%d]\n", refreshValue );
    showDevicePanel();
}

// -----------------------------------------------------------------------------
void    editDeviceClocktime()
{
    suspendUpdatingPanels();
    
    (void) dialogGetInteger( "Device Date and Time", "Press <Enter> to set device time to 'now'", 0, 9999, 1 );
    
    setRealtimeClockToNow( getContext() );
    resumeUpdatingPanels();
    showDevicePanel();
}

// -----------------------------------------------------------------------------
void    editDeviceOverTemperature()
{
    suspendUpdatingPanels();
    
    float val = dialogGetFloat( "Charge Controller Limit", 
            "The Upper Temperature for the Controller\nUse Fahrenheit. Factory default is 185*F", 
            100.0, 210.0, 185.0, 0, 4 );
    setBatteryTemperatureWarningUpperLimit( getContext(), val );
    resumeUpdatingPanels();
    Logger_LogInfo( "Setting Device Over Temperature to %f'\n", val );
    showDevicePanel();       
}

// -----------------------------------------------------------------------------
void    editDeviceRecoveryTemperature()
{
    suspendUpdatingPanels();
    
    float val = dialogGetFloat( "Charge Controller Limit", 
            "The Recovery Temperature for the Controller\nUse Fahrenheit. Factory default is 165*F", 
            85.0, 175.0, 165.0, 0, 4 );
    setBatteryTemperatureWarningUpperLimit( getContext(), val );
    resumeUpdatingPanels();
    Logger_LogInfo( "Setting Device Recovery Temperature to %f'\n", val );
    showDevicePanel();       

}
// -----------------------------------------------------------------------------
void    editBatteryUpperLimitTemperature()
{
    suspendUpdatingPanels();
    
    float val = dialogGetFloat( "Battery Limit", 
            "The Upper Limit Temperature for the Battery\nUse Fahrenheit. Factory default is 150*F", 
            120.0, 175.0, 150.0, 0, 4 );
    setBatteryTemperatureWarningUpperLimit( getContext(), val );
    resumeUpdatingPanels();
    Logger_LogInfo( "Setting battery Upper Limit Temperature to %f'\n", val );
    showDevicePanel();   
}

// -----------------------------------------------------------------------------
void    editBatteryLowerLimitTemperature()
{
    suspendUpdatingPanels();
    
    float val = dialogGetFloat( "Battery Limit", 
            "The Lower Limit Temperature for the Battery\nUse Fahrenheit. Factory default is -40.0*F", 
            -40.0, 32.0, -40.0, 0, 4 );
    setBatteryTemperatureWarningLowerLimit( getContext(), val );
    resumeUpdatingPanels();
    Logger_LogInfo( "Setting battery Lower Limit Temperature to %f'\n", val );
    showDevicePanel();   
}

// -----------------------------------------------------------------------------
void    editDevicePanel ()
{
    // modal... oh well...
    int done = FALSE;
    
    char    buffer[ 10 ];
    int     selection = 0;
   
    while (!done) {
        memset( buffer, '\0', sizeof buffer );
        getEditMenuSelection( buffer, sizeof buffer );
        
        if (!isdigit( buffer[ 0 ] )) {
            break;
        }
        
        selection = atoi( buffer );
        if (selection >= MIN_SELECTION && selection <= MAX_SELECTION)
            done = TRUE;
        else {
            beep();
            flash();
        }
    }
    
    if (done) {
        int value = atoi( buffer );
        Logger_LogInfo( "About to edit menu selection [%d]\n", value );
        
        switch (selection) {
            case 1:     editDeviceOverTemperature();        break;
            case 2:     editDeviceRecoveryTemperature();    break;
            case 3:     editBatteryUpperLimitTemperature(); break;
            case 4:     editBatteryLowerLimitTemperature(); break;
            case 5:     editDeviceClocktime();              break;
            case 6:     editDataRefreshValue();             break;
        }
    }
}
