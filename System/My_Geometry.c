#include "My_Geometry.h"
#include <math.h>
#include <stdint.h>
Vec4 vec4(double x, double y, double z, double w) {
    Vec4 v = {x, y, z, w};
    return v;
}

Vec4 vec_add(Vec4 a, Vec4 b) {
    return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

double dot(Vec4 a, Vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Vec4 mat_mul_vec(Mat4 M, Vec4 v) {
    Vec4 r;
    r.x = M.m[0] * v.x + M.m[4] * v.y + M.m[8] * v.z + M.m[12] * v.w;
    r.y = M.m[1] * v.x + M.m[5] * v.y + M.m[9] * v.z + M.m[13] * v.w;
    r.z = M.m[2] * v.x + M.m[6] * v.y + M.m[10] * v.z + M.m[14] * v.w;
    r.w = M.m[3] * v.x + M.m[7] * v.y + M.m[11] * v.z + M.m[15] * v.w;
    return r;
}

Mat4 mat_mul(Mat4 A, Mat4 B) {
    Mat4 C = {0};
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            double sum = 0.0;
            for (int k = 0; k < 4; k++) {
                sum += A.m[k * 4 + row] * B.m[col * 4 + k];
            }
            C.m[col * 4 + row] = sum;
        }
    }
    return C;
}

Mat4 mat_identity(void) {
    Mat4 I = {0};
    for (int i = 0; i < 4; i++) {
        I.m[i * 5] = 1.0;
    }
    return I;
}

Mat4 translate(double tx, double ty, double tz) {
    Mat4 T = mat_identity();
    T.m[12] = tx;
    T.m[13] = ty;
    T.m[14] = tz;
    return T;
}

Vec4 RotateY(Vec4 p, float angle)
{
    Vec4 res;
    float c = cos(angle);
    float s = sin(angle);
    res.x = p.x * c + p.z * s;
    res.y = p.y;
    res.z = -p.x * s + p.z * c;
    res.w = 1.0f;
    return res;
}

Vec4 RotateX(Vec4 p, double angle)
{
    Vec4 res;
    float c = cos(angle);
    float s = sin(angle);
    res.x = p.x;
    res.y = p.y * c - p.z * s;
    res.z = p.y * s + p.z * c;
    res.w = p.w;
    return res;
}


Vec4 perspective(Vec4 v, double fovy, double aspect, double znear, double zfar)
{
    Vec4 res;


    float x = v.x / v.z;
    float y = v.y / v.z;


    float z = v.z;
    float new_z = ((zfar + znear) / (zfar - znear)) * z + (-2.0*zfar*znear)/(zfar - znear);
    float new_w = z;  // w = z

    res.x = x;
    res.y = y;
    res.z = new_z;
    res.w = new_w;

    return res;
}


Vec4 viewport(Vec4 v, uint16_t w, uint16_t h)
{
    Vec4 out;
    out.x = (v.x + 1.0f) * w / 2.0f;
    out.y = (1.0f - v.y) * h / 2.0f;
    out.z = v.z;
    out.w = v.w;
    return out;
}