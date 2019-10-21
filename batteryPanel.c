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
    addFloatField( ratedPanel, 1, beginCol, "Voltage", batteryRatedVoltage, 2, 4 );
    
    beginCol = (maxCols / 2);
    beginCol -= 12;
    addFloatField( ratedPanel, 1, beginCol, "Load Current", batteryRatedLoadCurrent, 2, 4 );
    
    beginCol = maxCols;
    beginCol -= 24;
    addFloatField( ratedPanel, 1, beginCol, "Charge Current", batteryRatedChargingCurrent, 2, 4 ); 
    
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
    addTextField( settingsPanel, beginRow++, beginCol,     " 1. Battery Type       ", batteryType );

    addTextField( settingsPanel, beginRow++, beginCol,     " 2. Charging Mode      ", batteryChargingMode );
    addIntField( settingsPanel, beginRow++, beginCol,  " 3. Capacity (AH)      ", batteryCapacity, 0, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, " 4. Temp Comp Coeff    ", temperatureCompensationCoefficient, 1, 4 );

    addFloatField( settingsPanel, beginRow++,beginCol, " 5. Over V Disconnect V", overVDisconnectV, 1, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, " 6. Over V Reconnect V ", overVReconnectV, 1, 4 );

    addFloatField( settingsPanel, beginRow++,beginCol, " 7. Equalization Volts ", equalizationVoltage, 1, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, " 8. Boost Voltage      ", boostVoltage, 1, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, " 9. Float Voltage      ", floatVoltage, 1, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, "10. Boost Recon Chrg V ", boostReconnectVoltage, 1, 4 );
    
    //
    // Litte format - shove it down a row
    beginRow += 1;
    addFloatField( settingsPanel, beginRow++,beginCol, "11. Battery Charge %   ", batteryChargePercent, 1, 4 );
    wrefresh( settingsPanel );
    
    
    
    beginCol = (MaxCols / 2) + 2;
    beginRow = 2;
    addTextField( settingsPanel, beginRow++, beginCol,     "12. Battery Rated Code   ", batteryRatedVoltageCode );
    addIntField( settingsPanel, beginRow++, beginCol,  "13. Boost Duration       ", boostDuration, 0, 4 );
    addIntField( settingsPanel, beginRow++, beginCol,  "14. Equalization Duration", equalizeDuration, 0, 4 );
    //
    // Litte format - shove it down a row
    beginRow += 1;
    addFloatField( settingsPanel, beginRow++,beginCol, "15. Charging Limit V     ", chargingLimitVoltage, 1, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, "16. Discharging Limit V  ", dischargingLimitVoltage, 1, 4 );    
    addFloatField( settingsPanel, beginRow++,beginCol, "17. Low V Disconnect V   ", lowVoltageDisconnectVoltage, 1, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, "18. Low V Reconnect V    ", lowVoltageReconnectVoltage, 1, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, "19. Low V Warning V      ", underVoltageWarningVoltage, 1, 4 );
    addFloatField( settingsPanel, beginRow++,beginCol, "20. Low V Warning Recon V", underVolatageWarningReconnectVoltage, 1, 4 );
    //
    // Litte format - shove it down a row
    beginRow += 1;
    addFloatField( settingsPanel, beginRow++,beginCol, "21. Battery Discharge %  ", batteryDischargePercent, 1, 4 );
}

// -----------------------------------------------------------------------------
void    showBatteryPanel()
{
    setActivePanel( BATTERY_PANEL );
    int     battY = 0;
    int     battX = 0;
    int     battRows = 3;
    int     battCols = MaxCols;
    ratedPanel = createGroup( &ratedPanel, battY, battX, battRows, battCols, "Battery Rated Data" );
    paintBatteryRatedData();
    
    int     battY2 = battRows;
    int     battX2 = 0;
    int     battRows2 = 20;
    int     battCols2 = MaxCols;
    settingsPanel = createGroup( &settingsPanel, battY2, battX2, battRows2, battCols2, "Battery Settings" );
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
            "Sets the type of Battery. Use (1) Sealed, (2) Gel\n(3) Flooded, (0) User Defined\nDefault is Sealed", &val, 0, 3, 1 ) == INPUT_OK) {
    
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
            "Sets the Battery Charging Mode. Use (0) for Voltage\nCompensation, (1) for State Of Charge\nDefault is Voltage Comp", &val, 0, 1, 0 ) == INPUT_OK) {
    
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
    
    float   val = 13.8;
    if (dialogGetFloat( "Battery", 
            "Set the Float Voltage\nDefault is 13.8V", &val,
            batteryMinVoltages, batteryMaxVoltages, 
            13.8,           // figure out a way to double this
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Float Voltage to %f\n", val );
        eps_setFloatingVoltage( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editBoostChargeReconnectV()
{
    suspendUpdatingPanels();
    
    float   val = 13.2;
    if (dialogGetFloat( "Battery", 
            "Set the Boost Charge Reconnect Voltage\nDefault is 13.2V", &val,
            batteryMinVoltages, batteryMaxVoltages, 
            13.2,           // figure out a way to double this
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Boost Charge Reconnect Voltage to %f\n", val );
        eps_setBoostReconnectVoltage( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editDepthOfCharge()
{
    suspendUpdatingPanels();
    
    int   val = 100;
    if (dialogGetInteger( "Battery", 
            "Set the Depth of Charge Percentage\nDefault is 100%", &val,
            0, 100, 100) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Depth of Charge to %d\n", val );
        eps_setDepthOfCharge( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editDepthOfDischarge()
{
    suspendUpdatingPanels();
    
    int   val = 30;
    if (dialogGetInteger( "Battery", 
            "Set the Depth of Discharge Percentage\nDefault is 30%", &val,
            30, 80, 30 ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Depth of Discharge to %d\n", val );
        eps_setDepthOfDischarge( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editBoostDuration()
{
    suspendUpdatingPanels();
    
    int   val = 120;
    if (dialogGetInteger( "Battery", 
            "Set the max minutes we'll stay in Boost mode\nDefault is 120", &val,
            1, 120, val ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Boost Duration to %d\n", val );
        eps_setBoostDuration( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editEqualizationDuration()
{
    suspendUpdatingPanels();
    
    int   val = 120;
    if (dialogGetInteger( "Battery", 
            "Set the max minutes we'll stay in Equalization mode\nDefault is 120", &val,
            1, 120, val ) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Equalization Duration to %d\n", val );
        eps_setEqualizeDuration( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editChargingLimitVoltage()
{
    suspendUpdatingPanels();
    
    float   val = 15.0;
    if (dialogGetFloat( "Battery", 
            "Set Charging Limit Voltage\nDefault is 15.0V", &val,
            batteryMinVoltages, batteryMaxVoltages, 
            val,           
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Charging Limit Voltage to %f\n", val );
        eps_setChargingLimitVoltage( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}

// -----------------------------------------------------------------------------
void    editDischargingLimitVoltage()
{
    suspendUpdatingPanels();
    
    float   val = 10.6;
    if (dialogGetFloat( "Battery", 
            "Set Discharging Limit Voltage\nDefault is 10.6V", &val,
            batteryMinVoltages, batteryMaxVoltages, 
            val,           
            3, 1) == INPUT_OK) {
    
        Logger_LogInfo( "Setting Discharging Limit Voltage to %f\n", val );
        eps_setDischargingLimitVoltage( val );
    }

    resumeUpdatingPanels();
    showBatteryPanel();       
}


void    editLowVDisconnectV() { }
void    editLowVReconnectV() { }
void    editLowVWarningV() { }
void    editLowVWarningReconnectV() { }

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
            case 10:    editBoostChargeReconnectV();    break;
            case 11:    editDepthOfCharge();            break;
            
            //case 12:    editBatteryRatedCode();     break;
            case 13:    editBoostDuration();        break;
            case 14:    editEqualizationDuration();        break;
            
            case 15:    editChargingLimitVoltage();     break;
            case 16:    editDischargingLimitVoltage();     break;
            
            case 17:    editLowVDisconnectV();          break;
            case 18:    editLowVReconnectV();           break;
            case 19:    editLowVWarningV();             break;
            case 20:    editLowVWarningReconnectV();    break;
            
            case 21:    editDepthOfDischarge();         break;
        }
    }
}
