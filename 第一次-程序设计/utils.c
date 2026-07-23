// 工具模块实现

#include "utils.h"

// 计算视觉宽度（处理中文）

/*
直接 printf 不行，因为中文字符在 UTF-8 编码下占 3 个字节，
但显示宽度通常只占 2 个英文字符位。
所以咱写了一个 get_visual_width 函数，遍历字符串判断 ASCII 码，
如果是中文则视觉宽度+2，英文+1，然后根据总宽度动态补空格。
*/
int get_visual_width(const char *str) {
    int len = 0;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\033' && str[i+1] == '[') {
            i += 2;
            while (str[i] != '\0' && str[i] != 'm') {
                i++;
            }
            if (str[i] == 'm') i++;
            continue;
        }
        
        if ((unsigned char)str[i] < 128) {
            len += 1;
            i++;
        } else {
            len += 2;
            if ((unsigned char)str[i] >= 0xE0) {
                i += 3;
            } else {
                i += 2;
            }
        }
    }
    return len;
}

void print_aligned(const char *str, int width) {
    printf("%s", str);
    int visual_len = get_visual_width(str);
    int padding = width - visual_len;
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
}

void print_highlight(const char *str, const char *keyword, int width) {
    if (keyword == NULL || strlen(keyword) == 0) {
        print_aligned(str, width);
        return;
    }
    
    int visual_len = get_visual_width(str);
    char temp[200];
    strcpy(temp, str);
    char *ptr = strstr(temp, keyword);
    
    if (ptr != NULL) {
        int before_len = ptr - temp;
        char before[200];
        strncpy(before, temp, before_len);
        before[before_len] = '\0';
        const char *after = ptr + strlen(keyword);
        
        printf("%s", before);
        printf("\033[1;31m%s\033[0m", keyword);
        printf("%s", after);
    } else {
        printf("%s", temp);
    }
    
    int padding = width - visual_len;
    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
}

int isRunningEvent(char *eventName) {
    if (strstr(eventName, "跑") != NULL || strstr(eventName, "竞走") != NULL) return 1;
    if (strstr(eventName, "米") != NULL) return 1;
    return 0;
}

int getScoreByRank(int rank) {
    int scores[] = {0, 10, 8, 6, 4, 2};
    if (rank >= 1 && rank <= 5) return scores[rank];
    return 0;
}

void swapAthlete(Athlete *a, Athlete *b) {
    Athlete temp = *a;
    *a = *b;
    *b = temp;
}

void swapCollege(CollegeStat *a, CollegeStat *b) {
    CollegeStat temp = *a;
    *a = *b;
    *b = temp;
}