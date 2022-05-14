/* 
 * Vojtech Netrh (R21412)
 * april 2022
 */

/*
 * Program for working with matrices represented by 3 structures.
 * The description of task here (https://vyjidacek.cz/zpc2/ - in czech).
 * Solution was done as first main task of subject
 * 'Basics of programming in C, part 2' teached at Palacky Univeristy
 * (field of study computer science).
 * You can do following operations: create, delete, find value, set value,
 * add and substract 2 matrices and find maximal value.
 * You can use values that can be represented by 'double' type
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct matrix {
	/* structure representing whole matrix */
	int cols;
	int rows;
	struct index *first_col;
	struct index *first_row;
} Matrix;

typedef struct index {
	/* structure represeting row/column in matrix */
	int position;
	struct value *val;
	struct index *next_index;
} Index;

typedef struct value {
	/* 
	 * structure representing value in matrix
	 * if value is 0, then structure is replaced by NULL
	 */
	double val;
	int col;
	int row;
	struct value *next_val_col;
	struct value *next_val_row;
} Value;


int place_matrix_val(Matrix* whole_matrix, Value* cell_value) {
	/*
	 * procedure for right place of value "cell" into matrix
	 * right place ... right pointers
	 */
	Index* cell_row = whole_matrix->first_row;
	Index* cell_col = whole_matrix->first_col;

	while (cell_value->row != cell_row->position) {
		cell_row = cell_row->next_index;
	}

	while (cell_value->col != cell_col->position) {
		cell_col = cell_col->next_index;
	}

	if (!cell_row->val) {
		cell_row->val = cell_value;
	}
	else {
		Value* current_cell_value = cell_row->val;

		while (current_cell_value->next_val_row != NULL) {
			current_cell_value = current_cell_value->next_val_row;
		}
		current_cell_value->next_val_row = cell_value;
	}

	if (!cell_col->val) {
		cell_col->val = cell_value;
	}
	else {
		Value* current_cell_value = cell_col->val;

		while (current_cell_value->next_val_col != NULL) {
			current_cell_value = current_cell_value->next_val_col;
		}
		current_cell_value->next_val_col = cell_value;
	}

	return 0;
}

Value* create_val_cell(double value, unsigned int row, unsigned int col) {
	/* creating value strcut for given value and position */
	Value* cell = (Value*)malloc(sizeof(Value));
	if (!cell) {
		free(cell);
		return NULL;
	}
	cell->val = value;
	cell->row = row;
	cell->col = col;
	cell->next_val_col = cell->next_val_row = NULL;

	return cell;
}

Index* create_index_cell(int pos) {
	Index* index = (Index*)malloc(sizeof(Index));
	if (!index) {
		free(index);
		return NULL;
	}
	index->next_index = NULL;
	index->val = NULL;
	index->position = pos;

	return index;
}

Value* find_element_at(Matrix* matrix, unsigned int row, unsigned int col) {
	/* procedure to find struct value in given matrix, on given row and col */
	Index* index = matrix->first_col;

	while (col != index->position) {
		index = index->next_index;
	}

	Value* val_in_index = index->val;
	if (val_in_index) {
		while (row != val_in_index->row) {
			val_in_index = val_in_index->next_val_col;
		}
	}
	else {
		return NULL;
	}

	return val_in_index;
}

void place_value_in_empty(Matrix* matrix, Value* cell, unsigned int row, unsigned int col) {
	/*
	 * placing value in empty place
	 * empty place ... value 0 ... represented by pointer to NULL
	 */
	Index* index_row = matrix->first_row;
	Index* index_col = matrix->first_col;

	while (index_row->position != row) {
		index_row = index_row->next_index;
	}
	while (index_col->position != col) {
		index_col = index_col->next_index;
	}

	Value* temp_pointer_row = index_row->val;
	Value* temp_pointer_col = index_col->val;

	index_row->val = cell;
	index_col->val = cell;
	cell->next_val_row = temp_pointer_row;
	cell->next_val_col = temp_pointer_col;
}

double* matrix_to_array(Matrix* matrix) {
	/*
	 * converting matrix structure to 1d array of values
	 * using this pattern '2d_arr[row][col] = arr[row * #_cols + col]'
	 */
	int num_elements = matrix->cols * matrix->rows;
	double* values = (double*)malloc(num_elements * sizeof(double));
	if (!values) {
		free(values);
		return NULL;
	}

	for (int i = 0; i <= num_elements; i++) {
		values[i] = 0;
	}

	Index* current_index_row = matrix->first_row;
	while (current_index_row) {
		Value* current_value = current_index_row->val;
		while (current_value) {
			int arr_pos = matrix->cols * current_value->row + current_value->col;
			values[arr_pos] = current_value->val;
			current_value = current_value->next_val_row;
		}
		current_index_row = current_index_row->next_index;
	}

	return values;
}

void print_matrix(Matrix* matrix) {
	/* procedure to print matrix in typical view */
	
	if (!matrix) {
		printf("\nempty matrix ... ()");
		return;
	}

	int num_elements = matrix->cols * matrix->rows;
	double* values = matrix_to_array(matrix);

	printf("\n\n");
	printf("----- MATRIX -----");
	for (int k = 0; k < num_elements; k++) {
		if (k != 1) {
			if (k == 0 || (k % matrix->cols) == 0) {
				printf("\n(");
			}
		}
		printf(" %.1f", values[k]);
		if (k != 0 && k != 1) {
			if (((k + 1) % matrix->cols) == 0) {
				printf(" )");
			}
		}
	}
}

void matrix_delete(Matrix* matrix) {
	/* procedure to deallocate matrix from memory */

	Index* index_col = matrix->first_col;
	while (index_col) {
		Index* temp = index_col->next_index;
		free(index_col);
		index_col = temp;
	}

	Index* index_row = matrix->first_row;
	while (index_row) {
		Value* value = index_row->val;
		while (value) {
			Value* temp = value->next_val_row;
			free(value);
			value = temp;
		}
		Index* temp = index_row->next_index;
		free(index_row);
		index_row = temp;
	}

	free(matrix);
}

Matrix* create_matrix(double *values, size_t rows, size_t cols) {
	/* procedure that creates matrix with given values (array) and proportions */
	int len_values = rows * cols;

	Matrix *whole_matrix = (Matrix*)malloc(sizeof(Matrix));
	if (!whole_matrix) {
		free(whole_matrix);
		return NULL;
	}
	whole_matrix->cols = cols;
	whole_matrix->rows = rows;
	if (len_values == 0) {
		whole_matrix->first_col = whole_matrix->first_row = NULL;
		return whole_matrix;
	}

	Index* current_row_i = create_index_cell(0);
	if (!current_row_i) {
		matrix_delete(whole_matrix);
		return NULL;
	}
	whole_matrix->first_row = current_row_i;

	Index *previous_row_i = current_row_i;
	for (int i = 0; i < rows; i++) {
		/* creating index struct for rows */

		Index* temp = create_index_cell(i);
		if (!temp) {
			matrix_delete(whole_matrix);
			return NULL;
		}
		previous_row_i->next_index = temp;
		previous_row_i = temp;
	}

	Index* current_col_i = create_index_cell(0);
	if (!current_col_i) {
		matrix_delete(whole_matrix);
		return NULL;
	}
	whole_matrix->first_col = current_col_i;

	Index* previous_col_i = current_col_i;
	for (int j = 1; j < cols; j++) {
		/* creating index structs for columns */

		Index* temp = create_index_cell(j);
		if (!temp) {
			matrix_delete(whole_matrix);
			return NULL;
		}
		previous_col_i->next_index = temp;
		previous_col_i = temp;
	}

	for (int k = 0; k < len_values; k++) {
		/* creating value structs for individual values (0 ... NULL) */

 		if (values[k] != 0) {
			Value* cell = create_val_cell(values[k], k / cols, k % cols);
			if (!cell) {
				matrix_delete(whole_matrix);
				return NULL;
			}
			place_matrix_val(whole_matrix, cell);
		}
	}

	return whole_matrix;
}

double element_at(Matrix *matrix, unsigned int row, unsigned int col) {
	/* finding value at given row and column */
	Index* index = matrix->first_col;

	while (col != index->position) {
		index = index->next_index;
	}
	
	Value *val = index->val;
	while (row != val->row) {
		val = val->next_val_row;
	}

	if (val->val) {
		return val->val;
	}
	else {
		return 0;
	}
}

int set_element_at(Matrix *matrix, unsigned int row, unsigned int col, double value) {
	/* procedure for seting value on given place */
	Value *cell = find_element_at(matrix, row, col);
	if (cell) {
		cell->val = value;
		return 1;
	}
	else {
		Value *temp = create_val_cell(value, row, col);
		place_value_in_empty(matrix, temp, row, col);
	}
}

double matrix_max(Matrix *matrix) {
	/* going through whole matrix and returning highest value */
	Index *row = matrix->first_row;

	double max_val = row->val->val;
	while (row) {
		Value *temp = row->val;
		while (temp) {
			if (temp->val > max_val) {
				max_val = temp->val;
			}
			temp = temp->next_val_row;
		}
		row = row->next_index;
	}

	if (max_val) {
		return max_val;
	}
	else
		return 0;
}

Matrix* add(Matrix* left, Matrix* right) {
	/* procedure to add 2 matrices together (return new matrix) */
	int num_elements_left = left->cols * left->rows;
	int num_element_right = right->cols * right->rows;

	if (num_elements_left != num_element_right) {
		printf("\nError: unable to add 2 matrices with different element count");
		return NULL;
	}

	double* left_values = matrix_to_array(left);
	double* right_values = matrix_to_array(right);
	double* values = (double*)malloc(num_elements_left * sizeof(double));

	for (int i = 0; i < num_elements_left; i++) {
		values[i] = left_values[i] + right_values[i];
	}

	return create_matrix(values, left->rows, left->cols);
}

Matrix* substract(Matrix* left, Matrix* right) {
	/* procedure for substracting 2 matrices (return new matrix) */
	int num_elements_left = left->cols * left->rows;
	int num_element_right = right->cols * right->rows;

	if (num_elements_left != num_element_right) {
		printf("\nError: unable to substract 2 matrix with different element count");
		return NULL;
	}

	double* left_values = matrix_to_array(left);
	double* right_values = matrix_to_array(right);
	double* values = (double*)malloc(num_elements_left * sizeof(double));
	if (!values) {
		free(values);
		return NULL;
	}

	for (int i = 0; i < num_elements_left; i++) {
		values[i] = left_values[i] - right_values[i];
	}

	return create_matrix(values, left->rows, left->cols);
}

int main() {
	/* Tests for all functions */
	double A_values[] = { 1, 1, 1, 2, 5, 3, 4, 0, 7 };
	Matrix *A = create_matrix(A_values, 3, 3);
	double B_values[] = { 3, 2, 1, 5, -3, 0 , 6, 10, 0 };
	Matrix *B = create_matrix(B_values, 3, 3);

	//print_matrix(A);
	//print_matrix(B);

	Matrix* C = add(A, B);
	Matrix* D = substract(B, A);

	print_matrix(C);
	print_matrix(D);

	set_element_at(A, 0, 0, 50.5);
	double test_find = element_at(A, 0, 1);
	double test_max = matrix_max(A);

	printf("\nmatrix max value: %.1f", test_max);
	printf("\nmatrix wanted value: %.1f", test_find);
	print_matrix(A);

	return 0;
}