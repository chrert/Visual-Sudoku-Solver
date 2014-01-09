#include "sudoku.h"

using namespace std;

Sudoku::Sudoku(vector< vector<int> > field_to_solve)
  : _field_height(NUM_ROWS_CELLS),
    _field_width(NUM_ROWS_CELLS),
    _square_height(floor(sqrt(NUM_ROWS_CELLS))),
    _square_width(floor(sqrt(NUM_ROWS_CELLS))),
    _field(field_to_solve)
{
	cout << "Sudoku()>> square size: (" << _square_height << "x" << _square_width << ")"<< endl;
}

Sudoku::Sudoku(const char* filename)
  : _field_height(NUM_ROWS_CELLS),
    _field_width(NUM_ROWS_CELLS),
    _square_height(floor(sqrt(NUM_ROWS_CELLS))),
    _square_width(floor(sqrt(NUM_ROWS_CELLS)))
{
	cout << "Sudoku()>> square size: (" << _square_height << "x" << _square_width << ")"<< endl;
	cout << "Sudoku()>> square size:" << _field_width << endl;
	cout << "Sudoku()>> square size:" << _field_height << endl;

	if(_square_height*_square_width != NUM_ROWS_CELLS)
	{
		_square_width++;
		if(_square_height*_square_width != NUM_ROWS_CELLS)
		{
			throw(1);
		}
		cout << "Sudoku()>> _square_width has been changed to " << _square_width << endl;
	}

	for(int row=0;row<_field_height;row++)
	{
		vector<int> temp;
		for(int col=0;col<_field_width;col++)
		{
			temp.push_back(0);
		}
		_field.push_back(temp);
	}

	ifstream input_file(filename);
	if(!input_file.is_open())
	{
		throw(0);
	}
	for(int row=0;row<_field_height;row++)
		for(int col=0;col<_field_width;col++)
		{
			input_file >> _field[row][col];
			//cout << "field(" << row << "," << col << "): " << _field[row][col] << endl;
		}
	input_file.close();
	cout << "Sudoku()>> The unsolved sudoku field has been initialized:" << endl;
	printField();
}

Sudoku::~Sudoku()
{

}

bool Sudoku::checkRow(int row_number, int value)
{
	for(int row=0;row<_field_height;row++)
		{
			if(_field[row_number][row] == value)
				{return false;}
		}
	return true;
}

bool Sudoku::checkSquare(int row_number, int column_number, int value)
{

	//find the upper left corner of the corresponding square
	int left_corner_row = floor(row_number/_square_height) * _square_height;
	int left_corner_col = floor(column_number/_square_width) * _square_width;

	for(int row=left_corner_row;row<left_corner_row+_square_height;row++)
		for(int col=left_corner_col;col<left_corner_col+_square_width;col++)
		{
			if(_field[row][col] == value)
				{return false;}
		}
	return true;

}

bool Sudoku::checkColumn(int column_number, int value)
{
	for(int row=0;row<_field_height;row++)
		{
			if(_field[row][column_number] == value)
				{return false;}
		}
	return true;

}

bool Sudoku::backTracking(int row, int column)
{
	if(column == _field_width)
	{
		row++;
		if(row == _field_height)
			{return true;}

		column = 0;
	}

	if(_field[row][column] > 0)
		{return backTracking(row,column+1);}

	for(int candidate=1;candidate<NUM_ROWS_CELLS+1;candidate++)
	{
		if(checkRow(row,candidate) && checkColumn(column,candidate) && checkSquare(row,column,candidate))
		{
			_field[row][column] = candidate;
			if(backTracking(row,column+1))
				return true;
		}
	}
	_field[row][column] = 0;
	return false;
}

void Sudoku::solveSudoku()
{
	bool isSolvable = backTracking(0,0);
	cout << "solveSudoku()>> ";
	if(isSolvable)
		{cout << "Sudoku has been solved:" << endl;}
	else
		{cout << "Solving Sudoku failed!!!!!!!!!!!!" << endl;}
	printField();
}

void Sudoku::printField()
{
	//cout << "printField()>> This is what you get:" << endl;
	cout << "\n";
	for(int row=0;row<_field_height;row++)
	{
		for(int col=0;col<_field_width;col++)
		{
			cout << _field[row][col] << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

