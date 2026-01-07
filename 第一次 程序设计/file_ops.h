// 文件操作头文件

#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "common.h"

void zbh_initGameRecords();
void zbh_loadFromFile();
void zbh_saveToFile();
void zbh_backupData();
void zbh_batchImport();
void zbh_exportToCSV();
void zbh_generateCertificate();

#endif