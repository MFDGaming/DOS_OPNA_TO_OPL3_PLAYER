#include "vgm.h"
#include "vgm_opl3.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dos.h>

#define YMF262_CLOCK 14.32
#define YM2608_CLOCK 8.0

#define SELECT_AVEKM 0x01
#define SELECT_KT 0x02
#define SELECT_AD 0x04
#define SELECT_SR 0x08
#define SELECT_WS 0x10
#define SELECT_ALL 0x1f

#define PLAY_NOTE_ALL_OFF 0x01
#define PLAY_NOTE_KEYOFF 0x02
#define PLAY_NOTE_FREQSET 0x04
#define PLAY_NOTE_KEYON 0x08

typedef struct _op {
    unsigned char am, vib, egt, ksr, mult;
    unsigned char ksl, tl;
    unsigned char ar, dr;
    unsigned char sl, rr;
    unsigned char ws;
} op_t;

unsigned char four_op_f[3] = {0xA0, 0xA1, 0xA2};

unsigned char four_op_kbf[3] = {0xB0, 0xB1, 0xB2};

unsigned char two_op_f[9] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8};

unsigned char two_op_kbf[9] = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8};

unsigned char ssg_vol_conv[16] = {63, 62, 56, 52, 46, 42, 36, 32, 28, 24, 20, 16, 12, 8, 4, 0};

static void interrupt (*old_timer)(void);
static unsigned long current_time = 0;

void write_reg(unsigned short port, unsigned char reg, unsigned char data, unsigned char second) {
    int i;
    if (second != 0) {
        port += 2;
    }
    outp(port, reg);
    i = 6;
    while (i--) inp(port);
    outp(port + 1, data);
    i = 35;
    while (i--) inp(port);
}

static void interrupt new_timer(void) {
    static unsigned long count = 0;
    ++current_time;
    count += 1193;
    if (count >= 65536) {
        count -= 65536;
        _chain_intr(old_timer);
    } else {
        outp(0x20, 0x20);
    }
}

void stop_timer(void) {
    _disable();
    setvect(0x08, old_timer);
    outp(0x43, 0x36);
    outp(0x40, 0x00);
    outp(0x40, 0x00);
    _enable();
}

void start_timer(void) {
    old_timer = getvect(0x08);
    current_time = 0;
    _disable();
    setvect(0x08, new_timer);
    outp(0x43, 0x36);
    outp(0x40, 1193 & 0xff);
    outp(0x40, (1193 >> 8) & 0xff);
    _enable();
    atexit(stop_timer);
}

void read_timer(unsigned long *res) {
    _disable();
    *res = current_time;
    _enable();
}

void sleep_ms(unsigned long us) {
    unsigned long t1, t2;
    read_timer(&t1);
    for (;;) {
        read_timer(&t2);
        if (t2 < t1) {
            break;
        } else if (t2 - t1 >= us) {
            break;
        }
    }
}

void set_op(op_t *op, unsigned char reg, unsigned char second, unsigned char select) {
    unsigned char avekm, kt, ad, sr, w = 0;
    if (select == 0 || (select & 1) == 1) {
        avekm = VGM_OPL3_SET_AM(avekm, op->am);
        avekm = VGM_OPL3_SET_VIB(avekm, op->vib);
        avekm = VGM_OPL3_SET_EGT(avekm, op->egt);
        avekm = VGM_OPL3_SET_KSR(avekm, op->ksr);
        avekm = VGM_OPL3_SET_MULT(avekm, op->mult);
        write_reg(0x220, 0x20 + reg, avekm, second);
    }
    if (select == 0 || ((select >> 1) & 1) == 1) {
        kt = VGM_OPL3_SET_KSL(kt, op->ksl);
        kt = VGM_OPL3_SET_TL(kt, op->tl);
        write_reg(0x220, 0x40 + reg, kt, second);
    }
    if (select == 0 || ((select >> 2) & 1) == 1) {
        ad = VGM_OPL3_SET_AR(ad, op->ar);
        ad = VGM_OPL3_SET_DR(ad, op->dr);
        write_reg(0x220, 0x60 + reg, ad, second);
    }
    if (select == 0 || ((select >> 3) & 1) == 1) {
        sr = VGM_OPL3_SET_SL(ad, op->sl);
        sr = VGM_OPL3_SET_RR(ad, op->rr);
        write_reg(0x220, 0x80 + reg, sr, second);
    }
    if (select == 0 || ((select >> 4) & 1) == 1) {
        w = VGM_OPL3_SET_WS(w, op->ws);
        write_reg(0x220, 0xE0 + reg, w, second);
    }
}

void play_note(unsigned short note, unsigned char block, unsigned char channel, unsigned char second, unsigned char mode, unsigned char flags) {
    static unsigned char channels_first[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    static unsigned char channels_second[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char *kbf;
    unsigned char *f;
    unsigned char *channels = NULL;
    if (mode == 0) {
        f = four_op_f;
        kbf = four_op_kbf;
    } else {
        f = two_op_f;
        kbf = two_op_kbf;
    }
    if (second) {
        channels = channels_second;
    } else {
        channels = channels_first;
    }

    if ((flags & 1) == 0 || ((flags >> 1) & 1)) {
        write_reg(0x220, kbf[channel % 9], VGM_OPL3_SET_KON(channels[channel % 9], 0), second);
    }

    if ((flags & 1) == 0 || ((flags >> 2) & 1)) {
        write_reg(0x220, f[channel % 9], note & 0xff, second);
    }

    if ((flags & 1) == 0 || ((flags >> 3) & 1)) {
        channels[channel % 9] = VGM_OPL3_SET_KON(channels[channel % 9], 1);
    }
    if ((flags & 1) == 0 || ((flags >> 2) & 1)) {
        channels[channel % 9] = VGM_OPL3_SET_BLOCK(channels[channel % 9], block);
        channels[channel % 9] = VGM_OPL3_SET_FNUMH(channels[channel % 9], note >> 8);
    }
    write_reg(0x220, kbf[channel % 9], channels[channel % 9], second);
}

void stop_all_channels() {
    int i;
    for (i = 0; i < 9; ++i) {
        write_reg(0x220, two_op_kbf[i], 0, 0);
        write_reg(0x220, two_op_kbf[i], 0, 1);
    }
}

int main(int argc, char **argv) {
    FILE *fp = NULL;
    static vgm_header_t header;
    unsigned short samples = 0;
    unsigned char reg = 0;
    unsigned char reg_data = 0;
    unsigned long i = 0;
    unsigned char command = 0;
    unsigned char do_loop = 0;

    op_t op1_ssg[3];
    op_t op2_ssg[3];
    op_t op1_0[3], op2_0[3], op3_0[3], op4_0[3];
    op_t op1_1[3], op2_1[3], op3_1[3], op4_1[3];
    op_t *pop1_0, *pop2_0, *pop3_0, *pop4_0;
    op_t *pop1_1, *pop2_1, *pop3_1, *pop4_1;
    op_t *pop1, *pop2, *pop3, *pop4;
    op_t op_empty;
    unsigned short fnums_0[3] = {0, 0, 0};
    unsigned short fnums_1[3] = {0, 0, 0};
    unsigned short *pfnums;
    unsigned short ssg_ft[3] = {0, 0, 0};
    unsigned short ssg_ct[3] = {0, 0, 0};
    unsigned char two_op = 0;

    pop1_0 = op1_0;
    pop2_0 = op2_0;
    pop3_0 = op3_0;
    pop4_0 = op4_0;

    pop1_1 = op1_1;
    pop2_1 = op2_1;
    pop3_1 = op3_1;
    pop4_1 = op4_1;

    memset(&op_empty, 0, sizeof(op_t));

    memset(op1_ssg, 0, sizeof(op_t) * 3);
    memset(op2_ssg, 0, sizeof(op_t) * 3);

    memset(op1_0, 0, sizeof(op_t) * 3);
    memset(op2_0, 0, sizeof(op_t) * 3);
    memset(op3_0, 0, sizeof(op_t) * 3);
    memset(op4_0, 0, sizeof(op_t) * 3);

    memset(op1_1, 0, sizeof(op_t) * 3);
    memset(op2_1, 0, sizeof(op_t) * 3);
    memset(op3_1, 0, sizeof(op_t) * 3);
    memset(op4_1, 0, sizeof(op_t) * 3);

    write_reg(0x220, 0x01, 0x20, 0);
    write_reg(0x220, 0x01, 0, 1);
    write_reg(0x220, 0x05, 1, 1);
    write_reg(0x220, 0x04, 0x3f, 1);
    write_reg(0x220, 0x08, 0, 0);
    //write_reg(0x220, 0xbd, 0xe0, 0);
    //write_reg(0x220, 0xbd, 0xc0, 0);
    //write_reg(0x220, 0xbd, 0xc0, 1);
    //write_reg(0x220, 0xbd, 0xe0, 1);

    write_reg(0x220, 0xc0, 0xf0 | 0x0a, 0);
    write_reg(0x220, 0xc0, 0xf0 | 0x0a, 1);

    for (i = 0; i < 3; ++i) {
        op1_0[i].egt = 1;
        op1_1[i].egt = 1;
        op2_0[i].egt = 1;
        op2_1[i].egt = 1;
        op3_0[i].egt = 1;
        op3_1[i].egt = 1;
        op4_0[i].egt = 1;
        op4_1[i].egt = 1;

        op1_0[i].ws = 0;
        op1_1[i].ws = 0;
        op2_0[i].ws = 0;
        op2_1[i].ws = 0;
        op3_0[i].ws = 0;
        op3_1[i].ws = 0;
        op4_0[i].ws = 0;
        op4_1[i].ws = 0;

        set_op(&op1_0[i], i, 0, SELECT_ALL);
        set_op(&op1_1[i], i, 1, SELECT_ALL);
        set_op(&op2_0[i], i + 3, 0, SELECT_ALL);
        set_op(&op2_1[i], i + 3, 1, SELECT_ALL);
        set_op(&op3_0[i], i + 8, 0, SELECT_ALL);
        set_op(&op3_1[i], i + 8, 1, SELECT_ALL);
        set_op(&op4_0[i], i + 11, 0, SELECT_ALL);
        set_op(&op4_1[i], i + 11, 1, SELECT_ALL);

        op1_ssg[i].am = 0;
        op1_ssg[i].ksr = 0;
        op1_ssg[i].mult = 4;
        op1_ssg[i].ar = 0;
        op1_ssg[i].dr = 0;
        op1_ssg[i].sl = 0;
        op1_ssg[i].rr = 15;
        op1_ssg[i].ksl = 0;
        op1_ssg[i].egt = 0;
        op1_ssg[i].tl = 0x1a;
        op1_ssg[i].vib = 0;
        op1_ssg[i].ws = 6;

        op2_ssg[i].am = 0;
        op2_ssg[i].ksr = 0;
        op2_ssg[i].mult = 2;
        op2_ssg[i].ar = 15;
        op2_ssg[i].dr = 0;
        op2_ssg[i].sl = 0;
        op2_ssg[i].rr = 0;
        op2_ssg[i].ksl = 0;
        op2_ssg[i].egt = 0;
        op2_ssg[i].tl = 0x3f;
        op2_ssg[i].vib = 0;
        op2_ssg[i].ws = 6;

        set_op(&op1_ssg[i], i + 0x10, 1, SELECT_ALL);
        set_op(&op2_ssg[i], i + 0x13, 1, SELECT_ALL);
    }

    if (argc >= 2) {
        fp = fopen(argv[1], "rb");
    } else {
        printf("%s ", argv[0]);
        printf("\"file name\"\n");
        return 0;
    }

    fread(&header, 1, 256, fp);

    printf("VERSION: %X\n", header.version);
    printf("DATA_OFFSET: %u\n", header.vgm_data_offset);

    fseek(fp, header.vgm_data_offset + 52, SEEK_SET);

    start_timer();
    
    for (i = header.vgm_data_offset + 52; i < header.eof_offset;) {
        fread(&command, 1, 1, fp);
        if (command == 0x56 || command == 0x57) {
            fread(&reg, 1, 1, fp);
            fread(&reg_data, 1, 1, fp);
            if (command & 1) {
                pop1 = pop1_1;
                pop2 = pop2_1;
                pop3 = pop3_1;
                pop4 = pop4_1;
                pfnums = fnums_1;
            } else {
                pop1 = pop1_0;
                pop2 = pop2_0;
                pop3 = pop3_0;
                pop4 = pop4_0;
                pfnums = fnums_0;
            }
            if (reg == 0x07) {
                if ((reg_data >> 2) & 1) {
                    op2_ssg[0].tl = 0x3f;
                    set_op(&op2_ssg[0], 0x13, 1, SELECT_KT);
                }
                if ((reg_data >> 1) & 1) {
                    op2_ssg[1].tl = 0x3f;
                    set_op(&op2_ssg[1], 0x14, 1, SELECT_KT);
                }
                if (reg_data & 1) {
                    op2_ssg[2].tl = 0x3f;
                    set_op(&op2_ssg[2], 0x15, 1, SELECT_KT);
                }
            } else if (reg >= 0x08 && reg <= 0x0a) {
                op2_ssg[reg % 4].tl = ssg_vol_conv[reg_data & 0x0f];
                set_op(&op2_ssg[reg % 4], (reg % 4) + 0x13, 1, SELECT_KT);
            } else if ((reg == 0x00 || reg == 0x01 || reg == 0x02 ||
                reg == 0x03 || reg == 0x04 || reg == 0x05) &&
                command == 0x56
            ) {
                unsigned short tp;
                unsigned long fnum;
                unsigned char block = 0;
                double fnote;

                if (reg & 1) {
                    ssg_ct[reg / 2] = reg_data;
                } else {
                    ssg_ft[reg / 2] = reg_data;
                }
                
                tp = (ssg_ft[reg / 2] | (ssg_ct[reg / 2] << 8));
                fnote = (tp != 0) ? (8000000.0 / (64.0 * tp)) : 0;
                fnum = (unsigned short)((fnote * 524288.0) / (14318180.0 / 288.0));
                        
                for (; (fnum > 1023) && (block <= 7); ++block) {
                    fnum >>= 1;
                }
                play_note(fnum & 0x3ff, block, (reg / 2) + 6, 1, 1, 0);
            } else if (reg >= 0xa0 && reg <= 0xa2) {
                unsigned char block = (pfnums[reg % 4] >> 3) & 0x07;
                unsigned short old_fnum = reg_data | ((pfnums[reg % 4] & 0x07) << 8);

                play_note((unsigned short)((old_fnum * YM2608_CLOCK) / YMF262_CLOCK), block, reg % 4, command & 1, 0, 0);
                if (two_op) {
                    play_note((unsigned short)((old_fnum * YM2608_CLOCK) / YMF262_CLOCK), block, (reg % 4) + 3, command & 1, 0, 0);
                }
                pfnums[reg % 4] = 0;
            } else if (reg >= 0xa4 && reg <= 0xa6) {
                pfnums[reg % 4] = reg_data;
            } else if (reg >= 0x30 && reg <= 0x32) {
                pop1[reg % 4].mult = reg_data & 0x0f;
                set_op(&pop1[reg % 4], reg % 4, command & 1, SELECT_AVEKM);
            } else if (reg >= 0x34 && reg <= 0x36) {
                pop2[reg % 4].mult = reg_data & 0x0f;
                set_op(&pop2[reg % 4], (reg % 4) + 3, command & 1, SELECT_AVEKM);
            } else if (reg >= 0x38 && reg <= 0x3a) {
                pop3[reg % 4].mult = reg_data & 0x0f;
                set_op(&pop3[reg % 4], (reg % 4) + 8, command & 1, SELECT_AVEKM);
            } else if (reg >= 0x3c && reg <= 0x3e) {
                pop4[reg % 4].mult = reg_data & 0x0f;
                set_op(&pop4[reg % 4], (reg % 4) + 11, command & 1, SELECT_AVEKM);
            } else if (reg >= 0x80 && reg <= 0x82) {
                pop1[reg % 4].rr = reg_data & 0x0f;
                pop1[reg % 4].sl = (reg_data >> 4) & 0x0f;
                set_op(&pop1[reg % 4], reg % 4, command & 1, SELECT_SR);
            } else if (reg >= 0x84 && reg <= 0x86) {
                pop2[reg % 4].rr = reg_data & 0x0f;
                pop2[reg % 4].sl = (reg_data >> 4) & 0x0f;
                set_op(&pop2[reg % 4], (reg % 4) + 3, command & 1, SELECT_SR);
            } else if (reg >= 0x88 && reg <= 0x8a) {
                pop3[reg % 4].rr = reg_data & 0x0f;
                pop3[reg % 4].sl = (reg_data >> 4) & 0x0f;
                set_op(&pop3[reg % 4], (reg % 4) + 8, command & 1, SELECT_SR);
            } else if (reg >= 0x8c && reg <= 0x8e) {
                pop4[reg % 4].rr = reg_data & 0x0f;
                pop4[reg % 4].sl = (reg_data >> 4) & 0x0f;
                set_op(&pop4[reg % 4], (reg % 4) + 11, command & 1, SELECT_SR);
            } else if (reg >= 0x60 && reg <= 0x62) {
                pop1[reg % 4].dr = ((reg_data & 0x1f) * 15) / 31;
                pop1[reg % 4].am = (reg_data >> 7) & 1;
                set_op(&pop1[reg % 4], reg % 4, command & 1, SELECT_AVEKM | SELECT_AD);
            } else if (reg >= 0x64 && reg <= 0x66) {
                pop2[reg % 4].dr = ((reg_data & 0x1f) * 15) / 31;
                pop2[reg % 4].am = (reg_data >> 7) & 1;
                set_op(&pop2[reg % 4], (reg % 4) + 3, command & 1, SELECT_AVEKM | SELECT_AD);
            } else if (reg >= 0x68 && reg <= 0x6a) {
                pop3[reg % 4].dr = ((reg_data & 0x1f) * 15) / 31;
                pop3[reg % 4].am = (reg_data >> 7) & 1;
                set_op(&pop3[reg % 4], (reg % 4) + 8, command & 1, SELECT_AVEKM | SELECT_AD);
            } else if (reg >= 0x6c && reg <= 0x6e) {
                pop4[reg % 4].dr = ((reg_data & 0x1f) * 15) / 31;
                pop4[reg % 4].am = (reg_data >> 7) & 1;
                set_op(&pop4[reg % 4], (reg % 4) + 11, command & 1, SELECT_AVEKM | SELECT_AD);
            } else if (reg >= 0x50 && reg <= 0x52) {
                pop1[reg % 4].ar = ((reg_data & 0x1f) * 15) / 31;
                pop1[reg % 4].ksl = (reg_data >> 6) & 3;
                set_op(&pop1[reg % 4], reg % 4, command & 1, SELECT_KT | SELECT_AD);
            } else if (reg >= 0x54 && reg <= 0x56) {
                pop2[reg % 4].ar = ((reg_data & 0x1f) * 15) / 31;
                pop2[reg % 4].ksl = (reg_data >> 6) & 3;
                set_op(&pop2[reg % 4], (reg % 4) + 3, command & 1, SELECT_KT | SELECT_AD);
            } else if (reg >= 0x58 && reg <= 0x5a) {
                pop3[reg % 4].ar = ((reg_data & 0x1f) * 15) / 31;
                pop3[reg % 4].ksl = (reg_data >> 6) & 3;
                set_op(&pop3[reg % 4], (reg % 4) + 8, command & 1, SELECT_KT | SELECT_AD);
            } else if (reg >= 0x5c && reg <= 0x5e) {
                pop4[reg % 4].ar = ((reg_data & 0x1f) * 15) / 31;
                pop4[reg % 4].ksl = (reg_data >> 6) & 3;
                set_op(&pop4[reg % 4], (reg % 4) + 11, command & 1, SELECT_KT | SELECT_AD);
            } else if (reg >= 0x40 && reg <= 0x42) {
                pop1[reg % 4].tl = ((reg_data & 0x7f) * 63) / 127;
                set_op(&pop1[reg % 4], reg % 4, command & 1, SELECT_KT);
            } else if (reg >= 0x44 && reg <= 0x46) {
                pop2[reg % 4].tl = ((reg_data & 0x7f) * 63) / 127;
                set_op(&pop2[reg % 4], (reg % 4) + 3, command & 1, SELECT_KT);
            } else if (reg >= 0x48 && reg <= 0x4a) {
                pop3[reg % 4].tl = ((reg_data & 0x7f) * 63) / 127;
                set_op(&pop3[reg % 4], (reg % 4) + 8, command & 1, SELECT_KT);
            } else if (reg >= 0x4c && reg <= 0x4e) {
                pop4[reg % 4].tl = ((reg_data & 0x7f) * 63) / 127;
                set_op(&pop4[reg % 4], (reg % 4) + 11, command & 1, SELECT_KT);
            } else if (reg >= 0xb0 && reg <= 0xb2) {
                unsigned char fb = (reg_data << 3) & 0x7;
                unsigned char conn = reg_data & 0x7;
                unsigned char cnt_0 = 0;
                unsigned char cnt_1 = 0;

                if (conn == 0) {
                    cnt_0 = 0;
                    cnt_1 = 0;
                } else if (conn == 4) {
                    cnt_0 = 0;
                    cnt_1 = 1;
                } else {
                    cnt_0 = 1;
                    cnt_1 = 1;
                }
                write_reg(0x220, 0xc0 + (reg % 4), 0xf0 | (fb << 1) | cnt_0, command & 1);
                write_reg(0x220, 0xc3 + (reg % 4), 0xf0 | (fb << 1) | cnt_1, command & 1);
            } 
        } else if (command == 0x61) {
            fread(&samples, 1, 2, fp);

            sleep_ms((unsigned long)(((double)samples / 44100.0) * 1000));
        } else if (command == 0x62) {
            sleep_ms((unsigned long)(((double)735 / 44100.0) * 1000));
        } else if (command == 0x63) {
            sleep_ms((unsigned long)(((double)882 / 44100.0) * 1000));
        } else if (command == 0x66) {
            printf("END OF SOUND DATA\n");
            if (header.loop_offset == 0 || do_loop >= 1) {
                break;
            }
            stop_all_channels();
            i = header.loop_offset + 0x1c;
            fseek(fp, header.loop_offset + 0x1c, SEEK_SET);
            do_loop = 1;
        } else if (command >= 0x70 && command <= 0x7f) {
            sleep_ms((unsigned long)((((double)(command & 0x0f)) / 44100.0) * 1000));
        } else if (command == 0x67) {
            unsigned char comp;
            unsigned char data_type;
            unsigned long data_size = 0;
            fread(&comp, 1, 1, fp);
            fread(&data_type, 1, 1, fp);
            fread(&data_size, 1, 4, fp);
            fseek(fp, ftell(fp) + data_size, SEEK_SET);
        } else {
            printf("UNKNOWN COMMAND %x\n", command);
            stop_all_channels();
            return 0;
        }
    }

    fclose(fp);
    stop_all_channels();

    return 0;
}