#include <stdio.h>
#define MAXN 100000

int H[MAXN][2];
int N, N_CHEATS;


int path(int dependency, int current_weight) {
    int paths_sum[N];
    int children_found = 0;
    for (int i=0; i<N; i++) {
        if (H[i][0] == dependency) {
            paths_sum[children_found] = path(i, H[i][1]);
            children_found+=1;
        }
    }

    int greatest = 0;
    for (int i=0; i < children_found; i++) {
        if (paths_sum[i] > greatest)
            greatest = paths_sum[i];
    }

    return current_weight + greatest;
}

int main() {

    // ------------------------------------------ Initializations ------------------------------------------

    FILE *fr;
    int i;

    fr = fopen("/Users/fabioceccatelli/CLionProjects/MapOfTasks/input_2.txt", "r");

    // Setting the number of total tasks and the number of cheats allowed
    fscanf(fr, "%d %d", &N, &N_CHEATS);

    // Initializing the array
    for(i=0; i<N; i++)
        fscanf(fr, "%d %d", &H[i][0], &H[i][1]);


    // ------------------------------------------ Actual algorithm ------------------------------------------

    int longest_path = path(-1, 0);
    printf("Longest path: %d", longest_path);

    return 0;
}
