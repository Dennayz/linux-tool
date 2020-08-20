#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "HandleOptionsForPrint.h"
#include "ShowInodeNumber.h"
#include "LongListFormat.h"

#include <stdbool.h>

void HandleOptionsForPrint(char *pathName, char *fileName, bool opt_i, bool opt_l, bool isFile, bool needQuotes)
{
    //checks if -l was inputted
    if (opt_l) {
        LongListFormat(pathName, fileName, isFile, opt_i, needQuotes);
    }
    //checks if -i was inputed
    else if (opt_i) {
        ShowInodeNumber(pathName, fileName, isFile, needQuotes);
    }
    //just print out the file or name
    else {
        SimplePrintFile(pathName, fileName, isFile, needQuotes);
    }
   
}