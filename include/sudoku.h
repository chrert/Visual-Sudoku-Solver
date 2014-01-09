#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include "settings.hpp"

using namespace std;

class Sudoku
{
  public:

	Sudoku(vector< vector<int> > field_to_solve);
	Sudoku(const char* filename);
	~Sudoku();
	
	/*
	 * solveSudoku solves the given sudoku field by calling the method backTracking
	 */
	void solveSudoku();


  private:
	/*
	 * checkRow() checks if a certain number is already in a certain row of the sudoku field
	 * 	row_number: row to check
	 * 	value:  	value to check
	 * 	return:	checkRow returns true, if value could not be found in the corresponding row
	*/
	bool checkRow(int row_number, int value);
	
	/*
	 * CheckSquare() checks if a certain number is already in a given square of the sudoku field
	 * 	row_number, column_number: 	coordinates of the cell which is part of the square
	 * 	value:						value to check
	 * 	return:	checkSquare returns true, if value could not be found in the corresponding square
	 */
	bool checkSquare(int row_number, int column_number, int value);
	
	/*
	 * checkColumn() checks if a certain number is already in a certain column of the sudoku field
	 * 	row_number, column_number: 	coordinates of the corresponding cell
	 * 	value:  					value to check
	 * 	return:	checkColumn() returns true, if value could not be found in the corresponding column
	 */
	bool checkColumn(int column_number, int value);
	
	/*
	 * backtracking() solves the given sudoku field recursively by using the backtracking algorithm
	 */
	bool backTracking(int row, int column);
	
	/*
	 * printField() prints the current state of the sudoku field
	 */
	void printField();

  private:
	int _field_height;
	int _field_width;
	int _square_height;
	int _square_width;
	vector< vector<int> > _field;
	
};
