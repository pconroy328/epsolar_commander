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

static  int     MaxRows, MaxCols;
static  int     dialogRows = 8;
static  int     dialogCols = 45;

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
        case    HOME_PANEL:     showHomePanel();        break;
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
void openDialog (WINDOW **win, const char *title, const char *text)
{
    //
    //  Figure out how to pop window in the center of the screen
    getmaxyx( stdscr, MaxRows, MaxCols );
    
    int centerCol = (MaxCols / 2);
    int startCol = (centerCol - (dialogCols / 2));
    int centerRow = (MaxRows / 2);
    int startRow = (centerRow - (dialogRows / 2));
    
    
    *win = newwin( dialogRows, dialogCols, startRow, startCol );
    // *win = newwin( 5, 5, 10, 10 );
    box( *win, ACS_VLINE, ACS_HLINE );  
    // wrefresh( *win );
    
   
    //
    // Windows coordinate systems are local to that window!
    wmove( *win, 0, 1 );
    wprintw( *win, title );
    wrefresh( *win );


    startRow = 2;
    startCol = 2;
    int last = strlen( text );
    
    for (int i = 0; i < last; i += 1) {
        
        if (text[ i ] == '\n') {
            startRow += 1;
            startCol = 2;
            i += 1;
        }
        
        mvwaddch( *win, startRow, startCol++, text[ i ] );
    }
    
    wrefresh( *win );
}

// -----------------------------------------------------------------------------
float   dialogGetFloat(const char *title, const char *prompt, const float minVal, const float maxVal, const float defaultVal, const int width, const int precision)
{
    WINDOW  *d;
    openDialog( &d, title, prompt );

    int startRow = dialogRows - 2;
    int startCol = 2;

    char    buf[ 255 ];
    int     len = snprintf( buf, sizeof buf, "[Min:%*.*f, Max: %*.*f] -> ", width, precision, minVal, width, precision, maxVal );
    mvwprintw( d, startRow, startCol, buf );
    wrefresh( d );
    
    char    result[ 64 ]; 
    
    float returnValue = defaultVal;
    int done = FALSE;
    do {
        echo();
        mvwgetnstr( d, startRow, (startCol + len), result, sizeof result );
        noecho();
    
        if (strlen( result ) == 0) {
            returnValue = defaultVal;
        } else {
            returnValue = (result != NULL ? atof( result ) : -1 );
        }
        
        
        if ((returnValue <= minVal) || (returnValue >= maxVal)) {
            beep();
            flash();
            for (int i = 0; i < strlen( result ); i += 1)
                mvwaddch( d, startRow, (startCol + len + i), ' ' );
        } else 
            done = TRUE;
    } while (!done);

    return returnValue;
}

// -----------------------------------------------------------------------------
int dialogGetInteger2(const char *title, const char *prompt, const int minVal, const int maxVal, const int defaultVal)
{
    float   fValue = dialogGetFloat( title, prompt, (float) minVal, (float) maxVal, (float) defaultVal, 0, 0 );
    return (int) fValue;
}


// -----------------------------------------------------------------------------
int dialogGetInteger(const char *title, const char *prompt, const int minVal, const int maxVal, const int defaultVal)
{
    WINDOW  *d;
    openDialog( &d, title, prompt );

    int startRow = dialogRows - 2;
    int startCol = 2;

    char    buf[ 255 ];
    int     len = snprintf( buf, sizeof buf, "[Min:%d, Max: %d] -> ", minVal, maxVal );
    mvwprintw( d, startRow, startCol, buf );
    wrefresh( d );
    
    char    result[ 64 ]; 
    
    int returnValue = defaultVal;
    int done = FALSE;
    do {
        echo();
        mvwgetnstr( d, startRow, (startCol + len), result, sizeof result );
        noecho();
    
        if (strlen( result ) == 0) {
            returnValue = defaultVal;
        } else {
            returnValue = (result != NULL ? atoi( result ) : -1 );
        }
        
        
        if ((returnValue <= minVal) || (returnValue >= maxVal)) {
            beep();
            flash();
            for (int i = 0; i < strlen( result ); i += 1)
                mvwaddch( d, startRow, (startCol + len + i), ' ' );
        } else 
            done = TRUE;
    } while (!done);

    return returnValue;
}







static  WINDOW      *menuWin;
static  char        *mainMenu = "(H)ome (B)attery (L)oad (D)evice (E)dit   (Q)uit";
static  char        *editMenu = "Type number then <Enter> or               e(X)it";
static  int         maxMenuMsgLen = 0;

// ---------------------------------------------------------------------------------
void    showMenu ()
{    
    //
    // WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
    //
    //  Calling newwin creates and returns a pointer to a new window with the given 
    //  number of lines and columns. The upper left-hand corner of the window is 
    //  at line begin_y, column begin_x. If either nlines or ncols is zero, 
    //  they default to LINES - begin_y and COLS - begin_x. 
    //  A new full-screen window is created by calling newwin(0,0,0,0). 
    
    //
    // WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
    //
    // Calling newwin creates and returns a pointer to a new window with the given 
    //  number of lines and columns. The upper left-hand corner of the window 
    //  is at line begin_y, column begin_x. If either nlines or ncols is zero, 
    //  they default to LINES - begin_y and COLS - begin_x. 
    //  A new full-screen window is created by calling newwin(0,0,0,0).
    int numCols = COLS;
    int numLines = 1;
    int beginY = LINES - numLines;
    int beginX = 0;
    
    menuWin = newwin( numLines, numCols, beginY, beginX );
    // box( menuWin, ACS_VLINE, ACS_HLINE );  
    //wrefresh( menuWin );
    
    beginY = 0;
    beginX = 0;    
    wmove( menuWin, beginY, beginX );
    wattron( menuWin, A_REVERSE );
    wprintw( menuWin, mainMenu );
    wclrtoeol( menuWin );
    wattroff( menuWin, A_REVERSE );
    wrefresh( menuWin );
    
    //
    // keep track of the longest message we've displayed
    if (maxMenuMsgLen <= 0)
        maxMenuMsgLen = strlen( mainMenu );
}

// -----------------------------------------------------------------------------
char    getMenuSelection ()
{
    return toupper( wgetch( menuWin ) );
}

// -----------------------------------------------------------------------------
void    menuDisplayMessage (const char *msg)
{
    wmove( menuWin, 0, 0 );
    wattron( menuWin, A_REVERSE );
    wprintw( menuWin, msg );
    wclrtoeol( menuWin );
    wattroff( menuWin, A_REVERSE );
    wrefresh( menuWin );
}

// -----------------------------------------------------------------------------
void    showEditMenu ()
{
    menuDisplayMessage( editMenu );
}

// -----------------------------------------------------------------------------
void    getEditMenuSelection (char *buffer, const size_t bufsize)
{
    echo();
    wgetnstr( menuWin, buffer, bufsize );
    noecho();
}

//------------------------------------------------------------------------------
void    editCurrentPanel (const char ch)
{
    // show edit menu
    showEditMenu();
    
    // loop get key until enter
    if (getActivePanel() == DEVICE_PANEL)
        editDevicePanel();
    
    // send command to that panel
    
    // restore main menu
    showMenu();
}





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
