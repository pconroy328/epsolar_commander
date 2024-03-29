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




int     MaxRows;
int     MaxCols;

float   VoltageUpperBound = 17.0;
float   VoltageLowerBound = 9.0;


// -----------------------------------------------------------------------------
void    setVoltageUpperLowerBounds()
{
    //
    // for the Tracer series, most/all of the voltage settings should be between
    //  9V and 17V for a 12V system, double that for a 24V
    VoltageUpperBound = (batteryRatedVoltage / 12) * 17.0;
    VoltageLowerBound = (batteryRatedVoltage / 12) * 9.0;
}



static  char    *version = "0.9 [timer helper]";



// -----------------------------------------------------------------------------
/* Prints typical menus that you might see in games */
int main (int argc, char *argv[])
{

    Logger_Initialize( "/tmp/epsolarcommander.log", 5 );
    Logger_LogWarning( "Version: %s\n", version  );
    fprintf( stderr,  "Version: %s\n", version  );
    Logger_LogWarning( "libepsolar Version: %s\n", epsolarGetVersion() );
    fprintf( stderr,  "libepsolar Version: %s\n", epsolarGetVersion() );
    
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
   
    controllerClock[ 0 ] = '\0';
    computerClock[ 0 ] = '\0';
    
    //
    // Fire up the SCC Data Reader Thread
    pthread_t sccReaderThread;
    if (pthread_create( &sccReaderThread, NULL, local_readSCCValues, NULL ) ) {
        fprintf(stderr, "Error creating thread\n");
    }

    //
    // Start off with the Home Panel and the Menu
    showHomePanel();
    showMenu();
    menuDisplayMessage( "Waiting for data from solar charge controller..." );
    
    int loopCount = 0;
    while (deviceTemp < 0 && loopCount < 10) {
        sleep( 1 );
        loopCount += 1;
    }
    
    showMenu();
    
    while (TRUE) {       
        
        char ch = getMenuSelection();

        if (ch == 'H') {
            switchPanel( HOME_PANEL );
            showMenu();
        } else if (ch == 'B') {
            switchPanel( BATTERY_PANEL );
            showMenu();
        } else if (ch == 'L') {
            switchPanel( LOAD_PANEL );
            showMenu();
        } else if (ch == 'D') {
            switchPanel( DEVICE_PANEL );
            showMenu();
        } else if (ch == 'S') {
            switchPanel( SETTINGS_PANEL );
            showMenu();
        } else if (ch == 'Q') {
            break;
        } else if (ch == 'E') {
            editCurrentPanel( ch );
        }
    }
    endwin();
    
    fprintf( stderr, "waiting for second thread to end...\n" );
    pthread_cancel( sccReaderThread );
    
    /* wait for the second thread to finish */
    if (pthread_join( sccReaderThread, NULL )) {
    }
    
    epsolarModbusDisconnect();
    fprintf( stderr, "Goodbye!\n" );
    return 0;
}

