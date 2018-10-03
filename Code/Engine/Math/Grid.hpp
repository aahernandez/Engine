#pragma once

class Grid
{
public:
	Grid(int rows, int columns);
	~Grid();

	int GetNumRows() const;
	int GetNumColumns() const;

private:
	int m_rows;
	int m_columns;
};