/*
 */

#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <modbus/modbus.h>
#include "log4c.h"
#include "libepsolar.h"
#include "epsolar/tracerseries.h"
#include "epsolar_commander.h"


static  modbus_t            *ctx = NULL;
static  pthread_mutex_t     refreshLock = PTHREAD_MUTEX_INITIALIZER;
static  int                 panelUpdatesAllowed = TRUE;


int     refreshContollerDataTime = 10;
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
char    computerClock[ 64 ];

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

int     HH_LON, MM_LON;
int     backlightTime;
int     HH_T1On, MM_T1On, SS_T1On;
int     HH_T2On, MM_T2On, SS_T2On;
int     HH_T1Off, MM_T1Off, SS_T1Off;
int     HH_T2Off, MM_T2Off, SS_T2Off;
int     HH_WT1, MM_WT1, HH_WT2, MM_WT2;     

float   deviceOverTemperature = -9.9;
float   deviceRecoveryTemperature = -9.9;
float   batteryUpperLimitTemperature = -9.9;
float   batteryLowerLimitTemperature = -9.9;


#if 0
// -----------------------------------------------------------------------------
modbus_t    *getContext()
{
    return ctx;
}
#endif 

#define     eps_getBatteryTemperature()               getBatteryTemperature( epsolarModbusGetContext() )
#define     eps_getBatteryRealRatedVoltage()          getBatteryRealRatedVoltage( epsolarModbusGetContext() )
#define     eps_getRatedLoadCurrent()                 getRatedLoadCurrent( epsolarModbusGetContext() )
#define     eps_getRatedChargingCurrent()             getRatedChargingCurrent( epsolarModbusGetContext() )

// -----------------------------------------------------------------------------
void    connectLocally ()
{
    char    *devicePort = "/dev/ttyXRUSB0";
    
    if (!epsolarModbusConnect( devicePort, 1 )) {
        Logger_LogFatal( "Unable to open %s to connect to the solar charge controller", devicePort );
        return;
    }

    ctx = epsolarModbusGetContext();
    
    Logger_LogInfo( "Attempting to communicate w/ controller\n" );
    batteryRatedVoltage = eps_getBatteryRealRatedVoltage();
    batteryRatedLoadCurrent =  eps_getRatedLoadCurrent();
    batteryRatedChargingCurrent = eps_getRatedChargingCurrent();

    // Logger_LogInfo( "Load voltage: %.1f, current: %.2f, power: %.2f\n", loadVoltage, loadCurrent, loadPower );
    // Logger_LogInfo( "PV voltage: %.1f, current: %.2f, power: %.2f\n", pvInputVoltage, pvInputCurrent, pvInputPower );
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
void    suspendUpdatingPanels()
{
    pthread_mutex_lock( &refreshLock );
    panelUpdatesAllowed = FALSE;
    pthread_mutex_unlock( &refreshLock );
}

// -----------------------------------------------------------------------------
void    resumeUpdatingPanels()
{
    pthread_mutex_lock( &refreshLock );
    panelUpdatesAllowed = TRUE;
    pthread_mutex_unlock( &refreshLock );
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
        getCurrentDateTime( &computerClock[ 0 ], sizeof( computerClock ) );

        
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

        deviceOverTemperature = getControllerInnerTemperatureUpperLimit( ctx );
        deviceRecoveryTemperature = getControllerInnerTemperatureUpperLimitRecover( ctx );
        batteryUpperLimitTemperature = getBatteryTemperatureWarningUpperLimit( ctx );
        batteryLowerLimitTemperature = getBatteryTemperatureWarningLowerLimit( ctx );
        
        
        if (panelUpdatesAllowed) {
            if (getActivePanel() == HOME_PANEL)
                paintHomePanelData();
            else if (getActivePanel() == BATTERY_PANEL)
                paintBatteryPanelData();
            else if (getActivePanel() == LOAD_PANEL)
                paintLoadPanelData();
            else if (getActivePanel() == SETTINGS_PANEL)
                paintSettingsPanelData();
            else if (getActivePanel() == DEVICE_PANEL)
                paintDevicePanelData();
        }
        sleep( refreshContollerDataTime );
    }

    return NULL;

}
