#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ShowInodeNumber.h"

#define LEN_OF_FILE_NAME        4096

void AddBackSlashForDirPath(char *dirPath, char *fileName)
{
    int lengthOfDir = strlen(dirPath);
    int lengthOfFile = strlen(fileName);

    if (dirPath[lengthOfDir-1] != '/') {
        //adds a slash to the end of directory path if not specified
        strncpy(dirPath + lengthOfDir, "/", 1);
        lengthOfDir++;
    }
    // deals with ./*
    if (fileName[0] == '.' && fileName[1] == '/' && strlen(dirPath) > 2) {
        return;
    }
    // going backwards dealing with ../*
    if(dirPath[0] == '.' && dirPath[1] == '.' && dirPath[2] == '/' && lengthOfFile > 3) {
        return;
    }
    //add the directory path and the file name together
    strncpy(dirPath + lengthOfDir, fileName, lengthOfFile);

}

void ShowInodeNumber(char *dirName, char *fileName, bool isFile, bool needQuotes)
{
    struct stat statBuf;
    char buffer[LEN_OF_FILE_NAME];

    memset(buffer, 0, LEN_OF_FILE_NAME);
    strncpy(buffer, dirName, strlen(dirName));

    //add back slash if needed for full path
    AddBackSlashForDirPath(buffer, fileName);
    
    if (lstat(buffer, &statBuf) == -1) {
        perror("Failed lStat!\n");
        exit(EXIT_FAILURE);
    }

    //check if tis a file than print with directory path + file Name
    if (isFile) {
        //if special cases, add quotes around file or directory name
        if (needQuotes) {
            printf("%10lu '%s'\n", statBuf.st_ino, buffer);
        }
        else {
            printf("%10lu %s\n", statBuf.st_ino, buffer);
        }
        
    }
    //if not a file than just print fileName alone
    else {
        if (needQuotes) {
            printf("%10lu '%s'\n", statBuf.st_ino, fileName);
        }
        else {
            printf("%10lu %s\n", statBuf.st_ino, fileName);
        }
       
    }
}

// Handles printing files with directorypath + fileName or just fileName alone
void SimplePrintFile(char *dirName, char *fileName, bool isFile, bool needQuotes)
{
    char buffer[LEN_OF_FILE_NAME];

    memset(buffer, 0, LEN_OF_FILE_NAME);
    strncpy(buffer, dirName, strlen(dirName));

    AddBackSlashForDirPath(buffer, fileName);

    if (isFile) {
        if (needQuotes) {
            printf("'%s'\n", buffer);
        }
        else {
            printf("%s\n", buffer);
        }
    }
    else {
        if (needQuotes) {
            printf("'%s'\n", fileName);
        }
        else {
            printf("%s\n", fileName);
        }
        
    }
}