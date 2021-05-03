#define eraiser(n) ((1 << n) -1)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


//일단 필요한 거 리스트
// 1. instrument 종류 저장하는 변수
// 2. register 해석해주는 함수
// 3. instrument 종류 판단해주는 함수 + 없는 instrument일시 unknown 출력
// 4. test.bin의 16진수를 2진수로 변환하는 함수(Done)
// 5. test.bin의 16진수 bit를 8개씩 끊어주는 함수(Done)

//arithmetic, logical, memory, conditional, shift, system, constant 기록 변수

// (R):
// Arithmetic: add, addu, div, divu, mult, multu, sub, subu
// Logical: and, or, xor, nor
// Memory: mfhi, mflo, mthi, mtlo
// Shift: sll, srl, sra, sllv, srlv, srav
// Conditional: jr, jalr, slt, sltu
// system: syscall


// (I):
// Arithmetic: addi, addiu, 
// Logical: andi, ori, xori
// Memory: lw, lh, lb, sw, sh, sb, lui, lhu, lbu
// Conditional: beq, bne, slti, sltiu
// constant: lui


// (J):
// Conditional: j, jal

char *register_names[32] = 
{
    "$0",
    "$1",
    "$2",
    "$3",
    "$4",
    "$5",
    "$6",
    "$7",
    "$8",
    "$9",
    "$10",
    "$11",
    "$12",
    "$13",
    "$14",
    "$15",
    "$16",
    "$17",
    "$18",
    "$19",
    "$20",
    "$21",
    "$22",
    "$23",
    "$24",
    "$25",
    "$26",
    "$27",
    "$28",
    "$29",
    "$30",
    "$31"
};

int R_type_inst_arithmetic(int line_num, uint8_t funct, uint32_t bin_code, uint8_t rs, uint8_t rt, uint8_t rd)
{
    if(funct == 32){
        printf("inst %d: %08x add %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    else if(funct == 33){
        printf("inst %d: %08x addu %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    else if(funct == 34){
        printf("inst %d: %08x sub %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    else if(funct == 35){
        printf("inst %d: %08x subu %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    else if(funct == 24){
        printf("inst %d: %08x mult %s, %s", line_num, bin_code, register_names[rs], register_names[rt]);
    }
    else if(funct == 25){
        printf("inst %d: %08x multu %s, %s", line_num, bin_code, register_names[rs], register_names[rt]);
    }
    else if(funct == 26){
        printf("inst %d: %08x div %s, %s", line_num, bin_code, register_names[rs], register_names[rt]);
    }
    else if(funct == 27){
        printf("inst %d: %08x divu %s, %s", line_num, bin_code, register_names[rs], register_names[rt]);
    }
    return 0;
}

int R_type_inst_logical(int line_num, uint8_t funct, uint32_t bin_code, uint8_t rs, uint8_t rt, uint8_t rd)
{
    if(funct == 36){
        printf("inst %d: %08x and %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    else if(funct == 37){
        printf("inst %d: %08x or %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    else if(funct == 38){
        printf("inst %d: %08x xor %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    else if(funct == 39){
        printf("inst %d: %08x nor %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    return 0;
}

int R_type_inst_shift(int line_num, uint8_t funct, uint32_t bin_code, uint8_t rs, uint8_t rt, uint8_t rd, uint8_t shamt)
{
    if(funct == 0){
        printf("inst %d: %08x sll %s, %s, %d", line_num, bin_code, register_names[rd], register_names[rt], shamt);
    }
    else if(funct == 2){
        printf("inst %d: %08x srl %s, %s, %d", line_num, bin_code, register_names[rd], register_names[rt], shamt);
    }
    else if(funct == 3){
        printf("inst %d: %08x sra %s, %s, %d", line_num, bin_code, register_names[rd], register_names[rt], shamt);
    }
    else if(funct == 4){
        printf("inst %d: %08x sllv %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rt], register_names[rs]);
    }
    else if(funct == 6){
        printf("inst %d: %08x srlv %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rt], register_names[rs]);
    }
    else if(funct == 7){
        printf("inst %d: %08x srav %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rt], register_names[rs]);
    }
    return 0;
}

int R_type_inst_memory(int line_num, uint8_t funct, uint32_t bin_code, uint8_t rs, uint8_t rd)
{
    if(funct == 16){
        printf("inst %d: %08x mfhi %s", line_num, bin_code, register_names[rd]);
    }
    else if(funct == 17){
        printf("inst %d: %08x mthi %s", line_num, bin_code, register_names[rs]);
    }
    else if(funct == 18){
        printf("inst %d: %08x mflo %s", line_num, bin_code, register_names[rd]);
    }
    else if(funct == 19){
        printf("inst %d: %08x mtlo %s", line_num, bin_code, register_names[rs]);
    }
    return 0;
}

int R_type_inst_conditional(int line_num, uint8_t funct, uint32_t bin_code, uint8_t rs, uint8_t rt, uint8_t rd)
{
    if(funct == 8){
        printf("inst %d: %08x jr %s", line_num, bin_code, register_names[rs]);
    }
    else if(funct == 9){
        printf("inst %d: %08x jalr %s", line_num, bin_code, register_names[rs]);
    }
    else if(funct == 42){
        printf("inst %d: %08x slt %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    else if(funct == 43){
        printf("inst %d: %08x sltu %s, %s, %s", line_num, bin_code, register_names[rd], register_names[rs], register_names[rt]);
    }
    return 0;
}


int I_type_inst_arithmetic(int line_num, uint8_t op, uint32_t bin_code, uint8_t rs, uint8_t rt, int16_t imm)
{
    if(op == 8){
        printf("inst %d: %08x addi %s, %s, %d", line_num, bin_code, register_names[rt], register_names[rs], imm);
    }
    else if(op == 9){
        printf("inst %d: %08x addiu %s, %s, %d", line_num, bin_code, register_names[rt], register_names[rs], imm);
    }
    return 0;
}

int I_type_inst_logical(int line_num, uint8_t op, uint32_t bin_code, uint8_t rs, uint8_t rt, int16_t imm)
{
    if(op == 12){
        printf("inst %d: %08x andi %s, %s, %d", line_num, bin_code, register_names[rt], register_names[rs], imm);
    }
    else if(op == 13){
        printf("inst %d: %08x ori %s, %s, %d", line_num, bin_code, register_names[rt], register_names[rs], imm);
    }
    else if(op == 14){
        printf("inst %d: %08x xori %s, %s, %d", line_num, bin_code, register_names[rt], register_names[rs], imm);
    }
    return 0;
}

int I_type_inst_conditional(int line_num, uint8_t op, uint32_t bin_code, uint8_t rs, uint8_t rt, int16_t imm)
{
    if(op == 10){
        printf("inst %d: %08x slti %s, %s, %d", line_num, bin_code, register_names[rt], register_names[rs], imm);
    }
    else if(op == 11){
        printf("inst %d: %08x sltiu %s, %s, %d", line_num, bin_code, register_names[rt], register_names[rs], imm);
    }
    else if(op == 4){
        printf("inst %d: %08x beq %s, %s, %d", line_num, bin_code, register_names[rs], register_names[rt], imm);
    }
    else if(op == 5){
        printf("inst %d: %08x bne %s, %s, %d", line_num, bin_code, register_names[rs], register_names[rt], imm);
    }
    return 0;
}

int I_type_inst_memory(int line_num, uint8_t op, uint32_t bin_code, uint8_t rs, uint8_t rt, int16_t imm)
{
    if(op == 32){
        printf("inst %d: %08x lb %s, %d(%s)", line_num, bin_code, register_names[rt], imm, register_names[rs]);
    }
    else if(op == 33){
        printf("inst %d: %08x lh %s, %d(%s)", line_num, bin_code, register_names[rt], imm, register_names[rs]);
    }
    else if(op == 35){
        printf("inst %d: %08x lw %s, %d(%s)", line_num, bin_code, register_names[rt], imm, register_names[rs]);
    }
    else if(op == 36){
        printf("inst %d: %08x lbu %s, %d(%s)", line_num, bin_code, register_names[rt], imm, register_names[rs]);
    }
    else if(op == 37){
        printf("inst %d: %08x lhu %s, %d(%s)", line_num, bin_code, register_names[rt], imm, register_names[rs]);
    }
    else if(op == 40){
        printf("inst %d: %08x sb %s, %d(%s)", line_num, bin_code, register_names[rt], imm, register_names[rs]);
    }
    else if(op == 41){
        printf("inst %d: %08x sh %s, %d(%s)", line_num, bin_code, register_names[rt], imm, register_names[rs]);
    }
    else if(op == 43){
        printf("inst %d: %08x sw %s, %d(%s)", line_num, bin_code, register_names[rt], imm, register_names[rs]);
    }
    return 0;
}


int mips_disassemble(int line_num, uint32_t bin_code)
{
    uint8_t op, rs, rt, rd, shamt, funct;
    int16_t imm;
    int32_t target;

    //r,i,j 포멧을 판단해주는 변수
    int type_check;
    //arithmetic, logical, memory, conditional, shift, system, constant 기록 변수
    int type_inst;


    op = bin_code >> 26;
    rs = bin_code >> 21 & eraiser(5);
    rt = bin_code >> 16 & eraiser(5);
    rd = bin_code >> 11 & eraiser(5);
    shamt = bin_code >> 6 & eraiser(5);
    funct = bin_code & eraiser(6);
    imm = bin_code & eraiser(16);
    target = bin_code & eraiser(26);

    //printf("op: %d ", op);
    //printf("rs: %d ", rs);
    //printf("rt: %d ", rt);
    //printf("rd: %d ", rd);
    //printf("shamt: %d ", shamt);
    //printf("funct: %d ", funct);
    //printf("imm: %d ", imm);
    //printf("target: %07x\n", target);

    if(op == 0){
        //printf("R\n");
        if((24 <= funct && funct <= 27) || (32 <= funct && funct <= 35)){
            //printf("Arithmetic\n");
            R_type_inst_arithmetic(line_num, funct, bin_code, rs, rt, rd);
        }
        else if(36 <= funct && funct <= 39){
            //printf("Logical\n");
            R_type_inst_logical(line_num, funct, bin_code, rs, rt, rd);
        }
        else if((funct == 0) || (2 <= funct && funct <= 4) || (6 <= funct && funct <= 7)){
            //printf("Shift\n");
            R_type_inst_shift(line_num, funct, bin_code, rs, rt, rd, shamt);
        }
        else if(16 <= funct && funct <= 19){
            //printf("Memory\n");
            R_type_inst_memory(line_num, funct, bin_code, rs, rd);
        }
        else if((8 <= funct && funct <= 9) || (42 <= funct && funct <= 43)){
            //printf("Conditional\n");
            R_type_inst_conditional(line_num, funct, bin_code, rs, rt, rd);
        }
        else if(funct == 12){
            //printf("System\n");
            printf("inst %d: %08x syscall", line_num, bin_code);
        }
        else{
            printf("inst %d: %08x unknown instruction", line_num, bin_code);
        }
    }
    else if(op == 2 || op == 3){
        //printf("J\n");
        if(op == 2){
            printf("inst %d: %08x j %d", line_num, bin_code, target);
        }
        else if(op == 3){
            printf("inst %d: %08x jal %d", line_num, bin_code, target);
        }
        
    }
    else{
        //printf("I\n");
        if(8 <= op && op <= 9){
            //printf("Arithmetic\n");
            I_type_inst_arithmetic(line_num, op, bin_code, rs, rt, imm);
        }
        else if(12 <= op && op <= 14){
            //printf("Logical\n");
            I_type_inst_logical(line_num, op, bin_code, rs, rt, imm);
        }
        else if((10 <= op && op <= 11) || (4 <= op && op <= 5)){
            //printf("Conditional\n");
            I_type_inst_conditional(line_num, op, bin_code, rs, rt, imm);
        }
        else if(op == 15){
            //printf("32bit-constant\n");
            printf("inst %d: %08x lui %s, %d", line_num, bin_code, register_names[rt], imm);
        }
        else if((32 <= op && op <= 33) || (35 <= op && op <= 37) || (40 <= op && op <= 41) || (op == 43)){
            //printf("Memory\n");
            I_type_inst_memory(line_num, op, bin_code, rs, rt, imm);
        }
        else{
            printf("inst %d: %08x unknown instruction", line_num, bin_code);
        }
    }
    printf("\n");

    return 0;
}


//main 함수 선언
int main(int argc, char* argv[])
{
    //instrution의 번호를 세기 위한 변수 선언
    int line_num = 0;
    //test.1 이진 파일을 읽어서 FILE 포인터 fp에 저장
    FILE *fp = fopen(argv[1], "rb");
    //읽어온 이진 파일의 데이터를 저장하기 위한 char 배열 선언
    //char *buffer;
    
    //읽어온 파일이 비어 있다면(NULL)이라면
    if(fp == NULL){
        //출력 불가 메세지를 출력
        printf("Can not open file in reading mode. \n");
        //함수 종료
        return 0;
    }
    //char형 포인터에 1byte*4 = 4byte = 32bit 만큼 동적 할당
    char buffer[4] = {0, };
    //test1 이진 파일에 더이상 읽어올 데이터가 없을 때까지 반복
    while(!feof(fp)){
        //fread를 통해 buffer에 buffer사이즈만큼(32bit) 1번 fp 포인터를 통해 이진 데이터를 읽기
        fread(buffer, sizeof(buffer), 1, fp);
        //feof가 이진파일을 읽어오는데 실패한 경우, 탈출문(마지막으로 한번 더 읽는 feof 특징)
        if(feof(fp)) break;
        //buffer의 모든 인덱스에 대해
        for (int index=0; index < 4; index++){
            //각 인덱스에 저장된 1byte(8bit)의 이진 데이터를 16진수 형태로 출력
            //음수 출력 방지를 위해 unsigned char로 출력
            //printf("%02x ", (unsigned char) buffer[index]);
        }
        //printf("\n");
        //buffer 각 인덱스에 흩어져 있는 이진 데이터를 32bit짜리 하나의 변수에 담기
        uint32_t bin_code = ((unsigned char)buffer[0] << 24 | (unsigned char)buffer[1] << 16 | (unsigned char)buffer[2] << 8 | (unsigned char)buffer[3] << 0);
        //정상적으로 담겼는지 확인하는 디버깅
        //printf("%08x\n", bin_code);

        mips_disassemble(line_num, bin_code);
        line_num += 1;
        //printf("\n");
        //buffer를 0으로 초기화
        //memset(buffer, 0, sizeof(buffer));
    }


    //buffer에 동적할당된 메모리 해제
    //free(buffer);
    //함수 종료
    return 0;
}