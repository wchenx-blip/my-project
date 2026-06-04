#include <stdio.h>

int main() {
    char name[100];
    
    printf("Hello, World!\n");
    
    printf("请输入你的名字: ");
    scanf("%s", name);
    
    printf("你好, %s! 欢迎学习 GitHub。\n", name);
    
    return 0;
}