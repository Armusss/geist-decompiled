// Graphs and path search (0x800F14A8-0x800F2B08).
// Up to 16 graphs (handles 0x0E000000 | index) built from pooled nodes and links (256 each). A generic
// best-first search keeps search states in scratch memory; the open list order (depth first, breadth
// first, cheapest cost, cheapest estimate) is chosen by the insert callback.
// Names are descriptive guesses; the code is matched against the retail DOL.

typedef unsigned long u32;

void ScratchAReset(void);
void* ScratchAAlloc(int size);

extern "C" {
// Bit sets (code in another file, not named yet).
int fn_80025C14(u32* bits, int words, int max); // allocate a free bit, -1 if none
void fn_80025CFC(u32* bits, int index);         // free a bit
}

struct GraphLink {
    int active;
    int arg;
    int target;
    GraphLink* next;
};

struct GraphNode {
    int active;
    u32 data;
    GraphLink* links;
    GraphNode* next;
};

struct Graph {
    int numNodes;
    GraphNode* nodes;
    int unk8;
};

struct SearchState {
    int depth;
    int node;
    float cost;
    float estimate;
    int linkIndex;
    SearchState* parent;
    GraphLink* link;
    SearchState* prev;
    SearchState* next;
};

struct GraphQuery {
    int path[16];
    int pathArgs[16];
    float result;
    int mode;
    int endNode;
    SearchState* open;
    SearchState* closed;
};

typedef void (*SearchInsertFn)(SearchState** list, SearchState* state);
typedef float (*SearchCostFn)(int from, int to, int arg, void* ctx, SearchState* state);
typedef int (*SearchGoalFn)(int node, int goal, SearchState* state);

static Graph sGraphs[16];
static GraphNode sNodes[256];
static GraphLink sLinks[256];
static u32 sNodeBits[9];
static u32 sLinkBits[9];
static u32 sSearchBits[17];

static SearchState* sSearchStates;
static u32 sGraphBits[1];

inline void BitSet(u32* bits, int i) { bits[i / 32] |= 1 << (i & 31); }

inline Graph* GraphGet(u32 handle) {
    int index;

    if ((int)(handle >> 24) != 0xE) {
        return 0;
    }
    index = handle & 0xFFFFFF;
    if (index < 0 || index > 16) {
        return 0;
    }
    if (!(sGraphBits[index / 32] & (1 << (index & 31)))) {
        return 0;
    }
    return &sGraphs[index];
}

float SearchRun(Graph* graph, int start, int goal, void* ctx, SearchInsertFn insert, SearchCostFn cost,
                SearchCostFn estimate, SearchGoalFn isGoal, int goalArg, GraphQuery* query);
void SearchInsertByEstimate(SearchState** list, SearchState* state);
void SearchInsertByCost(SearchState** list, SearchState* state);
void SearchInsertDeep(SearchState** list, SearchState* state);
void SearchInsertShallow(SearchState** list, SearchState* state);
int SearchSameNode(int node, int goal, SearchState* state);
float SearchZero(int from, int to, int arg, void* ctx, SearchState* state);
float SearchOne(int from, int to, int arg, void* ctx, SearchState* state);

float GraphSearchFrom(u32 handle, int mode, int start, void* ctx, SearchCostFn cost, SearchGoalFn isGoal,
                      int goalArg, GraphQuery* query) {
    Graph* graph = GraphGet(handle);

    if (graph == 0) {
        return 1e9f;
    }
    if (graph->numNodes < start) {
        return 1e9f;
    }
    if (query->result == -1e9f && query->mode != mode) {
        return 0.0f;
    }

    switch (mode) {
    case 0:
        query->result = SearchRun(graph, start, -1, ctx, SearchInsertShallow, SearchOne, SearchZero, isGoal,
                                  goalArg, query);
        break;
    case 1:
        query->result = SearchRun(graph, start, -1, ctx, SearchInsertDeep, SearchOne, SearchZero, isGoal,
                                  goalArg, query);
        break;
    case 2:
        query->result = SearchRun(graph, start, -1, ctx, SearchInsertByCost, cost, SearchZero, isGoal, goalArg,
                                  query);
        break;
    case 3:
        return 1e9f;
    }
    return query->result;
}

float GraphSearchTo(u32 handle, int mode, int start, int goal, void* ctx, SearchCostFn cost,
                    SearchCostFn estimate, GraphQuery* query) {
    Graph* graph = GraphGet(handle);

    if (graph == 0) {
        return 1e9f;
    }
    if (graph->numNodes < start || graph->numNodes < goal) {
        return 1e9f;
    }
    if (query->result == -1e9f && query->mode != mode) {
        return 0.0f;
    }

    switch (mode) {
    case 0:
        query->result = SearchRun(graph, start, goal, ctx, SearchInsertShallow, SearchOne, SearchZero,
                                  SearchSameNode, goal, query);
        break;
    case 1:
        query->result = SearchRun(graph, start, goal, ctx, SearchInsertDeep, SearchOne, SearchZero,
                                  SearchSameNode, goal, query);
        break;
    case 2:
        query->result = SearchRun(graph, start, goal, ctx, SearchInsertByCost, cost, SearchZero, SearchSameNode,
                                  goal, query);
        break;
    case 3:
        query->result = SearchRun(graph, start, goal, ctx, SearchInsertByEstimate, cost, estimate,
                                  SearchSameNode, goal, query);
        break;
    }
    return query->result;
}

int GraphQueryInit(GraphQuery* query) {
    int i;

    query->result = 1e9f;
    for (i = 0; i < 16; i++) {
        query->path[i] = -1;
    }
    query->open = 0;
    query->closed = 0;
    query->mode = 4;
    return 1;
}

int GraphFindNode(u32 handle, u32 data) {
    Graph* graph = GraphGet(handle);
    GraphNode* node;
    int i;

    if (graph == 0) {
        return 0;
    }
    i = 0;
    for (node = graph->nodes; node; node = node->next) {
        if (node->data == data) {
            return i;
        }
        i++;
    }
    return -1;
}

u32 GraphGetNodeData(u32 handle, int index) {
    Graph* graph = GraphGet(handle);
    GraphNode* node;

    if (graph == 0) {
        return 0;
    }
    if (graph->numNodes < index) {
        return 0;
    }
    node = graph->nodes;
    while (index > 0 && node) {
        node = node->next;
        index--;
    }
    if (node) {
        return node->data;
    }
    return 0;
}

int GraphSetNode(u32 handle, int index, u32 data, int active) {
    Graph* graph = GraphGet(handle);
    GraphNode* node;

    if (graph == 0) {
        return 0;
    }
    if (graph->numNodes < index) {
        return 0;
    }
    node = graph->nodes;
    while (index > 0 && node) {
        node = node->next;
        index--;
    }
    if (node) {
        node->active = active;
        node->data = data;
    }
    return 1;
}

int GraphFindBestLink(u32 handle, SearchCostFn score, void* ctx, int* outNode, int* outTarget) {
    float best = 3.4028235e38f;
    int bestNode = -1;
    int bestTarget = -1;
    Graph* graph = GraphGet(handle);
    GraphNode* node;
    int i;
    GraphLink* link;

    if (graph == 0) {
        return 0;
    }

    i = 0;
    for (node = graph->nodes; node; node = node->next) {
        for (link = node->links; link; link = link->next) {
            float s = score(i, link->target, link->arg, ctx, 0);
            if (s >= 0.0f && s < best) {
                best = s;
                bestTarget = link->target;
                bestNode = i;
            }
        }
        i++;
    }

    if (best != 3.4028235e38f) {
        if (outNode) {
            *outNode = bestNode;
        }
        if (outTarget) {
            *outTarget = bestTarget;
        }
        return 1;
    }
    return 0;
}

inline GraphLink* LinkAlloc(void) {
    int slot = fn_80025C14(sLinkBits, 9, 256);
    BitSet(sLinkBits, slot);
    sLinks[slot].active = 1;
    return &sLinks[slot];
}

inline GraphNode* GraphNodeAt(GraphNode* list, int index) {
    int i;
    GraphNode* node;
    node = list;
    i = index;
    while (i > 0 && node) {
        node = node->next;
        i--;
    }
    return node;
}

int GraphAddLink(u32 handle, int index, int target, int arg) {
    Graph* graph = GraphGet(handle);
    GraphNode* node;
    GraphLink* link;
    int slot;

    if (graph == 0) {
        return 0;
    }
    if (graph->numNodes <= index || graph->numNodes <= target) {
        return 0;
    }
    node = GraphNodeAt(graph->nodes, index);
    if (node == 0) {
        return 0;
    }

    link = LinkAlloc();
    link->target = target;
    link->arg = arg;
    if (node->links == 0) {
        node->links = link;
        link->next = 0;
    } else {
        link->next = node->links;
        node->links = link;
    }
    return 1;
}

int GraphAllocNodes(u32 handle, int count) {
    Graph* graph = GraphGet(handle);
    GraphNode* node;
    GraphNode* next;
    int slot;

    if (graph == 0) {
        return 0;
    }
    if (graph->nodes != 0 || graph->numNodes != 0) {
        return 0;
    }
    if (count <= 0) {
        return 0;
    }

    slot = fn_80025C14(sNodeBits, 9, 256);
    if (slot == -1) {
        return 0;
    }
    node = &sNodes[slot];
    graph->nodes = node;
    graph->nodes->active = 1;
    graph->nodes->links = 0;
    graph->numNodes = 1;
    BitSet(sNodeBits, slot);

    while (graph->numNodes < count) {
        slot = fn_80025C14(sNodeBits, 9, 256);
        if (slot == -1) {
            break;
        }
        next = &sNodes[slot];
        node->next = next;
        next->active = 1;
        BitSet(sNodeBits, slot);
        next->links = 0;
        node = next;
        graph->numNodes++;
    }

    if (graph->numNodes != count) {
        for (next = graph->nodes; next != node; next = next->next) {
            fn_80025CFC(sNodeBits, next - sNodes);
        }
        return 0;
    }
    node->next = 0;
    return 1;
}

int GraphDestroy(u32 handle) {
    int index;
    GraphNode* node;
    GraphLink* link;

    if ((int)(handle >> 24) != 0xE) {
        return 0;
    }
    index = handle & 0xFFFFFF;
    if (index < 0 || index >= 16) {
        return 0;
    }
    if (!(sGraphBits[index / 32] & (1 << (index & 31)))) {
        return 0;
    }

    node = sGraphs[index].nodes;
    while (node) {
        for (link = node->links; link; link = link->next) {
            fn_80025CFC(sLinkBits, link - sLinks);
        }
        index = node - sNodes;
        node = node->next;
        fn_80025CFC(sNodeBits, index);
    }
    fn_80025CFC(sGraphBits, handle & 0xFFFFFF);
    return 1;
}

u32 GraphCreate(void) {
    int slot = fn_80025C14(sGraphBits, 1, 16);

    BitSet(sGraphBits, slot);
    sGraphs[slot].numNodes = 0;
    sGraphs[slot].nodes = 0;
    return (slot & 0xFFFFFF) | 0x0E000000;
}

inline SearchState* SearchNewState(SearchState* parent, GraphLink* link, int node, float cost, float estimate) {
    int slot = fn_80025C14(sSearchBits, 17, 0x200);

    if (slot == -1) {
        return 0;
    }
    BitSet(sSearchBits, slot);
    sSearchStates[slot].parent = parent;
    sSearchStates[slot].prev = 0;
    sSearchStates[slot].next = 0;
    sSearchStates[slot].link = link;
    sSearchStates[slot].node = node;
    sSearchStates[slot].linkIndex = 0;
    sSearchStates[slot].cost = cost;
    sSearchStates[slot].estimate = estimate;
    if (parent == 0) {
        sSearchStates[slot].depth = 0;
    } else {
        sSearchStates[slot].depth = parent->depth + 1;
        if (sSearchStates[slot].depth > 16) {
            fn_80025CFC(sSearchBits, slot);
            return 0;
        }
    }
    return &sSearchStates[slot];
}


inline GraphLink* GraphLinkAt(GraphLink* list, int index) {
    int i;
    GraphLink* link;
    link = list;
    i = index;
    while (i > 0 && link) {
        link = link->next;
        i--;
    }
    return link;
}

inline int SearchOnPath(SearchState* state, int node) {
    SearchState* p;
    for (p = state; p; p = p->parent) {
        if (p->node == node) {
            return 1;
        }
    }
    return 0;
}

inline void SearchListRemove(SearchState** list, SearchState* state) {
    SearchState* p;

    if (list && state) {
        if (*list == state) {
            *list = (*list)->next;
        } else {
            for (p = (*list)->next; p != state && p; p = p->next) {
            }
            if (p) {
                (*list)->next = p->next;
            }
        }
    }
}

float SearchRun(Graph* graph, int start, int goal, void* ctx, SearchInsertFn insert, SearchCostFn cost,
                SearchCostFn estimate, SearchGoalFn isGoal, int goalArg, GraphQuery* query) {
    int i;
    GraphLink* link;
    float g;
    float c;
    float h;
    SearchState* state;

    if (graph == 0 || cost == 0 || insert == 0 || estimate == 0 || query == 0) {
        return 1e9f;
    }

    ScratchAReset();
    sSearchStates = (SearchState*)ScratchAAlloc(0x4800);
    for (i = 0; i < 17; i++) {
        sSearchBits[i] = 0;
    }

    if (query->open == 0) {
        if (query->result == -1e9f) {
            ScratchAReset();
            return 1e9f;
        }
        query->open = SearchNewState(0, 0, start, 0.0f, 0.0f);
        if (query->open == 0) {
            ScratchAReset();
            return -1e9f;
        }
    }

    if (isGoal == 0) {
        goalArg = goal;
        isGoal = SearchSameNode;
    }

    c = 0.0f;
    state = query->open;
    while (state && !isGoal(state->node, goalArg, state) && c != -1e9f) {
        GraphNode* node = GraphNodeAt(graph->nodes, state->node);
        if (node == 0) {
            ScratchAReset();
            return 1e9f;
        }

        link = GraphLinkAt(node->links, state->linkIndex);
        while (state == query->open && link && c != -1e9f) {
            if (link->active) {
                int target = link->target;
                if (GraphNodeAt(graph->nodes, target)->active && !SearchOnPath(state, target)) {
                    c = cost(state->node, target, link->arg, ctx, state);
                    if (c != -1e9f) {
                        SearchState* child;
                        h = estimate(link->target, goal, link->arg, ctx, state);
                        g = state->cost + c;
                        child = SearchNewState(state, link, link->target, g, g + h);
                        if (child) {
                            insert(&query->open, child);
                        }
                        state->linkIndex++;
                        link = link->next;
                    }
                } else {
                    state->linkIndex++;
                    link = link->next;
                }
            } else {
                state->linkIndex++;
                link = link->next;
            }
        }

        if (link == 0) {
            SearchListRemove(&query->open, state);
            state->next = query->closed;
            query->closed = state;
            state = query->open;
        } else if (state != query->open) {
            state = query->open;
        }
    }

    if (c != -1e9f) {
        if (state && isGoal(state->node, goalArg, state)) {
            int depth;
            query->endNode = state->node;
            depth = state->depth;
            c = state->cost;
            for (i = depth; i >= 0; i--) {
                if (state) {
                    query->path[i] = state->node;
                    if (state->link) {
                        query->pathArgs[i] = state->link->arg;
                    } else {
                        query->pathArgs[i] = 0;
                    }
                    state = state->parent;
                }
            }
        } else {
            c = 1e9f;
        }
    }
    ScratchAReset();
    return c;
}

void SearchInsertByEstimate(SearchState** list, SearchState* state) {
    SearchState* p;

    if (list == 0 || *list == 0 || state == 0) {
        return;
    }
    p = *list;
    if (p == 0) {
        *list = state;
        return;
    }
    if (p->estimate > state->estimate) {
        state->next = p;
        state->prev = p->prev;
        p->prev = state;
        *list = state;
        return;
    }
    while (p->next && p->estimate <= state->estimate) {
        p = p->next;
    }
    if (p->estimate > state->estimate) {
        if (p->prev) {
            p->prev->next = state;
        }
        state->next = p;
        state->prev = p->prev;
        p->prev = state;
        return;
    }
    p->next = state;
    state->prev = p;
    state->next = 0;
}

void SearchInsertByCost(SearchState** list, SearchState* state) {
    SearchState* p;

    if (list == 0 || *list == 0 || state == 0) {
        return;
    }
    p = *list;
    if (p == 0) {
        *list = state;
        return;
    }
    if (p->cost > state->cost) {
        state->next = p;
        state->prev = p->prev;
        p->prev = state;
        *list = state;
        return;
    }
    while (p->next && p->cost <= state->cost) {
        p = p->next;
    }
    if (p->cost > state->cost) {
        if (p->prev) {
            p->prev->next = state;
        }
        state->next = p;
        state->prev = p->prev;
        p->prev = state;
        return;
    }
    p->next = state;
    state->prev = p;
    state->next = 0;
}

void SearchInsertDeep(SearchState** list, SearchState* state) {
    SearchState* p;

    if (list == 0 || state == 0) {
        return;
    }
    p = *list;
    if (p == 0) {
        *list = state;
        return;
    }
    if (p->depth < state->depth) {
        state->next = p;
        state->prev = p->prev;
        p->prev = state;
        *list = state;
        return;
    }
    while (p->next && p->depth > state->depth) {
        p = p->next;
    }
    if (p->depth <= state->depth) {
        if (p->prev) {
            p->prev->next = state;
        }
        state->next = p;
        state->prev = p->prev;
        p->prev = state;
        return;
    }
    p->next = state;
    state->prev = p;
    state->next = 0;
}

void SearchInsertShallow(SearchState** list, SearchState* state) {
    SearchState* p;

    if (list == 0 || state == 0) {
        return;
    }
    p = *list;
    if (p == 0) {
        *list = state;
        return;
    }
    if (p->depth > state->depth) {
        state->next = p;
        state->prev = p->prev;
        p->prev = state;
        *list = state;
        return;
    }
    while (p->next && p->depth <= state->depth) {
        p = p->next;
    }
    if (p->depth > state->depth) {
        if (p->prev) {
            p->prev->next = state;
        }
        state->next = p;
        state->prev = p->prev;
        p->prev = state;
        return;
    }
    p->next = state;
    state->prev = p;
    state->next = 0;
}

int SearchSameNode(int node, int goal, SearchState* state) { return node == goal; }

float SearchZero(int from, int to, int arg, void* ctx, SearchState* state) { return 0.0f; }

float SearchOne(int from, int to, int arg, void* ctx, SearchState* state) { return 1.0f; }
