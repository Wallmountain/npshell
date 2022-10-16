#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "command.h"
#include "pipe.h"
#include "process.h"
#include "build-in_command.h"

#define max_fork_num 100
#define max_command_len 15000