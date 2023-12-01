#include <stdio.h>
#include <stdlib.h>

#define MAXN 100000
#define DEPENDENCY_INDEX 0
#define VALUE_INDEX 1

int H[MAXN][2];
int N, N_CHEATS;

typedef struct node {
    int value;
    struct node **children;
    int nChildren;
} Node;

Node *created[MAXN];
Node *head;

Node *createNode(int position, int value) {
    if (H[position][DEPENDENCY_INDEX] == -1) {
        Node *n = malloc(sizeof(Node));
        n->value = value;
        head = n;
        created[position] = n;
        return n;
    }

    if (created[H[position][DEPENDENCY_INDEX]] == NULL) {
        int parentPosition = H[position][DEPENDENCY_INDEX];
        Node *parent = createNode(parentPosition, H[parentPosition][VALUE_INDEX]);
        Node *n = malloc(sizeof(Node));
        n->value = value;

        parent->children = realloc(parent->children, sizeof(Node)*(parent->nChildren+1));
        parent->children[parent->nChildren] = n;
        parent->nChildren++;

        created[position] = n;
        return n;
    }
    else
        return created[H[position][DEPENDENCY_INDEX]];
}

void createTree() {
    for(int i=0; i<MAXN; i++) {
        // Ci dice se è già stato creato il node.
        createNode(i, H[i][VALUE_INDEX]);

    }
    printf("%d", head->value);
}


int path(int dependency, int hours_current_task) {
    int paths_sum_hours[N];
    int children_found = 0;

    /* This section searches in the array all the children dependent on the current task
     the argument dependency is the index of the current task in the array.

     After finding a dependent task, a recursive call is made using its index.
     */
    for (int i=0; i<N; i++) {
        if (H[i][DEPENDENCY_INDEX] == dependency) {
            paths_sum_hours[children_found] = path(i, H[i][VALUE_INDEX]);
            children_found+=1;
        }
    }

    /* After finding all the elements which are dependent on the current task
     * it looks for the one that takes the longest time, which is then summed to the time of the current task and returned.
     * */
    int greatest = 0;
    for (int i=0; i < children_found; i++) {
        if (paths_sum_hours[i] > greatest)
            greatest = paths_sum_hours[i];
    }

    return hours_current_task + greatest;
}

int main() {

    // ------------------------------------------ Initializations ------------------------------------------
    FILE *fr;
    int i;
    fr = fopen("/Users/fabioceccatelli_uni/CLionProjects/MapOfTasks/input_2.txt", "r");

    // Setting the number of total tasks and the number of cheats allowed
    fscanf(fr, "%d %d", &N, &N_CHEATS);

    // Initializing the array
    for(i=0; i<N; i++)
        fscanf(fr, "%d %d", &H[i][DEPENDENCY_INDEX], &H[i][VALUE_INDEX]);


    // ------------------------------------------ Actual algorithm ------------------------------------------

    //int longest_path = path(-1, 0);
    //printf("Longest path: %d", longest_path);
    createTree();

    return 0;
}
