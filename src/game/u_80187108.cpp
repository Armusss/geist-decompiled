// u_80187108.cpp (0x80187108-0x80187370), split from u_8017D794.
// Small list/iterator API (fn_80187108..fn_8018721C) called only from the SLT unit u_8016BE58. fn_8018721C has its
// own 0.0f/double (0x804D8408/0x804D8410 vs 0x804D83F0/0x804D8400 before and 0x804D841C/0x804D8438 after).
// Names are descriptive guesses; the code is matched against the retail DOL.

#include <dolphin/types.h>

void VFree(void* p);

// An intrusive doubly linked list. Each element has a 0xC-byte link header in front of its data; the API
// takes and returns pointers to the data.
struct ListNode {
    ListNode* prev;   // 0x0
    ListNode* next;   // 0x4
    void* data;       // 0x8
};

struct List {
    ListNode* head;   // 0x00
    ListNode* tail;   // 0x04
    u32 count;        // 0x08
    ListNode* cur;    // 0x0C built-in iterator
    u32 flags;        // 0x10 bit 0: cur already advanced by a removal
};

struct ListIter {
    ListNode* cur;
};

inline float Max(float a, float b) {
    return a > b ? a : b;
}

inline ListNode* ListNodeOf(void* data) {
    return data != 0 ? (ListNode*)((u8*)data - 0xC) : 0;
}

inline int ListIndexOf(List* list, void* data) {
    if (list != 0) {
        ListNode* node = ListNodeOf(data);
        ListNode* it = node;
        int i = 0;
        for (; it != 0; it = it->next, i++) {
            if (it == node) {
                return i;
            }
        }
    }
    return -1;
}

extern "C" {

void* fn_80187108(List* list, ListIter* it) {
    if (list != 0) {
        if (it != 0) {
            if (it->cur != 0) {
                return it->cur->data;
            }
        } else {
            if (list->cur != 0) {
                return list->cur->data;
            }
        }
    }
    return 0;
}

int fn_80187148(List* list, ListIter* it) {
    if (list != 0) {
        if (it != 0) {
            return it->cur != 0;
        } else {
            return list->cur != 0;
        }
    }
    return 0;
}

void fn_80187188(List* list, ListIter* it) {
    if (list == 0) {
        return;
    }
    if (it != 0) {
        if (it->cur != 0) {
            it->cur = it->cur->next;
        }
    } else if (!(list->flags & 1)) {
        if (list->cur != 0) {
            list->cur = list->cur->next;
        }
    }
    list->flags &= ~1;
}

void fn_801871E0(List* list, ListIter* it) {
    if (list == 0) {
        return;
    }
    if (it != 0) {
        it->cur = list->head;
    } else {
        list->cur = list->head;
    }
    list->flags &= ~1;
}

void* fn_80187214(List* list, void* data) {
    return data;
}

void fn_8018721C(List* list, void* data) {
    ListNode* node;

    if (list == 0 || data == 0) {
        return;
    }
    node = ListNodeOf(data);
    if (node == 0) {
        return;
    }
    if (ListIndexOf(list, data) < 0) {
        return;
    }
    if (node == list->cur) {
        list->cur = node->next;
        list->flags |= 1;
    }
    if (node == list->head) {
        list->head = node->next;
    }
    if (node == list->tail) {
        list->tail = node->prev;
    }
    if (node->next != 0) {
        node->next->prev = node->prev;
    }
    if (node->prev != 0) {
        node->prev->next = node->next;
    }
    VFree(node);
    list->count = Max(0.0f, --list->count);
}

}
