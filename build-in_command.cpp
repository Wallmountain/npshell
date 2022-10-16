#include "build-in_command.h"

int check_buildin_command(char **cmd) {
    int len = 0;
    string_len(cmd, len);
    if(!len)
        return 0;
    if(!strcmp(cmd[0],"exit"))
        return -1;
    
    if(!strcmp(cmd[0], "setenv") && len == 3) {
        setenv(cmd[1], cmd[2], 1);
        return 1;
    } else if(!strcmp(cmd[0], "printenv") && len == 2) {
        char *temp = getenv(cmd[1]);
        if(temp)
            std::cout << temp << std::endl;
        return 1;
    }
    return 0;
}