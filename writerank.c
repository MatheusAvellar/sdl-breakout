#include <stdio.h>
int main() {
    typedef struct _RANKED {
        char name[4];
        unsigned int score;
    } RANKED;

    RANKED player_vector[5];

    sprintf(player_vector[0].name, "AAA");
    player_vector[0].score = 4000000000;

    sprintf(player_vector[1].name, "ADR");
    player_vector[1].score = 100000;

    sprintf(player_vector[2].name, "POS");
    player_vector[2].score = 300;

    sprintf(player_vector[3].name, "DIA");
    player_vector[3].score = 100;

    sprintf(player_vector[4].name, "COL");
    player_vector[4].score = 42;

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
