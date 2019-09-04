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

#define FIRST_PANEL     0
#define BATTERY_PANEL   1
#define SETTINGS_PANEL  3

    
    
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
extern  char    *battweryStatusInnerResistance;
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

extern  int     batteryRatedVoltage;        // will be 12, 24,36, 48...
extern  float   batteryRatedLoadCurrent;
extern  float   batteryRatedChargingCurrent;

extern void     *local_readSCCValues( void * );

extern  WINDOW *grouping (WINDOW **window, const int startY, const int startX, const int rows, const int cols, const char *title);


extern  int     getActivePanel();
extern  void    setActivePanel( const int panelNum );
extern  void    showBatteryPanel();
extern  void    cleanupBatteryPanel();

#ifdef __cplusplus
}
#endif

#endif /* EPSOLAR_COMMANDER_H */

