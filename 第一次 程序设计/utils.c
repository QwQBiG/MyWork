// 工具模块实现

#include "utils.h"

// 计算视觉宽度（处理中文）

/*
直接 printf 不行，因为中文字符在 UTF-8 编码下占 3 个字节，
但显示宽度通常只占 2 个英文字符位。
所以咱写了一个 zbh_get_visual_width 函数，遍历字符串判断 ASCII 码，
如果是中文则视觉宽度+2，英文+1，然后根据总宽度动态补空格。
*/
int zbh_get_visual_width(const char *zbh_str) {
    int zbh_len = 0;
    int zbh_i = 0;
    while (zbh_str[zbh_i] != '\0') {
        if (zbh_str[zbh_i] == '\033' && zbh_str[zbh_i+1] == '[') {
            zbh_i += 2;
            while (zbh_str[zbh_i] != '\0' && zbh_str[zbh_i] != 'm') {
                zbh_i++;
            }
            if (zbh_str[zbh_i] == 'm') zbh_i++;
            continue;
        }
        
        if ((unsigned char)zbh_str[zbh_i] < 128) {
            zbh_len += 1;
            zbh_i++;
        } else {
            zbh_len += 2;
            if ((unsigned char)zbh_str[zbh_i] >= 0xE0) {
                zbh_i += 3;
            } else {
                zbh_i += 2;
            }
        }
    }
    return zbh_len;
}

void zbh_print_aligned(const char *zbh_str, int zbh_width) {
    printf("%s", zbh_str);
    int zbh_visual_len = zbh_get_visual_width(zbh_str);
    int zbh_padding = zbh_width - zbh_visual_len;
    for (int zbh_i = 0; zbh_i < zbh_padding; zbh_i++) {
        printf(" ");
    }
}

void zbh_print_highlight(const char *zbh_str, const char *zbh_keyword, int zbh_width) {
    if (zbh_keyword == NULL || strlen(zbh_keyword) == 0) {
        zbh_print_aligned(zbh_str, zbh_width);
        return;
    }
    
    int zbh_visual_len = zbh_get_visual_width(zbh_str);
    char zbh_temp[200];
    strcpy(zbh_temp, zbh_str);
    char *zbh_ptr = strstr(zbh_temp, zbh_keyword);
    
    if (zbh_ptr != NULL) {
        int zbh_before_len = zbh_ptr - zbh_temp;
        char zbh_before[200];
        strncpy(zbh_before, zbh_temp, zbh_before_len);
        zbh_before[zbh_before_len] = '\0';
        const char *zbh_after = zbh_ptr + strlen(zbh_keyword);
        
        printf("%s", zbh_before);
        printf("\033[1;31m%s\033[0m", zbh_keyword);
        printf("%s", zbh_after);
    } else {
        printf("%s", zbh_temp);
    }
    
    int zbh_padding = zbh_width - zbh_visual_len;
    for (int zbh_i = 0; zbh_i < zbh_padding; zbh_i++) {
        printf(" ");
    }
}

int zbh_isRunningEvent(char *zbh_eventName) {
    if (strstr(zbh_eventName, "跑") != NULL || strstr(zbh_eventName, "竞走") != NULL) return 1;
    if (strstr(zbh_eventName, "米") != NULL) return 1;
    return 0;
}

int zbh_getScoreByRank(int zbh_rank) {
    int zbh_scores[] = {0, 10, 8, 6, 4, 2};
    if (zbh_rank >= 1 && zbh_rank <= 5) return zbh_scores[zbh_rank];
    return 0;
}

void zbh_swapAthlete(zbh_Athlete *zbh_a, zbh_Athlete *zbh_b) {
    zbh_Athlete zbh_temp = *zbh_a;
    *zbh_a = *zbh_b;
    *zbh_b = zbh_temp;
}

void zbh_swapCollege(zbh_CollegeStat *zbh_a, zbh_CollegeStat *zbh_b) {
    zbh_CollegeStat zbh_temp = *zbh_a;
    *zbh_a = *zbh_b;
    *zbh_b = zbh_temp;
}