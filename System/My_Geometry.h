#ifndef __MY_GEOMETRY_H
#define __MY_GEOMETRY_H
#include "math.h"
#include "stdint.h"
typedef struct{double x,y,z,w;}Vec4;
typedef struct{double m[16];}Mat4;

Vec4 vec4(double x,double y,double z,double w);
Vec4 vec_add(Vec4 a,Vec4 b);
double dot(Vec4 a, Vec4 b);
Vec4 mat_mul_vec(Mat4 M,Vec4 v);
Mat4 mat_mul(Mat4 A, Mat4 B);
Mat4 mat_identity(void);
Mat4 translate(double tx, double ty, double tz);
Vec4 perspective(Vec4 v, double fovy, double aspect, double znear, double zfar);
Vec4 viewport(Vec4 v, uint16_t w, uint16_t h);
Vec4 RotateY(Vec4 p, float angle);
Vec4 RotateX(Vec4 p, double angle);


#endif