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
static
void    paintRatedData ()
{
    //
    // Figure out how to space three values in the top window
    int maxCols;
    int maxRows;
    getmaxyx( ratedPanel, maxRows, maxCols ); 
    
    int beginCol = 3;
    HfloatAddTextField( ratedPanel, 1, beginCol, "Voltage", batteryRatedVoltage, 2, 4 );
    
    beginCol = (maxCols / 2);
    beginCol -= 12;
    HfloatAddTextField( ratedPanel, 1, beginCol, "Load Current", batteryRatedLoadCurrent, 2, 4 );
    
    beginCol = maxCols;
    beginCol -= 24;
    HfloatAddTextField( ratedPanel, 1, beginCol, "Charge Current", batteryRatedChargingCurrent, 2, 4 ); 
    
    wrefresh( ratedPanel );
}

// -----------------------------------------------------------------------------
static
void    paintSettingsData()
{
    int beginCol = 2;
    int beginRow = 2;
    HaddTextField( settingsPanel, beginRow++, beginCol, "Battery Type", batteryType );

    HaddTextField( settingsPanel, beginRow++, beginCol, "Charging Mode", "0x9070" );
    HintAddTextField( settingsPanel, beginRow++, beginCol, "Capacity", batteryCapacity, 0, 4 );
    HfloatAddTextField( settingsPanel, beginRow++, beginCol, "Temp C. Coeff", tempertureCompensationCoefficient, 1, 4 );

    HfloatAddTextField( settingsPanel, beginRow++, beginCol, "Over V Disconnect V", tempertureCompensationCoefficient, 1, 4 );
    wrefresh( settingsPanel );
}

// -----------------------------------------------------------------------------
void    showBatteryPanel()
{
    setActivePanel( BATTERY_PANEL );
    int     battY = 0;
    int     battX = 0;
    int     battRows = 3;
    int     battCols = MaxCols;
    ratedPanel = grouping( &ratedPanel, battY, battX, battRows, battCols, "Battery Rated Data" );
    paintRatedData();
    
    int     battY2 = battRows;
    int     battX2 = 0;
    int     battRows2 = 20;
    int     battCols2 = MaxCols;
    settingsPanel = grouping( &settingsPanel, battY2, battX2, battRows2, battCols2, "Battery Settings" );
    paintSettingsData();
    
    refresh();
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
        