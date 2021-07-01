#include <iostream>
#include "register.h"
#include "RAM.h"
#include "base.h"
using namespace std;

Register re;
RAM ram;

void Run(){
    ram.init();
    unsigned int cmd = 0;
    while (true){
        IF s1{};
        ID s2;
        EXE s3;
        MEM s4;
        WB s5;

        s1.Fitch();
        cmd = s1.code;
        if (cmd == 0x0ff00513) break; //0ff00513为终止
        s2.Decode(s1);
        s3.Execute(s2);
        s4.Memory(s3);
        s5.Write_back(s4);
    }
    cout << (re[10] & 255u);
}

int main() {
    Run();
    return 0;
}
