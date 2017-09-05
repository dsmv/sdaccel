
#include <stdio.h>
#include <stdlib.h>

#if defined(__linux__)
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <termios.h>
#include <stdarg.h>
#include <vector>
#endif

#include "table_engine_console.h"
//#include "gipcy.h"

//-----------------------------------------------------------------------------
#if defined(__linux__)
static const char* te[] = { "┌", "┐", "└", "┘", "─", "│", "├", "┤","┼", "┬", "┴" };
#endif
//-----------------------------------------------------------------------------

TableEngineConsole::TableEngineConsole() : TableEngine()
{
	flagTableCreate=0;
#if defined(__linux__)
    rows.clear();
    m_C = m_R = 0;
    WC = CELL_WIDTH;
    HC = CELL_HEIGHT;
    WCL = 25;
#else
#endif
}

//-----------------------------------------------------------------------------

TableEngineConsole::~TableEngineConsole()
{
#if defined(__linux__)
#else
#endif
}

//-----------------------------------------------------------------------------

int TableEngineConsole::CreateTable(const char *pColumnName[], unsigned nCount, unsigned isTStudio)
{
#if defined(__linux__)
    get_screen(&WS, &HS);
    get_pos(&XC, &YC);

    if(HS-YC < 20) {
        system("clear");
        YC = 1;
    }

    m_C = nCount;
    m_R = 1;

    for(int i=0; i<m_R; i++) {

        row_t row;

        for(unsigned j=0; j<nCount; j++) {

            cell_t c;

            c.row = i;
            c.col = j;
            c.x = XC + j*WC;
            c.y = YC + i*HC;

            cell_draw(c.x, c.y, WC, HC);

            row.c.push_back(c);
        }

        row.num = i;
        rows.push_back(row);
    }

    cell_draw_fix();

    for(unsigned i=0; i<nCount; i++) {
        SetValueTable(0, i, "%s", pColumnName[i]);
    }

    flagTableCreate=1;
    return 0;
#else
#endif
}

//-----------------------------------------------------------------------------

int TableEngineConsole::AddRowTable()
{
	if( 0==flagTableCreate )
		return 0;

#if defined(__linux__)
    row_t new_row;
    row_t& last_row = rows.at(rows.size()-1);

    new_row.num = last_row.num+1;
    for(int j=0; j<m_C; j++) {

        cell_t c;

        c.row = new_row.num;
        c.col = j;
        c.x = XC + c.col*WC;
        c.y = YC + c.row*HC;

        cell_draw(c.x, c.y, WC, HC);

        new_row.c.push_back(c);
    }

    m_R++;

    rows.push_back(new_row);

    cell_draw_fix();
#else
#endif
    return rows.size()-1;
}

//-----------------------------------------------------------------------------

int TableEngineConsole::SetValueTable(unsigned nRow, unsigned nColumn, const char *fmt, ...)
{
#if defined(__linux__)
    cell_t& c = get_cell(nRow, nColumn);
    gotoxy(c.x+2, c.y+1);
    va_list argptr;
    va_start(argptr, fmt);
    char msg[32];
    vsprintf(msg, fmt, argptr);
    printf("%s\n", msg);
#endif
    return 0;
}

//-----------------------------------------------------------------------------

//int TableEngineConsole::SetValueTable(unsigned nRow, unsigned nColumn, const char *pVal)
//{
//#if defined(__linux__)
//    SetValueTable(nRow, nColumn, "%s", pVal);
//#else
//#endif
//    return 0;
//}

//-----------------------------------------------------------------------------

int TableEngineConsole::SetValueTable(unsigned nRow, unsigned nColumn, signed nVal, const char* format)
{
#if defined(__linux__)
    SetValueTable(nRow, nColumn, format, nVal);
#else
#endif
    return 0;
}

//-----------------------------------------------------------------------------

int TableEngineConsole::SetValueTable(unsigned nRow, unsigned nColumn, unsigned nVal, const char *format)
{
#if defined(__linux__)
    SetValueTable(nRow, nColumn, format, nVal);
#else
#endif
    return 0;
}

//-----------------------------------------------------------------------------

int TableEngineConsole::SetValueTable(unsigned nRow, unsigned nColumn, float dVal, const char *format)
{
#if defined(__linux__)
    SetValueTable(nRow, nColumn, format, dVal);
#else
#endif
    return 0;
}

//-----------------------------------------------------------------------------

void TableEngineConsole::ClearTable()
{
#if defined(__linux__)
#else
#endif
}

//-----------------------------------------------------------------------------

void TableEngineConsole::SaveTable()
{
#if defined(__linux__)
#else
#endif
}

//-----------------------------------------------------------------------------

void TableEngineConsole::UpdateTable()
{
#if defined(__linux__)
#else
#endif
}

//-----------------------------------------------------------------------------

void TableEngineConsole::GetConsolePos(int& X, int& Y)
{
#if defined(__linux__)
    get_pos(&X, &Y);
#else
#endif
}

//-----------------------------------------------------------------------------

void TableEngineConsole::SetConsolePos(int X, int Y)
{
#if defined(__linux__)
    gotoxy(X, Y);
#endif
}

//-----------------------------------------------------------------------------

#if defined(__linux__)

cell_t& TableEngineConsole::get_cell(int row, int col)
{
    return rows.at(row).c.at(col);
}

//------------------------------------------------------------------------------

void TableEngineConsole::cell_draw(int x, int y, int w, int h)
{
    int i=0;

    //set_display_atrib(B_BLUE);

    gotoxy(x,y);
    puts(te[0]);

    gotoxy(x+w,y);
    puts(te[1]);

    gotoxy(x,y+h);
    puts(te[2]);

    gotoxy(x+w,y+h);
    puts(te[3]);

    for(i=1; i<w; i++) {
        gotoxy(x+i,y);
        puts(te[4]);
        gotoxy(x+i,y+h);
        puts(te[4]);
    }

    for(i=1; i<h; i++) {
        gotoxy(x,y+i);
        puts(te[5]);
        gotoxy(x+w,y+i);
        puts(te[5]);
    }
}

//------------------------------------------------------------------------------

void TableEngineConsole::cell_draw_fix()
{
    for(int col=0; col<m_C+1; col++) {

        for(int row=0; row<m_R+1; row++) {

            int x = XC + col*WC;
            int y = YC + row*HC;

            gotoxy(x, y);

            if((row==0) && (col != 0) && (col != m_C))
                puts(te[9]);
            if((row==m_R) && (col != 0) && (col != m_C))
                puts(te[10]);
            if((col && row) && (col != m_C) && (row != m_R))
                puts(te[8]);
            if((col==0) & (row != 0) && (row != m_R))
                puts(te[6]);
            if((col==m_C) & (row != 0) && (row != m_R))
                puts(te[7]);
        }
    }

    gotoxy(1, YC+m_R*HC+1);
}

//------------------------------------------------------------------------------

int TableEngineConsole::get_pos(int *X, int *Y)
{
    int res = 0;
    char buf[16];
    char cmd[]="\033[6n";
    struct termios save,raw;
    setvbuf(stdout, NULL, _IONBF, 0);
    tcgetattr(0,&save);
    cfmakeraw(&raw); tcsetattr(0,TCSANOW,&raw);
    if (isatty(fileno(stdin)))
    {
        int res = 0;
        write(1,cmd,sizeof(cmd));
        res = read (0 ,buf ,sizeof(buf));
        if(res <= 0)
            return -1;
        Y[0] = atoi(&buf[2]);
        if(Y[0]>=10)
            X[0] = atoi(&buf[5]);
        else
            X[0] = atoi(&buf[4]);
        //printf("\nX = %d Y = %d\n", Y[0], X[0]);
    } else {
        res = -1;
    }
    tcsetattr(0,TCSANOW,&save);
    return res;
}

//------------------------------------------------------------------------------

int TableEngineConsole::get_screen(int *W, int *H)
{
    struct winsize ws = { 0, 0, 0, 0 };
    if(ioctl(fileno(stdout), TIOCGWINSZ, &ws) == -1) {
        return -1;
    }
    //printf("TIOCGWINSZ: %d %d %d %d\n", ws.ws_row, ws.ws_col, ws.ws_xpixel, ws.ws_ypixel);
    W[0] = ws.ws_col;
    H[0] = ws.ws_row;
    return 0;
}

#endif

//-----------------------------------------------------------------------------
