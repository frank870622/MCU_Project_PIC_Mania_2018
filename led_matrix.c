/*
 Change log:
 * 
 * 2019/01/12 00:14
 *   in score_Draw():
 *     thousand is now (score/1000)%10 instead of score/1000
 *      as the score exceed 10000 in display test
 *   added global variables:
 *     score0, score1, score2, scoreB
 *       debug use
 *   in main(): 
 *     changed delay in forever loop from 100ms to 10ms
 *       so debug run ends faster
 *   todo:
 *     fix some number in matrixData_num are vertically inverted
 *     check interrupt, as 9000+ interrups in RB detected in display test
 * 
 * 2019/01/11 10:53
 *   started changelog in the beginning of file
 *   shift_draw now draws the content of matrixData_map
 *    24 blank line in the front and
 *    24 blank line in the behind will be auto added.
 */

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config PBADEN = OFF
#pragma config LVP = OFF

#include <xc.h>
#define DIN LATDbits.LATD0
#define CS LATDbits.LATD1
#define CLK LATDbits.LATD2
#define _XTAL_FREQ 8000000
void init();
void MAX7219_1Unit(char reg_addr, char reg_data);
void send_byte(char data);
void MAX7219_AllUnit(char reg_addr, char reg_data);
void MAX7219_indexUnit(int unit_index, int reg_addr, int reg_data);
void shift_Draw();
void score_Draw();
void interrupt_init();
const unsigned char getCurrentLine();

int MAX7219_units = 4;
int string_len = 48;
int game_over = 5000;
int score = 0;
unsigned int score0 = 0, score1 = 0, score2 = 0, scoreB = 0;
int hit_check = 0;
unsigned char* matrixData_map_head;
unsigned int matrixData_map_empty_front = 8 * 3 + 1;
unsigned char* matrixData_map_end = NULL;

char matrixData_hit[24] = {
    //space
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    //H
    0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x00,
    //M
    0x00, 0x41, 0x41, 0x49, 0x55, 0x63, 0x41, 0x00
};

char matrixData_num[80] = {
    //0
    0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00,
    //1
    0x7E, 0x10, 0x10, 0x10, 0x10, 0x14, 0x18, 0x00,
    //2
    0x7C, 0x08, 0x10, 0x20, 0x44, 0x44, 0x38, 0x00,
    //3
    0x18, 0x24, 0x20, 0x18, 0x20, 0x24, 0x18, 0x00,
    //4
    0x10, 0x10, 0x10, 0x7E, 0x12, 0x14, 0x18, 0x00,
    //5
    0x38, 0x44, 0x40, 0x40, 0x3C, 0x04, 0x7C, 0x00,
    //6
    0x38, 0x44, 0x44, 0x3C, 0x04, 0x08, 0x30, 0x00,
    //7
    0x20, 0x20, 0x20, 0x20, 0x24, 0x24, 0x3C, 0x00,
    //8
    0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38, 0x00,
    //9
    0x18, 0x20, 0x40, 0x78, 0x44, 0x44, 0x38, 0x00
};


unsigned char matrixData_map[] = {
    0x03, 0x03, 0x0c, 0x0c, 0x30, 0x30, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00,
    0x30, 0x30, 0x0c, 0x0c, 0x03, 0x03, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00,
    0x30, 0x30, 0x30, 0x30, 0x03, 0x03, 0x03, 0x03,
    0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x00, 0x00,
    0x03, 0x03, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0x30, 0x30, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0x30, 0x30, 0x03, 0x03,

    0xc0, 0xc0, 0x30, 0x30, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00,
    0x0c, 0x0c, 0x30, 0x30, 0xc0, 0xc0, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00,
    0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0,
    0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x00, 0x00,
    0xc0, 0xc0, 0xc0, 0xc0, 0x03, 0x03, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00,

    0x03, 0x03, 0x0c, 0x0c, 0x30, 0x30, 0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00,
    0x30, 0x30, 0x0c, 0x0c, 0x03, 0x03, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00,
    0x30, 0x30, 0x30, 0x30, 0x03, 0x03, 0x03, 0x03,
    0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x00, 0x00,
    0x03, 0x03, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0x30, 0x30, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0x30, 0x30, 0x03, 0x03,

    0xc0, 0xc0, 0x30, 0x30, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00,
    0x0c, 0x0c, 0x30, 0x30, 0xc0, 0xc0, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00,
    0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0,
    0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x00, 0x00,
    0xc0, 0xc0, 0xc0, 0xc0, 0x03, 0x03, 0x00, 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00,

    //11111 1111111 1111111 1111111 1111111 1111111 1111111 1111111
    0x03, 0x03, 0xc0, 0xc0, 0x0c, 0x0c, 0x30, 0x30, 0x03, 0x03, 0x00, 0x00,
    0xc0, 0xc0, 0x30, 0x30, 0xc0, 0xc0, 0x30, 0x30, 0xc0, 0xc0, 0x30, 0x30, 0xc0, 0xc0, 0x00, 0x00,
    0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x00, 0x00,
    0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x00, 0x00,
    0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x00, 0x00,
    0x03, 0x03, 0x30, 0x30, 0x03, 0x03, 0x30, 0x30, 0x03, 0x03, 0x30, 0x30, 0x03, 0x03, 0x00, 0x00,
    0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0x00, 0x00,
    0x30, 0x30, 0xc0, 0xc0, 0x0c, 0x0c, 0x03, 0x03, 0x30, 0x30, 0xc0, 0xc0, 0x0c, 0x0c, 0x00, 0x00,
    //1111111 1111111 1111111 1111111 1111111 1111111 1111111 1111111
    0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0x00, 0x00,
    0xf0, 0xf0, 0x0f, 0x0f, 0xf0, 0xf0, 0x0f, 0x0f, 0xf0, 0xf0, 0x0f, 0x0f, 0xf0, 0xf0, 0x00, 0x00,
    0x30, 0x30, 0xc0, 0xc0, 0x30, 0x30, 0xc0, 0xc0, 0x30, 0x30, 0xc0, 0xc0, 0x30, 0x30, 0x00, 0x00,
    0x3c, 0x3c, 0xc3, 0xc3, 0x3c, 0x3c, 0xc3, 0xc3, 0x3c, 0x3c, 0xc3, 0xc3, 0x3c, 0x3c, 0x00, 0x00,
    0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x0c, 0x0c, 0xc0, 0xc0, 0x00, 0x00,
    0x3c, 0x3c, 0xf0, 0xf0, 0x3c, 0x3c, 0xf0, 0xf0, 0x3c, 0x3c, 0xf0, 0xf0, 0x3c, 0x3c, 0x00, 0x00,
    0x03, 0x03, 0x30, 0x30, 0x03, 0x03, 0x30, 0x30, 0x03, 0x03, 0x30, 0x30, 0x03, 0x03, 0x00, 0x00,
    0x3c, 0x3c, 0x0f, 0x0f, 0x3c, 0x3c, 0x0f, 0x0f, 0x3c, 0x3c, 0x0f, 0x0f, 0x3c, 0x3c, 0x00, 0x00,

    //1111111 1111111 1111111 1111111 1111111 1111111 1111111 1111111
    0xc0, 0xc0, 0x0f, 0x0f, 0xc0, 0xc0, 0x0f, 0x0f, 0xc0, 0xc0, 0x0f, 0x0f, 0xc0, 0xc0, 0x00, 0x00,
    0x03, 0x03, 0xf0, 0xf0, 0x03, 0x03, 0xf0, 0xf0, 0x03, 0x03, 0xf0, 0xf0, 0x03, 0x03, 0x00, 0x00,
    0xc0, 0xc0, 0x30, 0x30, 0x0c, 0x0c, 0x03, 0x03, 0x0c, 0x0c, 0x30, 0x30, 0xc0, 0xc0, 0x00, 0x00,
    0xf0, 0xf0, 0x3c, 0x3c, 0x0f, 0x0f, 0x3c, 0x3c, 0xf0, 0xf0, 0x3c, 0x3c, 0x0f, 0x0f, 0x00, 0x00,
    0x0f, 0x0f, 0x3c, 0x3c, 0xf0, 0xf0, 0x3c, 0x3c, 0x0f, 0x0f, 0x3c, 0x3c, 0xf0, 0xf0, 0x00, 0x00,
    0xfc, 0xfc, 0x3f, 0x3f, 0xfc, 0xfc, 0x3f, 0x3f, 0xfc, 0xfc, 0x3f, 0x3f, 0xfc, 0xfc, 0x00, 0x00,
    0x03, 0x03, 0xfc, 0xfc, 0x03, 0x03, 0xfc, 0xfc, 0x03, 0x03, 0xfc, 0xfc, 0x03, 0x03, 0x00, 0x00,
    0xc0, 0xc0, 0x3f, 0x3f, 0xc0, 0xc0, 0x3f, 0x3f, 0xc0, 0xc0, 0x3f, 0x3f, 0xc0, 0xc0, 0x00, 0x00,


    //1111111 1111111 1111111 1111111 1111111 1111111 1111111 1111111
    0x03, 0x03, 0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0xc0, 0xc0, 0x03, 0x03, 0xc0, 0xc0, 0x00, 0x00,
    0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00,
    0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x0c, 0x0c, 0x30, 0x30, 0x00, 0x00,
    0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0, 0x00, 0x00,
    0x03, 0x03, 0x3c, 0x3c, 0x03, 0x03, 0xf0, 0xf0, 0x03, 0x03, 0x3c, 0x3c, 0x03, 0x03, 0x00, 0x00,
    0xc0, 0xc0, 0x3c, 0x3c, 0xc0, 0xc0, 0x0f, 0x0f, 0xc0, 0xc0, 0x3c, 0x3c, 0xc0, 0xc0, 0x00, 0x00,
    0x03, 0x03, 0x0f, 0x0f, 0x3f, 0x3f, 0xff, 0xff, 0x3f, 0x3f, 0x0f, 0x0f, 0x03, 0x03, 0x00, 0x00,
    0xc0, 0xc0, 0xf0, 0xf0, 0xfc, 0xfc, 0xff, 0xff, 0xfc, 0xfc, 0xf0, 0xf0, 0xc0, 0xc0, 0x00, 0x00
};

void __interrupt(high_priority) HI_ISR() {
    if (INTCONbits.INT0IF) {
        __delay_us(900);
        if (!PORTBbits.RB0) {
            if (getCurrentLine() & 0b00000011) {
                ++score;
                ++score0;
                hit_check = 5;
            } else {
                if(score)
                    score -= 1;
                hit_check = 10;
            }
        }
        INTCONbits.INT0IF = 0;
    }
    if (INTCON3bits.INT1IF) {
        __delay_us(900);
        if (!PORTBbits.RB1) {
            if (getCurrentLine() & 0b00001100) {
                ++score;
                ++score1;
                hit_check = 5;
            } else {
                if(score)
                    score -= 1;
                hit_check = 10;
            }
        }
        INTCON3bits.INT1IF = 0;
    }
    if (INTCON3bits.INT2IF) {
        __delay_us(900);
        if (!PORTBbits.RB2) {
            if (getCurrentLine() & 0b00110000) {
                ++score;
                ++score2;
                hit_check = 5;
            } else {
                if(score)
                    score -= 1;
                hit_check = 10;
            }
        }
        INTCON3bits.INT2IF = 0;
    }
    if (INTCONbits.RBIE && INTCONbits.RBIF) {
        if (!PORTBbits.RB4) {
            if (getCurrentLine() & 0b11000000) {
                ++score;
                ++scoreB;
                hit_check = 5;
            } else {
                if(score)
                    score -= 1;
                hit_check = 10;
            }
        }
        INTCONbits.RBIF = 0;
    }
}

void main(void) {
    TRISD = 0x00;
    TRISDbits.TRISD3 = 1;
    OSCCONbits.IRCF = 7;
    LATD = 0x00;
    interrupt_init();
    //while (1);
    init();
    while (PORTDbits.RD3 == 1);
    hit_check = 0;
    while (1) {
        if (game_over > 0) {
            --game_over;
            shift_Draw();
        } else score_Draw();
        __delay_ms(35);
        //delay();
    }

    return;
}

void interrupt_init() {
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    TRISBbits.RB2 = 1;
    TRISBbits.RB4 = 1;
    LATBbits.LATB0 = 0;
    LATBbits.LATB1 = 0;
    LATBbits.LATB2 = 0;
    INTCON2bits.INTEDG0 = 0;
    INTCON2bits.INTEDG1 = 0;
    INTCON2bits.INTEDG2 = 0;
    INTCONbits.GIE = 1;
    INTCONbits.INT0IE = 1;
    INTCONbits.INT0IF = 0;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IP = 1;
    INTCON3bits.INT2IE = 1;
    INTCON3bits.INT2IF = 0;
    INTCON3bits.INT2IP = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    INTCON2bits.RBIP = 1;
    INTCON2bits.RBPU = 1;
    RCONbits.IPEN = 1;
}

void init() {
    matrixData_map_head = matrixData_map;
    matrixData_map_end = matrixData_map + sizeof (matrixData_map);
    CS = 1;
    CLK = 1;
    MAX7219_AllUnit(0x0b, 0x07);
    MAX7219_AllUnit(0x09, 0x00);
    MAX7219_AllUnit(0x0c, 0x01);
    MAX7219_AllUnit(0x0f, 0x00);

    for (int i = 1; i <= 8; i++) {// ???? LED ????
        MAX7219_AllUnit(i, 0);
    }
    MAX7219_AllUnit(0x0a, 0x00);
    __delay_ms(1000);
    //delay();
}

void send_byte(char data) {
    int i = 8;
    int mask;
    while (i > 0) {
        mask = 0x01 << (i - 1);
        CLK = 0;
        if (data & mask) {
            DIN = 1;
        } else {
            DIN = 0;
        }
        CLK = 1;
        --i;
    }
}

void MAX7219_1Unit(char reg_addr, char reg_data) {
    CS = 0;
    send_byte(reg_addr);
    send_byte(reg_data);
    CS = 1;
}

void MAX7219_AllUnit(char reg_addr, char reg_data) {// ??????? MAX7219 ?????????
    CS = 0; // ??? LOAD ????? LOW
    for (int c = 1; c <= MAX7219_units; c++) {
        send_byte(reg_addr); // ????????????
        send_byte(reg_data); // ??????
    }
    CS = 1; // ??? LOAD ???? HIGH
}

void MAX7219_indexUnit(int unit_index, int reg_addr, int reg_data) {// ??????????? MAX7219 ???????????
    int c = 0;
    CS = 0; // ??? LOAD ?????LOW
    // ?????????????
    for (c = MAX7219_units; c > unit_index; c--) {
        send_byte(0); // NO-OP ???
        send_byte(0); // ?? = 0
    }

    send_byte(reg_addr); // ????????????
    send_byte(reg_data); // ??????

    for (c = unit_index - 1; c >= 1; c--) {
        send_byte(0); // NO-OP ???
        send_byte(0); // ?? = 0
    }
    CS = 1; // ??? LOAD ???? HIGH
}

void shift_Draw() {
    /*
        for (int j = 1; j <= 4; ++j) {
            for (int i = 1; i <= 8; ++i) {
                MAX7219_indexUnit(j, i, matrixData_song[(now_head + i + 8 * j - 2) % string_len]);
            }
        }
     * */
    /*
    for (int j = 2; j <= 4; ++j) {
        for (int i = 1; i <= 8; ++i) {
            MAX7219_indexUnit(j, i, matrixData_70[(now_head + i + 8 * (j - 2) - 1) % string_len]);
        }
    }
     * */

    //if(matrixData_map_head >= matrixData_map_end) {// no extra empty frame
    if (matrixData_map_head > matrixData_map_end) {// will leave an empty frame
        game_over = 0;
        matrixData_map_head = matrixData_map;
        matrixData_map_empty_front = 8 * 3 + 1;
    }

    if (matrixData_map_empty_front)
        --matrixData_map_empty_front;
    else
        matrixData_map_head += 1; // this time: start from next byte

    if (matrixData_map_head <= matrixData_map_end) {
        unsigned char* matrixData_map_cur = matrixData_map_head;
        int j = 2;
        int i = 1;
        unsigned int matrixData_map_empty_front_remain = matrixData_map_empty_front;
        for (; j <= 4 && matrixData_map_empty_front_remain; ++j, i = 1) {
            for (; i <= 8 && matrixData_map_empty_front_remain; ++i, --matrixData_map_empty_front_remain) {
                MAX7219_indexUnit(j, i, 0);
            }
            if (i <= 8)
                break;
        }
        for (; j <= 4; ++j, i = 1) {
            for (; i <= 8; ++i, ++matrixData_map_cur) {
                MAX7219_indexUnit(j, i, matrixData_map_cur < matrixData_map_end ? (int) *matrixData_map_cur : 0);
            }
        }
    }

    if (hit_check & 2) {
        for (int i = 1; i <= 8; ++i) {
            MAX7219_indexUnit(1, i, matrixData_hit[i - 1 + 16]);
        }
    } else if (hit_check & 1) {
        for (int i = 1; i <= 8; ++i) {
            MAX7219_indexUnit(1, i, matrixData_hit[i - 1 + 8]);
        }
    } else {
        for (int i = 1; i <= 8; ++i) {
            MAX7219_indexUnit(1, i, matrixData_hit[i - 1]);
        }
    }

    hit_check >>= 2;
}

void score_Draw() {
    int thousand = (score / 1000) % 10;
    int hundred = (score % 1000) / 100;
    int ten = (score % 100) / 10;
    int digit = score % 10;
    for (int i = 1; i <= 8; ++i) {
        MAX7219_indexUnit(4, i, matrixData_num[8 * thousand + i - 1]);
        MAX7219_indexUnit(3, i, matrixData_num[8 * hundred + i - 1]);
        MAX7219_indexUnit(2, i, matrixData_num[8 * ten + i - 1]);
        MAX7219_indexUnit(1, i, matrixData_num[8 * digit + i - 1]);
    }
}

const unsigned char getCurrentLine() {
    if (matrixData_map_empty_front || matrixData_map_head >= matrixData_map_end)
        return 0;
    return *matrixData_map_head;
}