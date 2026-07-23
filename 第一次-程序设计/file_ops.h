// 文件操作头文件

#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "common.h"

void initGameRecords();
void loadFromFile();
void saveToFile();
void backupData();
void batchImport();
void exportToCSV();
void generateCertificate();

#endif