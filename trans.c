/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#define min(a, b) ((a) < (b) ? (a) : (b))
int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(M == 32 && N == 32)
    {
        int temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7;

        for (int m = 0; m < 32; m += 8)
        {
            for (int n = 0; n < 32; n += 8)
            {
                if (m != n)
                {
                    for (int i = m; i < m + 8; ++i)
                    {
                        for (int j = n; j < n + 8; ++j)
                        {
                            B[j][i] = A[i][j];
                        }
                    }
                }
                else
                {
                    for (int i = m; i < m + 8; ++i)
                    {
                        temp0 = A[i][n];
                        temp1 = A[i][n + 1];
                        temp2 = A[i][n + 2];
                        temp3 = A[i][n + 3];
                        temp4 = A[i][n + 4];
                        temp5 = A[i][n + 5];
                        temp6 = A[i][n + 6];
                        temp7 = A[i][n + 7];

                        B[i][n] = temp0;
                        B[i][n + 1] = temp1;
                        B[i][n + 2] = temp2;
                        B[i][n + 3] = temp3;
                        B[i][n + 4] = temp4;
                        B[i][n + 5] = temp5;
                        B[i][n + 6] = temp6;
                        B[i][n + 7] = temp7;
                    }
                    for (int i = m; i < m + 8; ++i)
                    {
                        for (int j = n + (i - m + 1); j < n + 8; ++j)
                        {
                            if (i != j)
                            {
                                temp0 = B[i][j];
                                B[i][j] = B[j][i];
                                B[j][i] = temp0;
                            }
                        }
                    }
                }
            }
        }
    }
    else if(M == 64 && N == 64)
    {
        int n, m, i, j;
        int temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7; 
        for (n = 0; n < N; n += 8) {
            for (m = 0; m < M; m += 8) {
                for (i = n; i < n + 4; i++) {
                    temp0 = A[i][0+m]; 
                    temp1 = A[i][1+m]; 
                    temp2 = A[i][2+m]; 
                    temp3 = A[i][3+m];  
                    temp4 = A[i][4+m]; 
                    temp5 = A[i][5+m]; 
                    temp6 = A[i][6+m]; 
                    temp7 = A[i][7+m];  

                    B[0+m][i] = temp0; 
                    B[1+m][i] = temp1; 
                    B[2+m][i] = temp2; 
                    B[3+m][i] = temp3;              
                    B[0+m][4+i] = temp4; 
                    B[1+m][4+i] = temp5; 
                    B[2+m][4+i] = temp6; 
                    B[3+m][4+i] = temp7;  
                }
                for (j = m; j < m + 4; j++) {
                    temp0 = A[4+n][j]; 
                    temp1 = A[5+n][j]; 
                    temp2 = A[6+n][j]; 
                    temp3 = A[7+n][j];   
                    temp4 = B[j][4+n]; 
                    temp5 = B[j][5+n]; 
                    temp6 = B[j][6+n]; 
                    temp7 = B[j][7+n];   

                    B[j][4+n] = temp0; 
                    B[j][5+n] = temp1; 
                    B[j][6+n] = temp2; 
                    B[j][7+n] = temp3;      
                    B[4+j][0+n] = temp4; 
                    B[4+j][1+n] = temp5; 
                    B[4+j][2+n] = temp6; 
                    B[4+j][3+n] = temp7;  
                }
                for (i = n + 4; i < n + 8; i++) {
                    temp0 = A[i][4+m]; 
                    temp1 = A[i][5+m]; 
                    temp2 = A[i][6+m]; 
                    temp3 = A[i][7+m]; 
                    B[4+m][i] = temp0; 
                    B[5+m][i] = temp1; 
                    B[6+m][i] = temp2; 
                    B[7+m][i] = temp3; 
                }
            }
        }
    }
    else if(M == 61 && N == 67)
    {
        for (int m = 0; m < 67; m += 17)
        {
            for (int n = 0; n < 61; n += 4)
            {
                for (int i = m; i < min(m + 17, 67); ++i)
                {
                    for (int j = n; j < min(n + 4, 61); ++j)
                    {
                        B[j][i] = A[i][j];
                    }
                }
            }
        }
    }
}
/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

