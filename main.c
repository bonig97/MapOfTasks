#include <stdio.h>
#include <stdlib.h>

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
} Node;

Node *created[MAXN];
Node *head;

long long getHighestCost(Node *node) {
    long long parentValue = node->value;
    if (node->firstChild == NULL) return parentValue;

    node = node->firstChild;
    long long  highest = node->totalCost;
    while(node != NULL) {
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


Node *createNode(unsigned int position, long long value) {
    if (created[position] != NULL) {
        return created[position];
    }
    else if (H[position][DEPENDENCY_INDEX] == -1) {
        Node *n = malloc(sizeof(Node));
        n->value = value;
        n->totalCost = value;
        n->firstChild = NULL;
        n->sibling = NULL;
        n->parent = NULL;
        head = n;
        created[position] = n;
        return n;
    }

    else {
        unsigned int parentPosition = H[position][DEPENDENCY_INDEX];
        Node *parent = createNode(parentPosition, H[parentPosition][VALUE_INDEX]);
        Node *n = malloc(sizeof(Node));
        n->value = value;
        n->parent = parent;
        n->totalCost = value;
        n->firstChild = NULL;
        n->sibling = NULL;

        Node *child;
        if (parent->firstChild == NULL) {
            parent->firstChild = n;
        } else {
            child = parent -> firstChild;
            while (child->sibling != NULL) {
                child = child -> sibling;
            }

            child -> sibling = n;
        }
        setHighestCost(parent);

        created[position] = n;
        return n;
    }

}

void createTree() {
    for(int i=0; i<N; i++) {
        createNode(i, H[i][VALUE_INDEX]);
    }
}

Node *cheat (Node *node, int remaining_cheats) {
    Node *child = node->firstChild;
    if (child == NULL)
        return node;

    Node *highest = child;
    Node *secondHighest = NULL;
    child = child -> sibling;
    while (child) {
        if (highest->totalCost > child->totalCost) {
            secondHighest = secondHighest != NULL && secondHighest -> totalCost > child -> totalCost ? secondHighest : child;
        }
        else {
            secondHighest = highest;
            highest = child;
        }
        child = child -> sibling;
    }

    Node *cheatOpt = cheat(highest, remaining_cheats);
    if (secondHighest == NULL)
        return cheatOpt->value > node->value ? cheatOpt : node;
    if (remaining_cheats > 0 || node->totalCost-node->value  >= node->value + secondHighest->totalCost)
        return cheatOpt;
    return node;
}



int main() {

    // ------------------------------------------ Initializations ------------------------------------------
    FILE *fr;
    int i;
    fr = fopen("input.txt", "r");

    // Setting the number of total tasks and the number of cheats allowed
    fscanf(fr, "%d %d", &N, &N_CHEATS);

    // Initializing the array
    for(i=0; i<N; i++) {
        fscanf(fr, "%lld %lld", &H[i][DEPENDENCY_INDEX], &H[i][VALUE_INDEX]);
        created[i] = NULL;
    }
    fclose(fr);

    head = NULL;
    createTree();


    // ------------------------------------------ Actual algorithm ------------------------------------------

    for (i=0; i<N_CHEATS; i++) {
        Node *nodeToRemove = cheat(head, N_CHEATS-i-1);
        nodeToRemove->value = 0;
        setHighestCost(nodeToRemove);
    }
    FILE *fw = fopen("output.txt", "w");

    fprintf(fw, "%lld\n", head->totalCost);
    fclose(fw);
    return 0;
}
