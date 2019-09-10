/*
 */

#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <libmqttrv.h>
#include <log4c.h>
#include <libepsolar.h>
#include <time.h>

#include "epsolar_commander.h"

static  int     oldPanelID = HOME_PANEL;
static  int     activePanelID = HOME_PANEL;

// -----------------------------------------------------------------------------
void    switchPanel (const int newActivePanelID)
{
    //if (newActivePanelID > 0 && newActivePanelID == oldPanelID) {
    //    Logger_LogDebug( "New panel same as old one - nothing to do..." );
    //    return;
    // }
    
    activePanelID = newActivePanelID;
    
    switch (oldPanelID) {
        case    HOME_PANEL:     clearHomePanel();       break;
        case    BATTERY_PANEL:  clearBatteryPanel();    break;
        case    LOAD_PANEL:     clearLoadPanel();       break;
        case    DEVICE_PANEL:   clearDevicePanel();     break;
        case    SETTINGS_PANEL: clearSettingsPanel();   break;
    }
    
    switch (activePanelID) {
        case    HOME_PANEL:     showHomePanel();    break;
        case    BATTERY_PANEL:  showBatteryPanel();     break;
        case    LOAD_PANEL:     showLoadPanel();        break;
        case    DEVICE_PANEL:   showDevicePanel();      break;
        case    SETTINGS_PANEL: showSettingsPanel();    break;
    }
    
    oldPanelID = activePanelID;
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
    
    // Logger_LogDebug( "  Painting a field [%s].  Window's maxx: %d, string length %d\n", initialValue, windowCols, valueLength );
    
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

// -----------------------------------------------------------------------------
void    HaddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const char *initialValue)
{
    //
    // Before we write it out - figure out if our string is too long for the window?
    int valueLength = strlen( initialValue );
    int windowCols = window->_maxx;
    
    assert( initialValue != NULL );
    assert( fieldName != NULL );
    
    // Logger_LogDebug( "  Painting a field [%s].  Window's maxx: %d, string length %d\n", initialValue, windowCols, valueLength );
    
    wmove( window, startY, startX );
    wattron( window,  COLOR_PAIR( TF_PAIR ) );
    wattron( window, A_BOLD );			/* cut bold on */
    for (int i = 0; i < strlen( fieldName ); i += 1)
        waddch( window, fieldName[ i ] );
    wattroff( window, COLOR_PAIR( TF_PAIR ) );
    wattron( window, A_BOLD );			/* cut bold on */
    wrefresh( window );

    waddch( window, ':' );
    waddch( window, ' ' );
    
    wattron( window, COLOR_PAIR( VALUE_PAIR ) );
    for (int i = 0; i < strlen( initialValue ); i += 1)
        waddch( window, initialValue[ i ] );
    wattroff( window, COLOR_PAIR( VALUE_PAIR ) );
    wrefresh( window );
}



// -----------------------------------------------------------------------------
void    HfloatAddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const float fVal, const int precision, const int width)
{
    char    buffer[ 80 ];
    
    snprintf( buffer, sizeof buffer, "%-*.*f", width, precision, fVal );
    HaddTextField( window, startY, startX, fieldName, buffer );
}

// -----------------------------------------------------------------------------
void    HintAddTextField (WINDOW *window, const int startY, const int startX, const char *fieldName, const int iVal, const int precision, const int width)
{
    char    buffer[ 80 ];
    
    snprintf( buffer, sizeof buffer, "%-*.*d", width, precision, iVal );
    HaddTextField( window, startY, startX, fieldName, buffer );
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
char    *getCurrentDateTime (char *buffer, const size_t buffLen)
{
    //
    // Something quick and dirty... Fix this later - thread safe
    time_t  current_time;
    struct  tm  *tmPtr;
 
    memset( buffer, '\0', buffLen );
    
    /* Obtain current time as seconds elapsed since the Epoch. */
    current_time = time( NULL );
    if (current_time > 0) {
        /* Convert to local time format. */
        tmPtr = localtime( &current_time );
 
        if (tmPtr != NULL) {
            strftime( buffer,
                    buffLen,
                    "%m/%d/%y %H:%M:%S",
                    tmPtr );
            
        }
    }
    
    return buffer;
}
