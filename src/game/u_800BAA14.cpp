// u_800BAA14.cpp: navigation graph / path-following helpers (0x800BAA14-0x800C5028).
// An actor follows a route through a node graph (graph handles at 0x804D2B98/0x804D2B9C, route tables at
// 0x804499D0). Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;
typedef unsigned char u8;

struct Vec3 {
    float x, y, z;
};

struct IVec3 {
    int x, y, z;
};

struct Actor;

// Graph search types (u_800F14A8.cpp).
struct SearchState {
    int depth;
    int node;
    float cost;
    float estimate;
    int linkIndex;
    SearchState* parent; // 0x14
    void* link;
    SearchState* prev;
    SearchState* next;
};

struct GraphQuery {
    int path[16];     // 0x00 node ids, -1 terminated
    int pathArgs[16]; // 0x40
    float result;     // 0x80
    int mode;         // 0x84
    int endNode;      // 0x88
    SearchState* open;
    SearchState* closed;
};

typedef float (*SearchCostFn)(int from, int to, int arg, void* ctx, SearchState* state);
typedef int (*SearchGoalFn)(int node, int goal, SearchState* state);

// A waypoint either holds a position or the actor that marks it.
union WpPos {
    Vec3 v;
    Actor* target;
};

// One step of the actor's route.
struct Waypoint {
    WpPos at;     // 0x00
    int kind;     // 0x0c
    int link[3];  // 0x10
    int unk1c;    // 0x1c
};

// Actor placement lives at 0x24; the navigation state sits in the tail of the actor record.
struct Actor {
    u8 unk0[0x24];          // 0x00
    IVec3 pos;              // 0x24
    u8 unk30[0x2c0 - 0x30]; // 0x30
    int navMode;            // 0x2c0 (0 = route table, 1 = graph)
    u8 unk2c4[4];           // 0x2c4
    int navLinkA;           // 0x2c8
    int navLinkB;           // 0x2cc
    int navRoute;           // 0x2d0
    u8 unk2d4[8];           // 0x2d4
    Waypoint wp[15];        // 0x2dc
    u8 unk4bc[0x4c8 - 0x4bc]; // 0x4bc
    int navPhase;           // 0x4c8
    u8 unk4cc[0x4fc - 0x4cc]; // 0x4cc
    GraphQuery query;       // 0x4fc
    u8 unk594[4];           // 0x594
    int unk598;             // 0x598
};

// Global tuning block for the navigation code (0x34 bytes).
struct NavTuning {
    int none0;       // 0x00
    u8 unk4[8];      // 0x04
    int none0c;      // 0x0c
    u8 unk10[8];     // 0x10
    int range18;     // 0x18
    int range1c;     // 0x1c
    int range20;     // 0x20
    int range24;     // 0x24
    int range28;     // 0x28
    int range2c;     // 0x2c
    u8 unk30[4];     // 0x30
};

// One route record in the route tables.
struct RouteLeg {
    u8 unk0[0x1c]; // 0x00
    Vec3* points;  // 0x1c
    u8 unk20[0x10];
};

struct RouteBounds {
    int min[3]; // 0x00
    int max[3]; // 0x0c
};

struct RouteInfo {
    u8 unk0[0x38];     // 0x00
    u32 id;            // 0x38
    u8 unk3c[0x14];    // 0x3c
    RouteBounds bounds; // 0x50
    u8 unk68[0xc];     // 0x68
    int unk74;         // 0x74
    u8 unk78[8];       // 0x78
    RouteLeg* legs;    // 0x80
    int unk84;         // 0x84
    u32 flags;         // 0x88
    u8 unk8c[0xc];     // 0x8c
};

// Message block passed to the route/item message handlers.
struct RouteMsg {
    RouteInfo* route; // 0x00
    int tag;          // 0x04
    int unk8;         // 0x08
    int value;        // 0x0c
};

struct RouteTable {
    int unk0;
    int count; // 0x04
};

extern "C" void* memset(void* dst, int val, u32 n);

extern NavTuning lbl_8039B728; // nav tuning block, also read by u_800B1834 as a u32 table // 0x8039B728
extern u8 lbl_804D2B90[8];      // 0x804D2B90
extern int gNavGraphB;       // 0x804D2B98
extern int gNavGraph;        // 0x804D2B9C
extern RouteTable* gRoutes;  // 0x804D4848
extern RouteInfo* gRouteList[]; // 0x804499D0
extern u32 gRouteBlocks[][2];   // 0x80449BD0 (0x80 pairs: id, heap block)
extern u32 gRouteItems[];       // 0x80449FD0
extern RouteTable* gItems;      // 0x804D484C

void VFree(void* p);
void GraphDestroy(u32 graph);

u32 GraphGetNodeData(u32 graph, int node);
int GraphFindBestLink(u32 graph, SearchCostFn cost, void* ctx, int* a, int* b);
u32 GraphCreate(void);
int GraphAllocNodes(u32 graph, int count);
int GraphSetNode(u32 graph, int index, u32 data, int flag);
int GraphAddLink(u32 graph, int from, int to, int arg);
float GraphSearchFrom(u32 graph, int mode, int start, void* ctx, SearchCostFn cost, SearchGoalFn isGoal,
                      int goalArg, GraphQuery* query);
float GraphSearchTo(u32 graph, int mode, int start, int goal, void* ctx, SearchCostFn cost,
                    SearchCostFn estimate, GraphQuery* query);
void* VAlloc(int size);
extern "C" int SendMsg(int to, int msg, void* data);
extern "C" int fn_800B9388(u32 id, int skipUnused);
extern "C" int fn_800B949C(u32 id, u32* out, int max, int includeUnused);

float NavLinkCost(int from, int to, int arg, void* ctx, SearchState* state); // 0x800BECC8
// TODO (not drafted yet, 0x1C4 bytes at 0x800BEB04): search cost callback, distance to a target position
float NavTargetLinkCost(int from, int to, int arg, void* ctx, SearchState* state);
// TODO (not drafted yet, 0x2C0 bytes at 0x800BC2AC): steer toward a position along the route table
extern "C" int fn_800BC2AC(Actor* actor, Vec3* target);
// TODO (not drafted yet, 0x700 bytes at 0x800BF66C): follow the graph route
void NavGraphTrack(Actor* actor);
// TODO (not drafted yet, 0x6F0 bytes at 0x800BFD6C): follow the route table
void NavRouteTrack(Actor* actor);
// TODO (not drafted yet, 0x5D8 bytes at 0x800BE52C): search cost callback for route following
float NavRouteCost(int from, int to, int arg, void* ctx, SearchState* state);


// NavInit (reset the navigation tuning block)
extern "C" void fn_800BAA14() {
    memset(&lbl_8039B728, 0, 0x34);
    memset(lbl_804D2B90, 0, 8);
    lbl_8039B728.none0c = -1;
    lbl_8039B728.none0 = -1;
    lbl_8039B728.range1c = 7500;
    lbl_8039B728.range20 = 3000;
    lbl_8039B728.range24 = 3000;
    lbl_8039B728.range28 = 200;
    lbl_8039B728.range18 = 100;
    lbl_8039B728.range2c = 100;
}

// NavFindLinkNodes
extern "C" int fn_800BAA90(Actor* actor, u32* nodeA, u32* nodeB) {
    int a, b;
    if (!actor || !nodeA || !nodeB) {
        return 0;
    }
    if (GraphFindBestLink(gNavGraph, NavLinkCost, actor, &a, &b)) {
        *nodeA = GraphGetNodeData(gNavGraph, a);
        *nodeB = GraphGetNodeData(gNavGraph, b);
        return 1;
    }
    return 0;
}

// NavSetMode: switch between route-table and graph following.
extern "C" void fn_800BBCA8(Actor* actor);
extern "C" void fn_800BAEE4(Actor* actor, int mode) {
    if (actor->navMode != mode) {
        actor->navMode = mode;
        fn_800BBCA8(actor);
        if (mode == 1) {
            actor->navLinkA = -1;
            actor->navLinkB = -1;
        }
    }
}

// NavRouteStatus: classify the actor's route index.
extern "C" int fn_800BAF40(Actor* actor) {
    int r = actor->navRoute;
    if (r == -1) {
        return -1;
    }
    if (r == -2) {
        return -2;
    }
    if (r >= 0) {
        return -3;
    }
    return -1;
}

// NavGetRoutePoint: copy one point of a route leg, or zero on failure.
extern "C" int fn_800BAF78(Vec3* out, int route, int leg, int index) {
    if (gRoutes && route >= 0 && route < gRoutes->count) {
        Vec3* p = &gRouteList[route]->legs[leg].points[index];
        out->x = p->x;
        out->y = p->y;
        out->z = p->z;
        return 1;
    }
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
    return 0;
}

extern "C" int fn_800BC01C(void* owner, Vec3* target);

// NavSteerToActor: steer toward another actor's position.
extern "C" int fn_800BBF9C(Actor* actor, Actor* target) {
    Vec3 pos;
    pos.x = target->pos.x;
    pos.y = target->pos.y;
    pos.z = target->pos.z;
    return fn_800BC01C(actor, &pos);
}

// TODO (not drafted yet, 0x884 bytes at 0x800BC5E4): per-frame route-table update
void NavRouteUpdate(Actor* actor);
// TODO (not drafted yet, 0x2F0 bytes at 0x800BDC34): per-frame graph update
void NavGraphUpdate(Actor* actor);
// TODO (not drafted yet, 0xAC8 bytes at 0x800BCE68): route-table path step
void NavRouteStep(Actor* actor);
// TODO (not drafted yet, 0x2F4 bytes at 0x800BDF24): graph path step
void NavGraphStep(Actor* actor);

extern "C" void fn_800BC56C(Actor* actor) {
    if (actor->navMode == 0) {
        NavRouteUpdate(actor);
    } else if (actor->navMode == 1) {
        NavGraphUpdate(actor);
    }
}

extern "C" void fn_800BC5A8(Actor* actor) {
    if (actor->navMode == 0) {
        NavRouteStep(actor);
    } else if (actor->navMode == 1) {
        NavGraphStep(actor);
    }
}

// NavSetRouteFlags: store a per-route value, returns it (4 on a bad index).
int NavSetRouteFlags(int route, int value) {
    if (route < 0 || route > gRoutes->count) {
        return 4;
    }
    gRouteList[route]->unk74 = value;
    return gRouteList[route]->unk74;
}

extern "C" int fn_800BEF9C(int route) {
    if (route < 0 || route > gRoutes->count) {
        return 4;
    }
    return gRouteList[route]->unk74;
}

// NavRouteBounds: pointer to a route's bounding box.
RouteBounds* NavRouteBounds(int route) {
    if (gRoutes == 0) {
        return 0;
    }
    if (route < 0 || route >= gRoutes->count) {
        return 0;
    }
    return &gRouteList[route]->bounds;
}

// NavShutdown: release route blocks and graphs.
extern "C" void fn_800BF428() {
    int i;
    for (i = 0; i < 128; i++) {
        if (gRouteBlocks[i][1] != 0) {
            VFree((void*)gRouteBlocks[i][1]);
        }
    }
    if (gNavGraphB != -1) {
        GraphDestroy(gNavGraphB);
        gNavGraphB = -1;
    }
    if (gNavGraph != -1) {
        GraphDestroy(gNavGraph);
        gNavGraph = -1;
    }
}

// NavBuildLink: link two graph nodes whose data match the given ids (building the graph on first use).
extern "C" int fn_800BADAC(int idA, int idB) {
    int count;
    int* ids;
    int i;
    int nodeA = -1;
    int nodeB = -1;
    count = fn_800B9388(0x255C25, 1);
    ids = (int*)VAlloc(count << 2);
    fn_800B949C(0x255C25, (u32*)ids, count, 1);
    if (gNavGraph == -1) {
        gNavGraph = GraphCreate();
        GraphAllocNodes(gNavGraph, count);
        for (i = 0; i < count; i++) {
            GraphSetNode(gNavGraph, i, ids[i], 1);
        }
    }
    for (i = 0; i < count; i++) {
        if (idA == ids[i]) {
            nodeA = i;
        }
        if (idB == ids[i]) {
            nodeB = i;
        }
        if (nodeA >= 0 && nodeB >= 0) {
            break;
        }
    }
    VFree(ids);
    if (nodeA < 0 || nodeB < 0) {
        return 0;
    }
    return GraphAddLink(gNavGraph, nodeA, nodeB, 0);
}

// TODO (not drafted yet, 0x167C bytes at 0x800C045C): start a route toward a position
void NavStartRoute(Actor* actor, Vec3* target);

// NavSetTarget: restart the actor's route at a position.
void NavSetTarget(Actor* actor, Vec3* target) {
    if (actor != 0 && target != 0 && gRoutes != 0) {
        Vec3 pos;
        actor->navRoute = 0;
        actor->wp[0].at.v.x = target->x;
        actor->wp[0].at.v.y = target->y;
        actor->wp[0].at.v.z = target->z;
        actor->wp[0].kind = 1;
        actor->wp[0].link[0] = -1;
        actor->wp[0].link[1] = -1;
        actor->wp[0].link[2] = -1;
        pos.x = actor->pos.x;
        pos.y = actor->pos.y;
        pos.z = actor->pos.z;
        NavStartRoute(actor, &pos);
    }
}

struct NavStopCtx {
    int (*test)(u32 a, u32 b, int arg);
    int arg;
};

// NavStopTest: goal test for graph searches, forwards the two node data words to a callback.
int NavStopTest(int node, int ctxp, SearchState* state) {
    NavStopCtx* ctx = (NavStopCtx*)ctxp;
    if (state != 0 && state->parent != 0) {
        u32 a = GraphGetNodeData(gNavGraph, node);
        u32 b = GraphGetNodeData(gNavGraph, state->parent->node);
        return ctx->test(b, a, ctx->arg);
    }
    return 0;
}

// NavTrack: advance the actor along its current route.
int NavTrack(Actor* actor) {
    if (actor == 0) {
        return 0;
    }
    if (actor->navMode == 0) {
        if (gRoutes == 0) {
            return 0;
        }
        if (actor->navRoute != -1) {
            NavRouteTrack(actor);
        }
    } else {
        if (gNavGraph == 0) {
            return 0;
        }
        NavGraphTrack(actor);
    }
    return 1;
}

// NavSteerTo: plan a route to a position; returns 1 when one was found.
extern "C" int fn_800BC01C(void* owner, Vec3* target) {
    if (((Actor*)owner)->navMode == 0) {
        return fn_800BC2AC((Actor*)owner, target);
    } else if (((Actor*)owner)->navMode == 1) {
        Actor* actor = (Actor*)owner;
        int b, a;
        if (owner == 0 || target == 0) {
            return 0;
        }
        if (actor->navLinkA == -1 && actor->navLinkB == -1 &&
            GraphFindBestLink(gNavGraph, NavLinkCost, actor, &actor->navLinkA, &actor->navLinkB) == 0) {
            return 0;
        }
        if (GraphFindBestLink(gNavGraph, NavTargetLinkCost, target, &a, &b)) {
            int to;
            int link = actor->navLinkB;
            if (a == link) {
                to = b;
            } else if (b == link) {
                to = a;
            } else if (a == actor->navLinkA) {
                to = b;
            } else {
                to = a;
            }
            if (GraphSearchTo(gNavGraph, 2, link, to, actor, NavRouteCost, 0, &actor->query) != 1e9f) {
                return 1;
            }
        }
        return 0;
    }
    return 0;
}

// NavSteerToGraph: graph-only variant of NavSteerTo.
int NavSteerToGraph(void* owner, void* targetPos) {
    Actor* actor;
    Vec3* target;
    int a, b;
    if (owner == 0 || targetPos == 0) {
        return 0;
    }
    actor = (Actor*)owner;
    target = (Vec3*)targetPos;
    if (actor->navLinkA == -1 && actor->navLinkB == -1 &&
        GraphFindBestLink(gNavGraph, NavLinkCost, actor, &actor->navLinkA, &actor->navLinkB) == 0) {
        return 0;
    }
    if (GraphFindBestLink(gNavGraph, NavTargetLinkCost, target, &a, &b) != 0) {
        int to;
        int link = actor->navLinkB;
        if (a == link) {
            to = b;
        } else if (b == link) {
            to = a;
        } else if (a == actor->navLinkA) {
            to = b;
        } else {
            to = a;
        }
        if (GraphSearchTo(gNavGraph, 2, link, to, actor, NavRouteCost, 0, &actor->query) != 1e9f) {
            return 1;
        }
    }
    return 0;
}

// NavItemMsg: message handler for the item records (0x80449FD0 table).
int NavItemMsg(int index, int msg, RouteMsg* out) {
    int result = 0;
    if (index < 0 || index > gItems->count) {
        return 0;
    }
    out->route = (RouteInfo*)gRouteItems[index];
    if (out == 0) {
        return result;
    }
    if (gItems == 0) {
        return result;
    }
    if (msg == 0xC00) {
        return result;
    }
    return result;
}

// NavItemEvent
int NavItemEvent(int msg, void* data) {
    int result = 0;
    if (data != 0) {
        if (gItems != 0) {
            switch (msg) {
            case 0xC00:
                result = 0;
                break;
            default:
                result = 0;
                break;
            }
        }
    }
    return result;
}

// NavRouteMsg: message handler for the route records; gets and sets the route flags.
int NavRouteMsg(int index, int msg, RouteMsg* out) {
    if (index < 0 || gRoutes->count < index) {
        return 0;
    }
    out->route = gRouteList[index];
    if (out == 0 || gRoutes == 0) {
        return 0;
    }
    switch (msg) {
    case 0xC00: {
        RouteInfo* route = out->route;
        if (out->tag != 0) {
            route->flags |= 1;
            GraphSetNode(gNavGraphB, route->id & 0xFFFFFF, 0, 1);
        } else {
            route->flags &= ~1;
            GraphSetNode(gNavGraphB, route->id & 0xFFFFFF, 0, 0);
        }
        return 1;
    }
    case 0x41E: {
        RouteInfo* route = out->route;
        if (out->tag != 0x5E79B514) {
            return 0;
        }
        if (route->flags & 2) {
            out->value = 1;
        } else {
            out->value = 0;
        }
        return 1;
    }
    case 0x41D: {
        RouteInfo* route = out->route;
        if (out->tag != 0x5E79B514) {
            return 0;
        }
        if (out->value == 0) {
            route->flags &= ~2;
        } else {
            route->flags |= 2;
        }
        return 1;
    }
    }
    return 0;
}

// NavRouteEvent: same handler through a message callback (the route is already in the block).
int NavRouteEvent(int msg, RouteMsg* data) {
    if (data == 0 || gRoutes == 0) {
        return 0;
    }
    switch (msg) {
    case 0xC00: {
        RouteInfo* route = data->route;
        if (data->tag != 0) {
            route->flags |= 1;
            GraphSetNode(gNavGraphB, route->id & 0xFFFFFF, 0, 1);
        } else {
            route->flags &= ~1;
            GraphSetNode(gNavGraphB, route->id & 0xFFFFFF, 0, 0);
        }
        return 1;
    }
    case 0x41E: {
        RouteInfo* route = data->route;
        if (data->tag != 0x5E79B514) {
            return 0;
        }
        if (route->flags & 2) {
            data->value = 1;
        } else {
            data->value = 0;
        }
        return 1;
    }
    case 0x41D: {
        RouteInfo* route = data->route;
        if (data->tag != 0x5E79B514) {
            return 0;
        }
        if (data->value == 0) {
            route->flags &= ~2;
        } else {
            route->flags |= 2;
        }
        return 1;
    }
    }
    return 0;
}
