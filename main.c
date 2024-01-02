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
    long long * cache;
} Node;

Node *created[MAXN];
Node *head;

long long cheat(Node *node, int remaining_cheats);
long long min(long long a, long long b);
long long max(long long a, long long b);

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
    for (int i = 0; i <= N_CHEATS; i++) {
        node->cache[i] = -1;
    }
}

void addChildToParent(Node *parent, Node *child) {
    if (parent->firstChild == NULL) {
        parent->firstChild = child;
    } else {
        Node *lastChild = parent->firstChild;
        while (lastChild->sibling != NULL) {
            lastChild = lastChild->sibling;
        }
        lastChild->sibling = child;
    }
}

Node *createNode(unsigned int position, long long value) {
    // Return the existing node if already created
    if (created[position] != NULL) {
        return created[position];
    }

    // Allocate memory for a new node
    Node *n = malloc(sizeof(Node));
    if (n == NULL) {
        return NULL; // Handle memory allocation failure
    }

    // Initialize node fields
    n->value = value;
    n->totalCost = value;
    n->firstChild = NULL;
    n->sibling = NULL;
    n->parent = NULL;
    initCache(n);
    created[position] = n;

    // Handle node dependencies
    if (H[position][DEPENDENCY_INDEX] != -1) {
        unsigned int parentPosition = H[position][DEPENDENCY_INDEX];
        Node *parent = createNode(parentPosition, H[parentPosition][VALUE_INDEX]);
        if (parent == NULL) {
            // Handle failure in parent node creation
            free(n);
            return NULL;
        }

        n->parent = parent;
        addChildToParent(parent, n);
        setHighestCost(parent);
    } else {
        head = n; // Set as head node if no dependencies
    }

    return n;
}

Node *createTree() {
    for (int i = 0; i < N; i++) {
        createNode(i, H[i][VALUE_INDEX]);
    }

    return head;
}

long long min(long long a, long long b) {
    return a < b ? a : b;
}

long long max(long long a, long long b) {
    return a > b ? a : b;
}

long long cheat(Node *node, int remaining_cheats) {
    if (node == NULL) return 0;

    if (node->cache[remaining_cheats] != -1) return node->cache[remaining_cheats];

    long long lowest = LLONG_MAX;
    for (int cheatsUsed=remaining_cheats; cheatsUsed>=0; cheatsUsed--) {
        // Calculate the cost of the subtree excluding the tree head
        long long costChildNoHead = cheatsUsed > 0 ? cheat(node->firstChild, cheatsUsed-1) : LLONG_MAX;

        // Calculate the cost of the subtree including the tree head
        long long costChild = cheat(node->firstChild, cheatsUsed) + node->value;

        // Best option on the subtree
        long long bestSubtree = min(costChild, costChildNoHead);

        // The maximum cost of subtasks at the same level of the current node
        long long siblingCost = cheat(node->sibling, remaining_cheats-cheatsUsed);

        // The maximum cost between the current node and the nodes at the same level
        long long highestLevelCostSubtask = max(bestSubtree, siblingCost);
        lowest = min(lowest, highestLevelCostSubtask);
    }

    // Memoization
    node->cache[remaining_cheats] = lowest;

    return lowest;
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

    long long final = head->totalCost;
    if (N_CHEATS > 0)
        final = cheat(head, N_CHEATS);
    FILE *fw = fopen("output.txt", "w");

    fprintf(fw, "%lld\n", final);
    fclose(fw);

    freeTree(head);

    return 0;
}