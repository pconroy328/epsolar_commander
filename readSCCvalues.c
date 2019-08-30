/*
 */

#include <pthread.h>
#include <log4c.h>
#include <modbus/modbus.h>
#include <libepsolar.h>
#include <errno.h>

static  modbus_t    *ctx = NULL;

float   deviceTemp = -999.99;
float   batteryTemp = -999.99;
float   loadPower = -999.99;
float   loadCurrent = -999.99;
float   loadVoltage = -999.99;
float   pvInputPower =  -999.99;
float   pvInputCurrent = -999.99;
float   pvInputVoltage = -999.99;
int     isNight = -1;

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