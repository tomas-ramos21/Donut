#include "tools/validation.h"
#include "sys/stat.h"
#include "const/const.h"

int
validate_donut_repo(void)
{
        int ret;
        struct stat info = {0};

        ret = stat(DONUT_FOLDER_RELATIVE, &info);
        ret &= stat(DATA_FOLDER_RELATIVE, &info);

        return ret;
}
