#ifndef _TRAVERSEDIRECTORIES_H_
#define _TRAVERSEDIRECTORIES_H_

#include <dirent.h>
#include <stdbool.h>

void traverseDirectories(DIR * dir, bool opt_i, bool opt_l, bool opt_R, bool needTitles, char *stringPath);

#endif