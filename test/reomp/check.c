#include <stdio.h>

int main() {
    FILE *fp = fopen("rank_x.reomp", "r");
    int tid;

    if (fp == NULL) {
        printf("文件打开失败\n");
        return 1;
    }

    while (fread(&tid, sizeof(int), 1, fp) == 1) {
        printf("%d\n", tid);
    }

    fclose(fp);

    return 0;
}

 
