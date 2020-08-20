#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "CheckIfDirOrFile.h"

//sorts file and directory array
static void lexSort(char **files, int sizeOfFiles)
{   

    char *tmp = NULL;
    int i;
    for (i = 0; i < sizeOfFiles; i++) {
        for (int j = i + 1; j < sizeOfFiles; j++) {
            if (strcmp(files[i], files[j]) > 0) {
                tmp = files[i];
                files[i] = files[j];
                files[j] = tmp;
            }
        }
    }

}

//merges two arrays into one in order from files to directories
static void mergeArr(char **arr1, char **arr2, char **merge, int size1, int size2)
{   

    int i = 0;
    while (i < size1) {
        merge[i] = arr1[i];
        i++;
    }

    for (int j = 0; j < size2; j++) {
        merge[i] = arr2[j];
        i++;
    }

}

int main(int numArgs, char* args[])
{
    bool option_i = false;
    bool option_l = false;
    bool option_R = false;
    bool needTitles = false;
    char *optionArg = args[1];
    int argCounter = 1;
    int howManyArgs = 0;

    struct stat statBuf;

    //checking for options i, l, R starting with '-'
    for (int j = 1; j <  numArgs; j++) {
        if (strncmp(&optionArg[0], "-", 1) == 0) {
            for (int i = 1; i < strlen(optionArg); i++) {
                if (strncmp(&optionArg[i], "i", 1) == 0) {
                    option_i = true;
                }
                else if (strncmp(&optionArg[i], "l", 1) == 0) {
                    option_l = true;
                }
                else if (strncmp(&optionArg[i], "R", 1) == 0) {
                    option_R = true;
                }
                else {
                    printf("The following option: %s is not allowed. Please use i, l or R only!\n", optionArg);
                    exit(EXIT_FAILURE);
                }
            }
        }
        else {
            break;
        }
        //increment the counter to the next option if its there
        argCounter++;
        optionArg = args[argCounter];
    }

    //check from after the options part of the input to see how many file
    //folder related arguments are inputted
    for (int i = argCounter; i < numArgs; i++) {
        howManyArgs++;
    }

    //if theres more than one argument for files or directorys
    //then we need to print out their names
    if (howManyArgs > 1) {
        needTitles = true;
    }

    char *arrSortForDir[howManyArgs];
    char *arrSortForFile[howManyArgs];
    memset(arrSortForFile, 0, howManyArgs);
    memset(arrSortForDir, 0, howManyArgs);

    int arrSortFile_index = 0;
    int arrSortDir_index = 0;

    //checks whether each input is a file or a directory and sort them
    for (int i = argCounter; i < numArgs; i++) {
        if (stat(args[i], &statBuf) == 0 && S_ISREG(statBuf.st_mode)) {
            arrSortForFile[arrSortFile_index] = args[i];
            arrSortFile_index++;
        }
        else if (stat(args[i], &statBuf) == 0 && S_ISDIR(statBuf.st_mode)) {
            arrSortForDir[arrSortDir_index] = args[i];
            arrSortDir_index++;
        }
        else {
            fprintf(stderr, "myls: cannot access '%s': ", args[i]);
            perror("");
        }
        
    }

    //Sorts the array with only files
    lexSort(arrSortForFile, arrSortFile_index);
    //Sorts the array with only directory names
    lexSort(arrSortForDir, arrSortDir_index);
   
    int totalSize = arrSortFile_index + arrSortDir_index;

    char *mergedSortArr[totalSize];
    memset(mergedSortArr, 0, totalSize);

    //calls function to merge both arrays into one
    mergeArr(arrSortForFile, arrSortForDir, mergedSortArr, arrSortFile_index, arrSortDir_index);
    
    char *pathArg;

    //checks to see if theres only one argument
    if (argCounter - numArgs == 0) {
        CheckIfDirOrFile(".", option_i, option_l, option_R, needTitles);
    }
    else {
        int n = 0;
        while (n < totalSize) {
            pathArg = mergedSortArr[n];
            CheckIfDirOrFile(pathArg, option_i, option_l, option_R, needTitles);
            n++;
        }
        
    }

    return 0;
    
    
}


