/* 
 * File:   main.c
 * Author: pconroy
 *
 * Created on June 12, 2019, 1:12 PM
 * sudo apt-get install libcdk5-dev avahi-client-dev
 */

#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include "ncurses-menu.h"
#include <cdk/cdk.h>
#include <unistd.h>
#include <libmqttrv.h>
//#include <modbus/modbus.h>
#include <log4c.h>
#include <libepsolar.h>
#include <pthread.h>


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
extern float   energyGeneratedMonth;
extern  float   energyGeneratedYear;
extern  float   energyGeneratedTotal;

extern  float   energyConsumedToday;
extern  float   energyConsumedMonth;
extern  float   energyConsumedYear;
extern  float   energyConsumedTotal;

extern  char    controllerClock[];


static  CDKSCREEN *cdkscreen;
static  CDKSCROLL *dowList;
static  WINDOW *pvWin, *batteryWin, *loadWin, *ctlWin;
static  CDK_PARAMS params;

/*
// -----------------------------------------------------------------------------
void    showCurrentParameters ( int y, const int x)
{
    mvprintw( y, x, "Quick Glance" );  y += 1;
    mvprintw( y, x, " 08/20/19 16:24:36   Night: True" );  y += 1;
    mvprintw( y, x, " Battery SoC: 87%    Floating" );  y += 1;
    mvprintw( y, x, " PV: 14.79V, 0.26A" );  y += 1;
    mvprintw( y, x, " Load: 13.49V, 0.17A" );  y += 1;
    mvprintw( y, x, " Battery: 13.2V 0.1A" );  y += 1;
    mvprintw( y, x, " Charging: Yes" );  y += 1;
    mvprintw( y, x, " Discharging: Light Load" );  y += 1;

    mvprintw( y, x, "Load Control" );  y += 1;
    mvprintw( y, x, " Mode: Manual (1)" );  y += 1;
    mvprintw( y, x, " Turn On 1  - 05:30:00" );  y += 1;
    mvprintw( y, x, " Turn Off 1 - 21:00:00" );  y += 1;
    mvprintw( y, x, " Turn On 2  - 08:00:00" );  y += 1;
    mvprintw( y, x, " Turn Off 2 - 22:30:00" ); 
    
    refresh();
}
*/


// -----------------------------------------------------------------------------
WINDOW *grouping (WINDOW **window, const int startY, const int startX, const int rows, const int cols, const char *title)
{
    *window = newwin( rows, cols, startY, startX );
    box( *window, ACS_VLINE, ACS_HLINE );  
    wrefresh( *window );
    
    int  width = (cols);
    int  len = strlen( title );
   
    //
    // Windows coordinate systems are local to that window!
    wmove( *window, 0, 1 );

    int last = MIN( width, len );
    int mx = MAX( width, len );
    for (int i = 0; i < last; i += 1) {
        waddch( *window, title[ i ] );
    }
    
    wrefresh( *window );
    
    return *window;
}

// -----------------------------------------------------------------------------
void    addTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const char *initialValue)
{
    //
    // Before we write it out - figure out if our string is too long for the window?
    int valueLength = strlen( initialValue );
    int windowCols = window->_maxx;
    
    assert( initialValue != NULL );
    assert( fieldName != NULL );
    
    Logger_LogDebug( "  Painting a field [%s].  Window's maxx: %d, string length %d\n", initialValue, windowCols, valueLength );
    
    wmove( window, startY, startX );
    wattron( window,  COLOR_PAIR( TF_PAIR ) );
    wattron( window, A_BOLD );			/* cut bold on */
    for (int i = 0; i < strlen( fieldName ); i += 1)
        waddch( window, fieldName[ i ] );
    wattroff( window, COLOR_PAIR( TF_PAIR ) );
    wattron( window, A_BOLD );			/* cut bold on */
    wrefresh( window );

    wattron( window, COLOR_PAIR( VALUE_PAIR ) );
    wmove( window, startY + 1, startX +1 );
    for (int i = 0; i < strlen( initialValue ); i += 1)
        waddch( window, initialValue[ i ] );
    wattroff( window, COLOR_PAIR( VALUE_PAIR ) );
    wrefresh( window );
}



// -----------------------------------------------------------------------------
void    floatAddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const float fVal, const int precision, const int width)
{
    char    buffer[ 80 ];
    
    snprintf( buffer, sizeof buffer, "%-*.*f", width, precision, fVal );
    
    addTextField( window, startY, startX, fieldName, buffer );
}

// -----------------------------------------------------------------------------
void    intAddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const int iVal, const int precision, const int width)
{
    char    buffer[ 80 ];
    
    snprintf( buffer, sizeof buffer, "%-*.*d", width, precision, iVal );
    
    addTextField( window, startY, startX, fieldName, buffer );
}


// ---------------------------------------------------------------------------------
WINDOW *paintMenu ()
{    
    WINDOW  *win;
    
    win = newwin( 1, COLS, LINES - 1, 0  );
    box( win, ACS_VLINE, ACS_HLINE );  
    
    wmove( win, 0,0 );
    waddch( win, 'P' );
    wrefresh( win );
    
    return win;
}

static  char    *version = "0.1.d";

// -----------------------------------------------------------------------------
void    firstPanel ()
{
    
    WINDOW  *menuWin = paintMenu();
    sleep( 10 );
    
   /* Create a basic window. */
    int     pvY = 0, pvX = 0;
    int     pvRows = 10;
    int     pvCols = 17;
    
    pvWin = grouping( &pvWin, pvY, pvX, pvRows, pvCols, "PV" );
    floatAddTextField( pvWin, 1, 1, "Voltage", pvInputVoltage, 1, 4 );
    floatAddTextField( pvWin, 3, 1, "Current", pvInputCurrent, 2, 4 );
    floatAddTextField( pvWin, 5, 1, "Power", pvInputPower, 2, 4 );
    addTextField( pvWin, 7, 1, "Status",  pvInputStatus );
   
    int     battY = pvY;
    int     battX = pvCols;
    int     battRows = pvRows;
    int     battCols = 25;
    batteryWin = grouping( &batteryWin, battY, battX, battRows, battCols, "Battery" );
    floatAddTextField( batteryWin, 1, 1, "Voltage", batteryVoltage, 1, 4 );
    floatAddTextField( batteryWin, 1, 14, "Current", batteryCurrent, 2, 4 );
    floatAddTextField( batteryWin, 3, 1, "Min", minBatteryVoltage, 1, 4 );
    floatAddTextField( batteryWin, 3, 14, "Max", maxBatteryVoltage, 1, 4 );
    floatAddTextField( batteryWin, 5, 1, "Temp", batteryTemp,  0, 3 );
    floatAddTextField( batteryWin, 5, 14, "SoC", batterySoC,  0, 3 );
    addTextField( batteryWin, 7, 1, "Charging", chargingStatus );
    addTextField( batteryWin, 7, 14, "Status", batteryStatusVoltage );

    int     loadY = pvY;
    int     loadX = pvCols + battCols;
    int     loadRows = pvRows;
    int     loadCols = 16;
    loadWin = grouping( &loadWin, loadY, loadX, loadRows, loadCols, "Load" );
    floatAddTextField( loadWin, 1, 1, "Voltage", loadVoltage, 1, 4 );
    floatAddTextField( loadWin, 3, 1, "Current", loadCurrent, 2, 4 );
    floatAddTextField( loadWin, 5, 1, "Power", loadPower, 2, 4 );
    addTextField( loadWin, 7, 1, "Status", dischargeRunning );
   
    int     ctlY = pvY;
    int     ctlX = pvCols + battCols + + loadCols;
    int     ctlRows = pvRows;
    int     ctlCols = 20;
    ctlWin = grouping( &ctlWin, ctlY, ctlX, ctlRows, ctlCols, "Controller" );
    floatAddTextField( ctlWin, 1, 1, "Temp", deviceTemp, 1, 5 );
    addTextField( ctlWin, 3, 1, "Status", "Normal" );    
    addTextField( ctlWin, 5, 1, "Date/Time", controllerClock );    
    
    int egY = pvY + pvRows;
    int egX = pvX;
    int egRows = 10;
    int egCols = 15;
    WINDOW  *egWin = grouping( &egWin, egY, egX, egRows, egCols, "Generated" );
    floatAddTextField( egWin, 1, 2, "Daily", energyGeneratedToday, 2, 4 );
    floatAddTextField( egWin, 3, 2, "Monthly", energyGeneratedMonth, 2, 4 );
    floatAddTextField( egWin, 5, 2, "Annual", energyGeneratedYear, 1, 4 );
    floatAddTextField( egWin, 7, 2, "Total", energyGeneratedTotal, 1, 4 );

    int ecY = egY ;  // + egRows;
    int ecX = pvX + egCols;
    int ecRows = 10;
    int ecCols = 15;
    WINDOW  *ecWin = grouping( &ecWin, ecY, ecX, ecRows, ecCols, "Consumed" );
    floatAddTextField( ecWin, 1, 2, "Daily", energyConsumedToday, 2, 4 );
    floatAddTextField( ecWin, 3, 2, "Monthly", energyConsumedMonth, 2, 4 );
    floatAddTextField( ecWin, 5, 2, "Annual", energyConsumedYear, 1, 4 );
    floatAddTextField( ecWin, 7, 2, "Total", energyConsumedTotal, 1, 4 );
}


extern void *local_readSCCValues( void * );

// -----------------------------------------------------------------------------
/* Prints typical menus that you might see in games */
int main (int argc, char *argv[])
{

    CDKparseParams( argc, argv, &params, "s:" CDK_CLI_PARAMS );
    (void) initCDKScreen (NULL);
    curs_set( 0 );

    Logger_Initialize( "/tmp/epsolarcommander.log", 5 );
    Logger_LogWarning( "Version: %s\n", version  );
    fprintf( stderr,  "Version: %s\n", version  );
 
    if (has_colors() == FALSE) {
        Logger_LogWarning( "Your terminal does not support color\n"  );
        fprintf( stderr, "Your terminal does not support color\n"  );
    }

    start_color();
    init_pair( TF_PAIR, COLOR_WHITE, COLOR_BLACK );
    init_pair( VALUE_PAIR, COLOR_WHITE, COLOR_BLUE );
    init_pair( ERROR_PAIR, COLOR_WHITE, COLOR_RED );
    init_pair( TOOHIGH_PAIR, COLOR_WHITE, COLOR_RED );
    init_pair( TOOLOW_PAIR, COLOR_WHITE, COLOR_MAGENTA );
    init_pair( OK_PAIR, COLOR_WHITE, COLOR_GREEN );
   
    
    
    /* Start Cdk. */
    cdkscreen = initCDKScreen( pvWin );


    pthread_t sccReaderThread;
    
    if (pthread_create( &sccReaderThread, NULL, local_readSCCValues, NULL ) ) {
        fprintf(stderr, "Error creating thread\n");
        
    }

    while (TRUE) {
        firstPanel(); 
        
        sleep( 1 );
    
    }
    /* wait for the second thread to finish */
    if (pthread_join(sccReaderThread, NULL)) {
    }
    
    sleep( 20 );
    endwin();
    return 0;
}

