#include <stdio.h>
int main() {
    int resist_array[8 * 5] = {
        // RIGHT SIDE
        5, 5, 5, 4, 4,  //
        5, 5, 4, 4, 3,  //
        5, 4, 4, 3, 3,  //
        6, 4, 3, 3, 6,  //  THIS SIDE IS
        6, 4, 3, 3, 6,  //   THE BOTTOM
        5, 4, 4, 3, 3,  //
        5, 5, 4, 4, 3,  //
        5, 5, 5, 4, 4,  //
        // LEFT SIDE
    };
    FILE *pFile;

    pFile = fopen("./5.lvl","wb");
    if (!pFile) {
        printf("Unable to open file!");
        return 1;
    }
    fwrite(resist_array, sizeof(int), 8*5, pFile);
    fclose(pFile);
    return 0;
}
