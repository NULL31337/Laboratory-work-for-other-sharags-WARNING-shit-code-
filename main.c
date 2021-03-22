#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 128

// реализация стека примерно такая же как в плюсах с ассимптотикой добавления и удаления элемента O(1) и памятью O(n)

struct tab {
    char* str;
    int cnt;
};

struct stack {
    struct tab tab;
    int size;
    int pos;
};


// меняет местами 2 элемента массива
void swap (struct stack* lol, int a, int b) {
    struct stack c = lol[a];
    lol[a] = lol[b];
    lol[b] = c;
}

// изменяет размер
struct stack* resize(struct stack* a){
    (a->size) *= 2;
    a = (struct stack*)realloc(a, a->size * sizeof(struct stack));
    return  a;
}

// добавляет элемент в стек и увеличивает если он заполнен
struct stack* push(struct stack* a, int val, char* lol) {
    if (a->pos == a->size) {
        a = resize(a);
    }
    a[a->pos].tab.cnt = val;
    a[a->pos].tab.str = lol;
    (a->pos)++;
    return a;
}

// удаляет элемент из стека
struct tab pop(struct stack* a) {
    if (a->pos != 0) {
        return a[--(a->pos)].tab;
    }
}


//Можно на более крутую заменить но я заебался её дебажить
char* subString (int offset, char* input, char* dest) {
    for (int i = offset; i < strlen(input); i++) {
        if (input[i] == ' ' || input[i] == '\n' || input[i] == '\r') {
            dest[i - offset] = '\0';
            return dest;
        }
        dest[i - offset] = input[i];
    }
    return dest;
}

//Пытаемся положить в массив где храним то что нужно удалять
struct stack* tryPush(struct stack* stack, char* a, int val) {
    for (int i = 0; i < stack->pos; i++) {
        if (strcmp(stack[i].tab.str, a) == 0) {
            if (val == -1 || stack[i].tab.cnt == -1) {
                stack[i].tab.cnt = val;
            } else {
                stack[i].tab.cnt += val;
            }
            return stack;
        }
    }
    stack = push(stack, val, a);
    return stack;
}

// вывод для дебага(можно удалить)
void output (struct stack* stack) {
    for (int i = 0; i < stack->pos; i++) {
        printf("%d: %s - %d\n", i, stack[i].tab.str, stack[i].tab.cnt);
    }
}

//Пытаемся удалить из массива где храним то что нужно удалять
int tryPop(struct stack* stack, const char* a) {
    for (int i = 0; i < stack->pos; i++) {
        if (strcmp(stack[i].tab.str, a) == 0) {
            if (stack[i].tab.cnt > 0) {
                stack[i].tab.cnt--;
            }
            return 1;
        }
    }
    return 0;
}

//вырезаем часть строки
char* strcut(char* first, int offset, int cnt, char* second) {
    strncpy (second, first + offset, cnt);
    if (cnt < BUF_SIZE) {
        second[cnt] = '\0';
    }
    return second;
}

int main(int argc, char* argv[]) {
    // создаём массивы
    struct stack* prev = (struct stack*)(malloc(2 * sizeof(struct stack)));
    struct stack* post = (struct stack*)(malloc(2 * sizeof(struct stack)));
    prev->size = 2;
    prev->pos = 0;
    post->size = 2;
    post->pos = 0;
    //считываем по строчкам
    FILE* file = fopen(argv[1], "r");
    char* buf = (char*)(malloc(sizeof(char) * BUF_SIZE));
    while (fgets(buf, BUF_SIZE, file) != NULL) {
        char* push = (char*)(malloc(sizeof(char) * BUF_SIZE));
        // добавляем в массивы
        if(buf[0] == '^') {
            if (buf[1] == '1' && buf[2] == '0') {
                push = subString(3, buf, push);
                prev = tryPush(prev, push, 10);
            } else {
                push = subString(1, buf, push);
                prev = tryPush(prev, push, 1);
            }
        } else if (buf[0] == '#') {
            if (buf[1] == '1' && buf[2] == '0') {
                push = subString(3, buf, push);
                post = tryPush(post, push, 10);
            } else {
                push = subString(1, buf, push);
                post = tryPush(post, push, 1);
            }
        } else {
            push = subString(0, buf, push);
            prev = tryPush(prev, push, -1);
        }
    }
    fclose(file);
    file = fopen(argv[2], "r");
    struct stack* stackLOl = (struct stack*)(malloc(2 * sizeof(struct stack)));
    stackLOl->size = 2;
    stackLOl->pos = 0;

    // считываем 2 файл если у нас такое слово в удалённых сначала то мы просто его не добавляем в массив
    while (fgets (buf, BUF_SIZE, file) != NULL) {
        int cnt = 0;
        for (int i = 0; i < BUF_SIZE; i++) {
            if ((buf[i] == EOF || buf[i] == '\n' || buf[i] == ' ') && cnt != 0) {
                char* push2 = (char*)(malloc(sizeof(char) * BUF_SIZE));
                push2 = strcut(buf, i - cnt, cnt, push2);
                if (tryPop(prev, push2) == 0) {
                    stackLOl = push(stackLOl, 42, push2);
                }
                cnt = 0;
            } else {
                if ((buf[i] != EOF && buf[i] != '\n' && buf[i] != ' ')) {
                    cnt++;
                }
            }
            if (buf[i] == EOF) {
                break;
            }
        }
    }
    // из массива выкидываем элементы по одному и если этот эемент на удаление то удаляем его
    struct stack* ans = (struct stack*)(malloc(2 * sizeof(struct stack)));
    ans->size = 2;
    ans->pos = 0;

    while (stackLOl->pos != 0) {
        char* a = pop(stackLOl).str;
        if (tryPop(post, a) == 0) {
            ans =  push(ans, 42, a);
        }
    }

    // ans ревёрсит массив
    while (ans->pos != 0) {
        printf("%s ", pop(ans).str);
    }

    free(stackLOl);
    free(prev);
    free(ans);
    free(post);
    fclose(file);
    return 0;
}
