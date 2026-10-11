struct Vec3 {
    float x, y, z;
    Vec3& operator=(const Vec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
};
struct Plane { Vec3 n; float d; };

void PlaneFromNormalPoint(Plane* out, const Vec3* n, const Vec3* p) {
    float d = n->x * p->x + n->y * p->y + n->z * p->z;
    out->n = *n;
    out->d = -d;
}

extern void Lock(int);
extern void Unlock(int);
extern int Inner(int, int, int, float);

int Wrapped(int a, int b, int c, float d) {
    int r;
    Lock(9);
    r = Inner(a, b, c, d);
    Unlock(9);
    return r;
}
