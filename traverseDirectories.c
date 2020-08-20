#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <limits.h>

#include "CheckIfDirOrFile.h"
#include "traverseDirectories.h"

void traverseDirectories(DIR * dir, bool opt_i, bool opt_l, bool opt_R, bool needTitles, char *stringPath) {
    struct stat sb;
    struct dirent *dp = NULL;
    char pathBuffer[PATH_MAX];

    // rewinddir has no return values, just goes through all entries again 
    rewinddir(dir);

    // go through subdirectories
    while ((dp = readdir(dir)) != NULL) {
        if(dp->d_name[0] != '.') {
            memset(pathBuffer, 0, PATH_MAX);
            strncpy(pathBuffer, stringPath, PATH_MAX);

            basename(pathBuffer);

            strcat(pathBuffer, "/");
            strcat(pathBuffer, dp->d_name);

            if (lstat(pathBuffer, &sb) != -1) {
                if (S_ISDIR(sb.st_mode) != 0) {
                    needTitles = true;
                    CheckIfDirOrFile(pathBuffer, opt_i, opt_l, opt_R, needTitles);
                }
            }
            else {
                perror("");
                exit(EXIT_FAILURE);
            }

        }
    }
}
