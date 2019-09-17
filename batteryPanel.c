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
#include "libepsolar.h"



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
    HfloatAddTextField( settingsPanel, beginRow++,beginCol, " 4. Temp Comp Coeff    ", temperatureCompensationCoefficient, 1, 4 );

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
        

// -----------------------------------------------------------------------------
void    editBatteryType ()
{
    suspendUpdatingPanels();
    
    int val = 1;
    if (dialogGetInteger( "Battery", 
            "Sets the type of Battery. Use (1) Sealed\n(2) Gel, (3) Flooded, (4) User", &val, 0, 3, 1 ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Battery Type to %d\n", val );
        eps_setBatteryType( val );
    }

    resumeUpdatingPanels();
    showDevicePanel();   
}

// -----------------------------------------------------------------------------
void    editBatteryCapacity ()
{
    suspendUpdatingPanels();
    
    int val = 200;
    if (dialogGetInteger( "Battery", 
            "Sets the Battery Capacity in Amp Hours. Default is 200 AH", &val, 0, 1000, 200 ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Battery Capacity to %d\n", val );
        eps_setBatteryCapacity( val );
    }

    resumeUpdatingPanels();
    showDevicePanel();   
}

// -----------------------------------------------------------------------------
void    editChargingMode ()
{
    suspendUpdatingPanels();
    
    int val = 200;
    if (dialogGetInteger( "Battery", 
            "Sets the Charging Mode for the battery. Use (0) for Voltage\nCompensation (1) for State Of Charge", &val, 0, 1, 0 ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Battery Charging Mode to %d\n", val );
        eps_setManagementModesOfBatteryChargingAndDischarging( val );
    }

    resumeUpdatingPanels();
    showDevicePanel();   
}


#define MIN_SELECTION   1
#define MAX_SELECTION   21
// -----------------------------------------------------------------------------
void    editBatteryPanel ()
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
            case 1:     editBatteryType();                  break;
            case 2:     editChargingMode();    break;
            case 3:     editBatteryCapacity(); break;
            //case 4:     editBatteryLowerLimitTemperature(); break;
            //case 5:     editDeviceClocktime();              break;
            //case 6:     editDataRefreshValue();             break;
        }
    }
}
