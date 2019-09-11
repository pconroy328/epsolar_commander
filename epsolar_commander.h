/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   epsolar_commander.h
 * Author: pconroy
 *
 * Created on September 2, 2019, 9:37 AM
 */

#ifndef EPSOLAR_COMMANDER_H
#define EPSOLAR_COMMANDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ncurses.h>

#define HOME_PANEL      1
#define BATTERY_PANEL   2
#define LOAD_PANEL      3
#define DEVICE_PANEL    4
#define SETTINGS_PANEL  5

    
    
#ifndef MAX
 #define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
 #define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif 


#define TF_PAIR         1
#define VALUE_PAIR      2
#define ERROR_PAIR      3
#define TOOHIGH_PAIR    4
#define TOOLOW_PAIR     5
#define OK_PAIR         6

extern int      refreshContollerDataTime;
extern  float   deviceTemp;
extern  float   batteryTemp;
extern  float   loadPower;
extern  float   loadCurrent;
extern  float   loadVoltage;
extern  float   pvInputPower;
extern  float   pvInputCurrent;
extern  float   pvInputVoltage;
extern  int     isNight;
extern  int     batterySoC;
extern  float   batteryVoltage;
extern  float   batteryCurrent;
extern  float   batteryPower;
extern  float   minBatteryVoltage;
extern  float   maxBatteryVoltage;
extern  char    *batteryStatusVoltage;
extern  char    *batteryStatusID;
extern  char    *batteryStatusInnerResistance;
extern  char    *batteryStatusTemperature;

extern  char    *chargingStatus;
extern  char    *pvInputStatus;
extern  char    *dischargeRunning;

extern  float   energyGeneratedToday;
extern float    energyGeneratedMonth;
extern  float   energyGeneratedYear;
extern  float   energyGeneratedTotal;

extern  float   energyConsumedToday;
extern  float   energyConsumedMonth;
extern  float   energyConsumedYear;
extern  float   energyConsumedTotal;

extern  char    controllerClock[];
extern  char    computerClock[];

extern  float   batteryRatedVoltage;        // will be 12, 24,36, 48...
extern  float   batteryRatedLoadCurrent;
extern  float   batteryRatedChargingCurrent;

extern  char    *batteryType;
extern  char    *batteryChargingMode;
extern  int     batteryCapacity;
extern  float   tempertureCompensationCoefficient;
extern  float   overVDisconnectV;
extern  float   overVReconnectV;
extern  float   equalizationVoltage;
extern  float   boostVoltage;
extern  float   floatVoltage;
extern  float   boostReconnectVoltage;
extern  char    *batteryRatedVoltageCode;
extern  int     boostDuration;
extern  int     equalizeDuration;
extern  float   chargingLimitVoltage;
extern  float   dischargingLimitVoltage;
extern  float   lowVoltageDisconnectVoltage;
extern  float   lowVoltageReconnectVoltage;
extern  float   underVoltageWarningVoltage;
extern  float   underVolatageWarningReconnectVoltage;
extern  float   batteryChargePercent;
extern  float   batteryDischargePercent;

extern  int     loadControlMode;
extern  float   nighttimeThresholdVoltage;
extern  int     nighttimeThresholdVoltageDelay;
extern  float   daytimeThresholdVoltage;
extern  int     daytimeThresholdVoltageDelay;
extern  char    *timerOneOn, *timerOneOff;
extern  char    *timerTwoOn, *timerTwoOff;
extern  char    *workOneTime, *workTwoTime;
extern  char    *lengthOfNight;

extern  int     HH_LON, MM_LON;
extern  int     backlightTime;
extern  int     HH_T1On, MM_T1On, SS_T1On;
extern  int     HH_T2On, MM_T2On, SS_T2On;
extern  int     HH_T1Off, MM_T1Off, SS_T1Off;
extern  int     HH_T2Off, MM_T2Off, SS_T2Off;
extern  int     HH_WT1, MM_WT1, HH_WT2, MM_WT2;     

extern  float   deviceOverTemperature;
extern  float   deviceRecoveryTemperature;
extern  float   batteryUpperLimitTemperature;
extern  float   batteryLowerLimitTemperature;

extern void         *local_readSCCValues( void * );
extern  void        suspendUpdatingPanels( void );
extern  void        resumeUpdatingPanels( void );
extern  modbus_t    *getContext( void );


extern  WINDOW *grouping (WINDOW **window, const int startY, const int startX, const int rows, const int cols, const char *title);
extern  void    addTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const char *initialValue);
extern  void    floatAddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const float fVal, const int precision, const int width);
extern  void    intAddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const int iVal, const int precision, const int width);
extern  void    HaddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const char *initialValue);
extern  void    HfloatAddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const float fVal, const int precision, const int width);
extern  void    HintAddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const int iVal, const int precision, const int width);
extern  float   dialogGetFloat(const char *title, const char *prompt, const float minVal, const float maxVal, const float defaultVal, const int width, const int precision);
extern  int     dialogGetInteger(const char *title, const char *prompt, const int minVal, const int maxVal, const int defaultVal);



extern  void    showMenu ();
extern  char    getMenuSelection();
extern  void    getEditMenuSelection( char *buffer, const size_t bufsize );
extern  void    menuDisplayMessage( const char * );

extern  char    *getCurrentDateTime( char *, const size_t len );

extern  int     getActivePanel();
extern  void    setActivePanel( const int panelNum );
extern  void    switchPanel (const int newActivePanelID);
extern  void    editCurrentPanel( const char ch );


extern  void    showBatteryPanel();
extern  void    paintBatteryPanelData();
extern  void    clearBatteryPanel();

extern  void    showHomePanel();
extern  void    clearHomePanel();
extern  void    paintHomePanelData();

extern  void    showLoadPanel();
extern  void    paintLoadPanelData();
extern  void    clearLoadPanel();

extern  void    showDevicePanel();
extern  void    clearDevicePanel();
extern  void    paintDevicePanelData();
extern  void    editDevicePanel();

extern  void    showSettingsPanel();
extern  void    clearSettingsPanel();
extern  void    paintSettingsPanelData();


#ifdef __cplusplus
}
#endif

#endif /* EPSOLAR_COMMANDER_H */

