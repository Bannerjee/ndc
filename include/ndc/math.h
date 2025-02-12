#pragma once

typedef struct 
{
    float x;
    float y;
    float z;
    float w;
} ndc_vec4;
typedef struct
{
    float x;
    float y;
} ndc_vec2;
typedef struct
{
    uint8_t cols;
    uint8_t rows;
    float* data;
} ndc_matrix;
typedef struct
{
    float x;
    float y;
    float z;
} ndc_vec3;


#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDC_ROW_MAJOR

#define NDC_SET_MATRIX(m,c,r,v) (m.data[r * m.cols + c] = v)
#define NDC_GET_MATRIX(m,c,r) (m.data[r * m.cols + c])

#else

#define NDC_SET_MATRIX(m,c,r,v) (m.data[c * m.rows + r] = v)
#define NDC_GET_MATRIX(m,c,r) (m.data[c * m.rows + r])

#endif

float ndc_to_radians(float degrees);

// any matrix
ndc_matrix ndc_identity_matrix_4x4();
void ndc_copy_matrix(ndc_matrix* dst,const ndc_matrix* src);
ndc_matrix ndc_create_matrix(uint8_t rows,uint8_t cols,float v);
void ndc_free_matrix(ndc_matrix* m);

// 4x4 matrix
void ndc_translate( ndc_matrix* m,const ndc_vec3* v);
void ndc_mul_m4x4_m4x4( ndc_matrix* m1,const ndc_matrix* m2);
ndc_matrix ndc_look_at(const ndc_vec3* eye,const ndc_vec3* center,const ndc_vec3* up);
ndc_matrix ndc_perspective_4x4(float fov,float aspect,float znear,float zfar);

// misc vec3
float ndc_length_v3(const ndc_vec3* v);
void ndc_normalize_v3(ndc_vec3* v);
ndc_vec3 ndc_cross_v3(const ndc_vec3* v1,const ndc_vec3* v2);
float ndc_dot_v3(const ndc_vec3* v1,const ndc_vec3* v2);

// arithmetic vec3 scalar
void ndc_mul_v3_s(ndc_vec3* vec,float val);
void ndc_div_v3_s(ndc_vec3* vec,float val);
void ndc_add_v3_s(ndc_vec3* vec,float val);
void ndc_sub_v3_s(ndc_vec3* vec,float val);

// arithmetic vec3 vec3
void ndc_mul_v3_v3(ndc_vec3* v1,const ndc_vec3* v2);
void ndc_div_v3_v3(ndc_vec3* v1,const ndc_vec3* v2);
void ndc_add_v3_v3(ndc_vec3* v1,const ndc_vec3* v2);
void ndc_sub_v3_v3(ndc_vec3* v1,const ndc_vec3* v2);

#ifdef __cplusplus
}
#endif