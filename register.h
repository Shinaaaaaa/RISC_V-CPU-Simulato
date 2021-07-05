//
// Created by lenovo on 2021/6/28.
//

#ifndef RISC_V_REGISTER_H
#define RISC_V_REGISTER_H

#include <iostream>
using namespace std;

class Register{
    unsigned int x[32]{};

public:
    Register(){
        for (int i = 0 ; i <= 31 ; ++i) x[i] = 0;
    }

    unsigned int &operator[](unsigned int n){
        return x[n];
    }

    void show(){
        for (int i = 0 ; i <= 31 ; ++i) cout << x[i] << " ";
        cout << endl;
    }
};
#endif //RISC_V_REGISTER_H
