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
    HfloatAddTextField( panel, beginRow++, beginCol, "Over Temperature               ", deviceOverTemperature, 1, 3 );
    HfloatAddTextField( panel, beginRow++, beginCol, "Recovery Temperature           ", deviceRecoveryTemperature, 1, 3 );
    HfloatAddTextField( panel, beginRow++, beginCol, "Current Device Temperature     ", deviceTemp, 1, 3 );
    
    beginRow += 1;
    HfloatAddTextField( panel, beginRow++, beginCol, "Battery Upper Limit Temperature", batteryUpperLimitTemperature, 1, 3 );
    HfloatAddTextField( panel, beginRow++, beginCol, "Battery Lower Limit Temperature", batteryLowerLimitTemperature, 1, 3 );
    HfloatAddTextField( panel, beginRow++, beginCol, "Current Battery Temperature    ", batteryTemp, 1, 3 );
    
    beginRow += 1;
    HaddTextField( panel, beginRow++, beginCol, "Controller Time", controllerClock );
    HaddTextField( panel, beginRow++, beginCol, "Computer   Time", computerClock );
    
    beginRow += 1;
    HintAddTextField( panel, beginRow++, beginCol, "Data Refresh Time", refreshContollerDataTime, 1, 2 );
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
