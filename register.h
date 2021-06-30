//
// Created by lenovo on 2021/6/28.
//

#ifndef RISC_V_REGISTER_H
#define RISC_V_REGISTER_H

#include <iostream>
using namespace std;

class Register{
    unsigned int x[32]{};
    unsigned int pc;

public:
    Register(){
        for (int i = 0 ; i <= 31 ; ++i) x[i] = 0;
        pc = 0;
    }

    unsigned int &operator[](unsigned int n){
        return x[n];
    }

    unsigned int get_pc() const{
        return pc;
    }

    void set_pc(unsigned int pos){
        pc = pos;
    }

    void move_pc(unsigned int offset){
        pc += offset;
    }
};
#endif //RISC_V_REGISTER_H
