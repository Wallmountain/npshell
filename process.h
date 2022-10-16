#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <unistd.h>

#define finish 1

struct process_info {
    int *fd_in, *fd_out;
    int id;
    int status;
    int valid;
};

class process {
    public :
        process(){};
        void insert_process(int id, int *fd_in, int *fd_out);
        void close_process(int id, int status);

    private :
        std::vector<struct process_info> processes;
};
#endif
