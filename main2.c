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

Node* createTree() {
    for(int i=0; i<N; i++) {
        createNode(i, H[i][VALUE_INDEX]);
    }
    return head;
}

Node* findGreatestParent(Node *node) {
    long long highest = node->value;
    if (node->parent == NULL)
        return node;
    Node * ancestor = findGreatestParent(node->parent);
    return highest > ancestor->value ? node : ancestor;
}

int countSiblings(Node *node) {
    int count = 0;
    while (node != NULL) {
        count++;
        node = node->sibling;
    }
    return count;
}

Node** createArrayOfSiblingNode(Node* node) {
    // crates an array of all the sibling nodes by order of totalCost
    int scount = countSiblings(node);
    Node **array = malloc(sizeof(Node*) * scount);
    int i = 0;
    while (node != NULL) {
        array[i] = node;
        node = node->sibling;
        i++;
    }

    // sort the array decreasing order
    int j;
    Node *temp;
    for (i = 0; i < scount; i++) {
        for (j = i + 1; j < scount; j++) {
            if (array[i]->totalCost < array[j]->totalCost) {
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
    return array;
}



Node* cheat(Node *node, int remaining_cheats) {
    Node *child = node->firstChild;
    if (child == NULL)
        return findGreatestParent(node);


    Node ** arr = createArrayOfSiblingNode(node->firstChild);
    int siblings = countSiblings(node->firstChild);

    Node *gp = findGreatestParent(node);
    if (remaining_cheats < siblings-1) {
        if (gp->value >= arr[0]->totalCost - arr[remaining_cheats+1]->totalCost)
            return gp;
    }

    if(node->totalCost - gp->value  <= gp->value)
        return gp;
    return cheat(arr[0], remaining_cheats);

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
    for(i=0; i<N; i++) {
        fscanf(fr, "%lld %lld", &H[i][DEPENDENCY_INDEX], &H[i][VALUE_INDEX]);
        created[i] = NULL;
    }
    fclose(fr);

    Node* head = createTree();


    // ------------------------------------------ Actual algorithm ------------------------------------------

    for (i=0; i<N_CHEATS; i++) {
        Node *nodeToRemove = cheat(head, N_CHEATS-i-1);
        nodeToRemove->value = 0;
        setHighestCost(nodeToRemove);
    }
    FILE *fw = fopen("output.txt", "w");

    fprintf(fw, "%lld\n", head->totalCost);
    fclose(fw);

    freeTree(head);
    return 0;
}