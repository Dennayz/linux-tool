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

#include "CheckIfDirOrFile.h"
#include "HandleOptionsForPrint.h"
#include "traverseDirectories.h"

#define LEN_OF_DIRECTPATH 4096

bool checkIfStringRequiresQuotations(char *string)
{
    int strLen = strlen(string);
    bool hasQuotes = false;
    for (int j = 0; j < strLen; j++)
    {
        if (string[j] == ' ' || string[j] == '!' || string[j] == '$' || string[j] == 39 || string[j] == '&' || string[j] == '^' || string[j] == '(' || string[j] == ')' || string[j] == '"' || string[j] == '#' || string[j] == '*')
        {
            hasQuotes = true;
            break;
        }
    }
    return hasQuotes;
    
}


void CheckIfDirOrFile(char *dirPath, bool opt_i, bool opt_l, bool opt_R, bool needTitles)
{

    char DirecPath[LEN_OF_DIRECTPATH];
    char copyDirecPath[LEN_OF_DIRECTPATH];
    memset(DirecPath, 0, sizeof(DirecPath));
    memset(copyDirecPath, 0, sizeof(copyDirecPath));
    strncpy(DirecPath, dirPath, strlen(dirPath));
    bool isFile = false;
    struct stat statBuf;

    //used for checking if closedir was successful or not
    int n;
    //keeps track of total files that has been sorted by scandir
    int num;
    //helps assist in printing out the sorted files in ascending order
    int diff = 0;
    //help indicates that scandir runs only once in each directory
    bool sortOnce = false;
    
    struct dirent *dp = NULL;
    DIR *directory = NULL;

    //this struct dirent is needed for scandir becauise it required **
    struct dirent **p = &dp;

    directory = opendir(DirecPath);

    if (directory == NULL) {
        //saves the fileName or directory name
        strncpy(copyDirecPath, DirecPath, strlen(DirecPath));
        //dirname grabs the path only
        char *dname = dirname(DirecPath);
        //for current directory path that we are currently in
        if ((stat(copyDirecPath, &statBuf) == 0) && (S_ISREG(statBuf.st_mode)) && strncmp(dname, ".", 1) == 0) {
            bool needQuotes = checkIfStringRequiresQuotations(copyDirecPath);
            HandleOptionsForPrint(dname, copyDirecPath, opt_i, opt_l, isFile, needQuotes);
        }
        //for a specific directory path
        else if (stat(copyDirecPath, &statBuf) == 0 && S_ISREG(statBuf.st_mode)) {
            bool needQuotes = checkIfStringRequiresQuotations(copyDirecPath);
            //basename grabs the file name attach to a path
            char *bname = basename(copyDirecPath);
            isFile = true;
            HandleOptionsForPrint(dname, bname, opt_i, opt_l, isFile, needQuotes);
        }
        //file or directory is not found!
        else {
            printf("File or Directory not Found: %s\n", DirecPath);
        }
    }
    else if (directory) {
        if (needTitles) {
            bool needQuotes = checkIfStringRequiresQuotations(DirecPath);
            if (needQuotes) {
                printf("\n'%s':\n", DirecPath);
            }
            else {
                printf("\n%s:\n", DirecPath);
            }   
        }
        
        while ((dp = readdir(directory)) != NULL) {
            if (!sortOnce) {
                num = scandir(DirecPath, &p, NULL, alphasort);
                diff = num;
                sortOnce = true;
            }
            //checks if directory or file is hidden
            if (p[num-diff]->d_name[0] != '.') {
                bool needQuotes = checkIfStringRequiresQuotations(p[num-diff]->d_name);
                HandleOptionsForPrint(DirecPath, p[num-diff]->d_name, opt_i, opt_l, isFile, needQuotes);
            }
            free(p[num-diff]);
            diff--;  
        }

        if (opt_R) {
            traverseDirectories(directory, opt_i, opt_l, opt_R, needTitles, DirecPath);
        }

        free(p);
        n = closedir(directory);

        if (n == -1) {
            perror("Failed to close Directory!\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        perror("Directory not found!\n");
        exit(EXIT_FAILURE); 
    }


}
