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
char    *batteryStatusInnerResistance = "?";
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
char    *batteryRatedVoltageCode = "?";

int     boostDuration = -9;
int     equalizeDuration = -9;
float   chargingLimitVoltage = -9.9;
float   dischargingLimitVoltage = -9.9;
float   lowVoltageDisconnectVoltage = -9.9;
float   lowVoltageReconnectVoltage = -9.9;;
float   underVoltageWarningVoltage = -9.9;;
float   underVolatageWarningReconnectVoltage = -9.9;;
float   batteryChargePercent = -9.9;
float   batteryDischargePercent = -9.9;

int     loadControlMode = -9;
float   nighttimeThresholdVoltage = -9.9;
int     nighttimeThresholdVoltageDelay = -9;
float   daytimeThresholdVoltage= -9.9;
int     daytimeThresholdVoltageDelay= -9;
char    *timerOneOn = "?", *timerOneOff = "?";
char    *timerTwoOn = "?", *timerTwoOff = "?";
char    *workOneTime = "?", *workTwoTime = "?";
char    *lengthOfNight = "??:??";

int     HH_LON, MM_LON;
int     backlightTime;
int     HH_T1On, MM_T1On, SS_T1On;
int     HH_T2On, MM_T2On, SS_T2On;
int     HH_T1Off, MM_T1Off, SS_T1Off;
int     HH_T2Off, MM_T2Off, SS_T2Off;
int     HH_WT1, MM_WT1, HH_WT2, MM_WT2;     




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

// -----------------------------------------------------------------------------
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
        batteryStatusInnerResistance = getBatteryStatusInnerResistance( batteryStatusBits );
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
        batteryChargingMode = getManagementModesOfBatteryChargingAndDischarging( ctx );
        batteryCapacity = getBatteryCapacity( ctx );
        tempertureCompensationCoefficient = getTempertureCompensationCoefficient( ctx );
        overVDisconnectV = getHighVoltageDisconnect( ctx );
        overVReconnectV = getOverVoltageReconnect( ctx );
        equalizationVoltage = getEqualizationVoltage( ctx );
        boostVoltage = getBoostingVoltage( ctx );
        floatVoltage = getFloatingVoltage( ctx );
        boostReconnectVoltage = getBoostReconnectVoltage( ctx );
        batteryRatedVoltageCode = getBatteryRatedVoltageCode( ctx );                                  

        boostDuration = getBoostDuration( ctx );
        equalizeDuration = getEqualizeDuration( ctx );
        chargingLimitVoltage = getChargingLimitVoltage( ctx );
        dischargingLimitVoltage = getDischargingLimitVoltage( ctx );
        lowVoltageDisconnectVoltage = getLowVoltageDisconnectVoltage( ctx );
        lowVoltageReconnectVoltage = getLowVoltageReconnectVoltage( ctx );
        underVoltageWarningVoltage = getUnderVoltageWarningVoltage( ctx );
        underVolatageWarningReconnectVoltage = getUnderVoltageWarningRecoverVoltage( ctx );
        
        batteryChargePercent = getChargingPercentage( ctx );
        batteryDischargePercent = getDischargingPercentage( ctx );
        
        
        loadControlMode = getLoadControllingMode( ctx );
        nighttimeThresholdVoltage = getNightTimeThresholdVoltage( ctx );
     nighttimeThresholdVoltageDelay = getLightSignalStartupDelayTime( ctx );
   daytimeThresholdVoltage = getDayTimeThresholdVoltage( ctx );
     daytimeThresholdVoltageDelay = getLightSignalCloseDelayTime( ctx );


     getLengthOfNight( ctx, &HH_LON, &MM_LON );
     backlightTime = getBacklightTime( ctx );
     
     getTurnOffTiming1( ctx, &HH_T1Off, &MM_T1Off, &SS_T1Off );
     getTurnOnTiming1( ctx, &HH_T1On, &MM_T1On, &SS_T1On );
     getTurnOffTiming2( ctx, &HH_T2Off, &MM_T2Off, &SS_T2Off );
     getTurnOnTiming2( ctx, &HH_T2On, &MM_T2On, &SS_T2On );

     getWorkingTimeLength1( ctx, &HH_WT1, &MM_WT1 );
     getWorkingTimeLength2( ctx, &HH_WT2, &MM_WT2 );

     
extern  void    setWorkingTimeLength2( modbus_t *ctx, const int hour, const int minute );
extern  void    setWorkingTimeLength1( modbus_t *ctx, const int hour, const int minute );

        
        if (getActivePanel() == HOME_PANEL)
            paintHomePanelData();
        else if (getActivePanel() == BATTERY_PANEL)
            paintBatteryPanelData();
        else if (getActivePanel() == LOAD_PANEL)
            paintLoadPanelData();
    
        sleep( 10 );
    }

    return NULL;

}
