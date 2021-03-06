/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <debug.h>
#elif (defined BUILD_UBOOT)
#include <asm/arch/mt6577_gpio.h>
#else
#include <mach/mt_gpio.h>
#include <linux/xlog.h>
#endif


#if defined(BUILD_LK)
	#define LCM_DEBUG  printf
	#define LCM_FUNC_TRACE() printf("huyl [uboot] %s\n",__func__)
#else
	#define LCM_DEBUG  printk
	#define LCM_FUNC_TRACE() printk("huyl [kernel] %s\n",__func__)
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (1080)
#define FRAME_HEIGHT (1920)

#define LCM_ID (0x32)

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0x100   // END OF REGISTERS MARKER

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE							0

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[12];
};



static struct LCM_setting_table lcm_initialization_setting[] = {

//////////////////Initial  CODE///////////////////////
 
 {0xFF, 1, {0x00}},
 
 {0xBA, 1, {0x03}},
 
 {0x36, 1, {0x00}},
 
 {0xB0, 1, {0x00}},
 
 {0xD3, 1, {0x08}},
  
 {0xD4, 1, {0x0E}},
 
 {0xD5, 1, {0x0F}},
 
 {0xD6, 1, {0x48}},
  
 {0xD7, 1, {0x00}},
  
 {0xD9, 1, {0x00}},
  
 {0xFB, 1, {0x01}},
 
 {0xFF, 1, {0xEE}},
  
 {0x40, 1, {0x00}},
  
 {0x41, 1, {0x00}},
  
 {0x42, 1, {0x00}},
///////////////////////
	  
	 {0xFB, 1, {0x01}},
  
	 {0xFF, 1, {0x01}},
	  
	 {0xFB, 1, {0x01}},
	  
	 {0x01, 1, {0x55}},
	  
	 {0x04, 1, {0x0C}},
	  
	 {0x05, 1, {0x3A}},
	  
	 {0x06, 1, {0x50}},
	  
	 {0x07, 1, {0xD0}},
	  
	 {0x0A, 1, {0x0F}},
	  
	 {0x0D, 1, {0x7F}},
	  
	 {0x0E, 1, {0x7F}},
	   
	 {0x0F, 1, {0x70}},
	  
	 {0x10, 1, {0x63}},
	   
	 {0x11, 1, {0x3C}},
	  
	 {0x12, 1, {0x5C}},
	  
	 {0x15, 1, {0x60}},
	  
	 {0x16, 1, {0x11}},
	  
	 {0x17, 1, {0x11}},
	  
	 {0x5B, 1, {0xCA}},
	   
	 {0x5C, 1, {0x00}},
	  
	 {0x5D, 1, {0x00}},
	  
	 {0x5F, 1, {0x1B}},
	  
	 {0x60, 1, {0xD5}},
	  
	 {0x61, 1, {0xF7}},
	  
	 {0x6C, 1, {0xAB}},
	  
	 {0x6D, 1, {0x44}},
  
	 {0xFF, 1, {0x05}},	                                                
	  
	 {0xFB, 1, {0x01}},
	  
	 {0x00, 1, {0x3F}},
	  
	 {0x01, 1, {0x3F}},
	  
	 {0x02, 1, {0x3F}},
	  
	 {0x03, 1, {0x3F}},
	  
	 {0x04, 1, {0x38}},
	  
	 {0x05, 1, {0x3F}},
	  
	 {0x06, 1, {0x3F}},
	  
	 {0x07, 1, {0x19}},
	  
	 {0x08, 1, {0x1D}},
	  
	 {0x09, 1, {0x3F}},
	  
	 {0x0A, 1, {0x3F}},
	  
	 {0x0B, 1, {0x1B}},
	  
	 {0x0C, 1, {0x17}},
	  
	 {0x0D, 1, {0x3F}},
	 
	 {0x0E, 1, {0x3F}},
	  
	 {0x0F, 1, {0x08}},
	   
	 {0x10, 1, {0x3F}},
	   
	 {0x11, 1, {0x10}},
	  
	 {0x12, 1, {0x3F}},
	   
	 {0x13, 1, {0x3F}},
	  
	 {0x14, 1, {0x3F}},
	  
	 {0x15, 1, {0x3F}},
	  
	 {0x16, 1, {0x3F}},
	  
	 {0x17, 1, {0x3F}},
	  
	 {0x18, 1, {0x38}},
  
	 {0x19, 1, {0x18}},
	  
	 {0x1A, 1, {0x1C}},
	  
	 {0x1B, 1, {0x3F}},
	  
	 {0x1C, 1, {0x3F}},
	  
	 {0x1D, 1, {0x1A}},
	  
	 {0x1E, 1, {0x16}},
	  
	 {0x1F, 1, {0x3F}},
	  
	 {0x20, 1, {0x3F}},
	  
	 {0x21, 1, {0x3F}},
	   
	 {0x22, 1, {0x3F}},
	  
	 {0x23, 1, {0x06}},
	   
	 {0x24, 1, {0x3F}},
	  
	 {0x25, 1, {0x0E}},
	  
	 {0x26, 1, {0x3F}},
	  
	 {0x27, 1, {0x3F}},
	  
	 {0x54, 1, {0x06}},
	  
	 {0x55, 1, {0x05}},
	  
	 {0x56, 1, {0x04}},
	  
	 {0x58, 1, {0x03}},
	  
	 {0x59, 1, {0x1B}},
	  
	 {0x5A, 1, {0x1B}},
	  
	 {0x5B, 1, {0x01}},
	  
	 {0x5C, 1, {0x32}},
	  
	 {0x5E, 1, {0x18}},
	  
	 {0x5F, 1, {0x20}},
	  
	 {0x60, 1, {0x2B}},
	  
	 {0x61, 1, {0x2C}},
	  
	 {0x62, 1, {0x18}},
	  
	 {0x63, 1, {0x01}},
	  
	 {0x64, 1, {0x32}},
	  
	 {0x65, 1, {0x00}},
	   
	 {0x66, 1, {0x44}},
	  
	 {0x67, 1, {0x11}},
	  
	 {0x68, 1, {0x01}},
	  
	 {0x69, 1, {0x01}},
	  
	 {0x6A, 1, {0x04}},
	  
	 {0x6B, 1, {0x2C}},
	  
	 {0x6C, 1, {0x08}},
	  
	 {0x6D, 1, {0x08}},
	   
	 {0x78, 1, {0x00}},
	   
	 {0x79, 1, {0x00}},
	  
	 {0x7E, 1, {0x00}},
	  
	 {0x7F, 1, {0x00}},
	  
	 {0x80, 1, {0x00}},
	  
	 {0x81, 1, {0x00}},
	  
	 {0x8D, 1, {0x00}},
	  
	 {0x8E, 1, {0x00}},
	  
	 {0x8F, 1, {0xC0}},
	   
	 {0x90, 1, {0x73}},
	   
	 {0x91, 1, {0x10}},
	  
	 {0x92, 1, {0x07}},
  
	 {0x96, 1, {0x11}},
	  
	 {0x97, 1, {0x14}},
	  
	 {0x98, 1, {0x00}},
	  
	 {0x99, 1, {0x00}},
	  
	 {0x9A, 1, {0x00}},
	   
	 {0x9B, 1, {0x61}},
	  
	 {0x9C, 1, {0x15}},
	  
	 {0x9D, 1, {0x30}},
	  
	 {0x9F, 1, {0x0F}},
	  
	 {0xA2, 1, {0xB0}},
	  
	 {0xA7, 1, {0x0A}},
	   
	 {0xA9, 1, {0x00}},
	   
	 {0xAA, 1, {0x70}},
	   
	 {0xAB, 1, {0xDA}},
	  
	 {0xAC, 1, {0xFF}},
	  
	 {0xAE, 1, {0xF4}},
	  
	 {0xAF, 1, {0x40}},
	  
	 {0xB0, 1, {0x7F}},
	  
	 {0xB1, 1, {0x16}},
	  
	 {0xB2, 1, {0x53}},
	  
	 {0xB3, 1, {0x00}},
	  
	 {0xB4, 1, {0x2A}},
	   
	 {0xB5, 1, {0x3A}},
	  
	 {0xB6, 1, {0xF0}},
	   
	 {0xBC, 1, {0x85}},
	  
	 {0xBD, 1, {0xF4}},
	  
	 {0xBE, 1, {0x33}},
	  
	 {0xBF, 1, {0x13}},
	  
	 {0xC0, 1, {0x77}},
	  
	 {0xC1, 1, {0x77}},
	  
	 {0xC2, 1, {0x77}},
	  
	 {0xC3, 1, {0x77}},
	  
	 {0xC4, 1, {0x77}},
	  
	 {0xC5, 1, {0x77}},
	  
	 {0xC6, 1, {0x77}},
	  
	 {0xC7, 1, {0x77}},
	  
	 {0xC8, 1, {0xAA}},
	  
	 {0xC9, 1, {0x2A}},
	  
	 {0xCA, 1, {0x00}},
	  
	 {0xCB, 1, {0xAA}},
	  
	 {0xCC, 1, {0x92}},
	  
	 {0xCD, 1, {0x00}},
	  
	 {0xCE, 1, {0x18}},
	  
	 {0xCF, 1, {0x88}},
	  
	 {0xD0, 1, {0xAA}},
	  
	 {0xD1, 1, {0x00}},
	  
	 {0xD2, 1, {0x00}},
	  
	 {0xD3, 1, {0x00}},
	   
	 {0xD6, 1, {0x02}},
	  
	 {0xD7, 1, {0x31}},
	  
	 {0xD8, 1, {0x7E}},
	  
	 {0xED, 1, {0x00}},
	   
	 {0xEE, 1, {0x00}},
	  
	 {0xEF, 1, {0x70}},
	  
	 {0xFA, 1, {0x03}},
	 
	 {0xFF, 1, {0x00}},
/////////////////

	  {0x10, 1, {0x02}},//10=TX_CLK 30=PCLK
	  {0x00, 1, {0x00}},
	  {0x11,  0  ,{0x00}},//Sleep out
{REGFLAG_DELAY, 300, {}},
	
	  {0x29,  0  ,{0x00}},//Sleep out
{REGFLAG_DELAY, 300, {}},

{REGFLAG_END_OF_TABLE, 0x00, {}}

};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {0x00}},
    {REGFLAG_DELAY, 200, {}},

    // Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 100, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {

	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 60, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}

};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
				//MDELAY(2);
       	}
    }

}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{

		memset(params, 0, sizeof(LCM_PARAMS));

		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

    		params->dsi.mode   =  SYNC_PULSE_VDO_MODE; //SYNC_PULSE_VDO_MODE; //SYNC_PULSE_VDO_MODE;  //SYNC_EVENT_VDO_MODE; 


		// DSI
		/* Command mode setting */
		//1 Three lane or Four lane
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 0x2; //
		params->dsi.vertical_backporch					= 0x3; //=>{0XD3,1,{0X05}} VBP(lcm) = vertical_sync_active + vertical_backporch
		params->dsi.vertical_frontporch					= 0x4; //=>{0XD4,1,{0X04}} VFP
		params->dsi.vertical_active_line				= FRAME_HEIGHT;


        // Val get from MTK, #5294!!!
		params->dsi.horizontal_sync_active				= 0x05; //0x05;// 50  2
		params->dsi.horizontal_backporch				= 0x3c; //16; //0x3C;
		params->dsi.horizontal_frontporch				= 0x5e; //16; //0x5E;
    //params->dsi.horizontal_blanking_pixel              = 60;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	    //params->dsi.LPX=8;

		// Bit rate calculation
		//1 Every lane speed 410 380 
    params->dsi.PLL_CLOCK=450;  //450; //480;//227;//254;//254//247
    
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
    params->dsi.lcm_esd_check_table[0].count        = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;

}

static void en_reset(unsigned int x)
{
	mt_set_gpio_mode(GPIO_LCM_RST, GPIO_LCM_RST_M_GPIO);
	mt_set_gpio_dir(GPIO_LCM_RST, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_RST, x);
}

static void lcm_init(void)
{

	MDELAY(10);
	en_reset(1);
	MDELAY(5); 
	en_reset(0);
	MDELAY(5); 	// page 142
	en_reset(1);
	MDELAY(20);  

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	//LCM_DEBUG("[Hxw][func:%s][line:%d]\n", __func__, __LINE__);

	push_table(lcm_sleep_mode_in_setting, sizeof(lcm_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	//lcm_suspend_shut_down();
	unsigned int data_array[16];

	en_reset(0);
	MDELAY(120);  

}

static void lcm_resume(void)
{
	lcm_init();
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00290508; //HW bug, so need send one HS packet
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif

static unsigned int lcm_compare_id(void)
{
	unsigned int id=0;
	unsigned char buffer[2];
	unsigned int array[4];
	
	en_reset(1);
	en_reset(0);
	MDELAY(1);
	en_reset(1);
	MDELAY(20);

	array[0] = 0x00023700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
        read_reg_v2(0xF4, buffer, 2);
	id = buffer[0]; //we only need ID

        #ifdef BUILD_LK
		printf("%s, LK nt35592 debug: nt35592 id = 0x%08x\n", __func__, id);
        #else
		printk("%s, kernel nt35592 debug: nt35592 id = 0x%08x\n", __func__, id);
        #endif

        if(id == LCM_ID)
    	    return 1;
        else
            return 0;
}


static unsigned int lcm_esd_check(void)
{

    static int err_count = 0;
    unsigned char buffer_1[8];
    unsigned int array[4];
    int i;
    unsigned char fResult;

    LCM_DEBUG("lcm_esd_check<<<\n");
    for(i = 0; i < 11; i++)
      buffer_1[i] = 0x00;

    //---------------------------------
    // Set Maximum Return Size
    //---------------------------------    
    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    //---------------------------------
    // Read [9Ch, 00h, ECC] + Error Report(4 Bytes)
    //---------------------------------
    read_reg_v2(0x0A, buffer_1, 7);

    //LCM_DEBUG("lcm_esd_check: read(0x0A)\n");
    //for(i = 0; i < 7; i++)                
    //  LCM_DEBUG("buffer_1[%d]:0x%x \n",i,buffer_1[i]);

    //---------------------------------
    return TRUE;

}

static unsigned int lcm_esd_recover(void)
{
	lcm_init();
	lcm_resume();

	return TRUE;
}



LCM_DRIVER nt35592_fhd_dsi_vdo_lcm_drv = 
{
    .name			= "nt35592_fhd_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	.esd_check = lcm_esd_check,
	.esd_recover = lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
