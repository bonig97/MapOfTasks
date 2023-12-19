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
    struct node *copy;
    long long totalCost;
    long long resetVal;
} Node;


Node *created[MAXN];
Node *head;

int cheatsUsed = 0;
Node** prison = NULL;


void freeTree(Node *node) {
    if (node == NULL) return;
    freeTree(node->firstChild);
    freeTree(node->sibling);
    free(node);
}


Node* getGreaterChild(Node *node) {
    node = node->firstChild;
    if (node == NULL)
        return NULL;

    Node* highest = node;
    while(node != NULL) {
        highest = highest->totalCost >= node->totalCost ? highest : node;
        node = node->sibling;
    }
    return highest;
}

long long getHighestCost(Node *node) {
    long long parentValue = node->value;
    if (node->firstChild == NULL) return parentValue;

    Node* highest = getGreaterChild(node);
    return highest->totalCost + parentValue;
}

void setHighestCost(Node *node) {
    node->totalCost = getHighestCost(node);
    if (node->parent != NULL)
        setHighestCost(node->parent);
}

void replace(Node* source, Node* destination) {
    // Assegno parent e fc direttamente
    destination->parent = source->parent;
    destination->firstChild = source->firstChild;

    // Ciclo tutti i figli e assegno il padre
    Node* child = destination->firstChild;
    while(child != NULL){
        child->parent = destination;
        child = child->sibling;
    }


    if (source->parent->firstChild == source) {
        destination->sibling = source->parent->firstChild->sibling;
        source->parent->firstChild = destination;
    }
    else {
        Node *parentFC = source->parent->firstChild;
        while(parentFC->sibling != source) {
            parentFC = parentFC->sibling;
        }
        destination->sibling = parentFC->sibling->sibling;
        parentFC->sibling = destination;
    }
}

void resetNode(Node *node) {
    replace(node->copy, node);
    node->copy = NULL;
    setHighestCost(node);
    cheatsUsed--;
}

Node *createEmptyNode(long long value) {
    Node *n = malloc(sizeof(Node));
    n->value = value;
    n->resetVal = value;
    n->totalCost = value;
    n->firstChild = NULL;
    n->sibling = NULL;
    n->copy = NULL;
    n->parent = NULL;
    return n;
}

Node *createNode(unsigned int position, long long value) {
    if (created[position] != NULL) {
        return created[position];
    }
    else if (H[position][DEPENDENCY_INDEX] == -1) {
        Node *n = createEmptyNode(value);
        head = n;
        created[position] = n;
        return n;
    }
    else {
        unsigned int parentPosition = H[position][DEPENDENCY_INDEX];
        Node *parent = createNode(parentPosition, H[parentPosition][VALUE_INDEX]);
        Node *n = createEmptyNode(value);
        n->parent = parent;

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


void cheatNode(Node *node) {
    Node* new = createEmptyNode(node->value);
    new->value = 0;
    new->totalCost = node->totalCost-node->value;

    replace(node, new);

    node->copy = new;
    new->copy = node;

    setHighestCost(new);

    int i = 0;
    while(prison[i] != NULL)
        i++;
    prison[i] = node;
    cheatsUsed++;
}

int conflict(Node *node) {
    long long best = head->totalCost;
    Node* bestNode = NULL;
    Node* tested;
    int indexPr = -1;

    for (int i=N_CHEATS-1; i>=0; i--) {
        tested = prison[i];
        if (prison[i] != NULL) {
            long long initial = head->totalCost;

            tested->copy->value = tested->resetVal;
            node->value = 0;

            setHighestCost(node);
            setHighestCost(tested->copy);

            node->value = node->resetVal;
            tested->copy->value = 0;

            long long testedCost = head->totalCost;
            setHighestCost(node);
            setHighestCost(tested->copy);

            if (testedCost < initial && best > testedCost) {
                best = testedCost;
                bestNode = prison[i];
                indexPr = i;
            } else if (testedCost == best && bestNode != NULL && bestNode->resetVal < node->resetVal) {
                best = testedCost;
                bestNode = prison[i];
                indexPr = i;
            }

        }
    }

    if (indexPr >= 0) {
        prison[indexPr] = NULL;
        resetNode(bestNode);
        cheatNode(node);
        return 1;
    }
    return 0;
}

void cheat(Node *node) {
    if (node->copy != NULL)
        return;

    Node *gc = NULL;
    while (gc != getGreaterChild(node)) {
        gc = getGreaterChild(node);
        cheat(gc);
    }



    if (cheatsUsed < N_CHEATS) {
        cheatNode(node);
    }
    else {
        conflict(node);
    }



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
    prison = malloc(sizeof(Node) * N_CHEATS);
    for (i=0; i<N_CHEATS; i++)
        prison[i] = NULL;

    // Initializing the array
    for(i=0; i<N; i++) {
        fscanf(fr, "%lld %lld", &H[i][DEPENDENCY_INDEX], &H[i][VALUE_INDEX]);
        created[i] = NULL;
    }
    fclose(fr);

    createTree();
    Node* newhead = createEmptyNode(0);
    newhead->firstChild = head;
    head->parent = newhead;
    head = newhead;
    // ------------------------------------------ Actual algorithm ------------------------------------------

    long long final;
    if (N_CHEATS > 0) {
        cheat(head->firstChild);

    }
    setHighestCost(getGreaterChild(head->firstChild));
    final = head->totalCost;
    FILE *fw = fopen("output.txt", "w");

    fprintf(fw, "%lld\n", final);
    fclose(fw);

    freeTree(head);
    return 0;
}