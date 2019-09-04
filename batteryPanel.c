/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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
static  WINDOW  *ratedPanel;
static  WINDOW  *settingsPanel;

// -----------------------------------------------------------------------------
void    showBatteryPanel()
{
    setActivePanel( BATTERY_PANEL );
    int     battY = 0;
    int     battX = 0;
    int     battRows = 5;
    int     battCols = MaxCols;
    ratedPanel = grouping( &ratedPanel, battY, battX, battRows, battCols, "Battery Rated Data" );
    
    
    int     battY2 = battRows;
    int     battX2 = 0;
    int     battRows2 = 18;
    int     battCols2 = MaxCols;
    settingsPanel = grouping( &settingsPanel, battY2, battX2, battRows2, battCols2, "Battery Settings" );
    refresh();
}

// -----------------------------------------------------------------------------
static
void    paintRatedData ()
{
    //
    // Figure out how to space three values in the top window
    int maxCols;
    int maxRows;
    getmaxyx( ratedPanel, maxRows, maxCols ); 
    
    int     beginCol = (maxCols / 3);
    floatAddTextField( ratedPanel, 1, beginCol, "Voltage", batteryRatedVoltage, 1, 4 );
    
    beginCol = (maxCols / 2);
    beginCol -= 8;
    floatAddTextField( ratedPanel, 1, beginCol, "Load Current", batteryRatedLoadCurrent, 2, 4 );
    
    beginCol = (maxCols / 3);
    beginCol *= 2;
    floatAddTextField( ratedPanel, 1, beginCol, "Charge Current", batteryRatedChargingCurrent, 2, 4 );    
}

// -----------------------------------------------------------------------------
static
void    paintSettingsData()
{
    
}

// -----------------------------------------------------------------------------
void    paintBatteryPanelData()
{
    paintRatedData();
    paintSettingsData();
}

// -----------------------------------------------------------------------------
void    clearBatteryPanel()
{
    werase( ratedPanel );
    werase( settingsPanel );
    delwin( ratedPanel );
    delwin( settingsPanel );
    werase( stdscr );
    refresh();
}


void    clearLoadPanel() {}
void    clearDevicePanel() {}
void    showLoadPanel() {}
void    showDevicePanel() {}
        