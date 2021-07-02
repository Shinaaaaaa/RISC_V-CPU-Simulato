//
// Created by lenovo on 2021/6/29.
//

#ifndef RISC_V_BASE_H
#define RISC_V_BASE_H
extern Register re;
extern RAM ram;

extern int IF_flag;
extern int ID_flag;
extern int EXE_flag;
extern int MEM_flag;
extern int WB_flag;
extern unsigned npc;

extern unsigned EXE_value;
extern unsigned MEM_value;

unsigned check(unsigned data , unsigned int length){
    return data >> (length - 1);
}

unsigned sext(unsigned data , unsigned int length){
    return ((~(1 << length)) << (32 - length)) | data;
}

class IF{
public:
    unsigned int pc = 0;
    unsigned int code = 0;

    IF() = default;

    void Fitch(){
        if (IF_flag == 0) return;
        code = ram.getCmd(npc);
        npc += 4;
        re.move_pc(4);
        pc = re.get_pc();
    }
};

class ID{
public:
    unsigned int pc = 0 , code = 0;
    unsigned op{} , fun3 = 0 , fun7 = 0;
    unsigned int rd = 0, rs1 = 0, rs2 = 0, imm = 0;
    unsigned int value_rs1 = 0 , value_rs2 = 0;

    ID() = default;

    void Decode(const IF &fetch){
        pc = fetch.pc;
        code = fetch.code;
        op = fun3 = fun7 = rd = rs1 = rs2 = imm = value_rs1 = value_rs2 = 0;
        unsigned cmd = fetch.code;
        if (fetch.code == 0) {
            code = 0;
            op = 0;
            return;
        }
        if (ID_flag == 0) return;
        if (IF_flag == 0) {
            ID_flag = 0;
            return;
        }
        unsigned opcode = (cmd & 0b1111111);
        switch (opcode) {
            case 0b0110111:{// lui
                op = 0b110111;
                cmd = (cmd >> 7);
                rd = (cmd & 0b11111);
                cmd = (cmd >> 5);
                imm = cmd;
                imm = imm << 12;
                break;
            }
            case 0b0010111:{//auipc
                op = 0b10111;
                cmd = (cmd >> 7);
                rd = (cmd & 0b11111);
                cmd = (cmd >> 5);
                imm = cmd;
                imm = imm << 12;
                break;
            }
            case 0b1101111:{//jal
                op = 0b1101111;
                cmd = (cmd >> 7);
                rd = (cmd & 0b11111);
                cmd = (cmd >> 5);
                imm = ((cmd & 0b11111111) << 11);
                cmd = (cmd >> 8);
                imm += ((cmd & 0b1) << 10);
                cmd = (cmd >> 1);
                imm += (cmd & 0b1111111111);
                cmd = (cmd >> 10);
                imm += (cmd << 19);
                imm = (imm << 1);
                if (check(imm , 21)) imm = sext(imm , 11);
                re.move_pc(imm - 4);
                npc = re.get_pc();
                break;
            }
            case 0b1100111:{ //jalr
                op = 0b1100111;
                cmd = (cmd >> 7);
                rd = (cmd & 0b11111);
                cmd = (cmd >> 5);
                fun3 = (cmd & 0b111);
                cmd = (cmd >> 3);
                rs1 = (cmd & 0b11111);
                cmd = (cmd >> 5);
                imm = cmd;
                imm = (imm << 1);
                if (check(imm , 13)) imm = sext(imm , 19);
                value_rs1 = re[rs1];
                re.set_pc((value_rs1 + imm) & (~1));
                npc = re.get_pc();
                break;
            }
            case 0b1100011:{ //b
                op = 0b1100011;
                cmd = (cmd >> 7);
                imm = ((cmd & 0b1) << 10);
                cmd = (cmd >> 1);
                imm += (cmd & 0b1111);
                cmd = (cmd >> 4);
                fun3 = (cmd & 0b111);
                cmd = (cmd >> 3);
                rs1 = (cmd & 0b11111);
                cmd = (cmd >> 5);
                rs2 = (cmd & 0b11111);
                cmd = (cmd >> 5);
                imm += ((cmd & 0b111111) << 4);
                cmd = (cmd >> 6);
                imm += (cmd << 11);
                imm = (imm << 1);
                if (check(imm , 13)) imm = sext(imm , 19);
                value_rs1 = re[rs1];
                value_rs2 = re[rs2];
                break;
            }
            case 0b0000011:{ //lb , lh , lw , lbu , lhu
                op = 0b11;
                cmd = (cmd >> 7);
                rd = (cmd & 0b11111);
                cmd = (cmd >> 5);
                fun3 = (cmd & 0b111);
                cmd = (cmd >> 3);
                rs1 = (cmd & 0b11111);
                imm = (cmd >> 5);
                if (check(imm , 12)) imm = sext(imm , 20);
                value_rs1 = re[rs1];
                break;
            }
            case 0b0100011:{//sb , sh , sw
                op = 0b100011;
                cmd = (cmd >> 7);
                imm = (cmd & 0b11111);
                cmd = (cmd >> 5);
                fun3 = (cmd & 0b111);
                cmd = (cmd >> 3);
                rs1 = (cmd & 0b11111);
                cmd = (cmd >> 5);
                rs2 = (cmd & 0b11111);
                imm += ((cmd >> 5) << 5);
                if (check(imm , 12)) imm = sext(imm , 20);
                value_rs1 = re[rs1];
                value_rs2 = re[rs2];
                break;
            }
            case 0b0010011: { //addi , xori , ori , andi , slli , srli , srai , slti , sltiu
                op = 0b0010011;
                cmd = (cmd >> 7);
                rd = (cmd & 0b11111);
                cmd = (cmd >> 5);
                fun3 = (cmd & 0b111);
                cmd = (cmd >> 3);
                rs1 = (cmd & 0b11111);
                cmd = (cmd >> 5);
                imm = cmd;
                if (check(imm , 12)) imm = sext(imm , 20);
                if (fun3 == 0b101) { // srli srai
                    imm = (cmd & 0b11111);
                    fun7 = (cmd >> 5);
                }
                value_rs1 = re[rs1];
                break;
            }
            case 0b0110011:{ //add , sub , xor , or , and , sll , srl , sra , slt , sltu
                op = 0b110011;
                cmd = (cmd >> 7);
                rd = (cmd & 0b11111);
                cmd = (cmd >> 5);
                fun3 = (cmd & 0b111);
                cmd = (cmd >> 3);
                rs1 = (cmd & 0b11111);
                cmd = (cmd >> 5);
                rs2 = (cmd & 0b11111);
                fun7 = (cmd >> 5);
                value_rs1 = re[rs1];
                value_rs2 = re[rs2];
                break;
            }
            default: throw "error";
        }
    }
};

//TODO 把EXE中地址计算移动到ID中

class EXE{
public:
    unsigned int code = 0 , pc = 0;
    unsigned op{} , fun3 = 0 , fun7 = 0;
    unsigned int rd = 0;
    unsigned int value{} , pos{};

    EXE() = default;

    void Execute(const ID &decode){
        code = decode.code;
        pc = decode.pc;
        op = decode.op;
        fun3 = decode.fun3;
        fun7 = decode.fun7;
        rd = decode.rd;
        value = pos = 0;
        if (decode.code == 0) {
            code = 0;
            return;
        }
        if (EXE_flag == 0) return;
        if (ID_flag == 0) {
            EXE_flag = 0;
            return;
        }
        switch (decode.op) {
            case 0b0110111:{ //lui
                value = decode.imm;
                break;
            }
            case 0b0010111:{ //auipc
                value = re.get_pc() + decode.imm - 4;
                break;
            }
            case 0b1101111:{ //jal
                value = pc;
                break;
            }
            case 0b1100111:{
                value = pc;
                break;
            }
            case 0b1100011:{
                switch (fun3) {
                    case 0b000: //beq
                        if (decode.value_rs1 == decode.value_rs2) re.move_pc(decode.imm - 4) , npc = re.get_pc();
                        break;
                    case 0b001: //bne
                        if (decode.value_rs1 != decode.value_rs2) re.move_pc(decode.imm - 4) , npc = re.get_pc();
                        break;
                    case 0b100: //blt
                        if ((signed)decode.value_rs1 < (signed)decode.value_rs2) re.move_pc(decode.imm - 4) , npc = re.get_pc();
                        break;
                    case 0b101: //bge
                        if ((signed)decode.value_rs1 >= (signed)decode.value_rs2) re.move_pc(decode.imm - 4) , npc = re.get_pc();
                        break;
                    case 0b110:
                        if (decode.value_rs1 < decode.value_rs2) re.move_pc(decode.imm - 4) , npc = re.get_pc();
                        break;
                    case 0b111:
                        if (decode.value_rs1 >= decode.value_rs2) re.move_pc(decode.imm - 4) , npc = re.get_pc();
                        break;
                }
                break;
            }
            case 0b0000011:{ // l
                pos = decode.value_rs1 + decode.imm;
                break;
            }
            case 0b0100011:{ // s
                pos = decode.value_rs1 + decode.imm;
                value = decode.value_rs2;
                break;
            }
            case 0b0010011:{
                switch (fun3) {
                    case 0b000: //addi
                        value = decode.value_rs1 + decode.imm;
                        break;
                    case 0b010: // slti
                        value = ((signed)decode.value_rs1 < (signed)decode.imm);
                        break;
                    case 0b011: // sltiu
                        value = (decode.value_rs1 < decode.imm);
                        break;
                    case 0b100: // xori
                        value = decode.value_rs1 ^ decode.imm;
                        break;
                    case 0b110: // ori
                        value = decode.value_rs1 | decode.imm;
                        break;
                    case 0b111: // andi
                        value = decode.value_rs1 & decode.imm;
                        break;
                    case 0b001:
                        value = (decode.value_rs1 << decode.imm);
                        break;
                    case 0b101:
                        if (fun7 == 0b0000000) value = (decode.value_rs1 >> decode.imm);
                        else {
                            unsigned tmp = (decode.value_rs1 >> decode.imm);
                            if (check(tmp , 32 - decode.imm)) tmp = sext(tmp , decode.imm);
                            value = tmp;
                        }
                        break;
                }
                break;
            }
            case 0b0110011:{
                unsigned p;
                switch (fun3) {
                    case 0b000:
                        if (fun7 == 0){ //add
                            value = decode.value_rs1 + decode.value_rs2;
                        }
                        else{ // sub
                            value = decode.value_rs1 - decode.value_rs2;
                        }
                        break;
                    case 0b001: //sll
                        p = (decode.value_rs2 & 0b11111);
                        value = (decode.value_rs1 << p);
                        break;
                    case 0b101:
                        if (fun7 == 0){ //srl
                            p = (decode.value_rs2 & 0b11111);
                            value = (decode.value_rs1 >> p);
                        }
                        else { //sra
                            p = (decode.value_rs2 & 0b11111);
                            unsigned tmp = (decode.value_rs1 >> p);
                            if (check(tmp , 32 - p)) tmp = sext(tmp , p);
                            value = tmp;
                        }
                        break;
                    case 0b010: //slt
                        value = ((signed) decode.value_rs1 < (signed)decode.value_rs2);
                        break;
                    case 0b011: //sltu
                        value = (decode.value_rs1 < decode.value_rs2);
                        break;
                    case 0b100: //xor
                        value = decode.value_rs1 ^ decode.value_rs2;
                        break;
                    case 0b110: //or
                        value = decode.value_rs1 | decode.value_rs2;
                        break;
                    default: //and
                        value = decode.value_rs1 & decode.value_rs2;
                        break;
                }
                break;
            }
            default: throw "error";
        }
        EXE_value = value;

    }
};

class MEM{
public:
    unsigned int code = 0 , pc = 0;
    unsigned op{} , fun3 = 0 , fun7 = 0;
    unsigned int rd = 0;
    unsigned int value{};

    MEM() = default;

    void Memory(const EXE &execute){
        code = execute.code;
        pc = execute.pc;
        op = execute.op;
        fun3 = execute.fun3;
        fun7 = execute.fun7;
        rd = execute.rd;
        value = execute.value;
        if (execute.code == 0){
            code = 0;
            return;
        }
        if (MEM_flag == 0) return;
        if (EXE_flag == 0) {
            MEM_flag = 0;
            return;
        }
        switch (execute.op) {
            case 0b0000011:{
                unsigned data;
                switch (fun3) {
                    case 0b000: //lb
                        data = ram.read(1 , execute.pos);
                        if (check(data , 8)) data = data | (0b111111111111111111111111 << 8);
                        value = data;
                        break;
                    case 0b001: //lh
                        data = ram.read(2 , execute.pos);
                        if (check(data , 16)) data = data | (0b1111111111111111 << 16);
                        value = data;
                        break;
                    case 0b010: //lw
                        data = ram.read(4 , execute.pos);
                        value = data;
                        break;
                    case 0b100: //lbu
                        data = ram.read(1 , execute.pos);
                        value = data;
                        break;
                    default: //lhu
                        data = ram.read(2 , execute.pos);
                        value = data;
                        break;
                }
                break;
            }
            case 0b0100011:{
                switch (fun3) {
                    case 0b000: //sb
                        ram.write(1 , execute.pos , execute.value);
                        break;
                    case 0b001: //sh
                        ram.write(2 , execute.pos , execute.value);
                        break;
                    default: //sw
                        ram.write(4 , execute.pos , execute.value);
                        break;
                }
                break;
            }
            default:
                break;
        }
        MEM_value = value;
    }
};

class WB{
public:
    void Write_back(const MEM &memory){
        unsigned rd = memory.rd;
        unsigned value = memory.value;
        if (memory.code == 0 || WB_flag == 0) return;
        if (MEM_flag == 0) {
            WB_flag = 0;
            return;
        }
        switch (memory.op) {
            case 0b0110111:{
                re[rd] = value;
                break;
            }
            case 0b0010111:{
                re[rd] = value;
                break;
            }
            case 0b1101111:{
                if (rd != 0) re[rd] = value;
                break;
            }
            case 0b1100111:{
                if (rd != 0) re[rd] = value;
                break;
            }
            case 0b1100011:{
                break;
            }
            case 0b0000011:{ // l
                re[rd] = value;
                break;
            }
            case 0b0100011:{
                break;
            }
            case 0b0010011:{
                re[rd] = value;
                break;
            }
            case 0b0110011:{
                re[rd] = value;
                break;
            }
            default: throw "error";
        }
    }
};
#endif //RISC_V_BASE_H