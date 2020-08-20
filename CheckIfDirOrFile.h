#ifndef _CheckIfDirOrFile_H_
#define _CheckIfDirOrFile_H_

#include <stdbool.h>
void CheckIfDirOrFile(char *dirPath, bool opt_i, bool opt_l, bool opt_R, bool needTitles);
bool checkIfStringRequiresQuotations(char *string);

#endif