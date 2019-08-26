/*
{
	"topic":	"LS1024B/1/DATA",
	"version":	"2.0",                          (*)
      	"dateTime":	"2019-08-20T16:24:32-0600",     (*)
	"controllerDateTime":	"08/20/19 16:24:36",    (*)
	"isNightTime":	false,                          (*)
	"batterySOC":	87,                             (*)
	"pvArrayVoltage":	14.79,                  (*)
	"pvArrayCurrent":	0.26,
	"loadVoltage":	13.49,
	"loadCurrent":	0.17,
	"temperatures":	{
		"unit":	"Fahrenheit",
		"battery":	77,
		"case":	101.6,
		"remoteSensor":	32
	},
	"batteryStatus":	{
		"voltage":	"Normal",
		"temperature":	"Normal",
		"innerResistance":	"Normal",
		"identification":	"Correct"
	},
	"chargingStatus":	{
		"status":	"Floating",
		"isNormal":	false,
		"isRunning":	true,
		"inputVoltage":	"Normal",
		"MOSFETShort":	false,
		"someMOSFETShort":	false,
		"antiReverseMOSFETShort":	false,
		"inputIsOverCurrent":	false,
		"inputIsOverPressure":	false,
		"loadIsOverCurrent":	false,
		"loadIsShort":	false,
		"loadMOSFETIsShort":	false,
		"pvInputIsShort":	false
	},
	"dischargingStatus":	{
		"isNormal":	true,
		"isRunning":	true,
		"inputVoltageStatus":	"Normal",
		"outputPower":	"Light Load",
		"shortCircuit":	false,
		"unableToDischarge":	false,
		"unableToStopDischarging":	false,
		"outputVoltageAbnormal":	false,
		"inputOverpressure":	false,
		"highVoltageSideShort":	false,
		"boostOverpressure":	false,
		"outputOverpressure":	false
	},
	"settings":	{
		"batteryType":	"Sealed",
		"batteryCapacity":	200,
		"tempCompensationCoeff":	3,
		"highVoltageDisconnect":	16,
		"chargingLimitVoltage":	15,
		"overVoltageReconnect":	15,
		"equalizationVoltage":	14.6,
		"boostVoltage":	14.4,
		"floatVoltage":	13.8,
		"boostReconnectVoltage":	13.2,
		"lowVoltageReconnect":	12.6,
		"underVoltageRecover":	12.2,
		"underVoltageWarning":	12,
		"lowVoltageDisconnect":	11.1,
		"dischargingLimitVoltage":	10.6,
		"batteryTempWarningUpperLimit":	149,
		"batteryTempWarningLowerLimit":	1139.6,
		"controllerInnerTempUpperLimit":	185,
		"controllerInnerTempUpperLimitRecover":	167,
		"powerComponentTempUpperLimit":	0,
		"powerComponentTempUpperLimitRecover":	0,
		"daytimeThresholdVoltage":	5,
		"lightSignalStartupTime":	10,
		"lighttimeThresholdVoltage":	6,
		"lightSignalCloseDelayTime":	10,
		"localControllingModes":	0,
		"workingTimeLength1":	"01:00",
		"workingTimeLength2":	"01:00",
		"turnOnTiming1":	"19:00:00",
		"turnOffTiming1":	"06:00:00",
		"turnOnTiming2":	"19:00:00",
		"turnOffTiming2":	"06:00:00",
		"lengthOfNight":	"12:00",
		"batteryRatedVoltageCode":	"Auto",
		"loadTimingControlSelection":	"1 Timer",
		"defaultLoadOnOffManualMode":	"Off",
		"equalizeDuration":	120,
		"boostDuration":	120,
		"dischargingPercentage":	0,
		"chargingPercentage":	1,
		"batteryManagementMode":	0
	},
	"statistics":	{
		"maximumInputVoltageToday":	16.69,
		"minimumInputVoltageToday":	14.71,
		"maximumBatteryVoltageToday":	14.53,
		"minimumBatteryVoltageToday":	13.25,
		"consumedEnergyToday":	0.04,
		"consumedEnergyMonth":	0.15,
		"consumedEnergyYear":	0.15,
		"totalConsumedEnergy":	0.15,
		"generatedEnergyToday":	0.08,
		"generatedEnergyMonth":	0.29,
		"generatedEnergyYear":	0.29,
		"totalGeneratedEnergy":	0.29,
		"batteryVoltage":	13.49,
		"batteryCurrent":	0.1
	}
}

 * 

 * 
 * /*
{
 Quick Glance
 * 08/20/19 16:24:36   Night: True
 * Battery SoC: 87%    Floating 
 * PV: 14.79V, 0.26A
 * Load: 13.49V, 0.17A
 * Battery: 13.2V 0.1A
 * Charging: Yes
 * Discharging: Light Load

 Load Control
 * Mode: Manual (1)
 * Turn On 1  - 05:30:00
 * Turn Off 1 - 21:00:00
 * Turn On 2  - 08:00:00
 * Turn Off 2 - 22:30:00
 * 
 *  * 
	"temperatures":	{
		"unit":	"Fahrenheit",
		"battery":	77,
		"case":	101.6,
		"remoteSensor":	32
	},
	"batteryStatus":	{
		"voltage":	"Normal",
		"temperature":	"Normal",
		"innerResistance":	"Normal",
		"identification":	"Correct"
	},
	"chargingStatus":	{
		"status":	"Floating",
		"isNormal":	false,
		"isRunning":	true,
		"inputVoltage":	"Normal",
		"MOSFETShort":	false,
		"someMOSFETShort":	false,
		"antiReverseMOSFETShort":	false,
		"inputIsOverCurrent":	false,
		"inputIsOverPressure":	false,
		"loadIsOverCurrent":	false,
		"loadIsShort":	false,
		"loadMOSFETIsShort":	false,
		"pvInputIsShort":	false
	},
	"dischargingStatus":	{
		"isNormal":	true,
		"isRunning":	true,
		"inputVoltageStatus":	"Normal",
		"outputPower":	"Light Load",
		"shortCircuit":	false,
		"unableToDischarge":	false,
		"unableToStopDischarging":	false,
		"outputVoltageAbnormal":	false,
		"inputOverpressure":	false,
		"highVoltageSideShort":	false,
		"boostOverpressure":	false,
		"outputOverpressure":	false
	},
	"settings":	{
		"batteryType":	"Sealed",
		"batteryCapacity":	200,
		"tempCompensationCoeff":	3,
		"highVoltageDisconnect":	16,
		"chargingLimitVoltage":	15,
		"overVoltageReconnect":	15,
		"equalizationVoltage":	14.6,
		"boostVoltage":	14.4,
		"floatVoltage":	13.8,
		"boostReconnectVoltage":	13.2,
		"lowVoltageReconnect":	12.6,
		"underVoltageRecover":	12.2,
		"underVoltageWarning":	12,
		"lowVoltageDisconnect":	11.1,
		"dischargingLimitVoltage":	10.6,
		"batteryTempWarningUpperLimit":	149,
		"batteryTempWarningLowerLimit":	1139.6,
		"controllerInnerTempUpperLimit":	185,
		"controllerInnerTempUpperLimitRecover":	167,
		"powerComponentTempUpperLimit":	0,
		"powerComponentTempUpperLimitRecover":	0,
		"daytimeThresholdVoltage":	5,
		"lightSignalStartupTime":	10,
		"lighttimeThresholdVoltage":	6,
		"lightSignalCloseDelayTime":	10,
		"localControllingModes":	0,
		"workingTimeLength1":	"01:00",
		"workingTimeLength2":	"01:00",
		"turnOnTiming1":	"19:00:00",
		"turnOffTiming1":	"06:00:00",
		"turnOnTiming2":	"19:00:00",
		"turnOffTiming2":	"06:00:00",
		"lengthOfNight":	"12:00",
		"batteryRatedVoltageCode":	"Auto",
		"loadTimingControlSelection":	"1 Timer",
		"defaultLoadOnOffManualMode":	"Off",
		"equalizeDuration":	120,
		"boostDuration":	120,
		"dischargingPercentage":	0,
		"chargingPercentage":	1,
		"batteryManagementMode":	0
	},
	"statistics":	{
		"maximumInputVoltageToday":	16.69,
		"minimumInputVoltageToday":	14.71,
		"maximumBatteryVoltageToday":	14.53,
		"minimumBatteryVoltageToday":	13.25,
		"consumedEnergyToday":	0.04,
		"consumedEnergyMonth":	0.15,
		"consumedEnergyYear":	0.15,
		"totalConsumedEnergy":	0.15,
		"generatedEnergyToday":	0.08,
		"generatedEnergyMonth":	0.29,
		"generatedEnergyYear":	0.29,
		"totalGeneratedEnergy":	0.29,
		"batteryVoltage":	13.49,
		"batteryCurrent":	0.1
	}
}

 * 
 *  *  */

