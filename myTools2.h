// myTools2.h
#ifndef MYTOOLS2_H
#define MYTOOLS2_H

void matrixIdentity4x4(float m[16]);
void matrixMultiply4x4(float m2[16], float m1[16], float m[16]);
void matrixTranslate4x4(float x, float y, float z, float m[16]);
void matrixRotate4x4(float angle, float x, float y, float z, float m[16]);
void matrixScale4x4(float x, float y, float z, float m[16]);
void lookAt(float ex, float ey, float ez, float cx, float cy, float cz, float ux , float uy, float uz, float m[16]);
void ortho(float l, float r, float b, float t, float n, float f, float m[16]);
void frustum(float l, float r, float b, float t, float n, float f, float m[16]);
void quaternionCopy(float q[4], float r[4]);
float quaternionNorm(float q[4]);
void quaternionNormalize(float q[4]);
void quaternionAdd(float q[4], float p[4], float r[4]);
void quaternionMultiply(float q[4], float p[4], float r[4]);
void quaternionConjugate(float q[4], float r[4]);
void quaternionRotation(float angle, float x, float y,float z, float q[4]);
void quaternionRotationX(float angle, float q[4]);
void quaternionRotationY(float angle, float q[4]);
void quaternionRotationZ(float angle, float q[4]);
void quaternion2Matrix4x4(float q[4], float m[16]);
void matrix2Quaternion4x4(float m[16], float q[4]);
#endif
