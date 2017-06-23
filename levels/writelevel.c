#include <stdio.h>
int main() {
    int resist_array[8 * 5] = {
        // RIGHT SIDE
        6, 3, 4, 5, 7,  // 
        2, 9, 6, 9, 9,  // 
        9, 7, 2, 7, 1,  // 
        5, 9, 6, 2, 4,  //  THIS SIDE IS
        1, 8, 4, 3, 8,  //   THE BOTTOM
        4, 5, 8, 8, 8,  // 
        1, 3, 3, 3, 2,  // 
        3, 5, 2, 3, 0,  // 
        // LEFT SIDE
    };
    FILE *pFile;

    pFile = fopen("./3.lvl","wb");
    if (!pFile) {
        printf("Unable to open file!");
        return 1;
    }
    fwrite(resist_array, sizeof(int), 8*5, pFile);
    fclose(pFile);
    return 0;
}