/*
 * ---------------- www.spacesimulator.net --------------
 *   ---- Space simulators and 3d engine tutorials ----
 *
 * Author: Damiano Vitulli
 *
 * This program is released under the BSD licence
 * By using this program you agree to licence terms on spacesimulator.net copyright page
 *
 *
 * Maths library for matrices management
 *  
 */

#include <math.h>
#include "mat_matr.h"



/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/

float matr_sin_table[3600], matr_cos_table[3600]; // Look up tables



/**********************************************************
 *
 * SUBROUTINE MatrGenerateLookupTab(void)
 *
 * Look up tables generation. 
 * To speed up the cosine and sine operations we fill two arrays with
 * a complete turn of 360 degrees ;-)
 * We set a resolution of 1/10 degree
 * 
 *********************************************************/

void MatrGenerateLookupTab (void)
{
   int i;
   for(i=0; i<3600; i++)
   {
      matr_sin_table[i]=(float)sin(i*3.1415/1800);
      matr_cos_table[i]=(float)cos(i*3.1415/1800); 
   }    
}



/**********************************************************
 *
 * SUBROUTINE MatrIdentity_4x4(matrix_4x4_type p_matrix)
 *
 * Initialize the input matrix as an identity matrix
 *
 * Input parameters: p_matrix = matrix to initialize
 * 
 *********************************************************/

void MatrIdentity_4x4 (matrix_4x4_type p_matrix)
{
	int j,k;

	for (j=0;j<4;j++)
	{
		for (k=0;k<4;k++)
		{
			if (j==k)
				p_matrix[j][k]=1;
			else
				p_matrix[j][k]=0;
		}
	}
}



/**********************************************************
 *
 * SUBROUTINE MatrCopy_1x4(matrix_1x4_type p_dest, matrix_1x4_type p_source)
 *
 * To copy two 1x4 matrix.
 *
 * Input parameters: p_dest = destination matrix
 *					 p_source = source matrix
 * 
 *********************************************************/

void MatrCopy_1x4 (matrix_1x4_type p_dest, matrix_1x4_type p_source)
{
   int i;

   for(i=0;i<4;i++)
   {
        p_dest[i]=p_source[i];
   }
}



/**********************************************************
 *
 * SUBROUTINE MatrCopy_4x4(matrix_4x4_type p_dest, matrix_4x4_type p_source)
 *
 * To copy two 4x4 matrix.
 *
 * Input parameters: p_dest = destination matrix
 *					 p_source = source matrix
 * 
 *********************************************************/

void MatrCopy_4x4 (matrix_4x4_type p_dest, matrix_4x4_type p_source)
{
   int j,k;
   for(j=0;j<4;j++)
      for(k=0;k<4;k++)
        p_dest[j][k]=p_source[j][k];
}



/**********************************************************
 *
 * SUBROUTINE MatrCopy_3x3_trsp(matrix_4x4_type p_dest, matrix_4x4_type p_source)
 *
 * This subroutine calculates the transpose of the top-left 3x3 source matrix and stores the result in the destination matrix. 
 * This kind of operation, applied to ortonormal matrices (like the rotation matrices), generates exactly the inverse of the source matrix.
 *  
 * Input parameters: p_dest = destination matrix
 *					 p_source = source matrix
 * 
 *********************************************************/

void MatrCopy_3x3_trsp (matrix_4x4_type p_dest, matrix_4x4_type p_source)
{
   int j,k;

   for(j=0;j<3;j++)
      for(k=0;k<3;k++)
        p_dest[j][k]=p_source[k][j];
}



/**********************************************************
 *
 * SUBROUTINE MatrMul_1x4_4x4 (matrix_1x4_type p_matrix1, matrix_4x4_type p_matrix2, matrix_1x4_type p_matrix_res)
 *
 * This subroutine multiply a vector by a 4x4 matrix to generate a new vector
 *
 * Input parameters: p_matrix1 = Vector1
 *					 p_matrix2 = Matrix
 *					 p_matrix_res = New Vector = Vector1 x matrix
 * 
 *********************************************************/

void MatrMul_1x4_4x4 (matrix_1x4_type p_matrix1, matrix_4x4_type p_matrix2, matrix_1x4_type p_matrix_res)
{
    int j,k;
    float l_sum;

    for (j=0;j<4;j++)
    {
        l_sum=0;
        for(k=0;k<4;k++)
            l_sum+=p_matrix1[k]*p_matrix2[k][j];
        p_matrix_res[j]=l_sum;
    }
}



/**********************************************************
 *
 * SUBROUTINE MatrMul_4x4_4x4 (matrix_4x4_type p_matrix1, matrix_4x4_type p_matrix2, matrix_4x4_type p_matrix_res)
 *
 * This subroutine multiply a 4x4 matrix by a 4x4 matrix to generate a new 4x4 matrix
 *
 * Input parameters: p_matrix1 = matrix1
 *					 p_matrix2 = matrix2
 *					 p_matrix_res = New Vector = matrix1 x matrix2 
 * 
 *********************************************************/

void MatrMul_4x4_4x4 (matrix_4x4_type p_matrix1, matrix_4x4_type p_matrix2, matrix_4x4_type p_matrix_res)
{
    int i,j,k;
    float l_sum;

    for (i=0;i<4;i++)
    {
        for (j=0;j<4;j++)
        {
            l_sum=0;
            for(k=0;k<4;k++)
                l_sum+=p_matrix1[i][k]*p_matrix2[k][j];
            p_matrix_res[i][j]=l_sum;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//added by Mobeen
#define _USE_MATH_DEFINES
#include <math.h>
float Dot(float* v1, float* v2) {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}
void MatrCopy_4x4_3x3(matrix_4x4_type a, matrix_3x3_type b) {
	for(int j=0;j<3;j++)
		for(int i=0;i<3;i++)
			b[j][i]=a[j][i];
}
//based on info. given here http://gpwiki.org/index.php/MathGem:Fast_Matrix_Inversion
void Fast_Inverse(matrix_4x4_type matrix, matrix_4x4_type inverse) {
	MatrIdentity_4x4(inverse);
	MatrCopy_3x3_trsp(inverse,matrix);
	float Rx[3]={matrix[0][0],matrix[1][0],matrix[2][0]};
	float Ry[3]={matrix[0][1],matrix[1][1],matrix[2][1]};
	float Rz[3]={matrix[0][2],matrix[1][2],matrix[2][2]};
	float T[3]= {matrix[3][0], matrix[3][1],matrix[3][2]};
	inverse[3][0] = -Dot(T,Rx);
	inverse[3][1] = -Dot(T,Ry);
	inverse[3][2] = -Dot(T,Rz);
}

void Perspective(float fov, float asp, float zMin, float zMax, matrix_4x4_type matrix) {
	
	MatrIdentity_4x4(matrix);  
	float yMax = zMin * tanf(fov*float(M_PI) / 360.0f);
    float yMin = -yMax;
	float xMin = yMin * asp;
    float xMax = -xMin; 
    
	matrix[0][0]= (2.0f * zMin) / (xMax - xMin);
	matrix[1][1] = (2.0f * zMin) / (yMax - yMin);
	matrix[0][2] = (xMax + xMin) / (xMax - xMin);
	matrix[1][2] = (yMax + yMin) / (yMax - yMin);
	matrix[2][2] = -((zMax + zMin) / (zMax - zMin));
	matrix[2][3] = -1.0f;
	matrix[3][2] = -((2.0f * (zMax*zMin))/(zMax - zMin));
	matrix[3][3] = 0.0f;
}

void Orthographic(float left, float right, float bottom, float top, float nearV, float farV, matrix_4x4_type matrix) {
	MatrIdentity_4x4(matrix);
	matrix[0][0] = 2.0f/(right-left);
	matrix[1][1] = 2.0f/(top-bottom);
	matrix[2][2] = -2.0f/(farV-nearV);
	matrix[3][0] = - (right+left)/(right-left);
	matrix[3][1] = - (top+bottom)/(top-bottom);
	matrix[3][2] = - (farV+nearV)/(farV-nearV);	
}

//Transforms a given vector with the given matrix and stores the result in the result
//Note that opengl matrix are column major so we swap the indices
void MatrMul_Vec( float* result, matrix_4x4_type matrix, float* vector) {
	
	for(int j=0;j<4;j++)
	{
		float sum=0;
		for(int i=0;i<4;i++)
		{
			sum +=(matrix[i][j]*vector[i]);
		}
		result[j]= sum;
	}
}