#ifndef TABLEENGINE_H
#define TABLEENGINE_H

class TableEngine
{
public:
    TableEngine() {}
    virtual ~TableEngine() {}

	// Создание таблицы
    virtual int CreateTable(const char *pColumnName[], unsigned nCount, unsigned isTStudio) = 0;
	// Добавление строки в таблицу
    virtual int AddRowTable() = 0;
	// Установка значения в ячейку таблицы
    virtual int SetValueTable(unsigned nRow, unsigned nColumn, const char *fmt, ...) = 0;
    //virtual int SetValueTable(unsigned nRow, unsigned nColumn, const char *pVal) = 0;
    virtual int SetValueTable(unsigned nRow, unsigned nColumn, signed nVal, const char* format="%d") = 0;
    virtual int SetValueTable(unsigned nRow, unsigned nColumn, unsigned nVal, const char *format="%u") = 0;
    virtual int SetValueTable(unsigned nRow, unsigned nColumn, float dVal, const char *format="%f") = 0;

	// Очистка таблицы
    virtual void ClearTable() = 0;
	// Сохранение таблицы
    virtual void SaveTable() = 0;
    // Данные обновились
    virtual void UpdateTable() = 0;

    // Получить координаты курсора
    virtual void GetConsolePos(int& X, int& Y) = 0;

    // Установить координаты курсора
    virtual void SetConsolePos(int X, int Y) = 0;
};

#endif // TABLEENGINE_H
