#define eraiser(n) ((1 << n) -1)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>



int register_data[32] = {0, };
unsigned int data_memory[16384];
int pc = 0;



int mips_assemble(int line_num, uint32_t bin_code)
{
    uint8_t op, rs, rt, rd, shamt, funct;
    uint16_t uimm;
    int16_t imm;
    int32_t target;


    op = bin_code >> 26;
    rs = bin_code >> 21 & eraiser(5);
    rt = bin_code >> 16 & eraiser(5);
    rd = bin_code >> 11 & eraiser(5);
    shamt = bin_code >> 6 & eraiser(5);
    funct = bin_code & eraiser(6);
    imm = bin_code & eraiser(16);
    uimm = bin_code & eraiser(16);
    target = bin_code & eraiser(26);
    if(target > eraiser(25)){
        target |= 0xfc000000;
    }

    
    
    if(op == 0){
        
        if(funct == 32){ //add
            register_data[rd] = register_data[rs] + register_data[rt];
            
            return 1;
        }
        else if(funct == 34){ //sub
            register_data[rd] = register_data[rs] - register_data[rt];
            return 1;
        }
        else if(funct == 36){ //and
            register_data[rd] = register_data[rs] & register_data[rt];
            return 1;
        }
        else if(funct == 37){ //or
            register_data[rd] = register_data[rs] | register_data[rt];
            return 1;
        }
        else if(funct == 0){ //sll
            register_data[rd] = (unsigned int)register_data[rt] << shamt;
            return 1;
        }
        else if(funct == 2){ //srl
            register_data[rd] = (unsigned int)register_data[rt] >> shamt;
            return 1;
        }
        else if(funct == 42){ //slt
            register_data[rd] = (register_data[rs] < register_data[rt]) ? 1 : 0;
            return 1;
        }
    }

    else{
        
        if(op == 2){ //j
            pc = (((pc >> 28) & eraiser(4)) << 28) | (target << 2);
            return 1;
        }
        else if(op == 8){ //addi
            register_data[rt] = register_data[rs] + imm;
            return 1;
        }
        else if(op == 12){ //andi
            register_data[rt] = register_data[rs] & uimm;
            return 1;
        }
        else if(op == 13){ //ori
            register_data[rt] = register_data[rs] | uimm;
            return 1;
        }
        else if(op == 15){ //lui
            register_data[rt] = imm << 16;
            return 1;
        }
        else if(op == 10){ //slti
            register_data[rt] = (register_data[rs] < (int)imm) ? 1 : 0;
            return 1;
        }
        else if(op == 4){ //beq
            if (register_data[rs] == register_data[rt]){
                pc = pc + (imm * 4);
            }
            
            return 1;
        }
        else if(op == 5){ //bne
            if (register_data[rs] != register_data[rt]){
                pc = pc + (imm * 4);
            }
            return 1;
        }
        else if(op == 32){ //lb
            int temp = data_memory[(register_data[rs] + imm - 0x10000000) / 4];
            if((register_data[rs] + imm - 0x10000000) % 4 == 0){
                register_data[rt] = (temp >> 24);
            }
            else if((register_data[rs] + imm - 0x10000000) % 4 == 1){
                register_data[rt] = (temp << 8 >> 24);
            }
            else if((register_data[rs] + imm - 0x10000000) % 4 == 2){
                register_data[rt] = (temp << 16 >> 24);
            }
            else if((register_data[rs] + imm - 0x10000000) % 4 == 3){
                register_data[rt] = (temp << 24 >> 24);
            }
            return 1;
        }
        else if(op == 33){ //lh
            int temp = data_memory[(register_data[rs] + imm - 0x10000000) / 4];
            if((register_data[rs] + imm - 0x10000000) % 4 == 0){
                register_data[rt] = (temp >> 16);
            }
            else if((register_data[rs] + imm - 0x10000000) % 4 == 2){
                register_data[rt] = (temp << 16 >> 16);
            }
            return 1;
        }
        else if(op == 35){ //lw
            register_data[rt] = data_memory[(register_data[rs] + imm - 0x10000000) / 4];
            return 1;
        }
        else if(op == 36){ //lbu
            unsigned int temp = data_memory[(register_data[rs] + uimm - 0x10000000) / 4];
            if((register_data[rs] + uimm - 0x10000000) % 4 == 0){
                register_data[rt] = (temp >> 24);
            }
            else if((register_data[rs] + uimm - 0x10000000) % 4 == 1){
                register_data[rt] = (temp << 8 >> 24);
            }
            else if((register_data[rs] + uimm - 0x10000000) % 4 == 2){
                register_data[rt] = (temp << 16 >> 24);
            }
            else if((register_data[rs] + uimm - 0x10000000) % 4 == 3){
                register_data[rt] = (temp << 24 >> 24);
            }
            return 1;
        }
        else if(op == 37){ // lhu
            unsigned int temp = data_memory[(register_data[rs] + uimm - 0x10000000) / 4];
            if((register_data[rs] + uimm - 0x10000000) % 4 == 0){
                register_data[rt] = (temp >> 16);
            }
            else if((register_data[rs] + uimm - 0x10000000) % 4 == 2){
                register_data[rt] = (temp << 16 >> 16);
            }
            return 1;
        }
        else if(op == 40){ //sb
            unsigned int temp = data_memory[(register_data[rs] + imm - 0x10000000) / 4];
            // printf("temp: 0x%08x\n", temp);
            if((register_data[rs] + imm - 0x10000000) % 4 == 0){
                // rt의 4/4 + 기존 값의 2/4~4/4
                // printf("sb: 0x%08x\n", (register_data[rt] & eraiser(8)) << 24);
                data_memory[(register_data[rs] + imm - 0x10000000) / 4] = ((register_data[rt] & eraiser(8)) << 24 | (temp & eraiser(24)));
            }
            else if((register_data[rs] + imm - 0x10000000) % 4 == 1){
                // 기존 값의 1/4 + rt의 4/4 + 기존 값의 3/4~4/4
                // printf("sb: 0x%08x\n", (register_data[rt] & eraiser(8)) << 16);
                data_memory[(register_data[rs] + imm - 0x10000000) / 4] = ((temp >> 24 << 24) | (register_data[rt] & eraiser(8)) << 16 | (temp & eraiser(16)));
            }
            else if((register_data[rs] + imm - 0x10000000) % 4 == 2){
                // 기존 값의 1/4~2/4 + rt의 4/4 + 기존 값의 4/4
                // printf("sb: 0x%08x\n", (register_data[rt] & eraiser(8)) << 8);
                data_memory[(register_data[rs] + imm - 0x10000000) / 4] = ((temp >> 16 << 16) | (register_data[rt] & eraiser(8)) << 8 | (temp & eraiser(8)));
            }
            else if((register_data[rs] + imm - 0x10000000) % 4 == 3){
                // 기존 값의 1/4~3/4 + rt의 4/4
                // printf("sb: 0x%08x\n", (register_data[rt] & eraiser(8)));
                data_memory[(register_data[rs] + imm - 0x10000000) / 4] = ((temp >> 8 << 8) | (register_data[rt] & eraiser(8)));
            }
            return 1;
        
        }
        else if(op == 41){ //sh
            unsigned int temp = data_memory[(register_data[rs] + imm - 0x10000000) / 4];
            if((register_data[rs] + imm - 0x10000000) % 4 == 0){
                // rt의 2/2 + 기존 값의 2/2
                data_memory[(register_data[rs] + imm - 0x10000000) / 4] = ((register_data[rt] & eraiser(16) << 16) | (temp << 16 >> 16));
            }
            else if((register_data[rs] + imm - 0x10000000) % 4 == 2){
                // 기존 값의 1/2 + rt의 2/2
                data_memory[(register_data[rs] + imm - 0x10000000) / 4] = ((temp >> 16 << 16) | (register_data[rt] & eraiser(16)));
            }
            return 1;
        }
        else if(op == 43){ // sw
            data_memory[(register_data[rs] + imm - 0x10000000) / 4] = register_data[rt];
            return 1;
        }
    }
    
    return 0;
}


//main 함수 선언
int main(int argc, char* argv[])
{
    //instrution의 번호를 세기 위한 변수 선언
    int line_num = 0;
    uint32_t num;
    char buffer[4] = {0, };
    int inst_memory[16384];

    //test.1 이진 파일을 읽어서 FILE 포인터 fp에 저장
    FILE *fp = fopen(argv[1], "rb");
    memset(inst_memory, 0xFF, sizeof(inst_memory));
    memset(data_memory, 0xFF, sizeof(data_memory));
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
    
    //test1 이진 파일에 더이상 읽어올 데이터가 없을 때까지 반복
    while(!feof(fp)){
        //fread를 통해 buffer에 buffer사이즈만큼(32bit) 1번 fp 포인터를 통해 이진 데이터를 읽기
        fread(buffer, sizeof(buffer), 1, fp);
        //feof가 이진파일을 읽어오는데 실패한 경우, 탈출문(마지막으로 한번 더 읽는 feof 특징)
        if(feof(fp)) break;
        
        uint32_t bin_code = ((unsigned char)buffer[0] << 24 | (unsigned char)buffer[1] << 16 | (unsigned char)buffer[2] << 8 | (unsigned char)buffer[3] << 0);
        inst_memory[line_num] = bin_code;
        line_num += 1;

    }

    if(argc<=3){
        printf("unknown instruction\n");
        for (int index = 0; index < atoi(argv[2]); index++){
            num = inst_memory[pc/4];
            pc = pc + 4;
            if (!(mips_assemble(line_num, num))) {
                //printf("unknown instruction\n");
                break;
            }
        }
    }
    else{
        if (!(strcmp(argv[3], "mem"))){
        
            for (int index = 0; index < atoi(argv[2]); index++){
                num = inst_memory[pc/4];
                pc = pc + 4;
                if (!(mips_assemble(line_num, num))) {
                    //printf("unknown instruction\n");
                    break;
                }
            }
            
            for (int i = 0; i < 4; i++){
                //printf("0x%08x\n", data_memory[((atoi(argv[4]) + (4 * i)) - 0x10000000) / 4]);
                printf("0x%08x\n", data_memory[((strtol(argv[4], NULL, 16) + (4 * i)) - 0x10000000) / 4]);
            }
        }

        else if (!(strcmp(argv[3], "reg"))){
            for (int index = 0; index < atoi(argv[2]); index++){
                
                num = inst_memory[pc/4];
                pc = pc + 4;
                if (!(mips_assemble(line_num, num))) {
                    printf("unknown instruction\n");
                    break;
                }
            }

            for (int i = 0; i < 32; i++){
                printf("$%d: 0x%08x\n", i, register_data[i]);
            }
            printf("PC: 0x%08x\n", pc);
        }
    }

    fclose(fp);


    //buffer에 동적할당된 메모리 해제
    //free(buffer);
    //함수 종료
    return 0;
}