struct Vec3 { float x, y, z; };
struct Plane { float x, y, z, d; };
void E(float* out, const float* n, const float* p) {
    float d = n[0] * p[0] + n[1] * p[1] + n[2] * p[2];
    out[0] = n[0];
    out[1] = n[1];
    out[2] = n[2];
    out[3] = -d;
}
void F(Plane* out, const Vec3* n, const Vec3* p) {
    float d = n->x * p->x + n->y * p->y + n->z * p->z;
    out->x = n->x;
    out->y = n->y;
    out->z = n->z;
    out->d = -d;
}
