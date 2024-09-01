#ifndef VGM_H
#define VGM_H

#define U8 unsigned char
#define U16 unsigned short
#define U32 unsigned long

typedef struct _vgm_header {
    U8 vgm[4];
    U32 eof_offset;
    U32 version;
    U32 sn76489_clock;
    U32 ym2413_clock;
    U32 gd3_offset;
    U32 total_sample_count;
    U32 loop_offset;
    U32 loop_samples_count;
    U32 rate;
    U16 sn_fb;
    U8 snw;
    U8 sf;
    U32 ym2612_clock;
    U32 ym2151_clock;
    U32 vgm_data_offset;
    U32 sega_pcm_clock;
    U32 spcm_longerface;
    U32 rf5c68_clock;
    U32 ym2203_clock;
    U32 ym2608_clock;
    U32 ym2610_clock;
    U32 ym3812_clock;
    U32 ym3526_clock;
    U32 y8950_clock;
    U32 ymf262_clock;
    U32 ymf278b_clock;
    U32 ymf271_clock;
    U32 ymz280b_clock;
    U32 rf5c164_clock;
    U32 pwm_clock;
    U32 ay8910_clock;
    U8 ayt;
    U8 ay_flags[3];
    U8 vm;
    U8 padding0;
    U8 lb;
    U8 lm;
    U32 gb_dmg_clock;
    U32 nes_apu_clock;
    U32 multipcm_clock;
    U32 upd7759_clock;
    U32 okim6258_clock;
    U8 of;
    U8 kf;
    U8 cf;
    U8 padding1;
    U32 okim6395_clock;
    U32 k051649_clock;
    U32 k054539_clock;
    U32 huc6280_clock;
    U32 c140_clock;
    U32 k053260_clock;
    U32 pokey_clock;
    U32 qsound_clock;
    U32 scsp_clock;
    U32 extra_hdr_ofs;
    U32 wonderswan_clock;
    U32 vsu_clock;
    U32 saa1099_clock;
    U32 es5503_clock;
    U32 es5506_clock;
    U16 es_chns;
    U8 cd;
    U8 padding2;
    U32 x1010_clock;
    U32 c352_clock;
    U32 ga20_clock;
    U32 mikey_clock;
    U32 padding3[6];
} vgm_header_t;

#endif