#ifndef _ShowInodeNumber_H_
#define _ShowInodeNumber_H_

#include <stdbool.h>
void ShowInodeNumber(char *dirName, char *fileName, bool isFile, bool needQuotes);

void SimplePrintFile(char *dirName, char *fileName, bool isFile, bool needQuotes);

void AddBackSlashForDirPath(char *dirPath, char* fileName);

#endif