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
static  WINDOW  *panel, *manualPanel, *duskDawnPanel, *duskTimerPanel;


// -----------------------------------------------------------------------------
static
void    paintLoadData ()
{
    //
    int beginRow = 2;
    int beginCol = 3;

    HaddTextField( manualPanel, beginRow++, beginCol, "Manual            ", (loadControlMode == 0x00 ? "Yes" : "No ") );
    HaddTextField( panel, beginRow++, beginCol, "Night On and Timer", (loadControlMode == 0x02 ? "Yes" : "No ") );
    HaddTextField( panel, beginRow++, beginCol, "Timer On and Off  ", (loadControlMode == 0x03 ? "Yes" : "No ") );
    wrefresh( panel );

    beginRow ++;
    char    buf1[ 12 ], buf2[ 12 ];
    HaddTextField( duskDawnPanel, beginRow++, beginCol, "Night On/Day Off  ", (loadControlMode == 0x01 ? "Yes" : "No ") );
    snprintf( buf1, sizeof buf1, "%02d:%02d:%02d", HH_T1On, MM_T1On, SS_T1On );
    snprintf( buf2, sizeof buf2, "%02d:%02d:%02d", HH_T2On, MM_T2On, SS_T2On );
    HaddTextField( duskDawnPanel, beginRow++, beginCol, "Timer One - On", buf1 );
    HaddTextField( duskDawnPanel, beginRow++, beginCol, "Timer Two - On", buf2 ); 
    wrefresh( duskDawnPanel );
            
    char    buf3[ 12 ], buf4[ 12 ];
    snprintf( buf3, sizeof buf3, "%02d:%02d:%02d", HH_T1Off, MM_T1Off, SS_T1Off );
    snprintf( buf4, sizeof buf4, "%02d:%02d:%02d", HH_T2Off, MM_T2Off, SS_T2Off );
    HaddTextField( duskTimerPanel, beginRow++, beginCol, "Timer One - Off", buf3 );
    HaddTextField( duskTimerPanel, beginRow++, beginCol, "Timer Two - Off", buf4 ); 
    wrefresh( duskTimerPanel );
    
    
    beginRow++;
    //HfloatAddTextField( panel, beginRow++, beginCol, "Is Night Threshold <= V", nighttimeThresholdVoltage, 1, 4 );
    //HfloatAddTextField( panel, beginRow++, beginCol, "Is Day Threshold >= V  ", daytimeThresholdVoltage, 1, 4 );

    beginRow++;
    char    buf5[ 12 ], buf6[ 12 ];
    snprintf( buf5, sizeof buf5, "%02d:%02d", HH_WT1, MM_WT1 );
    snprintf( buf6, sizeof buf6, "%02d:%02d", HH_WT2, MM_WT2 );
    //HaddTextField( panel, beginRow++, beginCol, "Work Timer One", buf5 );
    //HaddTextField( panel, beginRow++, beginCol, "Work Timer Two", buf6 );
    
    beginRow++;
    char    buf7[ 10 ];
    snprintf( buf7, sizeof buf7, "%02d:%02d", HH_LON, MM_LON );
    //HaddTextField( panel, beginRow++, beginCol, "Length of Night", buf7 );
            
    wrefresh( panel );
}

// -----------------------------------------------------------------------------
void    showLoadPanel()
{
    setActivePanel( LOAD_PANEL );

    int     startY = 0;
    int     startX = 0;
    int     nRows = MaxRows;
    int     nCols = MaxCols;

    panel = grouping( &panel, startY, startX, nRows, nCols, "Load Control" );
    
    startY = 1;
    startX = 1;
    nRows = 5;
    nCols = MaxCols - startY;
    manualPanel = grouping( &manualPanel, startY, startX, nRows, nCols, "Manual Control" );

    startY += nRows;
    startX = 1;
    nRows = 8;
    nCols = MaxCols - startY;
    manualPanel = grouping( &manualPanel, startY, startX, nRows, nCols, "Dusk to Dawn Control" );
    
    startY += nRows;
    startX = 1;
    nRows = 8;
    nCols = MaxCols - startY;
    manualPanel = grouping( &manualPanel, startY, startX, nRows, nCols, "Dusk Plus Timer Control" );
    
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
