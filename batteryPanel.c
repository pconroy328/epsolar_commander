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
void    paintBatteryRatedData ()
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
void    paintBatterySettingsData()
{
    int beginCol = 2;
    int beginRow = 2;
    HaddTextField( settingsPanel, beginRow++, beginCol,     " 1. Battery Type       ", batteryType );

    HaddTextField( settingsPanel, beginRow++, beginCol,     " 2. Charging Mode      ", batteryChargingMode );
    HintAddTextField( settingsPanel, beginRow++, beginCol,  " 3. Capacity (AH)      ", batteryCapacity, 0, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, " 4. Temp Comp Coeff    ", tempertureCompensationCoefficient, 1, 4 );

    HfloatAddTextField( settingsPanel, beginRow++,beginCol, " 5. Over V Disconnect V", overVDisconnectV, 1, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, " 6. Over V Reconnect V ", overVReconnectV, 1, 4 );

    HfloatAddTextField( settingsPanel, beginRow++,beginCol, " 7. Equalization Volts ", equalizationVoltage, 1, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, " 8. Boost Voltage      ", boostVoltage, 1, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, " 9. Float Voltage      ", floatVoltage, 1, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "10. Boost Recon Chrg V ", boostReconnectVoltage, 1, 4 );
    
    //
    // Litte format - shove it down a row
    beginRow += 1;
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "11. Battery Charge %   ", batteryChargePercent, 1, 4 );
    wrefresh( settingsPanel );
    
    
    
    beginCol = (MaxCols / 2) + 2;
    beginRow = 2;
    HaddTextField( settingsPanel, beginRow++, beginCol,     "12. Battery Rated Code   ", batteryRatedVoltageCode );
    HintAddTextField( settingsPanel, beginRow++, beginCol,  "13. Boost Duration       ", boostDuration, 0, 4 );
    HintAddTextField( settingsPanel, beginRow++, beginCol,  "14. Equalization Duration", equalizeDuration, 0, 4 );
    //
    // Litte format - shove it down a row
    beginRow += 1;
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "15. Charging Limit V     ", chargingLimitVoltage, 1, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "16. Discharging Limit V  ", dischargingLimitVoltage, 1, 4 );    
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "17. Low V Disconnect V   ", lowVoltageDisconnectVoltage, 1, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "18. Low V Reconnect V    ", lowVoltageReconnectVoltage, 1, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "19. Low V Warning V      ", underVoltageWarningVoltage, 1, 4 );
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "20. Low V Warning Recon V", underVolatageWarningReconnectVoltage, 1, 4 );
    //
    // Litte format - shove it down a row
    beginRow += 1;
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, "21. Battery Discharge %  ", batteryDischargePercent, 1, 4 );
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
    paintBatteryRatedData();
    
    int     battY2 = battRows;
    int     battX2 = 0;
    int     battRows2 = 20;
    int     battCols2 = MaxCols;
    settingsPanel = grouping( &settingsPanel, battY2, battX2, battRows2, battCols2, "Battery Settings" );
    paintBatterySettingsData();
    
    refresh();
}

// -----------------------------------------------------------------------------
void    paintBatteryPanelData()
{
    paintBatteryRatedData();
    paintBatterySettingsData();
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
        