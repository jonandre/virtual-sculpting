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
 * Math library for matrices management
 *  
 * File header
 *  
 */

#ifndef _MAT_MATR_H
#define _MAT_MATR_H



/**********************************************************
 *
 * TYPES DECLARATION
 *
 *********************************************************/

typedef float matrix_1x4_type [4];
typedef float matrix_4x4_type [4][4];
typedef float matrix_3x3_type [3][3];



/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/

extern float matr_sin_table[3600], matr_cos_table[3600];



/**********************************************************
 *
 * FUNCTIONS DECLARATION
 *
 *********************************************************/

extern void MatrGenerateLookupTab (void);
extern void MatrIdentity_4x4 (matrix_4x4_type p_matrix);
extern void MatrCopy_1x4 (matrix_1x4_type p_dest, matrix_1x4_type p_source);
extern void MatrCopy_4x4 (matrix_4x4_type p_dest, matrix_4x4_type p_source);
extern void MatrCopy_3x3_trsp (matrix_4x4_type p_dest, matrix_4x4_type p_source);
extern void MatrMul_1x4_4x4 (matrix_1x4_type p_matrix1, matrix_4x4_type p_matrix2, matrix_1x4_type p_matrix_res);
extern void MatrMul_4x4_4x4 (matrix_4x4_type p_matrix1, matrix_4x4_type p_matrix2, matrix_4x4_type p_matrix_res);
/////////////////////////////////////////////////////////////////////////////////////
//added by Mobeen
float Dot(float* v1, float* v2);
void MatrCopy_4x4_3x3(matrix_4x4_type a, matrix_3x3_type b);
void Fast_Inverse(matrix_4x4_type matrix, matrix_4x4_type inverse);
void Perspective(float fov, float asp, float zMin, float zMax, matrix_4x4_type matrix);
void Orthographic(float left, float right, float bottom, float top, float nearV, float farV, matrix_4x4_type matrix);
void MatrMul_Vec( float* result, matrix_4x4_type matrix, float* vector);
#endif