/*
 */

#include <pthread.h>
#include <log4c.h>
#include <modbus/modbus.h>
#include <libepsolar.h>
#include <errno.h>

static  modbus_t    *ctx = NULL;

float   deviceTemp = -99.99;
float   batteryTemp = -99.99;
float   loadPower = -99.99;
float   loadCurrent = -99.99;
float   loadVoltage = -99.99;
float   pvInputPower =  -99.99;
float   pvInputCurrent = -99.99;
float   pvInputVoltage = -99.99;
int     isNight = -1;
int     batterySoC = -99;
float   batteryVoltage = -99.99;
float   batteryCurrent = -99.99;
float   batteryPower = -99.99;

float   minBatteryVoltage = -99.99;
float   maxBatteryVoltage = -99.99;

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

    Logger_LogInfo( "Load voltage: %.1f, current: %.2f, power: %.2f\n", loadVoltage, loadCurrent, loadPower );
    Logger_LogInfo( "PV voltage: %.1f, current: %.2f, power: %.2f\n", pvInputVoltage, pvInputCurrent, pvInputPower );
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

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
        
        sleep( 10 );
    }

    return NULL;

}