/*
 */

#include <pthread.h>
#include <log4c.h>
#include <modbus/modbus.h>
#include <libepsolar.h>
#include <epsolar/tracerseries.h>
#include <errno.h>
#include <unistd.h>
#include "epsolar_commander.h"


static  modbus_t    *ctx = NULL;

float   deviceTemp = -9.9;
float   batteryTemp = -9.9;
float   loadPower = -9.9;
float   loadCurrent = -9.9;
float   loadVoltage = -9.9;
float   pvInputPower =  -9.9;
float   pvInputCurrent = -9.9;
float   pvInputVoltage = -9.9;
int     isNight = -1;
int     batterySoC = -9;
float   batteryVoltage = -9.9;
float   batteryCurrent = -9.9;
float   batteryPower = -9.9;

float   minBatteryVoltage = -9.9;
float   maxBatteryVoltage = -9.9;


uint16_t    chargingStatusBits = 0xFFFF;
char    *chargingStatus = "?";
char    *pvInputStatus = "?";

uint16_t    dischargingStatusBits = 0xFFFF;
char    *dischargeRunning = "?";

uint16_t    batteryStatusBits = 0xFFFF;
char    *batteryStatusVoltage = "?";
char    *batteryStatusID = "?";
char    *battweryStatusInnerResistance = "?";
char    *batteryStatusTemperature = "?";

float   energyGeneratedToday = -9.9;
float   energyGeneratedMonth = -9.9;
float   energyGeneratedYear = -9.9;
float   energyGeneratedTotal = -9.9;

float   energyConsumedToday = -9.9;
float   energyConsumedMonth = -9.9;
float   energyConsumedYear = -9.9;
float   energyConsumedTotal = -9.9;

char    controllerClock[ 64 ];

float   batteryRatedVoltage = -9.9;
float   batteryRatedLoadCurrent = -9.9;
float   batteryRatedChargingCurrent = -9.9;
char    *batteryType = "?";
char    *batteryChargingMode = "?";
int     batteryCapacity = -9;
float   tempertureCompensationCoefficient = -9.9;
float   overVDisconnectV = -9.9;
float   overVReconnectV;
float   equalizationVoltage;
float   boostVoltage;
float   floatVoltage;
float   boostReconnectVoltage;



// -----------------------------------------------------------------------------
void    connectLocally ()
{
    char    *devicePort = "/dev/ttyXRUSB0";
    
    //
    // Modbus - open the SCC port. We know it's 115.2K 8N1
    Logger_LogInfo( "Opening %s, 115200 8N1\n", devicePort );
    ctx = modbus_new_rtu( devicePort, 115200, 'N', 8, 1 );
    if (ctx == NULL) {
        Logger_LogFatal( "Unable to create the libmodbus context\n" );
        return;
    }
    
    
    //
    // I don't know if we need to set the SCC Slave ID or not
    Logger_LogInfo( "Setting slave ID to %X\n", 1 );
    modbus_set_slave( ctx, 1 );

    if (modbus_connect( ctx ) == -1) {
        Logger_LogFatal( "Connection failed: %s\n", modbus_strerror( errno ) );
        modbus_free( ctx );
        return;
    }
    
    Logger_LogInfo( "Port to Solar Charge Controller is open.\n", devicePort );
    
    Logger_LogInfo( "Attempting to communicate w/ controller\n" );
    batteryRatedVoltage = getBatteryRealRatedVoltage( ctx );
    batteryRatedLoadCurrent =  getRatedLoadCurrent( ctx );
    batteryRatedChargingCurrent = getRatedChargingCurrent( ctx );

    Logger_LogInfo( "Load voltage: %.1f, current: %.2f, power: %.2f\n", loadVoltage, loadCurrent, loadPower );
    Logger_LogInfo( "PV voltage: %.1f, current: %.2f, power: %.2f\n", pvInputVoltage, pvInputCurrent, pvInputPower );
}


// -----------------------------------------------------------------------------
static  int     whichPanelActive = 0;
void    setActivePanel (const int panelNum)
{
    whichPanelActive = panelNum;
}
int getActivePanel ()
{
    return whichPanelActive;
}


// -----------------------------------------------------------------------------
void *local_readSCCValues ( void *x_void_ptr)
{
    connectLocally();
    while (TRUE) {
        Logger_LogInfo( "Reading values (locally) from SCC\n" );
        
        deviceTemp =  getDeviceTemperature( ctx );
        batteryTemp = getBatteryTemperature( ctx );
        loadPower = getLoadPower( ctx );
        loadCurrent = getLoadCurrent( ctx );
        loadVoltage = getLoadVoltage( ctx );
        pvInputPower =  getPVArrayInputPower( ctx );
        pvInputCurrent = getPVArrayInputCurrent( ctx );
        pvInputVoltage = getPVArrayInputVoltage( ctx );
        isNight = isNightTime( ctx );
        
        batterySoC = getBatteryStateOfCharge( ctx );
        batteryVoltage = getBatteryVoltage( ctx );
        batteryCurrent = getBatteryCurrent( ctx );
        batteryPower = (batteryVoltage * batteryCurrent);
        minBatteryVoltage = getMinimumBatteryVoltageToday( ctx );
        maxBatteryVoltage = getMaximumBatteryVoltageToday( ctx );
        
        chargingStatusBits = getChargingEquipmentStatusBits( ctx );
        chargingStatus = getChargingStatus( chargingStatusBits );
        pvInputStatus = getChargingEquipmentStatusInputVoltageStatus( chargingStatusBits );
        
        batteryStatusBits = getBatteryStatusBits( ctx );
        batteryStatusVoltage = getBatteryStatusVoltage( batteryStatusBits );
        batteryStatusID = getBatteryStatusIdentification( batteryStatusBits );
        battweryStatusInnerResistance = getBatteryStatusInnerResistance( batteryStatusBits );
        batteryStatusTemperature = getBatteryStatusTemperature( batteryStatusBits );

        
        dischargingStatusBits = getdisChargingEquipmentStatusBits( ctx );
        dischargeRunning = (isdischargeStatusRunning( dischargingStatusBits ) ? "On" : "Off" );
        energyConsumedToday = getConsumedEnergyToday( ctx );
        energyConsumedMonth = getConsumedEnergyMonth( ctx );
        energyConsumedYear = getConsumedEnergyYear (ctx)  ;
        energyConsumedTotal =  getConsumedEnergyTotal( ctx );
        energyGeneratedToday = getGeneratedEnergyToday( ctx );
        energyGeneratedMonth = getGeneratedEnergyMonth( ctx );
        energyGeneratedYear = getGeneratedEnergyYear (ctx)  ;
        energyGeneratedTotal = getGeneratedEnergyTotal( ctx );

        getRealtimeClockStr( ctx, &controllerClock[ 0 ], sizeof( controllerClock ) );

        
        batteryType = getBatteryType( ctx );
        //batteryChargingMode = getBatteryChargingMode( ctx );
        batteryCapacity = getBatteryCapacity( ctx );
        tempertureCompensationCoefficient = getTempertureCompensationCoefficient( ctx );
        overVDisconnectV = getHighVoltageDisconnect( ctx );
        overVReconnectV = getOverVoltageReconnect( ctx );
        equalizationVoltage = getEqualizationVoltage( ctx );
        boostVoltage = getBoostingVoltage( ctx );
        floatVoltage = getFloatingVoltage( ctx );
        boostReconnectVoltage = getBoostReconnectVoltage( ctx );
        
        if (getActivePanel() == HOME_PANEL)
            paintHomePanelData();
        else if (getActivePanel() == BATTERY_PANEL)
            paintBatteryPanelData();
    
        sleep( 10 );
    }

    return NULL;

}
