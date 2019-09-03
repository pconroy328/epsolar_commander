/* 
 * File:   main.c
 * Author: pconroy
 *
 * Created on June 12, 2019, 1:12 PM
 * sudo apt-get install libcdk5-dev avahi-client-dev
 */

#include <stdlib.h>
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <libmqttrv.h>
#include <log4c.h>
#include <libepsolar.h>
#include <pthread.h>

#include "epsolar_commander.h"



static  WINDOW *pvWin, *batteryWin, *loadWin, *ctlWin, *egWin, *ecWin;

int             MaxRows;
int             MaxCols;

float       VoltageUpperBound = 17.0;
float       VoltageLowerBound = 9.0;


// -----------------------------------------------------------------------------
void    setVoltageUpperLowerBounds()
{
    //
    // for the Tracer series, most/all of the voltage settings should be between
    //  9V and 17V for a 12V system, double that for a 24V
    VoltageUpperBound = (batteryRatedVoltage / 12) * 17.0;
    VoltageLowerBound = (batteryRatedVoltage / 12) * 9.0;
}

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
    //
    // WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
    //
    //  Calling newwin creates and returns a pointer to a new window with the given 
    //  number of lines and columns. The upper left-hand corner of the window is 
    //  at line begin_y, column begin_x. If either nlines or ncols is zero, 
    //  they default to LINES - begin_y and COLS - begin_x. 
    //  A new full-screen window is created by calling newwin(0,0,0,0). 
    
    int numLines = 1;
    int numCols = COLS;
    int beginY = LINES - 1;
    int beginX = 0;
    
    win = newwin( numLines, numCols, beginY, beginX );    
    
    wmove( win, 0,0 );
    wattron( win, A_REVERSE );
    wprintw( win, "(H)ome   (B)attery    (L)oad    (D)evice   (S)ettings   (Q)uit");
    wattroff( win, A_REVERSE );
    wrefresh( win );
    
    return win;
}

static  char    *version = "0.1.d";

// -----------------------------------------------------------------------------
void    paintPVGroupData()
{
    floatAddTextField( pvWin, 1, 1, "Voltage", pvInputVoltage, 1, 4 );
    floatAddTextField( pvWin, 3, 1, "Current", pvInputCurrent, 2, 4 );
    floatAddTextField( pvWin, 5, 1, "Power", pvInputPower, 2, 4 );
    addTextField( pvWin, 7, 1, "Status",  pvInputStatus );
}

// -----------------------------------------------------------------------------
void    paintBatteryGroupData()
{
    floatAddTextField( batteryWin, 1, 1, "Voltage", batteryVoltage, 1, 4 );
    floatAddTextField( batteryWin, 1, 14, "Current", batteryCurrent, 2, 4 );
    floatAddTextField( batteryWin, 3, 1, "Min", minBatteryVoltage, 1, 4 );
    floatAddTextField( batteryWin, 3, 14, "Max", maxBatteryVoltage, 1, 4 );
    floatAddTextField( batteryWin, 5, 1, "Temp", batteryTemp,  0, 3 );
    floatAddTextField( batteryWin, 5, 14, "SoC", batterySoC,  0, 3 );
    addTextField( batteryWin, 7, 1, "Charging", chargingStatus );
    addTextField( batteryWin, 7, 14, "Status", batteryStatusVoltage );    
}
// -----------------------------------------------------------------------------
void    paintLoadGroupData()
{
    floatAddTextField( loadWin, 1, 1, "Voltage", loadVoltage, 1, 4 );
    floatAddTextField( loadWin, 3, 1, "Current", loadCurrent, 2, 4 );
    floatAddTextField( loadWin, 5, 1, "Power", loadPower, 2, 4 );
    addTextField( loadWin, 7, 1, "Status", dischargeRunning );
}

// -----------------------------------------------------------------------------
void    paintDeviceGroupData()
{
    floatAddTextField( ctlWin, 1, 1, "Temp", deviceTemp, 1, 5 );
    addTextField( ctlWin, 3, 1, "Status", "Normal" );    
    addTextField( ctlWin, 5, 1, "Date/Time", controllerClock );
    addTextField( ctlWin, 7, 1, "Night Time", (isNight? "Yes" : "No" ) );
}

// -----------------------------------------------------------------------------
void    paintGeneratedGroupData()
{
    floatAddTextField( egWin, 1, 2, "Daily", energyGeneratedToday, 2, 4 );
    floatAddTextField( egWin, 3, 2, "Monthly", energyGeneratedMonth, 2, 4 );
    floatAddTextField( egWin, 5, 2, "Annual", energyGeneratedYear, 1, 4 );
    floatAddTextField( egWin, 7, 2, "Total", energyGeneratedTotal, 1, 4 );    
}
// -----------------------------------------------------------------------------
void    paintConsumedGroupData()
{
    floatAddTextField( ecWin, 1, 2, "Daily", energyConsumedToday, 2, 4 );
    floatAddTextField( ecWin, 3, 2, "Monthly", energyConsumedMonth, 2, 4 );
    floatAddTextField( ecWin, 5, 2, "Annual", energyConsumedYear, 1, 4 );
    floatAddTextField( ecWin, 7, 2, "Total", energyConsumedTotal, 1, 4 );    
}

// -----------------------------------------------------------------------------
void    paintFirstPanelData()
{
    paintPVGroupData();    
    paintBatteryGroupData();
    paintLoadGroupData();
    paintDeviceGroupData();
    paintGeneratedGroupData();
    paintConsumedGroupData();
}


// -----------------------------------------------------------------------------
void    firstPanel ()
{
    int     pvY = 0, pvX = 0;
    int     pvRows = 10;
    int     pvCols = 17;
    
    pvWin = grouping( &pvWin, pvY, pvX, pvRows, pvCols, "PV" );
    paintPVGroupData();
   
    int     battY = pvY;
    int     battX = pvCols;
    int     battRows = pvRows;
    int     battCols = 25;
    batteryWin = grouping( &batteryWin, battY, battX, battRows, battCols, "Battery" );
    paintBatteryGroupData();

    int     loadY = pvY;
    int     loadX = pvCols + battCols;
    int     loadRows = pvRows;
    int     loadCols = 16;
    loadWin = grouping( &loadWin, loadY, loadX, loadRows, loadCols, "Load" );
    paintLoadGroupData();
   
    int     ctlY = pvY;
    int     ctlX = pvCols + battCols + + loadCols;
    int     ctlRows = pvRows;
    int     ctlCols = 20;
    ctlWin = grouping( &ctlWin, ctlY, ctlX, ctlRows, ctlCols, "Controller" );
    paintDeviceGroupData();
    
    int egY = pvY + pvRows;
    int egX = pvX;
    int egRows = 10;
    int egCols = 15;
    egWin = grouping( &egWin, egY, egX, egRows, egCols, "Generated" );
    paintGeneratedGroupData();
    
    int ecY = egY ;  // + egRows;
    int ecX = pvX + egCols;
    int ecRows = 10;
    int ecCols = 15;
    ecWin = grouping( &ecWin, ecY, ecX, ecRows, ecCols, "Consumed" );
    paintConsumedGroupData();
}

// -----------------------------------------------------------------------------
void    cleanupHomePanel ()
{
    werase( pvWin ); delwin( pvWin );
    werase( batteryWin ); delwin( batteryWin );
    werase( loadWin ); delwin( loadWin );
    werase( ctlWin ); delwin( ctlWin );
    werase( egWin ); delwin( egWin );
    werase( ecWin ); delwin( ecWin );
}





// -----------------------------------------------------------------------------
/* Prints typical menus that you might see in games */
int main (int argc, char *argv[])
{

    //CDKparseParams( argc, argv, &params, "s:" CDK_CLI_PARAMS );
    //(void) initCDKScreen (NULL);
    //curs_set( 0 );

    Logger_Initialize( "/tmp/epsolarcommander.log", 5 );
    Logger_LogWarning( "Version: %s\n", version  );
    fprintf( stderr,  "Version: %s\n", version  );
    
    initscr();
    cbreak();
    noecho();
    keypad( stdscr, TRUE );
    curs_set(0);
    getmaxyx( stdscr, MaxRows, MaxCols );      /* find the boundaries of the screen */
  
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
   
    pthread_t sccReaderThread;
   
    if (pthread_create( &sccReaderThread, NULL, local_readSCCValues, NULL ) ) {
        fprintf(stderr, "Error creating thread\n");
        
    }

    setActivePanel( FIRST_PANEL );
    WINDOW  *menuWin;
    while (TRUE) {
        if (getActivePanel() == FIRST_PANEL)
            firstPanel();
        
        menuWin = paintMenu();
        
        char    ch = toupper( wgetch( menuWin ) );
        //     wprintw( win, "(R)efresh   (B)attery    (L)oad    (D)evice   (S)ettings   (Q)uit");

        if (ch == 'H') {
            wprintw( menuWin, "  Refresh" );
            cleanupBatteryPanel();
            setActivePanel( FIRST_PANEL );
            firstPanel();
        } else if (ch == 'B') {
            wprintw( menuWin, "  Battery" );
            cleanupHomePanel();
            setActivePanel( BATTERY_PANEL );
            showBatteryPanel();
        } else if (ch == 'L') {
            wprintw( menuWin, "  Load" );
        } else if (ch == 'D') {
            wprintw( menuWin, "  Device" );
        } else if (ch == 'S') {
            wprintw( menuWin, "  Settings" );
        } else if (ch == 'Q') {
            wprintw( menuWin, "  Quit" );
            break;
        }
    }
    endwin();
    
    fprintf( stderr, "waiting for second thread to end...\n" );
    pthread_cancel( sccReaderThread );
    
    /* wait for the second thread to finish */
    if (pthread_join( sccReaderThread, NULL )) {
    }
    
    fprintf( stderr, "Goodbye!\n" );
    return 0;
}

