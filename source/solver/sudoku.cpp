#include "../../include/solver/sudoku.hpp"

using namespace std;

Sudoku::Sudoku(vector< vector<int> > field_to_solve)
  : _field_height(NUM_ROWS_CELLS),
    _field_width(NUM_ROWS_CELLS),
    _square_height(BOX_HEIGHT),
    _square_width(BOX_WIDTH),
    _field(field_to_solve)
{
	cout << "Sudoku(vec)>> The unsolved sudoku field has been initialized:" << endl;
	cout << "Sudoku(vec)>> field size: (" << _field_height << "x" << _field_width << ")" << endl;
	cout << "Sudoku(vec)>> box size: (" << _square_height << "x" << _square_width << ")"<< endl;
	printField();

	if(_square_height == 0 || _square_width == 0)
	{
	  _square_height = floor(sqrt(NUM_ROWS_CELLS));
	  _square_width = floor(sqrt(NUM_ROWS_CELLS));

	  //If the sudoku field has no square-shaped box, for example a 12x12 sudoku field
	  //the while loop tries to modify the box size. It does not work for every field sizes.
	  while(_square_height*_square_width < NUM_ROWS_CELLS)
	  {
	    _square_width++;
		cout << "Sudoku(vec)>> Box size has been changed." << endl;
		cout << "Sudoku(vec)>> New box size: (" << _square_height << "x" << _square_width << ")"<< endl;
	  }
	}
	  if(_square_height*_square_width != NUM_ROWS_CELLS)
		{throw(1);}

	//The program checks, if the given numbers obey the sudoku conventions
	for(int row=0;row<_field_height;row++)
		for(int col=0;col<_field_width;col++)
		{
			int value_to_check = _field[row][col];
			_field[row][col] = 0;
			if(value_to_check == 0 )
			 {
		       _field[row][col] = value_to_check;
		       continue;
			 }
			else if(!checkRow(row,value_to_check) || !checkColumn(col,value_to_check) || !checkSquare(row,col,value_to_check))
			{
			  cout << "At (" << row << "," << col << "): " << endl;
			  _field[row][col] = value_to_check;
			  printField();
			  throw(2);
			}
			else
			  {_field[row][col] = value_to_check;}
		}
}

Sudoku::Sudoku(const char* filename)
  : _field_height(NUM_ROWS_CELLS),
    _field_width(NUM_ROWS_CELLS),
    _square_height(floor(sqrt(NUM_ROWS_CELLS))),
    _square_width(floor(sqrt(NUM_ROWS_CELLS)))
{
    //The sudoku field is initialized with zeros
	for(int row=0;row<_field_height;row++)
	{
		vector<int> temp;
		for(int col=0;col<_field_width;col++)
		{
			temp.push_back(0);
		}
		_field.push_back(temp);
	}

	//The given numbers of the sudoku are captured from the 'sudoku_src' file.
	ifstream input_file(filename);
	if(!input_file.is_open())
	{
		//If the file could not been opened, an exception will be thrown
		throw(0);
	}
	for(int row=0;row<_field_height;row++)
		for(int col=0;col<_field_width;col++)
		  {input_file >> _field[row][col];}
	input_file.close();

	cout << "Sudoku(file)>> The unsolved sudoku field has been initialized:" << endl;
	cout << "Sudoku(file)>> field size: (" << _field_height << "x" << _field_width << ")" << endl;
	cout << "Sudoku(file)>> box size: (" << _square_height << "x" << _square_width << ")"<< endl;
	printField();

	//The program checks, if the given numbers obey the sudoku conventions
	for(int row=0;row<_field_height;row++)
		for(int col=0;col<_field_width;col++)
		{
			int value_to_check = _field[row][col];
			_field[row][col] = 0;
			if(value_to_check == 0 )
			 {
		       _field[row][col] = value_to_check;
		       continue;
			 }
			else if(!checkRow(row,value_to_check) || !checkColumn(col,value_to_check) || !checkSquare(row,col,value_to_check))
			  {
				cout << "At (" << row << "," << col << "): " << endl;
				_field[row][col] = value_to_check;
				throw(2);
			  }
			else
			  {_field[row][col] = value_to_check;}
		}

	//If the sudoku field has no square-shaped box, for example a 12x12 sudoku field
	//the while loop tries to modify the box size. It does not work for every field
	//sizes.
	while(_square_height*_square_width < NUM_ROWS_CELLS)
	{
		_square_width++;
		cout << "Sudoku(file)>> box width has been changed." << endl;
		cout << "Sudoku(file)>> New box size: (" << _square_height << "x" << _square_width << ")"<< endl;
	}
	if(_square_height*_square_width != NUM_ROWS_CELLS)
	  {throw(1);}
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
	//cout << "backTracking>> Satz mit X, das war wohl nix! \n Retouniere false" << endl;
	return false;
}

bool Sudoku::solveSudoku()
{
  bool isSolvable = backTracking(0,0);
  cout << "solveSudoku()>> ";
  if(isSolvable)
    {cout << "Sudoku has been solved:" << endl;}
  else
  {
	cout << "Solving Sudoku failed!" << endl;
    cout << "The given sudoku is unsolvable. Please check the input." << endl;
  }
  printField();
  return isSolvable;
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

const vector<vector<int> > & Sudoku::getSolution() const
{
  return _field;
}
