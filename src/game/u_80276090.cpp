// u_80276090.cpp (0x80276090-0x802766F0): ODE mass functions (ODE's mass.cpp: 4/3*pi, 1/12, 0.4, 0.375
// constants). Split from u_8026C200. Evidence: own .sdata2 pool 0x804DA298-0x804DA2C0 (0.0f again).
// ODE 0.5 mass.cpp in reverse source order (dMassSetBox .. dMassSetZero, then checkMass); dMassSetCylinder,
// dMassAdjust/Translate/Rotate/Add are linker-stripped.

typedef float dReal;
typedef dReal dVector4[4];
typedef dReal dMatrix3[4 * 3];

#define REAL(x) (x##f)
#define M_PI REAL(3.1415926535897932384626433832795029)
#define _I(i, j) I[(i) * 4 + (j)]

#define dDOTpq(a, b, p, q) ((a)[0] * (b)[0] + (a)[p] * (b)[q] + (a)[2 * (p)] * (b)[2 * (q)])
#define dDOT14(a, b) dDOTpq(a, b, 1, 4)
#define dMULTIPLYOP0_333(A, op, B, C)   \
    (A)[0] op dDOT14((B), (C));         \
    (A)[1] op dDOT14((B), (C + 1));     \
    (A)[2] op dDOT14((B), (C + 2));     \
    (A)[4] op dDOT14((B + 4), (C));     \
    (A)[5] op dDOT14((B + 4), (C + 1)); \
    (A)[6] op dDOT14((B + 4), (C + 2)); \
    (A)[8] op dDOT14((B + 8), (C));     \
    (A)[9] op dDOT14((B + 8), (C + 1)); \
    (A)[10] op dDOT14((B + 8), (C + 2));
#define dMULTIPLY0_333(A, B, C) dMULTIPLYOP0_333(A, =, B, C)
#define dCROSSMAT(A, a, skip, plus, minus)     \
    do {                                       \
        (A)[1] = minus(a)[2];                  \
        (A)[2] = plus(a)[1];                   \
        (A)[(skip) + 0] = plus(a)[2];          \
        (A)[(skip) + 2] = minus(a)[0];         \
        (A)[2 * (skip) + 0] = minus(a)[1];     \
        (A)[2 * (skip) + 1] = plus(a)[0];      \
    } while (0)

struct dMass {
    dReal mass;
    dVector4 c;
    dMatrix3 I;
};

void dSetZero(dReal* a, int n);
int dIsPositiveDefinite(const dReal* A, int n);

static int checkMass(dMass* m);

static inline void massSetZero(dMass* m) {
    m->mass = REAL(0.0);
    dSetZero(m->c, sizeof(m->c) / sizeof(dReal));
    dSetZero(m->I, sizeof(m->I) / sizeof(dReal));
}

// dMassSetBoxTotal (public in ODE, inlined here and linker-stripped)
static inline void dMassSetBoxTotal(dMass* m, dReal total_mass, dReal lx, dReal ly, dReal lz) {
    massSetZero(m);
    m->mass = total_mass;
    m->_I(0, 0) = total_mass / REAL(12.0) * (ly * ly + lz * lz);
    m->_I(1, 1) = total_mass / REAL(12.0) * (lx * lx + lz * lz);
    m->_I(2, 2) = total_mass / REAL(12.0) * (lx * lx + ly * ly);
}

// dMassSetBox
extern "C" void fn_80276090(dMass* m, dReal density, dReal lx, dReal ly, dReal lz) {
    dMassSetBoxTotal(m, lx * ly * lz * density, lx, ly, lz);
}

// dMassSetCylinderTotal and dMassSetCylinder: not referenced in Geist (linker-stripped, as in Memory.cpp), but
// their constants (0.25, 1/12, 0.5, pi) sit in this file's .sdata2 pool between dMassSetBox's and
// dMassSetCappedCylinder's, where ODE's source order puts them.
void dMassSetCylinderTotal(dMass* m, dReal total_mass, int direction, dReal r, dReal l) {
    dReal r2, I;
    massSetZero(m);
    r2 = r * r;
    m->mass = total_mass;
    I = total_mass * (REAL(0.25) * r2 + (REAL(1.0) / REAL(12.0)) * l * l);
    m->_I(0, 0) = I;
    m->_I(1, 1) = I;
    m->_I(2, 2) = I;
    m->_I(direction - 1, direction - 1) = total_mass * REAL(0.5) * r2;
}

void dMassSetCylinder(dMass* m, dReal density, int direction, dReal r, dReal l) {
    dMassSetCylinderTotal(m, M_PI * r * r * l * density, direction, r, l);
}

// dMassSetCappedCylinder
extern "C" void fn_80276170(dMass* m, dReal density, int direction, dReal a, dReal b) {
    dReal M1, M2, Ia, Ib;
    massSetZero(m);
    M1 = M_PI * a * a * b * density;                        // cylinder mass
    M2 = (REAL(4.0) / REAL(3.0)) * M_PI * a * a * a * density;  // total cap mass
    m->mass = M1 + M2;
    Ia = M1 * (REAL(0.25) * a * a + (REAL(1.0) / REAL(12.0)) * b * b) +
         M2 * (REAL(0.4) * a * a + REAL(0.375) * a * b + REAL(0.25) * b * b);
    Ib = (M1 * REAL(0.5) + M2 * REAL(0.4)) * a * a;
    m->_I(0, 0) = Ia;
    m->_I(1, 1) = Ia;
    m->_I(2, 2) = Ia;
    m->_I(direction - 1, direction - 1) = Ib;
}

// dMassSetSphereTotal (public in ODE, inlined here and linker-stripped)
static inline void dMassSetSphereTotal(dMass* m, dReal total_mass, dReal radius) {
    massSetZero(m);
    m->mass = total_mass;
    dReal II = REAL(0.4) * total_mass * radius * radius;
    m->_I(0, 0) = II;
    m->_I(1, 1) = II;
    m->_I(2, 2) = II;
}

// dMassSetSphere
extern "C" void fn_802762AC(dMass* m, dReal density, dReal radius) {
    dMassSetSphereTotal(m, (REAL(4.0) / REAL(3.0)) * M_PI * radius * radius * radius * density, radius);
}

// dMassSetParameters
extern "C" void fn_80276350(dMass* m, dReal themass, dReal cgx, dReal cgy, dReal cgz, dReal I11, dReal I22,
                            dReal I33, dReal I12, dReal I13, dReal I23) {
    massSetZero(m);
    m->mass = themass;
    m->c[0] = cgx;
    m->c[1] = cgy;
    m->c[2] = cgz;
    m->_I(0, 0) = I11;
    m->_I(1, 1) = I22;
    m->_I(2, 2) = I33;
    m->_I(0, 1) = I12;
    m->_I(0, 2) = I13;
    m->_I(1, 2) = I23;
    m->_I(1, 0) = I12;
    m->_I(2, 0) = I13;
    m->_I(2, 1) = I23;
    checkMass(m);
}

// dMassSetZero
extern "C" void fn_8027649C(dMass* m) {
    massSetZero(m);
}

// checkMass: return 1 if ok, 0 if bad
static int checkMass(dMass* m) {
    if (m->mass <= 0) {
        return 0;
    }
    if (!dIsPositiveDefinite(m->I, 3)) {
        return 0;
    }

    // verify that the center of mass position is consistent with the mass and inertia matrix
    dMatrix3 I2, chat;
    dSetZero(chat, 12);
    dCROSSMAT(chat, m->c, 4, +, -);
    dMULTIPLY0_333(I2, chat, chat);
    int i;
    for (i = 0; i < 3; i++) I2[i] = m->I[i] + m->mass * I2[i];
    for (i = 4; i < 7; i++) I2[i] = m->I[i] + m->mass * I2[i];
    for (i = 8; i < 11; i++) I2[i] = m->I[i] + m->mass * I2[i];
    if (!dIsPositiveDefinite(I2, 3)) {
        return 0;
    }
    return 1;
}
