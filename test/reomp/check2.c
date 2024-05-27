#include <stdio.h>

int compare_files(const char* file1, const char* file2) {
    FILE *fp1, *fp2;
    int num1, num2;

    // 打开第一个文件
    fp1 = fopen(file1, "r");
    if (fp1 == NULL) {
        printf("无法打开 %s 文件\n", file1);
        return -1;
    }

    // 打开第二个文件
    fp2 = fopen(file2, "r");
    if (fp2 == NULL) {
        printf("无法打开 %s 文件\n", file2);
        fclose(fp1);
        return -1;
    }

    int line = 1;
    int differ = 0;

    // 比较文件内容
    while (fscanf(fp1, "%d", &num1) == 1 && fscanf(fp2, "%d", &num2) == 1) {
        if (num1 != num2) {
            printf("在第 %d 行，两个文件的内容不一致\n", line);
            differ = 1;
            break;
        }
        line++;
    }

    // 检查是否有剩余数据
    if (fscanf(fp1, "%d", &num1) == 1 || fscanf(fp2, "%d", &num2) == 1) {
        printf("两个文件的行数不一致\n");
        differ = 1;
    }

    // 关闭文件
    fclose(fp1);
    fclose(fp2);

    if (!differ) {
        printf("两个文件的内容完全一致\n");
    }

    return 0;
}

int main() {
    const char* file1 = "test1.txt";
    const char* file2 = "test2.txt";

    compare_files(file1, file2);

    return 0;
}

