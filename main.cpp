#include <iostream>
#include "register.h"
#include "RAM.h"
#include "base.h"
#include "predict.h"
using namespace std;

Register re;
RAM ram;

int IF_flag = 1;
int ID_flag = 1;
int EXE_flag = 1;
int MEM_flag = 1;
int WB_flag = 1;
int npc = 0;

IF add_bubble(){
    IF bubble{};
    bubble.code = bubble.pc = 0;
    return bubble;
}

void Run(){
    ram.init();
    unsigned int cmd = 0;
//    IF s1{};
//    ID s2;
//    EXE s3;
//    MEM s4;
//    WB s5;
    while (WB_flag){
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
//        s5.Write_back(s4);
//        s4.Memory(s3);
//        s3.Execute(s2);
//        s2.Decode(s1);
//        if (s2.op == )
//        s1.Fitch();
//        if (s1.code == 0x0ff00513) IF_flag = 0; //0ff00513为终止
    }
    cout << (re[10] & 255u);
}

int main() {
//    freopen("testcases/bulgarian.data" , "r" , stdin);
    Run();
    return 0;
}
