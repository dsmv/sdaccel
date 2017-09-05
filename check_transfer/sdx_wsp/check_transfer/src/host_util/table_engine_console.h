#ifndef TABLE_ENGINE_CONSOLE_H_
#define TABLE_ENGINE_CONSOLE_H_

#include "table_engine.h"
#if defined(__linux__)
#include "term_table.h"
#endif

class TableEngineConsole : public TableEngine
{
public:
    TableEngineConsole();
    ~TableEngineConsole();

public:
    int CreateTable(const char *pColumnName[], unsigned nCount, unsigned isTStudio);
    int AddRowTable();
    int SetValueTable(unsigned nRow, unsigned nColumn, const char *fmt, ...);
    //int SetValueTable(unsigned nRow, unsigned nColumn, const char *pVal);
    int SetValueTable(unsigned nRow, unsigned nColumn, signed nVal, const char* format="%d");
    int SetValueTable(unsigned nRow, unsigned nColumn, unsigned nVal, const char *format="%u");
    int SetValueTable(unsigned nRow, unsigned nColumn, float dVal, const char *format="%f");
    void ClearTable();
    void SaveTable();
    void UpdateTable();
    void GetConsolePos(int& X, int& Y);
    void SetConsolePos(int  X, int  Y);

private:
    int	flagTableCreate;
#if defined(__linux__)
    int m_C, m_R;
    int WC, HC, WCL;
    int WS, HS, XC, YC;
    std::vector<struct row_t> rows;

    int get_screen(int *W, int *H);
    int get_pos(int *X, int *Y);
    void cell_draw(int x, int y, int w, int h);
    void cell_draw_fix();
    cell_t& get_cell(int row, int col);
#else
#endif
};

#endif // TABLE_ENGINE_CONSOLE_H_
