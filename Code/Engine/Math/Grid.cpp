#include "Engine/Math/Grid.hpp"

Grid::Grid(int rows, int columns)
{
	m_rows = rows;
	m_columns = columns;
}

Grid::~Grid()
{

}

int Grid::GetNumRows() const
{
	return m_rows;
}

int Grid::GetNumColumns() const
{
	return m_columns;
}