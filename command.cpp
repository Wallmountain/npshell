#include "command.h"

bool command::isempty() {
    return cmd_now == cmd_count;
}

bool command::check(char *input) {
    std::string str(input);
    std::string token;
    std::stringstream read_str(str);

    const std::regex STRING ("[A-Za-z0-9\\.\\-:]+");
    const std::regex SYMBOL ("[|>]");
    const std::regex NUMPIPE ("[|!][1-9][0-9]*");

    int type = 0, last_type = 0;

    while(read_str >> token) {
        if(format_check(token, STRING)) {
            type = TOKEN::STRING;
        } else if(format_check(token, SYMBOL)) {
            type = TOKEN::SYMBOL;
            if(last_type != TOKEN::STRING)
                return false;
        } else if(format_check(token, NUMPIPE)) {
            type = TOKEN::NUMPIPE;
            if(last_type != TOKEN::STRING)
                return false;
            type = 0;
        } else {
            return false;
        }
        last_type = type;
    }

    return last_type != TOKEN::SYMBOL;
}

bool command::format_check(const std::string& str, const std::regex& format) {
    std::smatch smt;

    if(!std::regex_search(str, smt, format))
        return false;

    return smt[0].str().length() == str.length();
}

int command::read_string(char *input) {
    std::string str(input);
    std::string token;
    std::stringstream read_str(str);
    int instrution_end = 0;

    if(!check(input)) {
        std::vector<std::string> tokens;       
        while(read_str >> token)
            tokens.push_back(token);
        if(tokens.size()) {
            cmds.push_back(tokens);
            ++cmd_count;
        }
    }

    while(read_str >> token) {
        instrution_end = 0;
        ++cmd_count;

        std::vector<std::string> tokens;
        tokens.push_back(token);
        while(!instrution_end && read_str >> token) {
            if(token[0] == '|' || token[0] == '!') {
                if(token.length() == 1)
                    instrution_end = 1;
                else
                    instrution_end = 2;
            } else if(token[0] == '>') {
                cmds.push_back(tokens);
                
                while(tokens.size())
                    tokens.pop_back();

                tokens.push_back(token);
                read_str >> token;

                ++cmd_count;
                instrution_end = 0;
            }
            if(!instrution_end)
                tokens.push_back(token);
        }

        cmds.push_back(tokens);
        if(instrution_end)
            cmds.push_back({token});

        if(instrution_end == 2)
            cmds.push_back({});
        cmd_count += instrution_end;
    }
    if(instrution_end < 2) {
        cmds.push_back({});
        ++cmd_count;
    }
    return 1;
}

void command::print(){
    std::cout << cmd_count << std::endl;
    for(auto cmd : cmds) {
        for(auto token : cmd) {
            std::cout << token << " ";
        }
        std::cout << std::endl;
    }
}

char **command::get_next_cmd() {
    if(isempty())
        return NULL;
    if(cmd_now + 2 <= cmd_count && cmds[cmd_now + 1].size()== 0)
        ++instrusion_count;
    return change_cmd_type(cmds[cmd_now++]);
}

unsigned command::get_instruction_count() {
    return instrusion_count;
}

char **command::change_cmd_type(std::vector<std::string> cmd) {
    char **res = new char*[cmd.size() + 1];
    int i = 0;
    for(std::string x : cmd) {
        res[i] = new char[x.length()];
        strcpy(res[i++], x.c_str());
    }
    res[i] = NULL;
    return res;
}

bool command::tail_of_instruction() {
    return cmd_now + 1 == cmd_count;
}