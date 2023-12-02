#include <stdio.h>
#include <stdlib.h>

#define MAXN 100000
#define DEPENDENCY_INDEX 0
#define VALUE_INDEX 1

int H[MAXN][2];
int N, N_CHEATS;

typedef struct node {
    int value;
    struct node *parent;
    struct node *firstChild;
    struct node *sibling;
    int nChildren;
    int totalCost;
} Node;

Node *created[MAXN];
Node *head;

int getHighestCost(Node *node) {
    int parentValue = node->value;
    if (node->firstChild == NULL) return parentValue;

    node = node->firstChild;
    int highest = node->totalCost;
    while(node != NULL) {
        highest = highest > node->totalCost ? highest : node->totalCost;
        node = node->sibling;
    }
    return highest + parentValue;
}

void setHighestCost(Node *node) {
    node->totalCost = getHighestCost(node);
    if (node->parent != NULL)
        setHighestCost(node->parent);
}

Node *createNode(int position, int value) {
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
        int parentPosition = H[position][DEPENDENCY_INDEX];
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
            while (child->sibling != NULL)
                child = child -> sibling;
            child -> sibling = n;
        }
        parent->nChildren++;
        setHighestCost(parent);

        created[position] = n;
        return n;
    }

}

void createTree() {
    for(int i=0; i<N; i++) {
        // Ci dice se è già stato creato il node.
        createNode(i, H[i][VALUE_INDEX]);

    }
}

Node *removeNode(Node *node){
    Node *hvn = node;
    while(node != NULL) {
        hvn = hvn->totalCost > node->totalCost ? hvn : node;
        node = node->sibling;
    }
    if (hvn->firstChild != NULL) {
        Node *nchild = removeNode(hvn->firstChild);
        hvn = hvn->value > nchild->value ? hvn : nchild;
    }
    return hvn;

}


int main() {

    // ------------------------------------------ Initializations ------------------------------------------
    FILE *fr;
    int i;
    fr = fopen("input.txt", "r");

    // Setting the number of total tasks and the number of cheats allowed
    fscanf(fr, "%d %d", &N, &N_CHEATS);

    // Initializing the array
    for(i=0; i<N; i++)
        fscanf(fr, "%d %d", &H[i][DEPENDENCY_INDEX], &H[i][VALUE_INDEX]);
    fclose(fr);
    for (int j = 0; j < N; ++j) {
        created[j] = NULL;
    }
    createTree();


    // ------------------------------------------ Actual algorithm ------------------------------------------

    for (i=0; i<N_CHEATS; i++) {
        Node *nodeToRemove = removeNode(head);
        nodeToRemove->value = 0;
        setHighestCost(nodeToRemove);
    }
    FILE *fw = fopen("output.txt", "w");

    fprintf(fw, "%d", head->totalCost);
    fclose(fw);
    return 0;
}
