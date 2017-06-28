#include <stdio.h>
int main() {
    typedef struct _RANKED {
        char name[4];
        unsigned int score;
    } RANKED;

    RANKED player_vector[5];

    sprintf(player_vector[0].name, "AAA");
    player_vector[0].score = 15000;

    sprintf(player_vector[1].name, "ADR");
    player_vector[1].score = 10000;

    sprintf(player_vector[2].name, "POS");
    player_vector[2].score = 5000;

    sprintf(player_vector[3].name, "DIA");
    player_vector[3].score = 1000;

    sprintf(player_vector[4].name, "AVL");
    player_vector[4].score = 100;

    FILE *pFile;
    pFile = fopen("./ranking","wb");
    if (!pFile) {
        printf("Unable to open file!");
        return 1;
    }
    fwrite(player_vector, sizeof(RANKED), 5, pFile);
    fclose(pFile);
    return 0;
}
