/*
 */

#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "log4c.h"
#include "libepsolar.h"
#include "epsolar_commander.h"


static  pthread_mutex_t     refreshLock = PTHREAD_MUTEX_INITIALIZER;
static  int                 panelUpdatesAllowed = TRUE;

//
//  Global Variables - updated by the 'readSCCValues' thread
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
float   temperatureCompensationCoefficient = -9.9;
float   overVDisconnectV = -9.9;
float   overVReconnectV = -9.9;
float   equalizationVoltage = -9.9;
float   boostVoltage = -9.9;
float   floatVoltage = -9.9;
float   boostReconnectVoltage = -9.9;
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

int     HH_LON = 0, MM_LON = 0;
int     backlightTime = 0;
int     HH_T1On = 0, MM_T1On = 0, SS_T1On = 0;
int     HH_T2On = 0, MM_T2On = 0, SS_T2On = 0;
int     HH_T1Off = 0, MM_T1Off = 0, SS_T1Off = 0;
int     HH_T2Off = 0, MM_T2Off = 0, SS_T2Off = 0;
int     HH_WT1 = 0, MM_WT1 = 0, HH_WT2 = 0, MM_WT2 = 0;     

float   deviceOverTemperature = -9.9;
float   deviceRecoveryTemperature = -9.9;
float   batteryUpperLimitTemperature = -9.9;
float   batteryLowerLimitTemperature = -9.9;




// -----------------------------------------------------------------------------
void    connectLocally (const char *devicePort)
{   
    if (!epsolarModbusConnect( devicePort, 1 )) {
        Logger_LogFatal( "Unable to open %s to connect to the solar charge controller", devicePort );
        return;
    }

   
    Logger_LogInfo( "Attempting to communicate w/ controller\n" );
    batteryRatedVoltage = eps_getBatteryRealRatedVoltage();
    batteryRatedLoadCurrent =  eps_getRatedLoadCurrent();
    batteryRatedChargingCurrent = eps_getRatedChargingCurrent();
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
void *local_readSCCValues (void *x_void_ptr)
{
    connectLocally( "/dev/ttyXRUSB0" );
    
    while (TRUE) {
        Logger_LogInfo( "Reading values (locally) from SCC\n" );
        
        deviceTemp = eps_getDeviceTemperature();
        batteryTemp = eps_getBatteryTemperature();
        loadPower = eps_getLoadPower();
        loadCurrent = eps_getLoadCurrent();
        loadVoltage = eps_getLoadVoltage();
        pvInputPower = eps_getPVArrayInputPower();
        pvInputCurrent = eps_getPVArrayInputCurrent();
        pvInputVoltage = eps_getPVArrayInputVoltage();
        isNight = eps_isNightTime();
        
        batterySoC = eps_getBatteryStateOfCharge();
        batteryVoltage = eps_getBatteryVoltage();
        batteryCurrent = eps_getBatteryCurrent();
        batteryPower = (batteryVoltage * batteryCurrent);
        minBatteryVoltage = eps_getMinimumBatteryVoltageToday();
        maxBatteryVoltage = eps_getMaximumBatteryVoltageToday();
        
        chargingStatusBits = eps_getChargingEquipmentStatusBits();
        chargingStatus = eps_getChargingStatus( chargingStatusBits );
        pvInputStatus = eps_getChargingEquipmentStatusInputVoltageStatus( chargingStatusBits );
        
        batteryStatusBits = eps_getBatteryStatusBits();
        batteryStatusVoltage = eps_getBatteryStatusVoltage( batteryStatusBits );
        batteryStatusID = eps_getBatteryStatusIdentification( batteryStatusBits );
        batteryStatusInnerResistance = eps_getBatteryStatusInnerResistance( batteryStatusBits );
        batteryStatusTemperature = eps_getBatteryStatusTemperature( batteryStatusBits );

        
        dischargingStatusBits = eps_getDischargingEquipmentStatusBits();
        dischargeRunning = (eps_isDischargeStatusRunning( dischargingStatusBits ) ? "On " : "Off" );
        
        energyConsumedToday = eps_getConsumedEnergyToday();
        energyConsumedMonth = eps_getConsumedEnergyMonth();
        energyConsumedYear = eps_getConsumedEnergyYear();
        energyConsumedTotal = eps_getConsumedEnergyTotal();
        energyGeneratedToday = eps_getGeneratedEnergyToday();
        energyGeneratedMonth = eps_getGeneratedEnergyMonth();
        energyGeneratedYear = eps_getGeneratedEnergyYear();
        energyGeneratedTotal = eps_getGeneratedEnergyTotal();

        eps_getRealtimeClockStr(&controllerClock[ 0 ], sizeof( controllerClock ) );
        getCurrentDateTime( &computerClock[ 0 ], sizeof( computerClock ) );

        temperatureCompensationCoefficient = eps_getTemperatureCompensationCoefficient();
        
        batteryType = eps_getBatteryType();
        batteryChargingMode = eps_getManagementModesOfBatteryChargingAndDischarging();
        
        batteryCapacity = eps_getBatteryCapacity();
        
        
        
        overVDisconnectV = eps_getHighVoltageDisconnect();
        overVReconnectV = eps_getOverVoltageReconnect();
        equalizationVoltage = eps_getEqualizationVoltage();
        boostVoltage = eps_getBoostingVoltage();
        floatVoltage = eps_getFloatingVoltage();
        boostReconnectVoltage = eps_getBoostReconnectVoltage();
        batteryRatedVoltageCode = eps_getBatteryRatedVoltageCode();                                  

        boostDuration = eps_getBoostDuration();
        equalizeDuration = eps_getEqualizeDuration();
        chargingLimitVoltage = eps_getChargingLimitVoltage();
        dischargingLimitVoltage = eps_getDischargingLimitVoltage();
        lowVoltageDisconnectVoltage = eps_getLowVoltageDisconnectVoltage();
        lowVoltageReconnectVoltage = eps_getLowVoltageReconnectVoltage();
        underVoltageWarningVoltage = eps_getUnderVoltageWarningVoltage();
        underVolatageWarningReconnectVoltage = eps_getUnderVoltageWarningRecoverVoltage();
        
        batteryChargePercent = eps_getChargingPercentage();
        batteryDischargePercent = eps_getDischargingPercentage();
        
        
        loadControlMode = eps_getLoadControllingMode();
        nighttimeThresholdVoltage = eps_getNightTimeThresholdVoltage();
        nighttimeThresholdVoltageDelay = eps_getLightSignalStartupDelayTime();
        daytimeThresholdVoltage = eps_getDayTimeThresholdVoltage();
        daytimeThresholdVoltageDelay = eps_getLightSignalCloseDelayTime();


        eps_getLengthOfNight( &HH_LON, &MM_LON );
        backlightTime = eps_getBacklightTime();

        eps_getTurnOffTiming1( &HH_T1Off, &MM_T1Off, &SS_T1Off );
        eps_getTurnOnTiming1( &HH_T1On, &MM_T1On, &SS_T1On );
        eps_getTurnOffTiming2( &HH_T2Off, &MM_T2Off, &SS_T2Off );
        eps_getTurnOnTiming2( &HH_T2On, &MM_T2On, &SS_T2On );

        eps_getWorkingTimeLength1( &HH_WT1, &MM_WT1 );
        eps_getWorkingTimeLength2( &HH_WT2, &MM_WT2 );

        deviceOverTemperature = eps_getControllerInnerTemperatureUpperLimit();
        deviceRecoveryTemperature = eps_getControllerInnerTemperatureUpperLimitRecover();
        batteryUpperLimitTemperature = eps_getBatteryTemperatureWarningUpperLimit();
        batteryLowerLimitTemperature = eps_getBatteryTemperatureWarningLowerLimit();
        
        
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
