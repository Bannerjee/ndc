#include <ndc/allocator.h>
#include <ndc/math.h>
#include <ndc/error.h>

#include <math.h>
#include <string.h>


// todo : change this later
void ndc_mul_m4x4_m4x4(ndc_matrix* m1,const ndc_matrix* m2)
{
    NDC_ASSERT(m1->rows == m2->rows && m1->cols == m2->cols && m1->rows == 4 && m1->cols == 4);
    for (uint32_t i = 0; i < 16; ++i) 
    {
        uint32_t row = i / 4;
        uint32_t column = i % 4;
        uint32_t row_offset = row * 4;

        m1->data[row_offset + column] = 
        (m1->data[row_offset + 0] * m2->data[column + 0]) + 
        (m1->data[row_offset + 1] * m2->data[column + 4]) +
        (m1->data[row_offset + 2] * m2->data[column + 8]) +
        (m1->data[row_offset + 3] * m2->data[column + 12]); 
    }

}


float ndc_to_radians(float degrees)
{
    return degrees * M_PI / 180;
}
ndc_matrix ndc_identity_matrix_4x4()
{
    ndc_matrix mat = ndc_create_matrix(4,4,0);
    NDC_SET_MATRIX(mat,0,0,1);
    NDC_SET_MATRIX(mat,1,1,1);
    NDC_SET_MATRIX(mat,2,2,1);
    NDC_SET_MATRIX(mat,3,3,1);
    return mat;
}
ndc_matrix ndc_create_matrix(uint8_t rows,uint8_t cols,float v)
{
    ndc_matrix mat = {cols,rows,ndc_malloc(rows*cols * sizeof(float))};
    int i = -1;
    while(++i < (rows*cols)) mat.data[i] = v;
    return mat;
}

void ndc_free_matrix(ndc_matrix* m)
{
    ndc_free(m->data);
    m->data = NULL;
}

ndc_matrix ndc_perspective_4x4(float fov,float aspect,float znear,float zfar)
{
    float half_tan = tan(fov / 2.f);

    ndc_matrix m = ndc_create_matrix(4,4,0);

    m.data[0] = 1 / (aspect * half_tan);
    m.data[5] = 1.f / half_tan;
    m.data[10] = zfar / (znear - zfar);
    m.data[11] = 1.f;
    m.data[14] = -(zfar * znear) / (zfar - znear);

    return m;
}
float ndc_length_v3(const ndc_vec3* v)
{
    NDC_ASSERT(v);
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}
void ndc_mul_v3_s(ndc_vec3* vec,float val)
{
    NDC_ASSERT(vec);
    vec->x *= val;
    vec->y *= val;
    vec->z *= val;
}
void ndc_div_v3_s(ndc_vec3* vec,float val)
{
    NDC_ASSERT(vec);
    vec->x /= val;
    vec->y /= val;
    vec->z /= val;
}
void ndc_add_v3_s(ndc_vec3* vec,float val)
{
    NDC_ASSERT(vec);
    vec->x += val;
    vec->y += val;
    vec->z += val; 
}
void ndc_sub_v3_s(ndc_vec3* vec,float val)
{
    NDC_ASSERT(vec);
    vec->x -= val;
    vec->y -= val;
    vec->z -= val; 
}
void ndc_mul_v3_v3(ndc_vec3* v1,const ndc_vec3* v2)
{
    NDC_ASSERT(v1 && v2);
    v1->x *= v2->x;
    v1->y *= v2->y;
    v1->z *= v2->z;
}
void ndc_div_v3_v3(ndc_vec3* v1,const ndc_vec3* v2)
{
    NDC_ASSERT(v1 && v2);
    v1->x /= v2->x;
    v1->y /= v2->y;
    v1->z /= v2->z;
}
void ndc_add_v3_v3(ndc_vec3* v1,const ndc_vec3* v2)
{
    NDC_ASSERT(v1 && v2);
    v1->x += v2->x;
    v1->y += v2->y;
    v1->z += v2->z; 
}
void ndc_sub_v3_v3(ndc_vec3* v1,const ndc_vec3* v2)
{
    NDC_ASSERT(v1 && v2);
    v1->x -= v2->x;
    v1->y -= v2->y;
    v1->z -= v2->z; 
}
void ndc_normalize_v3(ndc_vec3* v)
{
    NDC_ASSERT(v);
    ndc_div_v3_s(v,ndc_length_v3(v));
}
ndc_vec3 ndc_cross_v3(const ndc_vec3* v1,const ndc_vec3* v2)
{
    NDC_ASSERT(v1 && v2);
    ndc_vec3 crs = {-1,-1,-1};
    crs.x = v1->y * v2->z - v1->z * v2->y;
    crs.y = v1->z * v2->x - v1->x * v2->z;
    crs.z = v1->x * v2->y - v1->y * v2->x;

    return crs;
}
float ndc_dot_v3(const ndc_vec3* v1,const ndc_vec3* v2)
{
    NDC_ASSERT(v1 && v2);
    return v1->x * v2->x + v1->y * v2->y + v1->z + v2->z;
}
ndc_matrix ndc_look_at(const ndc_vec3* eye,const ndc_vec3* center,const ndc_vec3* up)
{
    NDC_ASSERT(eye && center && up);
    ndc_matrix m =  ndc_create_matrix(4,4,-1);

    ndc_vec3 f = *center;
    ndc_vec3 s;
    ndc_vec3 u;
    
    ndc_sub_v3_v3(&f,eye);
    ndc_normalize_v3(&f);
    s = ndc_cross_v3(&f,up);
    ndc_normalize_v3(&s);
    u = ndc_cross_v3(&s,&f);

    
    NDC_SET_MATRIX(m,0,0,s.x);
    NDC_SET_MATRIX(m,1,0,s.y);
    NDC_SET_MATRIX(m,2,0,s.z);

    NDC_SET_MATRIX(m,0,1,u.x);
    NDC_SET_MATRIX(m,1,1,u.y);
    NDC_SET_MATRIX(m,2,1,u.z);

    NDC_SET_MATRIX(m,0,2,-f.x);
    NDC_SET_MATRIX(m,1,2,-f.y);
    NDC_SET_MATRIX(m,2,2,-f.z);

    NDC_SET_MATRIX(m,3,0,ndc_dot_v3(&s,eye));
    NDC_SET_MATRIX(m,3,1,ndc_dot_v3(&u,eye));
    NDC_SET_MATRIX(m,3,2,ndc_dot_v3(&f,eye));

    return m;
}
void ndc_copy_matrix(ndc_matrix* dst,const ndc_matrix* src)
{
    NDC_ASSERT(dst && src);
    NDC_ASSERT(dst->rows * dst->cols == src->rows * src->cols);
    memcpy(dst->data,src->data,dst->rows * dst->cols);
}
void ndc_translate(ndc_matrix* m,const ndc_vec3* v)
{
    NDC_ASSERT(m && v);
    ndc_matrix id = ndc_identity_matrix_4x4(4,4);
    NDC_SET_MATRIX(id,3,0,v->x);
    NDC_SET_MATRIX(id,3,1,v->y);
    NDC_SET_MATRIX(id,3,2,v->z);

    ndc_mul_m4x4_m4x4(m,&id);
    ndc_free_matrix(&id);
}
