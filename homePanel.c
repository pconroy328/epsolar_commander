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

#include "epsolar_commander.h"

static  WINDOW *pvWin, *batteryWin, *loadWin, *ctlWin, *egWin, *ecWin;

// -----------------------------------------------------------------------------
void    paintPVGroupData()
{
    addFloatField( pvWin, 1, 1, "Voltage", pvInputVoltage, 1, 4 );
    addFloatField( pvWin, 3, 1, "Current", pvInputCurrent, 2, 4 );
    addFloatField( pvWin, 5, 1, "Power", pvInputPower, 2, 4 );
    addTextField( pvWin, 7, 1, "Status",  pvInputStatus );
}

// -----------------------------------------------------------------------------
static
void    paintBatteryGroupData()
{
    addFloatField( batteryWin, 1, 1, "Voltage", batteryVoltage, 1, 4 );
    addFloatField( batteryWin, 1, 14, "Current", batteryCurrent, 2, 4 );
    addFloatField( batteryWin, 3, 1, "Min", minBatteryVoltage, 1, 4 );
    addFloatField( batteryWin, 3, 14, "Max", maxBatteryVoltage, 1, 4 );
    addFloatField( batteryWin, 5, 1, "Temp", batteryTemp,  0, 3 );
    addFloatField( batteryWin, 5, 14, "SoC", batterySoC,  0, 3 );
    addTextField( batteryWin, 7, 1, "Charging", chargingStatus );
    addTextField( batteryWin, 7, 14, "Status", batteryStatusVoltage );    
}

// -----------------------------------------------------------------------------
static
void    paintLoadGroupData()
{
    addFloatField( loadWin, 1, 1, "Voltage", loadVoltage, 1, 4 );
    addFloatField( loadWin, 3, 1, "Current", loadCurrent, 2, 4 );
    addFloatField( loadWin, 5, 1, "Power", loadPower, 2, 4 );
    addTextField( loadWin, 7, 1, "Status", dischargeRunning );
}

// -----------------------------------------------------------------------------
static
void    paintDeviceGroupData()
{
    addFloatField( ctlWin, 1, 1, "Temp", deviceTemp, 1, 5 );
    addTextField( ctlWin, 3, 1, "Status", "Normal" );    
    addTextField( ctlWin, 5, 1, "Date/Time", controllerClock );
    addTextField( ctlWin, 7, 1, "Night Time", (isNight? "Yes" : "No " ) );
}

// -----------------------------------------------------------------------------
static
void    paintGeneratedGroupData()
{
    addFloatField( egWin, 1, 2, "Daily", energyGeneratedToday, 2, 4 );
    addFloatField( egWin, 3, 2, "Monthly", energyGeneratedMonth, 2, 4 );
    addFloatField( egWin, 5, 2, "Annual", energyGeneratedYear, 1, 4 );
    addFloatField( egWin, 7, 2, "Total", energyGeneratedTotal, 1, 4 );    
}
// -----------------------------------------------------------------------------
static
void    paintConsumedGroupData()
{
    addFloatField( ecWin, 1, 2, "Daily", energyConsumedToday, 2, 4 );
    addFloatField( ecWin, 3, 2, "Monthly", energyConsumedMonth, 2, 4 );
    addFloatField( ecWin, 5, 2, "Annual", energyConsumedYear, 1, 4 );
    addFloatField( ecWin, 7, 2, "Total", energyConsumedTotal, 1, 4 );    
}

// -----------------------------------------------------------------------------
void    paintHomePanelData()
{
    paintPVGroupData();    
    paintBatteryGroupData();
    paintLoadGroupData();
    paintDeviceGroupData();
    paintGeneratedGroupData();
    paintConsumedGroupData();
    // refresh();
}


// -----------------------------------------------------------------------------
void    showHomePanel ()
{
    setActivePanel( HOME_PANEL );
    
    int     pvY = 0, pvX = 0;
    int     pvRows = 10;
    int     pvCols = 17;
    
    pvWin = createGroup( &pvWin, pvY, pvX, pvRows, pvCols, "PV" );
    paintPVGroupData();
   
    int     battY = pvY;
    int     battX = pvCols;
    int     battRows = pvRows;
    int     battCols = 25;
    batteryWin = createGroup( &batteryWin, battY, battX, battRows, battCols, "Battery" );
    paintBatteryGroupData();

    int     loadY = pvY;
    int     loadX = pvCols + battCols;
    int     loadRows = pvRows;
    int     loadCols = 16;
    loadWin = createGroup( &loadWin, loadY, loadX, loadRows, loadCols, "Load" );
    paintLoadGroupData();
   
    int     ctlY = pvY;
    int     ctlX = pvCols + battCols + + loadCols;
    int     ctlRows = pvRows;
    int     ctlCols = 21;
    ctlWin = createGroup( &ctlWin, ctlY, ctlX, ctlRows, ctlCols, "Controller" );
    paintDeviceGroupData();
    
    int egY = pvY + pvRows;
    int egX = pvX;
    int egRows = 10;
    int egCols = 15;
    egWin = createGroup( &egWin, egY, egX, egRows, egCols, "Generated" );
    paintGeneratedGroupData();
    
    int ecY = egY ;  // + egRows;
    int ecX = pvX + egCols;
    int ecRows = 10;
    int ecCols = 15;
    ecWin = createGroup( &ecWin, ecY, ecX, ecRows, ecCols, "Consumed" );
    paintConsumedGroupData();
    
    //refresh();
}

// -----------------------------------------------------------------------------
void    clearHomePanel ()
{
    werase( pvWin );        delwin( pvWin );
    werase( batteryWin );   delwin( batteryWin );
    werase( loadWin );      delwin( loadWin );
    werase( ctlWin );       delwin( ctlWin );
    werase( egWin );        delwin( egWin );
    werase( ecWin );        delwin( ecWin );
    werase( stdscr );
    refresh();
}
