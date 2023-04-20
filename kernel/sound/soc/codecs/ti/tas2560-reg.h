#ifndef _TAS2560_REG_H_
#define _TAS2560_REG_H_

typedef unsigned char cfg_u8;
    

typedef union {
    struct {
        cfg_u8 offset;
        cfg_u8 value;
    };
    struct {
        cfg_u8 command;
        cfg_u8 param;
    };
} cfg_reg;

#define CFG_META_SWITCH (255)
#define CFG_META_DELAY  (254)  
#define CFG_META_BURST  (253)

#define BUF_MAX 300

cfg_reg registers_tas2560_init_table[] =  {
	{ .command=0x00,.param=0x00 },		// Page-0
	{ .command=0x7f,.param=0x00 },		// Book-0
	{ .command=0x01,.param=0x01 },		// Software reset (PAGE0_REG1)
	
	{ .command=0x04,.param=0x4f },		// 4-ohm load setting
	
	// ##### INIT SECTION START	
	{ .command=0x49,.param=0x0c },
	{ .command=0x3c,.param=0x33 },	
	{ .command=0x09,.param=0x93 },		// 4-ohm load setting
	{ .command=0x14,.param=0x00 },		// I2S, 16bits
	{ .command=0x15,.param=0x03 },		// monoPCM
	
	// ##### INIT SECTION END
	
	// ##### DSP PROG SETTING START
	{ .command=0x02,.param=0x02 },		// operate device in dev mode 2
	{ .command=0x21,.param=0x00 },		// disable clock error detection
	{ .command=0x08,.param=0x81 },		// SSM enabled
	// ##### DSP PROG SETTING END
	
#if 1
	{ .command=CFG_META_DELAY, .param=10},	// 10msec delay
	{ .command=0x07,.param=0x41 }, 
	{ .command=0x00,.param=0x32 },
	{ .command=0x28,.param=0x7F },
	{ .command=0x29,.param=0xFB },
	{ .command=0x2a,.param=0xB5 },
	{ .command=0x2b,.param=0x00 },
	
	{ .command=0x2c,.param=0x80 },
	{ .command=0x2d,.param=0x04 },
	{ .command=0x2e,.param=0x4c },
	{ .command=0x2f,.param=0x00 },
	
	{ .command=0x30,.param=0x7F },
	{ .command=0x31,.param=0xF7 },
	{ .command=0x32,.param=0x6A },
	{ .command=0x33,.param=0x00 },
	
	
	{ .command=0x1C,.param=0x7f }, // 1c 7f ff ff ff
	{ .command=0x1d,.param=0xFF }, 
	{ .command=0x1e,.param=0xFF }, 
	{ .command=0x1f,.param=0xFF }, 
	
	
	{ .command=0x20,.param=0x00 }, // 20 00 00 00 00
	{ .command=0x21,.param=0x00 }, 
	{ .command=0x22,.param=0x00 }, 
	{ .command=0x23,.param=0x00 }, 
	
	{ .command=0x24,.param=0x00 }, // 24 00 00 00 00
	{ .command=0x25,.param=0x00 }, 
	{ .command=0x26,.param=0x00 }, 
	{ .command=0x27,.param=0x00 }, 
	
	{ .command=0x00,.param=0x33 }, 
		
	{ .command=0x10,.param=0x6f }, // 10 6f 5c 28 f5
	{ .command=0x11,.param=0x5c }, 
	{ .command=0x12,.param=0x28 }, 
	{ .command=0x13,.param=0xf5 }, 	
	{ .command=0x14,.param=0x67 }, // 14 67 ae 14 7a
	{ .command=0x15,.param=0xae }, 
	{ .command=0x16,.param=0x14 }, 
	{ .command=0x17,.param=0x7a }, 	
	{ .command=0x20,.param=0x1c }, // 20 1c 00 00 00
	{ .command=0x21,.param=0x00 }, 
	{ .command=0x22,.param=0x00 }, 
	{ .command=0x23,.param=0x00 }, 	
	{ .command=0x24,.param=0x1f }, // 24 1f 0a 3d 70
	{ .command=0x25,.param=0x0a }, 
	{ .command=0x26,.param=0x3d }, 
	{ .command=0x27,.param=0x70 }, 
	{ .command=0x28,.param=0x22 }, // 28 22 14 7a e1
	{ .command=0x29,.param=0x14 }, 
	{ .command=0x2a,.param=0x7a }, 
	{ .command=0x2b,.param=0xe1 }, 	
	{ .command=0x2c,.param=0x25 }, // 2c 25 1e b8 51
	{ .command=0x2d,.param=0x1e }, 
	{ .command=0x2e,.param=0xb8 }, 
	{ .command=0x2f,.param=0x51 }, 	
	{ .command=0x30,.param=0x28 }, // 30 28 28 f5 c2
	{ .command=0x31,.param=0x28 }, 
	{ .command=0x32,.param=0xf5 }, 
	{ .command=0x33,.param=0xc2 }, 
	{ .command=0x34,.param=0x2b }, // 34 2b 33 33 33
	{ .command=0x35,.param=0x33 }, 
	{ .command=0x36,.param=0x33 }, 
	{ .command=0x37,.param=0x33 }, 
	{ .command=0x38,.param=0x2e }, // 38 2e 3d 70 a3
	{ .command=0x39,.param=0x3d }, 
	{ .command=0x3a,.param=0x70 }, 
	{ .command=0x3b,.param=0xa3 }, 
	{ .command=0x3c,.param=0x31 }, // 3c 31 47 ae 14
	{ .command=0x3d,.param=0x47 }, 
	{ .command=0x3e,.param=0xae }, 
	{ .command=0x3f,.param=0x14 }, 
		
	{ .command=0x00,.param=0x33 },
	
	{ .command=0x18,.param=0x06 }, // 18 06 66 66 66
	{ .command=0x19,.param=0x66 }, 
	{ .command=0x1a,.param=0x66 }, 
	{ .command=0x1b,.param=0x66 }, 
	
	{ .command=0x00,.param=0x34 },
	
	{ .command=0x34,.param=0x3a }, // 34 3a 46 74 00
	{ .command=0x35,.param=0x46 }, 
	{ .command=0x36,.param=0x74 }, 
	{ .command=0x37,.param=0x00 }, 	
	{ .command=0x38,.param=0x22 }, // 38 22 f3 07 00
	{ .command=0x39,.param=0xf3 }, 
	{ .command=0x3a,.param=0x07 }, 
	{ .command=0x3b,.param=0x00 }, 	
	{ .command=0x3c,.param=0x80 }, // 3c 80 77 61 00
	{ .command=0x3d,.param=0x77 }, 
	{ .command=0x3e,.param=0x61 }, 
	{ .command=0x3f,.param=0x00 }, 	
	{ .command=0x40,.param=0x22 }, // 40 22 a7 cc 00
	{ .command=0x41,.param=0xa7 }, 
	{ .command=0x42,.param=0xcc }, 
	{ .command=0x43,.param=0x00 }, 	
	{ .command=0x44,.param=0x3a }, // 44 3a 0c 93 00
	{ .command=0x45,.param=0x0c }, 
	{ .command=0x46,.param=0x93 }, 
	{ .command=0x47,.param=0x00 }, 	
	{ .command=0x00,.param=0x00 },	
#endif
	{ .command=0x07,.param=0x40 },
	
	{ .command=0x00,.param=0x00 },		// Page-0
	{ .command=0x7f,.param=0x00 },		// Book-0
};

#endif 
