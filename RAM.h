//
// Created by lenovo on 2021/6/28.
//
#ifndef RISC_V_RAM_H
#define RISC_V_RAM_H
#include <iostream>
using namespace std;

extern Register re;

class RAM{
    unsigned int memory[500001]{};

public:
    RAM(){
        for (int i = 0 ; i <= 500000 ; ++i) memory[i] = 0;
    }

    unsigned int &operator[](const int n){
        return memory[n];
    }

    void init(){
        char ch;
        int p;
        while(cin >> ch){
            if (ch == '@'){
                cin >> hex >> p;
            }
            else{
                char ch2;
                cin >> ch2;
                int n1 = 0;
                if (ch >= '0' && ch <= '9') n1 += (ch - '0') * 16;
                else n1 += (ch - 'A' + 10) * 16;
                if (ch2 >= '0' && ch2 <= '9') n1 += (ch2 - '0');
                else n1 += (ch2 - 'A' + 10);
                memory[p++] = n1;
            }
        }
    }

    unsigned getCmd(unsigned int Pc){
        unsigned cmd = 0;
        for (int i = 3 ; i >= 0 ; --i){
            cmd = (cmd << 8);
            cmd += memory[Pc + i];
        }
        return cmd;
    }

    unsigned read(unsigned int length , unsigned int pos){
        unsigned int ans = 0;
        for (int i = length - 1 ; i >= 0 ; --i){
            ans = (ans << 8);
            ans += memory[i + pos];
        }
        return ans;
    }

    void write(unsigned int length , unsigned int pos , unsigned int code){
        for (int i = 0 ; i < length ; ++i){
            memory[pos + i] = (code & 0b11111111);
            code = (code >> 8);
        }
    }
};

#endif //RISC_V_RAM_H
