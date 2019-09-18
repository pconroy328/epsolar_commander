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
#include <pthread.h>
#include "epsolar_commander.h"

extern  int     MaxRows;
extern  int     MaxCols;
static  WINDOW  *panel, *manualPanel, *duskDawnPanel, *duskTimerPanel, *timerPanel;


// -----------------------------------------------------------------------------
static
void    paintLoadData ()
{
    //
    char    buf1[ 12 ], buf2[ 12 ];
    char    buf3[ 12 ], buf4[ 12 ];
    char    buf5[ 12 ], buf6[ 12 ];
    char    buf7[ 10 ];
    
    int beginRow = 1;
    int beginCol = 3;
    HaddTextField( manualPanel, beginRow, beginCol, "1. Manual ", (loadControlMode == 0x00 ? "Enabled " : "Disabled") );
    HaddTextField( manualPanel, beginRow, beginCol + 20, "Status ", (dischargeRunning ? "On " : "Off") );
    HaddTextField( manualPanel, beginRow, beginCol + 35, "<F1> Turn On", "" );
    HaddTextField( manualPanel, beginRow, beginCol + 45, "<F2> Turn Off", "" );
    wrefresh( manualPanel );

    beginRow = 1;
    snprintf( buf1, sizeof buf1, "%02d:%02d:%02d", HH_T1On, MM_T1On, SS_T1On );
    snprintf( buf2, sizeof buf2, "%02d:%02d:%02d", HH_T2On, MM_T2On, SS_T2On );
    snprintf( buf3, sizeof buf3, "%02d:%02d:%02d", HH_T1Off, MM_T1Off, SS_T1Off );
    snprintf( buf4, sizeof buf4, "%02d:%02d:%02d", HH_T2Off, MM_T2Off, SS_T2Off );
    
    HaddTextField( duskDawnPanel, beginRow++, beginCol,             "2. Dusk On/Dawn Off", (loadControlMode == 0x01 ? "Enabled " : "Disabled") );
    HaddTextField( duskDawnPanel, beginRow, beginCol,               "3. Timer One - On  ", buf1 );
    HaddTextField( duskDawnPanel, beginRow++, beginCol + 30,        "4. Timer One - Off ", buf3 );
    HaddTextField( duskDawnPanel, beginRow, beginCol,               "5. Timer Two - On  ", buf2 ); 
    HaddTextField( duskDawnPanel, beginRow++, beginCol + 30,        "6. Timer Two - Off ", buf4 ); 
    HfloatAddTextField( duskDawnPanel, beginRow, beginCol,          "7. Dusk Threshold <= V", nighttimeThresholdVoltage, 1, 4 );
    HfloatAddTextField( duskDawnPanel, beginRow++, beginCol + 30,   "8. Dawn Threshold >= V", daytimeThresholdVoltage, 1, 4 );
    wrefresh( duskDawnPanel );
            
    
    beginRow = 1;
    HaddTextField( duskTimerPanel, beginRow++, beginCol, "9. Dusk On and Timer", (loadControlMode == 0x02 ? "Enabled " : "Disabled") );
    snprintf( buf5, sizeof buf5, "%02d:%02d", HH_WT1, MM_WT1 );
    snprintf( buf6, sizeof buf6, "%02d:%02d", HH_WT2, MM_WT2 );
    HaddTextField( duskTimerPanel, beginRow, beginCol, "10. Work Timer One", buf5 );
    HaddTextField( duskTimerPanel, beginRow++, beginCol + 30, "11. Work Timer Two", buf6 );

    snprintf( buf7, sizeof buf7, "%02d:%02d", HH_LON, MM_LON );
    HaddTextField( duskTimerPanel, beginRow++, beginCol, " 12. Length of Night", buf7 );
    wrefresh( duskTimerPanel );
    
    beginRow = 1;
    HaddTextField( timerPanel, beginRow++, beginCol, "13.Timer Mode", (loadControlMode == 0x03 ? "Enabled " : "Disabled") );
    HaddTextField( timerPanel, beginRow, beginCol,   "14. Timer One - On  ", buf1 );
    HaddTextField( timerPanel, beginRow++, beginCol + 30, "15. Timer One - Off ", buf3 );
            
    wrefresh( panel );
}

// -----------------------------------------------------------------------------
void    showLoadPanel()
{
    setActivePanel( LOAD_PANEL );

    int     startY = 0;
    int     startX = 0;
    int     nRows = MaxRows - 1;
    int     nCols = MaxCols;

    panel = grouping( &panel, startY, startX, nRows, nCols, "Load Control" );
    
    startY = 1;
    startX = 1;
    nRows = 3;
    nCols = MaxCols - startY - 1;
    manualPanel = grouping( &manualPanel, startY, startX, nRows, nCols, "Manual Control" );

    startY += nRows;
    nRows = 6;
    //nCols = MaxCols - startY;
    duskDawnPanel = grouping( &duskDawnPanel, startY, startX, nRows, nCols, "Dusk to Dawn Control" );
    
    startY += nRows;
    nRows = 5;
    //nCols = MaxCols - startY;
    duskTimerPanel = grouping( &duskTimerPanel, startY, startX, nRows, nCols, "Dusk Plus Timer Control" );
    
    startY += nRows;
    nRows = 4;
    //nCols = MaxCols - startY;
    timerPanel = grouping( &timerPanel, startY, startX, nRows, nCols, "Timer Control" );
    
    paintLoadData();
}

// -----------------------------------------------------------------------------
void    paintLoadPanelData()
{
    paintLoadData();
}

// -----------------------------------------------------------------------------
void    clearLoadPanel()
{
    werase( panel );
    delwin( panel );
    werase( stdscr );
    refresh();
}

#define     MIN_SELECTION       1
#define     MAX_SELECTION       15
// -----------------------------------------------------------------------------
void    editLoadPanel ()
{
    // modal... oh well...
    int done = FALSE;
    
    char    buffer[ 10 ];
    int     selection = 0;
   
    while (!done) {
        memset( buffer, '\0', sizeof buffer );
        getEditMenuSelection( buffer, sizeof buffer );
        
        if (!isdigit( buffer[ 0 ] )) {
            break;
        }
        
        selection = atoi( buffer );
        if (selection >= MIN_SELECTION && selection <= MAX_SELECTION)
            done = TRUE;
        else {
            beep();
            flash();
        }
    }
    
    if (done) {
        int value = atoi( buffer );
        Logger_LogInfo( "About to edit menu selection [%d]\n", value );
        
    }
}
