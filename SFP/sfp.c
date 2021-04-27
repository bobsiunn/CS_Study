#define eraiser(n) ((1 << n) -1)
#include "sfp.h"
#include <stdlib.h>
#include <stdio.h>

sfp int2sfp(int input){
    //최종 결과값 저장
    sfp sfp_16bit = 0;
    //최종 2진수 비트를 배열에 저장
    int sfp_binary[16] = {0, };

    //step1. sign 구하기
    if(input == 0){
        return sfp_16bit;
    }
    //양수의 경우 sign = 0
    else if(input > 0){
        sfp_binary[0] = 0;
    }
    //음수의 경우 sign = 1
    else if(input < 0){
        sfp_binary[0] = 1;
    }

    //step2. frac 구하기
    //input의 이진수 데이터를 저장하기 위한 배열
    int frac_binary_32bit[31] = {0, };
    //input을 2로 나눈 나머지를 저장하기 위한 배열
    int frac_remain[31] = {0, };
    //input을 2로 나누고 나머지를 저장
    int frac_index = 0;
    while(1){
        frac_remain[frac_index] = input % 2;
        input = input / 2;
        frac_index += 1;
            
        if(input == 0){
            break;
        }
    }
    //나머지를 저장한 배열을 반대 순서로 frac_binary_32bit 배열에 저장
    int bit_cnt = 0;
    for (int i = frac_index-1; i >= 0; i--){
        frac_binary_32bit[bit_cnt] = frac_remain[i];
        bit_cnt++;
    }
    //round zero이므로 10bit 아래는 무시하며 sfp_binary 배열에 값 복사
    for(int i = 1; i<10; i++){
        sfp_binary[i+5] = frac_binary_32bit[i];
    }

    //step3. exp 구하기
    int bias = 15;
    int e = bit_cnt -1;
    int exp = e + bias;

    //예외 처리 (오버플로우)
	if (exp > 30) {
        //0 11111 0000000000 (양)
		if (sfp_binary[0] == 0) {
            //sign
            sfp_16bit = sfp_16bit << 1;
            //exp
			for (int i = 0; i < 5; i++) {
				sfp_16bit = sfp_16bit << 1;
				sfp_16bit++;
			}
            //frac
            for (int i = 0; i < 10; i++) {
				sfp_16bit = sfp_16bit << 1;
			}
		}
        //1 11111 0000000000 (음)
        else{
            //sign
            sfp_16bit = sfp_16bit << 1;
            sfp_16bit++;
            //exp
			for (int i = 0; i < 5; i++) {
				sfp_16bit = sfp_16bit << 1;
				sfp_16bit++;
			}
            //frac
            for (int i = 0; i < 10; i++) {
				sfp_16bit = sfp_16bit << 1;
			}
        }
	}
    //sfp의 범위 안에 있는 경우 (예외x)
    else{
        //exp를 2로 나눈 나머지를 저장하기 위한 배열
        int exp_remain[5] = {0, };
        //exp를 2로 나누고 나머지를 저장
        int exp_index = 0;
        while(1){
            exp_remain[exp_index] = exp % 2;
            exp = exp / 2;
            exp_index += 1;
            
            if(exp == 0){
                break;
            }
        }
        //나머지를 반대 순서로 sfp_binary 배열에 저장
        for (int i = 4; i >= 0; i--){
            sfp_binary[1 + 4 - i] = exp_remain[i];
        }
    }


    //step4. bit 조작
    for(int i=0; i<16; i++){
        //디버깅용
        printf("%d", sfp_binary[i]);
        //배열의 값이 1이면, shift 후 1을 더해서 비트 1조작
        if(sfp_binary[i] == 1){
            sfp_16bit = sfp_16bit << 1;
			sfp_16bit++;
        }
        //배열의 값이 1이면, shift해서 비트 0조작
        else{
            sfp_16bit = sfp_16bit << 1;
        }
    }
    //비트조작된 16bit sfp 변수 자료형 반환
	return sfp_16bit;
}


int sfp2int(sfp input){
    //printf("\nstart");
    //sfp에서 변환한 int를 저장할 변수
    int int_to_sfp = 0;
    //input에서 sign을 추출한 변수
    sfp sign;
    //input에서 exp를 추출할 변수
    sfp exp;
    //input에서 frac을 추출할 변수
    sfp frac;

    //input에서 sign, exp, frac을 추출
    sign = input >> 15;
    exp = input >> 10 & eraiser(5);
    frac = input & eraiser(10);

    //input 중 frac의 비트 코드를 저장할 배열
    int int_binary[10] = {0, };
    int binary_bit, binary_i = 0;
    //leading 1을 포함하여 비트 코드 저장
    int_binary[0] = 1;
    for (int i = 9; i > 0; i--) {
		binary_bit = input >> i & eraiser(1);
		int_binary[10-i] = binary_bit;
	}
    //input에서 추출한 sign, exp, frac을 int 자료형에 저장
    int sign_num;
    int exp_num = exp;
    int frac_num = frac;
    int bias = 15;

    //예외 처리
    //1 11111 0000000000 (음의 무한대)
    if(sign == 1 && exp_num == 31 && frac_num == 0){
        //TMIN
        return -2147483648;
    }
    //0 11111 0000000000 (양의 무한대)
    else if(sign == 0 && exp_num == 31 && frac_num == 0){
        //TMAX
        return 2147483647;
    }
    //0 or 1 11111 00000000001 (NaN)
    else if(exp_num == 31 && frac_num != 0){
        //TMIN
        return -2147483648;
    }

    //sign 구하기
    if(sign == 0){
        sign_num = 1;
    }
    else{
        sign_num = -1;
    }
    //e 구하기
    int e = exp_num - bias;
    // printf("\n");
    // for(int j=0; j<10; j++){
    //     printf("%d", int_binary[j]);
    // }
    //Decimal 구하기
    //math.h 없이 제곱 연산을 하기 위한 배열 선언
    int plus_power[16] = 
    {
        1, 2, 4, 8, 16, 
        32, 64, 128, 256, 512, 
        1024, 2048, 4096, 8192, 16384, 32768
    };
    //1.xxxx * 2^e를 역산하여 decimal 값을 도출
    for(int i = e; i >= 0; i--){
        if(int_binary[i] == 1){
            int_to_sfp += plus_power[e - i];
        }
    }
    //sign을 곱해서 부호 결정
    int_to_sfp *= sign_num;
    //디버깅
    //printf("\nresult: %d", int_to_sfp);
    //int 자료형 반환
    return int_to_sfp;
}

sfp float2sfp(float input){
    //최종 결과값 저장
    sfp sfp_16bit = 0;
    //최종 2진수 비트를 배열에 저장
    int sfp_binary[16] = {0, };

    //step1. sign 구하기
    if(input == 0){
        return sfp_16bit;
    }
    //양수의 경우 sign = 0
    else if(input > 0){
        sfp_binary[0] = 0;
    }
    //음수의 경우 sign = 1
    else if(input < 0){
        sfp_binary[0] = 1;
    }

    //step2. frac 구하기

    //step2-1. 정수-소수부분 분리
    int up;
    float down;
    
    if(input > 0){
        up = input;
        down = input - up;
    }
    else if(input < 0){
        up = -1*input;
        down = -1*(input + up);
    }
    //step2-2. 정수부분 이진수 변환

    //input의 이진수 데이터를 저장하기 위한 배열
    int frac_binary_32bit[31] = {0, };
    //input을 2로 나눈 나머지를 저장하기 위한 배열
    int frac_up_remain[31] = {0, };
    //input을 2로 나누고 나머지를 저장
    int frac_up_index = 0;
    //정수부분이 0.xx가 아닐때 (0.xx면 정수부분은 그냥 0)
    if(up != 0){
        while(1){
            frac_up_remain[frac_up_index] = up % 2;
            up = up / 2;
            frac_up_index += 1;
            
            if(up == 0){
                break;
            }
        }
    }
    //나머지를 저장한 배열을 반대 순서로 frac_binary_32bit 배열에 저장
    int bit_up_cnt = 0;
    for (int i = frac_up_index-1; i >= 0; i--){
        frac_binary_32bit[bit_up_cnt] = frac_up_remain[i];
        bit_up_cnt++;
    }
    //step2-3. 소수부분 이진수 변환
    int frac_down_remain[31] = {0, };
    //소수 부분 나머지 개수
    int bit_down_cnt = 0;
    //frac_binary_32bit에 저장하기 위한 인덱싱
    int bit_down_index = bit_up_cnt;
    //frac의 정수부분과 소수부분 길이의 합이 31을 넘어가지 않는 선에서 소수부분 bit code 생성
    while(bit_down_cnt + bit_up_cnt < 31){
        if(2 * down < 1){
            frac_binary_32bit[bit_down_index++] = 0;
            down = 2 * down;
        }
        else{
            frac_binary_32bit[bit_down_index++] = 1;
            down = 2 * down -1;
        }
        if(down == 0){
            break;
        }
        bit_down_cnt += 1;
    }
    //step3. exp 구하기
    int bias = 15;
    int e;
    int exp;

    //e 찾기
    //0.xxx 형태일 경우
    if(bit_up_cnt == 0){
        for(int j=0; j<32; j++){
            if(frac_binary_32bit[j] == 1){
                e = j+1;
                exp = -1 * e + bias;
                break;
            }
        }
        //round zero이므로 10bit 아래는 무시하며 sfp_binary 배열에 값 복사
        //소수 아래 1이 가장 먼저 나오는 인덱스 +1부터 frac code를 저장
        for(int i = 0; i<9; i++){
            sfp_binary[i+6] = frac_binary_32bit[e+i];
        }
    }
    //1.xxx 형태일 경우
    else{
        e = bit_up_cnt -1;
        exp = e + bias;

        //round zero이므로 10bit 아래는 무시하며 sfp_binary 배열에 값 복사
        //leading 1 다음부터 frac code를 저장
        for(int i = 1; i<10; i++){
            sfp_binary[i+5] = frac_binary_32bit[i];
        }
    }

    //예외 처리 (오버플로우)
	if (exp > 30) {
        //0 11111 0000000000 (양)
	    if (sfp_binary[0] == 0) {
            //sign
            sfp_16bit = sfp_16bit << 1;
            //exp
	        for (int i = 0; i < 5; i++) {
	            sfp_16bit = sfp_16bit << 1;
	            sfp_16bit++;
	        }
            //frac
            for (int i = 0; i < 10; i++) {
	            sfp_16bit = sfp_16bit << 1;
	        }
	    }
        //1 11111 0000000000 (음)
        else{
            //sign
            sfp_16bit = sfp_16bit << 1;
            sfp_16bit++;
            //exp
	        for (int i = 0; i < 5; i++) {
	            sfp_16bit = sfp_16bit << 1;
	            sfp_16bit++;
	        }
            //frac
            for (int i = 0; i < 10; i++) {
	            sfp_16bit = sfp_16bit << 1;
	        }
        }
    }
    //예외 처리 (언더플로우)
    else if(exp < 0){
        return 0;
    }
    //sfp의 범위 안에 있는 경우 (예외x)
    else{
        //exp를 2로 나눈 나머지를 저장하기 위한 배열
        int exp_remain[5] = {0, };
        //exp를 2로 나누고 나머지를 저장
        int exp_index = 0;
        while(1){
            exp_remain[exp_index] = exp % 2;
            exp = exp / 2;
            exp_index += 1;
            
            if(exp == 0){
                break;
            }
        }
        //나머지를 반대 순서로 sfp_binary 배열에 저장
        for (int i = 4; i >= 0; i--){
            sfp_binary[1 + 4 - i] = exp_remain[i];
        }
    }
    //step4. bit 조작
    for(int i=0; i<16; i++){
        //디버깅용
        //printf("%d", sfp_binary[i]);
        //배열의 값이 1이면, shift 후 1을 더해서 비트 1조작
        if(sfp_binary[i] == 1){
            sfp_16bit = sfp_16bit << 1;
	        sfp_16bit++;
        }
        //배열의 값이 1이면, shift해서 비트 0조작
        else{
            sfp_16bit = sfp_16bit << 1;
        }
    }
    //비트조작된 16bit sfp 변수 자료형 반환
	return sfp_16bit;
}

float sfp2float(sfp input){
    //printf("\nstart");
    //float값을 저장하는 변수 선언
    float float_to_sfp = 0;
    //input에서 sign을 추출하는 변수
    sfp sign;
    //input에서 exp를 추출하는 변수
    sfp exp;
    //input에서 frac을 추출하는 변수
    sfp frac;
    //input에서 sign, exp, frac을 추출
    sign = input >> 15;
    exp = input >> 10 & eraiser(5);
    frac = input & eraiser(10);
    //frac의 bit code를 저장하기 위한 배열 선언
    int float_binary[10] = {0, };
    int binary_bit, binary_i = 0;
    //leading 1을 포함한 frac bit code 저장
    float_binary[0] = 1;
    for (int i = 9; i > 0; i--) {
		binary_bit = input >> i & eraiser(1);
		float_binary[10-i] = binary_bit;
	}
    //추출한 sign, exp, frac을 int 자료형에 저장
    int sign_num;
    int exp_num = exp;
    int frac_num = frac;
    int bias = 15;

    //예외처리
    //1 11111 0000000000 (음의 무한대)
    if(sign == 1 && exp_num == 31 && frac_num == 0){
        //float과 같은 32bit 자료형 unsigned int 선언 (shift 연산을 위한 자료형)
        unsigned int float_32bit = 0;
        //sign
        float_32bit = float_32bit << 1;
        float_32bit++;
        //exp
        for(int k=0; k<8; k++){
            float_32bit = float_32bit << 1;
            float_32bit++;
        }
        //frac
        for (int k=0; k<23; k++){
            float_32bit = float_32bit << 1;
        }
        //floating point의 음의 무한대 표기법에 따라 bit code 생성 후 float 자료형으로 변환하여 반환
        return (float)float_32bit;
    }
    //0 11111 0000000000 (양의 무한대)
    else if(sign == 0 && exp_num == 31 && frac_num == 0){
        //float과 같은 32bit 자료형 unsigned int 선언 (shift 연산을 위한 자료형)
        unsigned int float_32bit = 0;
        //sign
        float_32bit = float_32bit << 1;
        //exp
        for(int k=0; k<8; k++){
            float_32bit = float_32bit << 1;
            float_32bit++;
        }
        //frac
        for (int k=0; k<23; k++){
            float_32bit = float_32bit << 1;
        }
        //floating point의 양의 무한대 표기법에 따라 bit code 생성 후 float 자료형으로 변환하여 반환
        return (float)float_32bit;
    }
    else if(exp_num == 31 && frac_num != 0){
        //float과 같은 32bit 자료형 unsigned int 선언 (shift 연산을 위한 자료형)
        unsigned int float_32bit = 0;
        //sign(0으로 해도 무관)
        float_32bit = float_32bit << 1;
        //exp
        for(int k=0; k<8; k++){
            float_32bit = float_32bit << 1;
            float_32bit++;
        }
        //frac
        for (int k=0; k<23; k++){
            float_32bit = float_32bit << 1;
            float_32bit++;
        }
        //floating point의 NaN 표기법에 따라 bit code 생성 후 float 자료형으로 변환하여 반환
        return (float)float_32bit;
    }

    //sign 구하기
    if(sign == 0){
        sign_num = 1;
    }
    else{
        sign_num = -1;
    }
    //e 구하기
    int e = exp_num - bias;
    //decimal 구하기
    //math.h 없이 양의 거듭제곱 연산을 위한 배열 선언
    float plus_power[16] = 
    {
        1, 2, 4, 8, 16, 
        32, 64, 128, 256, 512, 
        1024, 2048, 4096, 8192, 16384, 32768};
    //math.h 없이 음의 거듭제곱 연산을 위한 배열 선언
    float minus_power[16] = 
    {
        1, 0.5, 0.25, 0.125, 0.0625, 
        0.03125, 0.015625, 0.0078125, 0.00390625, 0.001953125, 
        0.0009765625, 0.00048828125, 0.000244140625, 0.0001220703125, 
        0.00006103515625, 0.000030517578125
    };
    //10bit까지의 frac에 대해
    for(int i=0; i<10; i++){
        //양의 지수 시 양의 거듭제곱 연산
        if(float_binary[i] == 1 && e - i >= 0){
            float_to_sfp += plus_power[e - i];
        }
        //음의 지수 시 음의 거듭제곱 연산
        else if(float_binary[i] == 1 && e - i < 0){
            float_to_sfp += minus_power[i - e];
        }
    }
    //sign을 곱하여 부호 결정
    float_to_sfp *= sign_num;
    //디버깅용
    //printf("\nresult: %f", float_to_sfp);
    //float 자료형 반환
    return float_to_sfp;
}

sfp sfp_add(sfp a, sfp b){
    //printf("\nadd:");
    sfp result = 0;
    int sfp_add_16bit[16] = {0, };

    //input에서 sign을 추출하는 변수
    sfp sign_a, sign_b;
    //input에서 exp를 추출하는 변수
    sfp exp_a, exp_b;
    sfp exp_s;
    //input에서 frac을 추출하는 변수
    sfp frac_a, frac_b;

    
    //예외처리를 위한 변수
    sfp p_infin, m_infin, nan = 0;

    //input에서 sign, exp, frac을 추출
    sign_a = a >> 15;
    exp_a = a >> 10 & eraiser(5);
    frac_a = a & eraiser(10);
    
    //input에서 sign, exp, frac을 추출
    sign_b = b >> 15;
    exp_b = b >> 10 & eraiser(5);
    frac_b = b & eraiser(10);

    int e_a = exp_a -15;
    int e_b = exp_b -15;
    int e_s;
    
    //printf("%d %d\n", e_a, e_b);

    //예외 체크 배열
    int p_infin_check[16] =  { 0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 };
	int m_infin_check[16] = { 1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 };
	int nan_check[16] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
    for(int i=0; i<16; i++){
        if(p_infin_check[i] == 0){
            p_infin = p_infin << 1;
        }
        else{
            if(p_infin_check[i] == 1){
                p_infin = p_infin << 1;
                p_infin++;
            }
        }
    }
    for(int i=0; i<16; i++){
        if(m_infin_check[i] == 0){
            m_infin = m_infin << 1;
        }
        else{
            if(m_infin_check[i] == 1){
                m_infin = m_infin << 1;
                m_infin++;
            }
        }
    }
    for(int i=0; i<16; i++){
        if(nan_check[i] == 0){
            nan = nan << 1;
        }
        else{
            if(nan_check[i] == 1){
                nan = nan << 1;
                nan++;
            }
        }
    }


    //예외 처리
    if(sign_a == 0 && exp_a == 31 && frac_a == 0){
        if(sign_b == 0 && exp_b == 31 && frac_b == 0){
            return p_infin;
        }
        else if(sign_b == 1 && exp_b == 31 && frac_b == 0){
            return nan;
        }
        else if(exp_b == 31 && frac_b != 0){
            return nan;
        }
        else{
            return p_infin;
        }
    }
    else if(sign_a == 1 && exp_a == 31 && frac_a == 0){
        if(sign_b == 0 && exp_b == 31 && frac_b == 0){
            return nan;
        }
        else if(sign_b == 1 && exp_b == 31 && frac_b == 0){
            return m_infin;
        }
        else if(exp_b == 31 && frac_b != 0){
            return nan;
        }
        else{
            return m_infin;
        }
    }
    else if(exp_a == 31 && frac_a != 0){
        return nan;
    }

    int m_10bit[13] = {0, };
    int sfp_a_16bit[16] = {0, };
    int sfp_b_16bit[16] = {0, };

    int num_a, index_a = 0;
    int num_b, index_b = 0;

    for (int i=0; i<16; i++){
        num_a = a >> i & eraiser(1);
        sfp_a_16bit[15-index_a++] = num_a;

        num_b = b >> i & eraiser(1);
        sfp_b_16bit[15-index_b++] = num_b;
    }


    //m 연산
    if(e_a > e_b){
        //printf("\nexp_a big");
        e_s = e_a;
        int m_a_10bit[12] = { 0,1,0,0,0,0,0,0,0,0,0,0 };
        int m_b_10bit[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

        if(sign_a > 0 && a < 1){
            m_a_10bit[1] = 0;
        }
        else if(sign_a < 0 && a > -1){
            m_a_10bit[1] = 0;

        }
        if(sign_b > 0 && b < 1){
            m_b_10bit[1] = 0;
        }
        else if(sign_b < 0 && b > -1){
            m_b_10bit[1] = 0;
        }

        int gap = e_a - e_b;
        //printf("\ngap: %u\n", gap);
        frac_b = frac_b >> gap;
        // m_b_10bit[gap + 1] = 1;
        
        int num_frac_a, index_frac_a = 0;
        int num_frac_b, index_frac_b = 0;

        for(int i=0; i<10; i++){
            num_frac_a = frac_a >> i & eraiser(1);
            //printf("%d ", num_frac_a);
            m_a_10bit[11-i] = num_frac_a;
        }
        
        for(int i=0; i<10; i++){
            num_frac_b = frac_b >> i & eraiser(1);
            //printf("%d ", num_frac_b);
            m_b_10bit[11-i] = num_frac_b;
        }
        m_b_10bit[gap + 1] = 1;
        
        //carry 저장하는 변수
        int carry = 0;
        // + +
        if(sign_a == 0 && sign_b == 0){
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0)
						m_10bit[i] = 0;
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 1;
					}
					else { // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0) {
						m_10bit[i] = 1;
						carry = 0;
					}
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else { // 둘 중 하나만 1인 경우
						m_10bit[i] = 0;
						carry = 1;
					}
				}
			}
        }
        else if(sign_a == 1 && sign_b == 1){
            sfp_add_16bit[0] = 1;
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0)
						m_10bit[i] = 0;
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 1;
					}
					else { // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0) {
						m_10bit[i] = 1;
						carry = 0;
					}
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else { // 둘 중 하나만 1인 경우
						m_10bit[i] = 0;
						carry = 1;
					}
				}
			}
        }
        else if(sign_a == 1 || sign_b == 1){
            if(sign_a == 1){
                sfp_add_16bit[0] = 1;
            }
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0){
                        m_10bit[i] = 0;
                    }
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 0;
					}
					else if(m_a_10bit[i] == 0 && m_b_10bit[i] == 1){ // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
                        carry = 1;
					}
                    else if(m_a_10bit[i] == 1 && m_b_10bit[i] == 0){ // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
                        carry = 0;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0){
                        m_10bit[i] = 1;
                        carry = 1;
                    }
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else if(m_a_10bit[i] == 0 && m_b_10bit[i] == 1){ // 둘 중 하나만 1인경우
						m_10bit[i] = 0;
                        carry = 1;
					}
                    else if(m_a_10bit[i] == 1 && m_b_10bit[i] == 0){ // 둘 중 하나만 1인경우
						m_10bit[i] = 0;
                        carry = 0;
					}
				}
			}
        }
    }
    else if(e_a < e_b){
        //printf("\nexp_b big");
        e_s = e_b;
        int gap = e_b - e_a;

        int m_a_10bit[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
        int m_b_10bit[12] = { 0,1,0,0,0,0,0,0,0,0,0,0 };

        if(sign_a > 0 && a < 1){
            m_a_10bit[1] = 0;
        }
        else if(sign_a < 0 && a > -1){
            m_a_10bit[1] = 0;

        }
        if(sign_b > 0 && b < 1){
            m_b_10bit[1] = 0;
        }
        else if(sign_b < 0 && b > -1){
            m_b_10bit[1] = 0;
        }

        //printf("\ngap: %u", gap);
        frac_a = frac_a >> gap;
        // m_a_10bit[gap + 1] = 1;
        
        int num_frac_a, index_frac_a = 0;
        int num_frac_b, index_frac_b = 0;

        for(int i=0; i<10; i++){
            num_frac_b = frac_b >> i & eraiser(1);
            //printf(" %d", num_frac_b);
            m_b_10bit[11-i] = num_frac_b;
        }
        for(int i=0; i<10; i++){
            num_frac_a = frac_a >> i & eraiser(1);
            //printf(" %d", num_frac_a);
            m_a_10bit[11-i] = num_frac_a;
        }
        m_a_10bit[gap + 1] = 1;
        
        //carry 저장하는 변수
        int carry = 0;
        // + +
        if(sign_a == 0 && sign_b == 0){
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0)
						m_10bit[i] = 0;
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 1;
					}
					else { // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0) {
						m_10bit[i] = 1;
						carry = 0;
					}
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else { // 둘 중 하나만 1인 경우
						m_10bit[i] = 0;
						carry = 1;
					}
				}
			}
        }
        else if(sign_a == 1 && sign_b == 1){
            sfp_add_16bit[0] = 1;
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0)
						m_10bit[i] = 0;
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 1;
					}
					else { // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0) {
						m_10bit[i] = 1;
						carry = 0;
					}
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else { // 둘 중 하나만 1인 경우
						m_10bit[i] = 0;
						carry = 1;
					}
				}
			}
        }
        else if(sign_a == 1 || sign_b == 1){
            if(sign_b == 1){
                sfp_add_16bit[0] = 1;
            }
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0){
                        m_10bit[i] = 0;
                    }
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 0;
					}
					else if(m_a_10bit[i] == 0 && m_b_10bit[i] == 1){ // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
                        carry = 0;
					}
                    else if(m_a_10bit[i] == 1 && m_b_10bit[i] == 0){ // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
                        carry = 1;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0){
                        m_10bit[i] = 1;
                        carry = 1;
                    }
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else if(m_a_10bit[i] == 0 && m_b_10bit[i] == 1){ // 둘 중 하나만 1인경우
						m_10bit[i] = 0;
                        carry = 0;
					}
                    else if(m_a_10bit[i] == 1 && m_b_10bit[i] == 0){ // 둘 중 하나만 1인경우
						m_10bit[i] = 0;
                        carry = 1;
					}
				}
			}
        }

    }
    else{
        //printf("\nsame exp");
        e_s = e_a;

        int m_a_10bit[12] = { 0,1,0,0,0,0,0,0,0,0,0,0 };
        int m_b_10bit[12] = { 0,1,0,0,0,0,0,0,0,0,0,0 };

        if(sign_a > 0 && a < 1){
            m_a_10bit[1] = 0;
        }
        else if(sign_a < 0 && a > -1){
            m_a_10bit[1] = 0;

        }
        if(sign_b > 0 && b < 1){
            m_b_10bit[1] = 0;
        }
        else if(sign_b < 0 && b > -1){
            m_b_10bit[1] = 0;
        }
        
        int num_frac_a, index_frac_a = 0;
        int num_frac_b, index_frac_b = 0;

        for(int i=0; i<10; i++){
            num_frac_a = frac_a >> i & eraiser(1);
            m_a_10bit[11-i] = num_frac_a;
        }
        for(int i=0; i<10; i++){
            num_frac_b = frac_b >> i & eraiser(1);
            m_b_10bit[11-i] = num_frac_b;
        }

        
        //carry 저장하는 변수
        int carry = 0;
        // + +
        if(sign_a == 0 && sign_b == 0){
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0)
						m_10bit[i] = 0;
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 1;
					}
					else { // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0) {
						m_10bit[i] = 1;
						carry = 0;
					}
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else { // 둘 중 하나만 1인 경우
						m_10bit[i] = 0;
						carry = 1;
					}
				}
			}
        }
        else if(sign_a == 1 && sign_b == 1){
            sfp_add_16bit[0] = 1;
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0)
						m_10bit[i] = 0;
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 1;
					}
					else { // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0) {
						m_10bit[i] = 1;
						carry = 0;
					}
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else { // 둘 중 하나만 1인 경우
						m_10bit[i] = 0;
						carry = 1;
					}
				}
			}
        }
        else if(sign_a == 1 || sign_b == 1){
            for (int i = 11; i >= 0; i--) {
				if (carry == 0) {// 직전 연산에서 넘어온 1이 없다면
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0){
                        m_10bit[i] = 0;
                    }
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 0;
						carry = 0;
					}
					else if(m_a_10bit[i] == 0 && m_b_10bit[i] == 1){ // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
                        carry = 1;
					}
                    else if(m_a_10bit[i] == 1 && m_b_10bit[i] == 0){ // 둘 중 하나만 1인경우
						m_10bit[i] = 1;
                        carry = 0;
					}
				} 
				else {
					if (m_a_10bit[i] == 0 && m_b_10bit[i] == 0){
                        m_10bit[i] = 1;
                        carry = 1;
                    }
					else if (m_a_10bit[i] == 1 && m_b_10bit[i] == 1) {
						m_10bit[i] = 1;
						carry = 1;
					}
					else if(m_a_10bit[i] == 0 && m_b_10bit[i] == 1){ // 둘 중 하나만 1인경우
						m_10bit[i] = 0;
                        carry = 1;
					}
                    else if(m_a_10bit[i] == 1 && m_b_10bit[i] == 0){ // 둘 중 하나만 1인경우
						m_10bit[i] = 0;
                        carry = 0;
					}
				}
			}
        }

    }
    
    exp_s = e_s + 15;
    
    if(exp_s > 30 && sfp_add_16bit[0] == 0){
        return p_infin;
    }
    else if(exp_s > 30 && sfp_add_16bit[0] == 1){
        return m_infin;
    }

    if(m_10bit[0] == 1){
        exp_s += 1;
        for(int k=11; k>0; k--){
            m_10bit[k] = m_10bit[k-1];
        }
    }
    if(m_10bit[0] == 0 && sfp_add_16bit[0] == 1){
        exp_s -= 1;
        for(int k=2; k<11; k++){
            m_10bit[k] = m_10bit[k+1];
        }
    }
    
    int round[11] = { 0,0,0,0,0,0,0,0,0,0,1 };
	int carry_round = 0;
    
	if (m_10bit[9] == 1 && m_10bit[10] == 1) {

		for (int i = 10; i >= 0; i--) {
			if (carry_round == 0) {// 직전 연산에서 넘어온 1이 없다면
				if (m_10bit[i] == 0 && round[i] == 0)
					m_10bit[i] = 0;
				else if (m_10bit[i] == 1 && round[i] == 1) {
					m_10bit[i] = 0;
					carry_round = 1;
				}
				else { // 둘 중 하나만 1인경우
					m_10bit[i] = 1;
				}
			}
			else {
				if (m_10bit[i] == 0 && round[i] == 0) {
					m_10bit[i] = 1;
					carry_round = 0;
				}
				else if (m_10bit[i] == 1 && round[i] == 1) {
					m_10bit[i] = 1;
					carry_round = 1;
				}
				else { // 둘 중 하나만 1인 경우
					m_10bit[i] = 0;
					carry_round = 1;
			    }
		    }
	    }
    }    
        

    for (int i=1; i<11; i++){
        //printf(" %d", m_10bit[i + 2]);
        sfp_add_16bit[i + 5] = m_10bit[i + 1];
    }


    // exp 2진수 변환 후 SFParr에 담기
	int exp_remain[5] = { 0, }; // 0<= exp <= 62  
	int idx2 = 0;
	while (1) {
		exp_remain[idx2++] = exp_s % 2;
		exp_s = exp_s / 2;
		if (exp_s == 0)
			break;
	}
	// exp를 담는다.
	for (int i = 4; i >= 0; i--) {
		sfp_add_16bit[4 - i + 1] = exp_remain[i];
	}

    //step4. bit 조작
    for(int i=0; i<16; i++){
        //디버깅용
        //printf("%d", sfp_binary[i]);
        //배열의 값이 1이면, shift 후 1을 더해서 비트 1조작
        if(sfp_add_16bit[i] == 1){
            result = result << 1;
	        result++;
        }
        //배열의 값이 1이면, shift해서 비트 0조작
        else{
            result = result << 1;
        }
    }


    return result;  
}

sfp sfp_mul(sfp a, sfp b){
    sfp result = 0;

    //input에서 sign을 추출하는 변수
    sfp sign_a, sign_b;
    //input에서 exp를 추출하는 변수
    sfp exp_a, exp_b;
    //input에서 frac을 추출하는 변수
    sfp frac_a, frac_b;
    //예외처리를 위한 변수
    sfp p_infin, m_infin, nan = 0;

    //input에서 sign, exp, frac을 추출
    sign_a = a >> 15;
    exp_a = a >> 10 & eraiser(5);
    frac_a = a & eraiser(10);
    int frac_a_num = 10;
    while((frac_a & 1) == 0){
        frac_a = frac_a >> 1;
        frac_a_num -= 1;
    }
    
    //input에서 sign, exp, frac을 추출
    sign_b = b >> 15;
    exp_b = b >> 10 & eraiser(5);
    frac_b = b & eraiser(10);
    int frac_b_num = 10;
    while((frac_b & 1) == 0){
        frac_b = frac_b >> 1;
        frac_b_num -= 1;
    }

    int e_a = exp_a -15;
    int e_b = exp_b -15;
    //printf("\ne_a: %d e_b: %d", e_a, e_b);
    //printf("\nexp_a: %02x exp_b: %02x", exp_a, exp_b);
    //printf("\nfrac_a: %04x frac_b: %04x", frac_a, frac_b);
    //printf("\nfrac_a_num: %d frac_b_num: %d", frac_a_num, frac_b_num);

    //예외 체크 배열
    int p_infin_check[16] =  { 0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 };
	int m_infin_check[16] = { 1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 };
	int nan_check[16] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
    for(int i=0; i<16; i++){
        if(p_infin_check[i] == 0){
            p_infin = p_infin << 1;
        }
        else{
            if(p_infin_check[i] == 1){
                p_infin = p_infin << 1;
                p_infin++;
            }
        }
    }
    for(int i=0; i<16; i++){
        if(m_infin_check[i] == 0){
            m_infin = m_infin << 1;
        }
        else{
            if(m_infin_check[i] == 1){
                m_infin = m_infin << 1;
                m_infin++;
            }
        }
    }
    for(int i=0; i<16; i++){
        if(nan_check[i] == 0){
            nan = nan << 1;
        }
        else{
            if(nan_check[i] == 1){
                nan = nan << 1;
                nan++;
            }
        }
    }


    //예외 처리
    if(sign_a == 0 && exp_a == 31 && frac_a == 0){
        if(sign_b == 0 && exp_b == 31 && frac_b == 0){
            return p_infin;
        }
        else if(sign_b == 1 && exp_b == 31 && frac_b == 0){
            return m_infin;
        }
        else if(exp_b == 31 && frac_b != 0){
            return nan;
        }
        else if(exp_b == 0 && frac_b == 0){
            return nan;
        }
        else{
            return p_infin;
        }
    }
    else if(sign_a == 1 && exp_a == 31 && frac_a == 0){
        if(sign_b == 0 && exp_b == 31 && frac_b == 0){
            return m_infin;
        }
        else if(sign_b == 1 && exp_b == 31 && frac_b == 0){
            return p_infin;
        }
        else if(exp_b == 31 && frac_b != 0){
            return nan;
        }
        else if(exp_b == 0 && frac_b == 0){
            return nan;
        }
        else{
            return m_infin;
        }
    }
    else if(exp_a == 31 && frac_a != 0){
        return nan;
    }

    //새로운 수의 e 구하기
    int e_s = e_a + e_b;
    //printf("\ne_s: %x", e_s);

    sfp sign_s;
    sfp exp_s;
    unsigned int frac_s = 0;
    
    //sign_s 결정
    if(sign_a != sign_b){
        sign_s = 1;
    }
    else{
        sign_s = 0;
    }
    //printf("\nsign_s: %x", sign_s);

    //exp_s 결정
    exp_s = exp_a + exp_b - 15;
    //printf("\nexp_s: %02x", exp_s);

    if(exp_a != 0){
        frac_a = 1 << frac_a_num | frac_a;
    }
    if(exp_a != 0){
        frac_b = 1 << frac_b_num | frac_b;
    }

    //printf("\nfrac_a: %04x frac_b: %04x", frac_a, frac_b);
    
    //frac_s 결정
    while(frac_b > 0){
        if((frac_b & 1) != 0){
            frac_s = frac_s + frac_a;
        }
        frac_a = frac_a << 1;
        frac_b = frac_b >> 1;
    }
    //printf("\nfrac_s: %08x", frac_s);

    if(frac_a_num > frac_b_num){
        frac_s = frac_s << (frac_a_num - frac_b_num - 1);
        exp_s = exp_s + (frac_a_num - frac_b_num - 1);
        //printf("\nchanged exp_s: %02x", exp_s);
    }
    else if(frac_a_num > frac_b_num){
        frac_s = frac_s << (frac_b_num - frac_a_num - 1);
        exp_s = exp_s + (frac_a_num - frac_b_num - 1);
        //printf("\nchanged exp_s: %02x", exp_s);
    }
    //printf("\nfrac_s: %08x", frac_s & eraiser(10));

    result = (sign_s << 15 | exp_s << 10 | frac_s & eraiser(10));
    //printf("\nresult: %04x", result);

    return result;
}

char* sfp2bits(sfp result){
    
    char * result_string = (char *)malloc(sizeof(char)*17);

    int num_result, index_result = 0;

    for (int i=0; i<16; i++){
        num_result = result >> i & eraiser(1);
        
        if(num_result == 0){
            result_string[15-index_result++] = '0';
        }
        else if(num_result == 1){
            result_string[15-index_result++] = '1';
        }
    }
    result_string[16] = '\0';

    // for(int j=0; j<16; j++){
    //     printf("%s", result_string[j]);
    // }

    return result_string;
}