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
#include <stdbool.h>
#include <libgen.h>
#include <fcntl.h> 
#include <limits.h>

#include "LongListFormat.h"
#include "CheckIfDirOrFile.h"
#include "ShowInodeNumber.h"

#define LPERM_MAX 14
#define FILETIME_MAX 32
#define INO_MAX 32


// cited from professor with modification
static void getAndPrintGroup(gid_t grpNum, char *groupName)
{
    struct group *grp = getgrgid(grpNum);

    if (grp)
    {
        memset(groupName, 0, PATH_MAX);
        strncpy(groupName, grp->gr_name, strlen(grp->gr_name));
    }
    else
    {
        printf("No group name for %u found\n", grpNum);
    }
}

// cited from professor with modification
static void getAndPrintUserName(uid_t uid, char *userName)
{
    struct passwd *pw = getpwuid(uid);

    if (pw)
    {
        memset(userName, 0, PATH_MAX);
        strncpy(userName, pw->pw_name, strlen(pw->pw_name));
    }
    else
    {
        perror("Hmm not found???");
        printf("No name found for %u\n", uid);
    }
}


void LongListFormat(char *pathToDirectory, char *baseName, bool isFile, bool opt_i, bool needQuotes) 
{
    struct stat sb;
    memset(&sb, 0, sizeof(struct stat));

    // used to convert the time for file
    struct tm *fileTime;
    
    // used to store inode number
    char storeIno[INO_MAX];
    memset(storeIno, 0, INO_MAX);

    // used to store directory name path
    char dirBuf[PATH_MAX];
    memset(dirBuf, 0, PATH_MAX);

    // holds the symlink with the arrow
    char beforeSymlink[PATH_MAX];
    // the actual link
    char linkName[PATH_MAX];
    memset(beforeSymlink, 0, PATH_MAX);
    memset(linkName, 0, PATH_MAX);
    
    strncpy(dirBuf, pathToDirectory, strlen(pathToDirectory));

    AddBackSlashForDirPath(dirBuf, baseName);

    // use lstat instead of stat to check file characteristics
    // lstat is the same as stat but also gives information on symlinks
    if (lstat(dirBuf, &sb) == -1)
    {
        perror("");
        exit(EXIT_FAILURE);
    }

    // if user specifies -i then we need to also print -i
    if (opt_i)
    {
        snprintf(storeIno, INO_MAX, "%10lu ", sb.st_ino);
    }

    // check the representation of the file -> directory, file, or link
    char fileType;

    // manpages for  stat -> S_IFMT is bitmask for a file type
    switch (sb.st_mode & S_IFMT)
    {
    case S_IFDIR:
        fileType = 'd';
        break;
    case S_IFLNK:
        fileType = 'l';
        // readlink man pages cited from https://linux.die.net/man/3/readlink, with little modification
        // beforeSymlink holds the " -> " right now then readlink adds the link to the end of " -> "
        snprintf(beforeSymlink, strlen(" -> ") + 1, " -> ");
        ssize_t linkLength;
        linkLength = readlink(dirBuf, linkName, sizeof(linkName) - 1);
        // error handling : https://www.informit.com/articles/article.aspx?p=23618&seqNum=12
        if (linkLength == -1) {
            if (errno == EINVAL) {
                fprintf(stderr, "myls: %s: ", linkName);
                perror("");
            }
            else {
                fprintf(stderr, "myls: %s: ", linkName);
                perror("");
            }
            exit(EXIT_FAILURE);
        }
        // check if linkname needs quotes
        bool linkNeedQuotes = checkIfStringRequiresQuotations(linkName);
        // add the quotes to the name and cat it to full name with "->"
        //https://stackoverflow.com/questions/26432589/appending-single-quotes-to-strings-in-c
        if (linkNeedQuotes) {
            ssize_t len = strlen(linkName);
            memmove(linkName + 1, linkName, len);
            linkName[0] = linkName[++len] = '\'';
            linkName[++len] = '\0';
            strcat(beforeSymlink, linkName);
        } 
        else {
           strcat(beforeSymlink, linkName);
        }

        break;
    default:
        fileType = '-';
        break;
    }

    // nice format cited from https://stackoverflow.com/questions/31449688/what-is-s-ifmt-in-unix-system-programming
    // modified code and rewrote with same format
    char *fileUsablilty = (char *)malloc(sizeof(char) * LPERM_MAX);
    memset(fileUsablilty, 0, sizeof(char) * LPERM_MAX);
    fileUsablilty[0] = fileType;
    fileUsablilty[1] = (sb.st_mode & S_IRUSR) ? 'r' : '-';
    fileUsablilty[2] = (sb.st_mode & S_IWUSR) ? 'w' : '-';
    fileUsablilty[3] = (sb.st_mode & S_IXUSR) ? 'x' : '-';
    fileUsablilty[4] = (sb.st_mode & S_IRGRP) ? 'r' : '-';
    fileUsablilty[5] = (sb.st_mode & S_IWGRP) ? 'w' : '-';
    fileUsablilty[6] = (sb.st_mode & S_IXGRP) ? 'x' : '-';
    fileUsablilty[7] = (sb.st_mode & S_IROTH) ? 'r' : '-';
    fileUsablilty[8] = (sb.st_mode & S_IWOTH) ? 'w' : '-';
    fileUsablilty[9] = (sb.st_mode & S_IXOTH) ? 'x' : '-';

    // gets the time of file
    fileTime = localtime(&sb.st_mtime);
    char *dateString = (char *)malloc(sizeof(char) * FILETIME_MAX);

    //time and dates from https://downloads.haskell.org/~ghc/6.12.1/docs/html/libraries/time-1.1.4/Data-Time-Format.html
    // formats the time in appropriate order in a string
    // %b - National abbreviated month name
    // %e - Day of the month, space-padded ( 1-31)
    // %Y - Year in full
    // %R - 24-hour HH:MM time, equivalent to %H:%M
    strftime(dateString, FILETIME_MAX, "%b %e %Y %R", fileTime);

    // get the username of file using st_uid
    char *getUserName = (char *)malloc(PATH_MAX);
    getAndPrintUserName(sb.st_uid, getUserName);

    // get the group name of file using st_gid
    char *getGroupName = (char *)malloc(PATH_MAX);
    getAndPrintGroup(sb.st_gid, getGroupName); 

    if (opt_i) {
        printf("%*s", -15, storeIno);
    }
    else {
        printf("%s", storeIno);
    }
    printf("%s ", fileUsablilty);
    printf("%4lu ", sb.st_nlink);
    printf("%s  ", getUserName);
    printf("%s  ", getGroupName);
    printf("%10lu ", sb.st_size);
    printf("%s ", dateString);

    if (isFile) {
        if (needQuotes) {
            printf("'%s'", dirBuf);
        }
        else {
            printf("%s", dirBuf);
        }
    }
    else {
        if (needQuotes) {
            printf("'%s'", baseName);
        }
        else {
            printf("%s", baseName);
        }
    }
    printf("%s\n", beforeSymlink);

    // free everything that is allocated
    free(getUserName);
    free(dateString);
    free(getGroupName);
    free(fileUsablilty);

}


