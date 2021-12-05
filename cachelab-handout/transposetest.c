#include <stdio.h>


void transpose_submit() {
    int M = 16;
    int N = 16;
    int A[N][M];
    int count = 0;
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            A[i][j] = count;
            count++;
        }
    }

    printf("A matrix:\n");
    for(int i = 0; i< N; i++) {
        for(int j = 0; j < M; j++) {
            fprintf(stdout, "%d|", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    int B[M][N];
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            B[i][j] = 0;
        }
    }


    // int matrix_col;
    // int matrix_row;
    // int block_row;
    // int block_col;
    // int diag_value = 0;
    // int diag_value_pos = 0;
    // if((N == 16 && M == 16) || (N == 64 && M == 64)) {
        // for(int matrix_row = 0; matrix_row < N; matrix_row += 4) {              // looping through the big matrix, row-wise
        //
        //     for(int matrix_col = 0; matrix_col < M; matrix_col += 4) {          // looping through the big matrix, column-wise
        //
        //         for(int block_row = 0; block_row < 4; block_row++) {            // looping through the 8 int wide block, row-wise
        //
        //             for(int block_col = 0; block_col < 4; block_col++) {        // looping through the 8 int wide block, column-wise
        //
        //                 if((matrix_row+block_row) == (matrix_col+block_col)) {                            // if on the diagonal
        //
        //                     diag_value = A[matrix_row+block_row][matrix_col+block_col];             // save the value to temporal variable
        //
        //                     diag_value_pos = matrix_row + block_row;                        // and the position of the value in the big matrix
        //
        //                 } else {                                                // if not on the diagonal
        //                     B[matrix_col+block_col][matrix_row+block_row] = A[matrix_row+block_row][matrix_col+block_col];
        //                 }
        //             }
                    // B[diag_value_pos][diag_value_pos] = diag_value;
                    //
                    // printf("B matrix:\n");
                    // for(int i = 0; i< N; i++) {
                    //     for(int j = 0; j < M; j++) {
                    //         fprintf(stdout, " %d |", B[i][j]);
                    //     }
                    //     printf("\n");
        //             }
        //         }
        //     }
        // }

        // for(matrix_col = 0; matrix_col < N; matrix_col += 8) {
        //     for(matrix_row = 0; matrix_row < N; matrix_row += 8) {
        //
        //         for(block_row = matrix_row; block_row < matrix_row + 8; block_row++) {
        //             for(block_col = matrix_col; block_col < matrix_col + 8; block_col++) {
        //                 // printf("block_row = %d\nblock_col = %d\n", block_row, block_col);
        //                 if(block_row != block_col) {
        //                     // printf("block_row != block_col true, block_row = %d block_col = %d\n", block_row, block_col);
        //                     B[block_col][block_row] = A[block_row][block_col];
        //                 } else {
        //                     // printf("block_row != block_col not true, block_row = %d block_col = %d\n", block_row, block_col);
        //                     diag_value_pos = block_row;
        //                     diag_value = A[block_row][block_col];
        //                     // printf("diag_value_pos = %d\ndiag_value = %d\n", diag_value_pos, diag_value);
        //                 }
        //             }
        //             B[diag_value_pos][diag_value_pos] = diag_value;
                    // printf("B matrix:\n");
                    // for(int i = 0; i< N; i++) {
                    //     for(int j = 0; j < M; j++) {
                    //         fprintf(stdout, "%d|", B[i][j]);
                    //     }
                    //     printf("\n");
        //             }
        //         }
        //     }
        // }

        int rowCount;
    	int colCount;
    	int rowSector;
    	int columnSector;
    	//This is to prevent cache misses along the diagonal
    	int diagonal = 0;
    	//Variable that takes advantage of temporal locality (i.e. keeping something inside a cahce to prevent misses
    	int temporal = 0;
    	//For the 64x64 matrix case
    	//We use the same looping structure for 64x64 or 32x32 matrices, only this that changes is size of the sector (based on blocks' proximity to diagonal line)
    	//Also, thought there seems to be duplication between the loops, it is due to the general avoidance of local variables
    	if(N == 16 && M == 16){
    			//We can block 8 at a time, optimal size since its a cache at a time
    			for(columnSector = 0; columnSector < N; columnSector += 8){
    				for(rowSector = 0; rowSector < N; rowSector += 8){
    					//Now we cycle within one block, so we loop sector at a time
    					for(rowCount = rowSector; rowCount < rowSector + 8; rowCount++){
    						for(colCount = columnSector; colCount < columnSector + 8; colCount++){
    							//As long as we are not on the diagonal, we put the value
    							//from A[row][column] into B[column][row], the heart of transposing
    							if(rowCount != colCount){
    								B[colCount][rowCount] = A[rowCount][colCount];

    							}
    							else{
    								//The digonal variable keeps to data in a cache utilizing spatial locality
    								diagonal = rowCount;
    								//Temporal uses...temporal locality
    								temporal = A[rowCount][colCount];
    							}
    						}
    						//In a square matrix, if the sector we are blocking off contains
    						//the diagonal, then we keep those elements the same.
    						//Doint it this way reduces the amount of misses
    						if(columnSector == rowSector){
    							B[diagonal][diagonal] = temporal;
    						}//end if statement
                            printf("B matrix:\n");
                            for(int i = 0; i< N; i++) {
                                for(int j = 0; j < M; j++) {
                                    fprintf(stdout, "%d|", B[i][j]);
                                }
                                printf("\n");
    					}//end rowCount for loop
    				}//end rowSector for loop
    			}//end columnSector for loop
    		}//end N == 32 If statement
    }

    printf("B matrix:\n");
    for(int i = 0; i< N; i++) {
        for(int j = 0; j < M; j++) {
            fprintf(stdout, "%d|", B[i][j]);
        }
        printf("\n");
    }

}

int main() {
    transpose_submit();
}
