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


//
// If the battery is in 12V mode - then the mins are always 9.0V and the Max 17.0V
//  You double those for 24V systems

static  float   batteryMinVoltages = 9.0;
static  float   batteryMaxVoltages = 17.0;


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
    
    //
    // Adjust the min and max
    if (batteryRatedVoltage == 24.0) {
        Logger_LogWarning( "Battery Rated Voltage is 24V - Min and Max's doubling!\n" );
        batteryMinVoltages = 18.0;
        batteryMaxVoltages = 34.0;
    }
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
            "Sets the type of Battery. Use (1) Sealed, (2) Gel\n(3) Flooded, (0) User Defined", &val, 0, 3, 1 ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Battery Type to %d\n", val );
        eps_setBatteryType( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();   
}

// -----------------------------------------------------------------------------
void    editBatteryCapacity ()
{
    suspendUpdatingPanels();
    
    int val = 200;
    if (dialogGetInteger( "Battery", 
            "Sets the Battery Capacity in Amp Hours\nDefault is 200 AH", &val, 0, 1000, 200 ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Battery Capacity to %d\n", val );
        eps_setBatteryCapacity( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();   
}

// -----------------------------------------------------------------------------
void    editChargingMode ()
{
    suspendUpdatingPanels();
    
    int val = 200;
    if (dialogGetInteger( "Battery", 
            "Sets the Battery Charging Mode. Use (0) for Voltage\nCompensation, (1) for State Of Charge", &val, 0, 1, 0 ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Battery Charging Mode to %d\n", val );
        eps_setManagementModesOfBatteryChargingAndDischarging( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();   
}

// -----------------------------------------------------------------------------
void    editTempCompCoeff ()
{
    suspendUpdatingPanels();
    
    int val = 3;
    if (dialogGetInteger( "Battery", 
            "Sets the Temperature Compensation Coefficient \nDefault is 3", &val, 0, 9, 3 ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Temperature Compensation Coefficient to %d\n", val );
        eps_setTemperatureCompensationCoefficient( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();   
}

// -----------------------------------------------------------------------------
void    editOverVDisconnectV()
{
    suspendUpdatingPanels();
    
    float   val = 16.0;
    if (dialogGetFloat( "Battery", 
            "Set the Over Voltage DISCONNECT Voltage\nDefault is 16.0V", &val, 
            batteryMinVoltages, batteryMaxVoltages, 
            16.0,           // figure out a way to double this
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Over Voltage Disconnect Voltage to %f\n", val );
        eps_setHighVoltageDisconnect( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editOverVReconnectV()
{
    suspendUpdatingPanels();
    
    float   val = 15.0;
    if (dialogGetFloat( "Battery", 
            "Set the Over Voltage RECONNECT Voltage\nDefault is 15.0V", &val,
            batteryMinVoltages, batteryMaxVoltages, 
            15.0,           // figure out a way to double this
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Over Voltage Reconnect Voltage to %f\n", val );
        eps_setOverVoltageReconnect( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editEqualizationV()
{
    suspendUpdatingPanels();
    
    float   val = 15.0;
    if (dialogGetFloat( "Battery", 
            "Set the Equalization Voltage\nDefault is 14.6V", &val,
            batteryMinVoltages, batteryMaxVoltages, 
            14.6,           // figure out a way to double this
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Equalization Voltage to %f\n", val );
        eps_setEqualizationVoltage( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editBoostV()
{
    suspendUpdatingPanels();
    
    float   val = 15.0;
    if (dialogGetFloat( "Battery", 
            "Set the Boost Voltage\nDefault is 14.4V", &val,
            batteryMinVoltages, batteryMaxVoltages, 
            14.4,           // figure out a way to double this
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Boost Voltage to %f\n", val );
        eps_setBoostingVoltage( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editFloatV()
{
    suspendUpdatingPanels();
    
    float   val = 15.0;
    if (dialogGetFloat( "Battery", 
            "Set the Float Voltage\nDefault is 13.8V", &val,
            batteryMinVoltages, batteryMaxVoltages, 
            14.4,           // figure out a way to double this
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Float Voltage to %f\n", val );
        eps_setFloatingVoltage( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
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
            case 1:     editBatteryType();          break;
            case 2:     editChargingMode();         break;
            case 3:     editBatteryCapacity();      break;
            case 4:     editTempCompCoeff();        break;
            case 5:     editOverVDisconnectV();     break;
            case 6:     editOverVReconnectV();      break;

            case 7:     editEqualizationV();      break;
            case 8:     editBoostV();      break;
            case 9:     editFloatV();      break;
        }
    }
}
