#include "process.h"

void process::insert_process(int id, int *fd_in, int *fd_out) {
    struct process_info temp;
    temp.id = id;
    temp.fd_in = fd_in;
    temp.fd_out = fd_out;
    temp.valid = true;
    processes.push_back(temp);
}

void process::close_process(int id, int status) {
    for(auto x : processes) {
        if(x.valid && x.id == id) {
            x.status = status;
            x.valid = false;
            if(x.fd_in) {
                close(x.fd_in[0]);
            }
            break;
        }
    }
}