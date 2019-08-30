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

#include <libmqttrv.h>
#include <modbus/modbus.h>
#include <log4c.h>
#include <libepsolar.h>

extern  int doBatteryMenu (int topLeft_Y, int topLeft_X, int menuMinWidth);
extern  int doChargingMenu (int topLeft_Y, int topLeft_X, int menuMinWidth);
extern  int doChargingBoundsMenu (int topLeft_Y, int topLeft_X, int menuMinWidth);
extern  int doLoadMenu (int topLeft_Y, int topLeft_X, int menuMinWidth);



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



// ----------------------------------------------------------------------------
void    showErrorMessage (const int y, const int x, const char *errorMessage, const char *inputStr)
{
    char    buffer[ 255 ];
    
    snprintf( buffer, sizeof( buffer ), "%s   %s", errorMessage, "Press <Enter>" );
    mvprintw( y, x, buffer );
    clrtoeol();
}

// ----------------------------------------------------------------------------
void    showSuccessMessage (const int y, const int x, const char *successMessage, const char *inputStr)
{
    char    buffer[ 255 ];
    
    snprintf( buffer, sizeof( buffer ), "%s   %s", successMessage, "Proceed" );
    mvprintw( y, x, buffer );
    clrtoeol();
}

// ----------------------------------------------------------------------------
int getIntParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const int minVal, const int maxVal)
{
    char    inputStr[ 1024 ];
    
    mvprintw( LINES - 1, 0, "" );               clrtoeol();
    mvprintw( LINES - 2, 0, description );      clrtoeol();
    mvprintw( LINES - 3, 0, prompt );           clrtoeol();
    refresh();
    
    echo();
    getstr( inputStr );
    noecho();
    
    int intVal = atoi( inputStr );
    
    if (strlen( inputStr ) > 0 && (intVal >= minVal && intVal <= maxVal)) {
        showSuccessMessage( LINES - 1, 0, successMsg, inputStr );
    } else {
        //mvprintw( LINES - 1, 0, errMsg, inputStr ); clrtoeol();
        showErrorMessage( LINES - 1, 0, errMsg, inputStr );
    }

   return intVal; 

}

// ----------------------------------------------------------------------------
double  getFloatParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const double minVal, const double maxVal)
{
    char    inputStr[ 1024 ];
    
    mvprintw( LINES - 1, 0, "" );               clrtoeol();
    mvprintw( LINES - 2, 0, description );      clrtoeol();
    mvprintw( LINES - 3, 0, prompt );           clrtoeol();
    refresh();
    
    echo();
    getstr( inputStr );
    noecho();
    
    double dVal = atof( inputStr );
    
    if (dVal >= minVal && dVal <= maxVal) {
        showSuccessMessage( LINES - 1, 0, successMsg, inputStr );
    } else {
        //mvprintw( LINES - 1, 0, errMsg, inputStr ); clrtoeol();
        showErrorMessage( LINES - 1, 0, errMsg, inputStr );
    }

   return dVal; 

}

// ----------------------------------------------------------------------------
char    *getHHMMSSParam (const char *prompt, const char *description, const char *errMsg, const char *successMsg, const char *minVal, const char *maxVal)
{
    static char    inputStr[ 1024 ];
    
    
    memset( inputStr, '\0', sizeof inputStr );
 
    mvprintw( LINES - 1, 0, "" );               clrtoeol();
    mvprintw( LINES - 2, 0, description );      clrtoeol();
    mvprintw( LINES - 3, 0, prompt );           clrtoeol();
    refresh();
    
    echo();
    getstr( inputStr );
    noecho();

    
    if (strlen( inputStr ) != 6)
        return FALSE;
    
    long    lVal = atol( inputStr );
    int     hour = ((int) (inputStr[ 0 ] - '0') * 10) + (int) (inputStr[ 1 ] - '0');
    int     min  = ((int) (inputStr[ 2 ] - '0') * 10) + (int) (inputStr[ 3 ] - '0');
    int     sec  = ((int) (inputStr[ 4 ] - '0') * 10) + (int) (inputStr[ 5 ] - '0');
    
    if ((lVal >= 0L && lVal <= 235959L) &&
       (hour >= 0 && hour <= 23)  &&
       (min >= 0 && min <= 59)  &&
       (sec >= 0 && sec <= 59))
    {
        showSuccessMessage( LINES - 1, 0, successMsg, inputStr );
    } else {
        //mvprintw( LINES - 1, 0, errMsg, inputStr ); clrtoeol();
        showErrorMessage( LINES - 1, 0, errMsg, inputStr );
    }


   return inputStr;
}

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
    
    Logger_LogDebug( "  Painting a field [%s].  Window's maxx: %d, string length %5\n", initialValue, windowCols, valueLength );
    
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


static  char    *version = "0.1.a";

// -----------------------------------------------------------------------------
/* Prints typical menus that you might see in games */
int main (int argc, char *argv[])
{
    CDKSCREEN *cdkscreen;
    CDKSCROLL *dowList;
    WINDOW *pvWin, *batteryWin, *loadWin, *ctlWin;
    CDK_PARAMS params;

    CDKparseParams( argc, argv, &params, "s:" CDK_CLI_PARAMS );
    (void) initCDKScreen (NULL);
    curs_set( 0 );

    Logger_Initialize( "/tmp/epsolarcommander.log", 5 );
    Logger_LogWarning( "Version: %s\n", version  );
    fprintf( stderr,  "Version: %s\n", version  );
    sleep( 3 );
 
    connectLocally();
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
   
    
    
   /* Create a basic window. */
    int     pvY = 0, pvX = 0;
    int     pvRows = 10;
    int     pvCols = 12;
    
    pvWin = grouping( &pvWin, pvY, pvX, pvRows, pvCols, "PV" );
    floatAddTextField( pvWin, 1, 1, "Voltage", pvInputVoltage, 1, 4 );
    floatAddTextField( pvWin, 3, 1, "Current", pvInputCurrent, 2, 4 );
    floatAddTextField( pvWin, 5, 1, "Power", pvInputPower, 2, 4 );
    addTextField( pvWin, 7, 1, "Status", "Cut Out" );
   
    int     battY = pvY;
    int     battX = pvCols + 1;
    int     battRows = pvRows;
    int     battCols = 25;
    batteryWin = grouping( &batteryWin, battY, battX, battRows, battCols, "Battery" );
    floatAddTextField( batteryWin, 1, 1, "Voltage", 13.2, 1, 4 );
    floatAddTextField( batteryWin, 1, 14, "Current", 8.2, 2, 4 );
    floatAddTextField( batteryWin, 3, 1, "Min", 11.4, 1, 4 );
    floatAddTextField( batteryWin, 3, 14, "Max", 14.4, 1, 4 );
    floatAddTextField( batteryWin, 5, 1, "Temp", 72.123456789, 0, 3 );
    floatAddTextField( batteryWin, 5, 14, "SoC", 93, 0, 3 );
    addTextField( batteryWin, 7, 1, "Charging", "Equalizing" );
    addTextField( batteryWin, 7, 14, "Status", "Normal" );

    int     loadY = pvY;
    int     loadX = pvCols + battCols + 2;
    int     loadRows = pvRows;
    int     loadCols = 15;
    loadWin = grouping( &loadWin, loadY, loadX, loadRows, loadCols, "Load" );
    floatAddTextField( loadWin, 1, 1, "Voltage", loadVoltage, 1, 4 );
    floatAddTextField( loadWin, 3, 1, "Current", loadCurrent, 2, 4 );
    floatAddTextField( loadWin, 5, 1, "Power", loadPower, 2, 4 );
    addTextField( loadWin, 7, 1, "Status", "On" );
   
    int     ctlY = pvY;
    int     ctlX = pvCols + battCols + + loadCols + 3;
    int     ctlRows = pvRows - 4;
    int     ctlCols = 15;
    ctlWin = grouping( &ctlWin, ctlY, ctlX, ctlRows, ctlCols, "Controller" );
    addTextField( ctlWin, 1, 1, "Temp", "76*F" );
    addTextField( ctlWin, 3, 1, "Status", "Normal" );

    
    
    /* Start Cdk. */
   cdkscreen = initCDKScreen( pvWin );

   /* Box our window. */
   //box(subWindow, ACS_VLINE, ACS_HLINE );
   //wrefresh( subWindow );




    int menu_ret = 1, menu_ret2 = 1;
    
    
/*
    setlocale (LC_CTYPE, "");

    initscr();                   Most of the below initialisers are 
    noecho();                    not necessary for this example.    
    keypad (stdscr, TRUE);       It's just a template for a         
    meta (stdscr, TRUE);        hypothetical program that might    
    nodelay (stdscr, FALSE);     need them.                         
    notimeout (stdscr, TRUE);
    raw();
    curs_set (0);

    do {
        int topLeft_Y = 0;
        int topLeft_X = 0;
        int numMenuEntries = NUM_MENU_ITEMS;
        int menuMinWidth = 15;
        char *menuTitle = "EPSOLAR Solar Charge Controller";
        int menuStartIndex = 1;
        

        showCurrentParameters( topLeft_Y + 4, topLeft_X + 30);
        menu_ret = print_menu ( topLeft_Y, 
                                topLeft_X, 
                                numMenuEntries, 
                                menuMinWidth,
                                menuTitle, topMenu, menuStartIndex );

   
        if (menu_ret == 1) {
            doBatteryMenu( topLeft_Y, topLeft_X, menuMinWidth );
            
        } else if (menu_ret == 2) {
            doChargingMenu( topLeft_Y, topLeft_X, menuMinWidth );
            
        } else if (menu_ret == 3) {
            doChargingBoundsMenu( topLeft_Y, topLeft_X, menuMinWidth );
            
        } else if (menu_ret == 4) {
            doLoadMenu( topLeft_Y, topLeft_X, menuMinWidth );
        }

        //  erase();    
    }               
    while (menu_ret != NUM_MENU_ITEMS); 
 
    */
    sleep( 20 );
    endwin();
    return 0;
}

