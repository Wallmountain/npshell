#include "npshell.h"

using namespace std;

class process process;
int fork_count = 0;

/* deal with the end of child process */
void child_handler(int sig) { 
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        --fork_count;
        process.close_process(pid, status);
    }
    return;
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, child_handler);
    char command[max_command_len + 1];
    int old_out = dup(1); // store stdout
    setenv("PATH", "bin:.", 1); // set PATH to bin/ and ./
    class command cmd;
    class pipe_box pipe_box;
    std::cout << "% ";

    while(cin.getline(command, max_command_len)) {
        cmd.read_string(command); // tokenize the command and check if there is any error
        int *fd_in, *fd_out, *fd_err;
        bool num_pipe = false;
        fd_in = fd_out = fd_err = NULL;
        while(!cmd.isempty()) {
            unsigned int instruction_count =cmd.get_instruction_count();
            int number = 0;
            num_pipe = false;
            char **event = cmd.get_next_cmd(); // get instruction
            fd_in = fd_out; // last output will be the input of current instruction
            if(!*event) { //no instruction now
                fd_out = fd_err = NULL;
                continue;
            }

            if(!fd_in) { // if no input, check if there is number pipe for current instruction
                fd_in = pipe_box.get_fd(instruction_count);
                pipe_box.clean(instruction_count);
                if(fd_in)
                    close(fd_in[1]);
            }

            char **in_out_operator = cmd.get_next_cmd(); // operator for current instruction
            if(!*in_out_operator) {
                fd_out = fd_err = NULL;
            } else {
                int len = strlen(*in_out_operator);
                if(in_out_operator[0][0] == '>') {
                    fd_out = fd_err = NULL;
                } else if(len == 1) { // SYMBOL '|'
                    fd_out = pipe_box.get_new_fd();
                } else { // NUMBER PIPE
                    num_pipe = true;
                    number = stoi(&(*in_out_operator)[1]);

                    fd_out = pipe_box.get_fd(instruction_count, number); 
                    if(!fd_out) { // if no inherent number pipe
                        fd_out = pipe_box.get_new_fd();
                        pipe_box.set_fd(instruction_count, number, fd_out);
                    }
                }
                switch((*in_out_operator)[0]) {
                    case '|':
                        fd_err = NULL;
                        break;
                    case '!':
                        fd_err = fd_out;
                        break;
                }
            }

            int build_in = 0;
            if((build_in = check_buildin_command(event)) == -1) // exit
                return 0;
            else if(build_in)
                continue;

            int filefd  = 0;
            if(*in_out_operator && in_out_operator[0][0] == '>') {
                filefd = open(in_out_operator[1], O_WRONLY | O_CREAT| O_TRUNC, 0644);
            }

            int childpid;
            while(fork_count > max_fork_num) { // limit of the fork number
                usleep(100000);
            }
            if((childpid = fork()) == -1) {
                exit(0);
            }
            ++fork_count;

            if(childpid > 0) {
                /* parent process */
                if(!num_pipe && fd_out) {
                    close(fd_out[1]);
                }
                if(filefd > 0) {
                    close(filefd);
                }
                
                process.insert_process(childpid, fd_in, fd_out);

                if(!fd_out) { // no fd_out represent that the end of the instruction 
                    int status;
                    waitpid(childpid, &status, 0);
                    --fork_count;
                    usleep(10000);
                    process.close_process(childpid, status);
                }
            } else if(!childpid) {
                /* child process  */
                
                // close all pipe in NUMBER PIPE
                if(num_pipe) 
                    pipe_box.close_all(instruction_count, number);
                else   
                    pipe_box.close_all();

                if(fd_in) {
                    dup2(fd_in[0], 0);
                }
                if(fd_out) {
                    dup2(fd_out[1], 1);
                    close(fd_out[0]);
                }
                if(fd_err) {
                    dup2(fd_err[1], 2);
                }
                if(filefd) {
                    dup2(filefd, 1);
                } 

                if(execvp(event[0], event) == -1) {
                    std::cerr << "Unknown command: [" << event[0] << "]." << std::endl;
                }

                if(fd_out)
                    close(fd_out[1]);
                if(fd_in) {
                    char buffer[max_command_len];
                    while(std::cin.getline(buffer, max_command_len));
                    close(fd_in[0]);
                }
                exit(0);
            }
        }
        usleep(10000);
        std::cout << "% " << std::flush;
    }

    return 0;
}
