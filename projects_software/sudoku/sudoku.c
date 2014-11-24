#include <stdio.h>
#include <stdlib.h>

int number_fits_rules(int number, int sud[9][9], int i, int j){

	if( (i > 8) || (j > 8) )
		return -2;

	int _i,_j;//In function index
	int cell_s_i = (i / 3) * 3;
	int cell_s_j = (j / 3) * 3;

	for (_i = cell_s_i; _i < (cell_s_i+3); _i++){
		for(_j = cell_s_j; _j < (cell_s_j+3); _j++){
			if( (_i != i) && (_j != j) )
				if (number == sud[_i][_j])
					return -1;
		}
	}
	
	for (_i = 0; _i < 9; _i++)//Through row looking
		if( (_i != i) && (sud[_i][j] == number))
			return -1;

	for (_j = 0; _j < 9; _j++)//Through column looking
		if( (_j != j) && (sud[i][_j] == number))
			return -1;
	return 0;
}

int next_value(int sud[9][9], int i, int j){
	int _value = sud[i][j] + 1;
	if (_value > 9)
		return -1;

	while(number_fits_rules(_value, sud, i, j) != 0){
		_value++;
		if (_value > 9) 
			return -1;
	}
	return _value;
}

int print_sud(int sud[9][9]){
	int tmpi, tmpj;

	for (tmpi = 0; tmpi < 9; tmpi++){
		for(tmpj = 0; tmpj < 9; tmpj++){
      		printf("%d ", sud[tmpi][tmpj]);
      	}
      	printf("\n");
    }
}

int solve (int sud[9][9], int cur_d_i,int cur_d_j){
	int i, j;

	if(cur_d_j = 8)
		cur_d_j = 0;

	for (i = cur_d_i; i < 9; i++){
		for(j = cur_d_j; j < 9; j++){
			if(sud[i][j] == 0){//Initiation of recursion. Recursion enter point.

				if(next_value(sud, i, j) == -1){
					sud[i][j] = 0;
					return -1;
				}else
					sud[i][j] = next_value(sud, i, j);

				while (solve(sud, i, j) == -1){
					if(next_value(sud, i, j) == -1){
						sud[i][j] = 0;
						return -1;
					}else
						sud[i][j] = next_value(sud, i, j);
				}
			}

		}
	}
	return 0;
}

int  main(int argc, char const *argv[])
{
	FILE *in_file = fopen("in.txt", "rt");
	int data[9][9];
	int i,j;
	for (i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			char tmp = fgetc(in_file);
			data[i][j] = atoi(&tmp);
      		printf("%d ", data[i][j]);
      	}
      	printf("\n");
    }
    solve( data, 0,0 );
    printf("=======================\n");

    for (i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
      		printf("%d ", data[i][j]);
      	}
      	printf("\n");
    }
	return 0;
}
