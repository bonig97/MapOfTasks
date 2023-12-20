#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAXN 100000
#define DEPENDENCY_INDEX 0
#define VALUE_INDEX 1

long long H[MAXN][2];
int N, N_CHEATS;

typedef struct node {
    long long value;
    struct node *parent;
    struct node *firstChild;
    struct node *sibling;
    long long totalCost;
    long long * cache
} Node;

Node *created[MAXN];
Node *head;



void freeTree(Node *node) {
    if (node == NULL) return;
    freeTree(node->firstChild);
    freeTree(node->sibling);
    free(node);
}

long long getHighestCost(Node *node) {
    long long parentValue = node->value;
    if (node->firstChild == NULL) return parentValue;

    node = node->firstChild;
    long long highest = node->totalCost;
    while (node != NULL) {
        highest = highest >= node->totalCost ? highest : node->totalCost;
        node = node->sibling;
    }
    return highest + parentValue;
}

void setHighestCost(Node *node) {
    node->totalCost = getHighestCost(node);
    if (node->parent != NULL)
        setHighestCost(node->parent);
}

void initCache(Node *node) {
    node->cache = malloc((N_CHEATS+1)*sizeof(long long));
    for (int i=0; i<=N_CHEATS; i++) {
        node->cache[i] = -1;
    }

}

Node *createNode(unsigned int position, long long value) {
    if (created[position] != NULL) {
        return created[position];
    } else if (H[position][DEPENDENCY_INDEX] == -1) {
        Node *n = malloc(sizeof(Node));
        n->value = value;
        n->totalCost = value;
        n->firstChild = NULL;
        n->sibling = NULL;
        n->parent = NULL;

        initCache(n);
        head = n;
        created[position] = n;
        return n;
    } else {
        unsigned int parentPosition = H[position][DEPENDENCY_INDEX];
        Node *parent = createNode(parentPosition, H[parentPosition][VALUE_INDEX]);
        Node *n = malloc(sizeof(Node));
        n->value = value;
        n->parent = parent;
        n->totalCost = value;
        n->firstChild = NULL;
        n->sibling = NULL;
        initCache(n);

        Node *child;
        if (parent->firstChild == NULL) {
            parent->firstChild = n;
        } else {
            child = parent->firstChild;
            while (child->sibling != NULL) {
                child = child->sibling;
            }

            child->sibling = n;
        }
        setHighestCost(parent);

        created[position] = n;
        return n;
    }

}

Node *createTree() {
    for (int i = 0; i < N; i++) {
        createNode(i, H[i][VALUE_INDEX]);
    }
    Node *n = malloc(sizeof(Node));
    n->value = 0;
    n->parent = NULL;
    n->totalCost = 0;
    n->firstChild = NULL;
    n->sibling = NULL;
    n->totalCost = head->totalCost;
    initCache(n);

    n->firstChild = head;
    head->parent = n;
    head = n;
    return head;
}



long long min(long long a, long long b) {
    return a < b ? a : b;
}

long long max(long long a, long long b) {
    return a > b ? a : b;
}

long long cheat(Node *node, int remaining_cheats, Node* firstSibling);


long long cheat(Node *node, int remaining_cheats, Node* firstSibling) {
    if (node == NULL) return 0;
    if (node->cache[remaining_cheats] != -1) return node->cache[remaining_cheats];

    long long highest = LLONG_MAX;
    for (int cheatsUsed=remaining_cheats; cheatsUsed>=0; cheatsUsed--) {
        long long costChild = cheatsUsed > 0 ? cheat(node->firstChild, cheatsUsed-1, NULL) : LLONG_MAX;
        long long costChildNoHead = cheat(node->firstChild, cheatsUsed, NULL) + node->value;

        long long siblingcost = cheat(node->sibling, remaining_cheats-cheatsUsed, NULL);
        highest = min(highest, max(min(costChild, costChildNoHead), siblingcost));
    }

    node->cache[remaining_cheats] = highest;
    return highest;
}


/*
 * First case is the simplest, I have a tree with only one leaf
 */

int main() {
    // ------------------------------------------ Initializations ------------------------------------------
    FILE *fr;
    int i;
    fr = fopen("input.txt", "r");

    // Setting the number of total tasks and the number of cheats allowed
    fscanf(fr, "%d %d", &N, &N_CHEATS);

    // Initializing the array
    for (i = 0; i < N; i++) {
        fscanf(fr, "%lld %lld", &H[i][DEPENDENCY_INDEX], &H[i][VALUE_INDEX]);
        created[i] = NULL;
    }
    fclose(fr);

    Node *head = createTree();


    // ------------------------------------------ Actual algorithm ------------------------------------------

    //long long final = head->value + cheat(head, N_CHEATS);
    //long long finalNoParent = cheat(head, N_CHEATS-1);
    long long final = head->totalCost;
    if (N_CHEATS > 0)
        final = min(cheat(head, N_CHEATS, head)+head->value, cheat(head, N_CHEATS-1, head))  ;
    FILE *fw = fopen("output.txt", "w");

    fprintf(fw, "%lld\n", final);
    fclose(fw);

    freeTree(head);
    return 0;
}