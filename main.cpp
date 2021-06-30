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
    cmd = ram.getCmd(0);
    while (cmd != 0x0ff00513){ //0ff00513为终止
        base b;
        b.deCode(cmd);
        b.executeCode();
        if (b.getOp() != 0b1100011 && b.getOp() != 0b1101111 && b.getOp() != 0b1100111) re.move_pc(4);
        cmd = ram.getCmd(re.get_pc());
    }
    cout << (re[10] & 255u);
}

int main() {
    Run();
    return 0;
}
