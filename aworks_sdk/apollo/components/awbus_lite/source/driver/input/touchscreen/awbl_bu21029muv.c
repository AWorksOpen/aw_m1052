/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief touch screen controller
 *
 *  The BU21029MUV controller operates as an I2C slave device. At the start of 
 *  the communication, it receives an address byte transmitted by the host 
 *  processor and then then sends back an acknowledgement byte. The host processor 
 *  can transmit a command to execute conversion or to access registers only 
 *  after receiving the acknowledgement for the address byte. Communication ends 
 *  when BU21029MUV receives a stop command.
 *
 *  Address byte
 *      On the address byte, the slave address for 2-wire interface is written 
 *      on the upper 7 bits and the READ/WRITE bit is written on the last bit. 
 *      The upper six bits of the slave address are fixed to “100000” while the 
 *      last bit is determined by the “AD0” input.
 *
 *  Command byte
 *      The operation of BU21029MUV is dictated by the command byte. The host 
 *      processor sets CID (D7) to 1 for conversion
 *      function or to 0 for register access.
 *      bit7:           CID=1:Command Byte.     CID=0:R/W registers.
 *      bit6~bit3:      CF
 *      bit2:           CMSK
 *      bit1:           PDM
 *      bit0:           STP
 *
 *          CF  Description
 *          0x0 Touch screen scan function: X, Y, Z1 and Z2 coordinates are 
 *          converted.This makes BU21029MUV periodically and automatically 
 *          scan the screen and convert data upon detecting touch. When the 
 *          device cannot detect touch, it stops and stays in power down state 
 *          until it detects the next pen-down. The order of scan
 *              process is Z1, Z2, X and Y.
 *          0x1 NOP
 *          0x2 Auxiliary input is converted.
 *          0x3 Clipping value is changed.
 *          0x4 Free scan function: Status of Driver and input of A/D is 
 *              assigned by the Host.
 *          0x5 Calibration: Parameters for dual touch detection are calibrated.
 *          0x6 NOP
 *          0x7 NOP
 *          0x8 X+, X- driver status
 *          0x9 Y+, Y- driver status
 *          0xA Y+, X- driver status
 *          0xB NOP
 *          0xC Touch screen scan function: X coordinate is converted.
 *          0xD Touch screen scan function: Y coordinate isconverted.
 *          0xE Touch screen scan function: Z1 and Z2 coordinates are converted.
 *          0xF Reserved
 *
 *      CF=0101: Calibration
 *              This calibrates the parameters for dual touch detection. To 
 *              activate the dual touch function, setting of CF to 0101 and
 *              execution of the calibration command should be done after power-on.
 *
 * \internal
 * \par modification history:
 * - 1.01 17-02-08  anu, modify.
 * - 1.01 16-12-13  ucc, first implementation.
 * \endinternal
 */
#include "driver/input/touchscreen/awbl_bu21029muv.h"
#include "aw_gpio.h"
#include <string.h>
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_mem.h"

static aw_const size_t g_bu21029_conver_tab[1024] = {
 46341,  63304,  75975,  86475,  95610, 103785, 111240, 118129, 124559, 130607, 136330, 141774, 146974, 151958, 156749, 161369,
 165831, 170152, 174343, 178414, 182374, 186232, 189995, 193669, 197260, 200772, 204211, 207580, 210884, 214126, 217309, 220436,
 223510, 226532, 229507, 232435, 235318, 238159, 240959, 243720, 246444, 249131, 251783, 254401, 256987, 259541, 262065, 264560,
 267026, 269464, 271876, 274263, 276624, 278960, 281273, 283563, 285831, 288076, 290301, 292505, 294689, 296853, 298998, 301124,
 303233, 305323, 307396, 309452, 311492, 313516, 315523, 317515, 319492, 321454, 323402, 325335, 327255, 329160, 331053, 332932,
 334798, 336652, 338493, 340322, 342140, 343945, 345739, 347522, 349293, 351054, 352804, 354543, 356272, 357991, 359700, 361399,
 363088, 364768, 366438, 368099, 369751, 371394, 373028, 374654, 376270, 377879, 379479, 381071, 382655, 384230, 385798, 387358, /* 7 */
 388911, 390456, 391994, 393524, 395047, 396563, 398071, 399573, 401068, 402557, 404038, 405513, 406981, 408443, 409899, 411348,
 412791, 414228, 415659, 417084, 418503, 419916, 421323, 422725, 424120, 425511, 426896, 428275, 429649, 431017, 432381, 433739, /* 9 */
 435091, 436439, 437782, 439119, 440452, 441780, 443103, 444421, 445734, 447043, 448347, 449646, 450941, 452231, 453517, 454798,
 456075, 457348, 458616, 459880, 461140, 462395, 463647, 464894, 466137, 467377, 468612, 469843, 471070, 472294, 473513, 474729,
 475941, 477149, 478353, 479554, 480751, 481945, 483134, 484321, 485503, 486683, 487858, 489031, 490199, 491365, 492527, 493686,
 494841, 495993, 497142, 498288, 499430, 500569, 501705, 502838, 503968, 505095, 506219, 507339, 508457, 509572, 510683, 511792,
 512898, 514001, 515101, 516198, 517292, 518383, 519472, 520558, 521641, 522721, 523799, 524874, 525946, 527016, 528083, 529147,
 530209, 531268, 532325, 533379, 534430, 535479, 536525, 537569, 538611, 539650, 540686, 541721, 542752, 543782, 544809, 545833,
 546856, 547875, 548893, 549908, 550921, 551932, 552941, 553947, 554951, 555953, 556952, 557950, 558945, 559938, 560929, 561918,
 562905, 563889, 564872, 565852, 566830, 567807, 568781, 569753, 570723, 571691, 572658, 573622, 574584, 575544, 576502, 577459,
 578413, 579366, 580316, 581265, 582212, 583156, 584099, 585041, 585980, 586917, 587853, 588787, 589719, 590649, 591578, 592504,
 593429, 594352, 595274, 596193, 597111, 598028, 598942, 599855, 600766, 601675, 602583, 603489, 604394, 605296, 606197, 607097,
 607995, 608891, 609786, 610679, 611570, 612460, 613348, 614235, 615120, 616004, 616886, 617766, 618645, 619523, 620399, 621273,
 622146, 623017, 623887, 624756, 625623, 626488, 627352, 628215, 629076, 629936, 630794, 631651, 632506, 633360, 634213, 635064,
 635914, 636762, 637610, 638455, 639300, 640143, 640984, 641824, 642663, 643501, 644337, 645172, 646006, 646838, 647669, 648499,
 649327, 650154, 650980, 651805, 652628, 653450, 654271, 655090, 655908, 656725, 657541, 658356, 659169, 659981, 660792, 661601,
 662410, 663217, 664023, 664828, 665632, 666434, 667235, 668035, 668834, 669632, 670429, 671224, 672018, 672812, 673604, 674394,
 675184, 675973, 676760, 677547, 678332, 679116, 679899, 680681, 681462, 682242, 683020, 683798, 684574, 685350, 686124, 686897,
 687670, 688441, 689211, 689980, 690748, 691515, 692281, 693046, 693810, 694573, 695334, 696095, 696855, 697614, 698372, 699128,
 699884, 700639, 701393, 702145, 702897, 703648, 704398, 705147, 705895, 706642, 707388, 708133, 708877, 709620, 710362, 711103,
 711843, 712583, 713321, 714059, 714795, 715531, 716265, 716999, 717732, 718464, 719195, 719925, 720654, 721383, 722110, 722836,
 723562, 724287, 725011, 725734, 726456, 727177, 727897, 728617, 729335, 730053, 730770, 731486, 732201, 732915, 733629, 734341,
 735053, 735764, 736474, 737183, 737892, 738599, 739306, 740012, 740717, 741422, 742125, 742828, 743529, 744231, 744931, 745630,
 746329, 747027, 747724, 748420, 749115, 749810, 750504, 751197, 751889, 752581, 753272, 753962, 754651, 755339, 756027, 756714,
 757400, 758085, 758770, 759454, 760137, 760819, 761501, 762182, 762862, 763542, 764220, 764898, 765575, 766252, 766928, 767603,
 768277, 768950, 769623, 770295, 770967, 771637, 772307, 772977, 773645, 774313, 774980, 775647, 776313, 776978, 777642, 778306,
 778969, 779631, 780293, 780953, 781614, 782273, 782932, 783590, 784248, 784905, 785561, 786216, 786871, 787525, 788179, 788832,
 789484, 790136, 790786, 791437, 792086, 792735, 793383, 794031, 794678, 795324, 795970, 796615, 797260, 797903, 798547, 799189,
 799831, 800472, 801113, 801753, 802392, 803031, 803669, 804307, 804944, 805580, 806216, 806851, 807485, 808119, 808752, 809385,
 810017, 810648, 811279, 811909, 812539, 813168, 813796, 814424, 815052, 815678, 816304, 816930, 817555, 818179, 818803, 819426,
 820049, 820671, 821292, 821913, 822533, 823153, 823772, 824391, 825009, 825626, 826243, 826859, 827475, 828090, 828705, 829319,
 829932, 830545, 831158, 831770, 832381, 832992, 833602, 834212, 834821, 835429, 836037, 836645, 837252, 837858, 838464, 839070,
 839674, 840279, 840883, 841486, 842088, 842691, 843292, 843893, 844494, 845094, 845694, 846293, 846891, 847489, 848087, 848684,
 849280, 849876, 850472, 851067, 851661, 852255, 852848, 853441, 854034, 854626, 855217, 855808, 856398, 856988, 857578, 858167,
 858755, 859343, 859931, 860517, 861104, 861690, 862275, 862860, 863445, 864029, 864613, 865196, 865778, 866360, 866942, 867523,
 868104, 868684, 869264, 869843, 870422, 871000, 871578, 872156, 872732, 873309, 873885, 874460, 875036, 875610, 876184, 876758,
 877331, 877904, 878476, 879048, 879620, 880191, 880761, 881331, 881901, 882470, 883039, 883607, 884175, 884742, 885309, 885875,
 886441, 887007, 887572, 888137, 888701, 889265, 889828, 890391, 890954, 891516, 892078, 892639, 893200, 893760, 894320, 894879,
 895439, 895997, 896555, 897113, 897671, 898228, 898784, 899340, 899896, 900451, 901006, 901561, 902115, 902668, 903221, 903774,
 904327, 904879, 905430, 905981, 906532, 907082, 907632, 908182, 908731, 909280, 909828, 910376, 910923, 911470, 912017, 912563,
 913109, 913655, 914200, 914744, 915289, 915833, 916376, 916919, 917462, 918004, 918546, 919088, 919629, 920170, 920710, 921250,
 921790, 922329, 922868, 923406, 923944, 924482, 925019, 925556, 926093, 926629, 927164, 927700, 928235, 928770, 929304, 929838,
 930371, 930904, 931437, 931970, 932502, 933033, 933565, 934095, 934626, 935156, 935686, 936215, 936745, 937273, 937802, 938330,
 938857, 939385, 939911, 940438, 940964, 941490, 942015, 942541, 943065, 943590, 944114, 944637, 945161, 945684, 946206, 946729,
 947251, 947772, 948293, 948814, 949335, 949855, 950375, 950894, 951413, 951932, 952450, 952969, 953486, 954004, 954521, 955038,
 955554, 956070, 956586, 957101, 957616, 958131, 958645, 959159, 959673, 960186, 960699, 961212, 961724, 962236, 962748, 963259,
 963770, 964281, 964791, 965301, 965811, 966320, 966829, 967338, 967846, 968354, 968862, 969369, 969876, 970383, 970889, 971396,
 971901, 972407, 972912, 973417, 973921, 974425, 974929, 975433, 975936, 976439, 976941, 977444, 977946, 978447, 978949, 979450,
 979950, 980451, 980951, 981451, 981950, 982449, 982948, 983447, 983945, 984443, 984940, 985438, 985935, 986431, 986928, 987424,
 987919, 988415, 988910, 989405, 989899, 990394, 990888, 991381, 991875, 992368, 992860, 993353, 993845, 994337, 994828, 995320,
 995811, 996301, 996792, 997282, 997772, 998261, 998750, 999239, 999728,1000216,1000704,1001192,1001679,1002166,1002653,1003140,
1003626,1004112,1004598,1005083,1005569,1006053,1006538,1007022,1007506,1007990,1008473,1008957,1009439,1009922,1010404,1010886,
1011368,1011850,1012331,1012812,1013292,1013773,1014253,1014732,1015212,1015691,1016170,1016649,1017127,1017605,1018083,1018561,
1019038,1019515,1019992,1020468,1020945,1021421,1021896,1022372,1022847,1023322,1023796,1024271,1024745,1025219,1025692,1026165,
1026638,1027111,1027584,1028056,1028528,1028999,1029471,1029942,1030413,1030883,1031354,1031824,1032294,1032763,1033233,1033702,
1034170,1034639,1035107,1035575,1036043,1036510,1036978,1037445,1037911,1038378,1038844,1039310,1039776,1040241,1040706,1041171,
1041636,1042100,1042565,1043028,1043492,1043956,1044419,1044882,1045344,1045807,1046269,1046731,1047192,1047654,1048115,1048576
};

#pragma pack(1)
struct bu21029_register_map
{
    /* page 0 */
    uint8_t rsv0_0 :1;
    uint8_t rsv0_1 :1;
    uint8_t rsv0_2 :1;
    uint8_t rsv0_3 :1;
    uint8_t intrm  :1;
    uint8_t calib  :1;
    uint8_t rsv0_6 :1;
    uint8_t rsv0_7 :1;

    uint8_t smpl   :3;
    uint8_t rsv1_3 :1;
    uint8_t ave    :3;
    uint8_t mav    :1;

    uint8_t time_st_adc: 4;
    uint8_t intvl_time : 4;

    uint8_t evr_x;
    uint8_t evr_y;
    uint8_t evr_xy;
    uint8_t rsv6;
    uint8_t rsv7;
    uint8_t rsv8;
    uint8_t pimir_x:5;
    uint8_t rsv9_5:3;
    uint8_t pimir_y:5;
    uint8_t rsva_5:3;

    uint8_t pidac_ofs: 4;
    uint8_t dual: 1;
    uint8_t pu90k: 1;
    uint8_t stretch: 1;
    uint8_t rm8: 1;

    uint8_t avdd: 3;
    uint8_t rsvc_3: 1;
    uint8_t pvdd: 3;
    uint8_t rsvc_7: 1;

    uint8_t touch:1;
    uint8_t calib_done:1;
    uint8_t active:1;
    uint8_t busy:1;
    uint8_t rsvd_4:1;
    uint8_t pdm:1;
    uint8_t _auto:1;
    uint8_t test:1;

    uint8_t hw_idh;
    uint8_t hw_idl;

    /* page 1 */
    uint8_t sw_xn_gnd:1;
    uint8_t sw_xn_pow:1;
    uint8_t sw_xp_gnd:1;
    uint8_t sw_xp_pow:1;
    uint8_t sw_yn_gnd:1;
    uint8_t sw_yn_pow:1;
    uint8_t sw_yp_gnd:1;
    uint8_t sw_yp_pow:1;

    uint8_t sw_xnm :1;
    uint8_t sw_xpm:1;
    uint8_t sw_ynm:1;
    uint8_t sw_ypm:1;
    uint8_t sw_aux:1;
    uint8_t rsv11_5:3;

    uint8_t rsv12_14[3];

    uint8_t rsv15_0:6;
    uint8_t dprm   :1;
    uint8_t rsv15_7:1;

    uint8_t rsv16[10];
};

struct output_data_format
{
    uint8_t x_h;
    uint8_t x_l;
    uint8_t y_h;
    uint8_t y_l;
    uint8_t z1_h;
    uint8_t z1_l;
    uint8_t z2_h;
    uint8_t z2_l;

    uint8_t px_h;
    uint8_t px_l;
    uint8_t py_h;
    uint8_t py_l;
    uint8_t gh_h;
    uint8_t gh_l;
};

#define __DECL_THIS() struct awbl_bu21029muv *p_this = \
                        AW_CONTAINER_OF(p_serv, struct awbl_bu21029muv, ts)

static int __mkcmd_reg(int page, int reg_addr, int swrst, int stp)
{
    int cmd = (0<<7) /* CID=0: Read/Write data register  */
            |((reg_addr&0x0f) << 3)
            |((page&0x01)<<2)
            |((0x01&swrst)<<1)
            |((0x01&stp)<<0);
    return cmd;
}

static int __mkcmd_fun(int cf, int cmsk, int pdm, int stp)
{
    int cmd = (1<<7) /* CID=0: Read/Write data register  */
            |((cf&0x0f) << 3)
            |((cmsk&0x01)<<2)
            |((0x01&pdm)<<1)
            |((0x01&stp)<<0);
    return cmd;
}

static aw_err_t __start_automatic_scan(struct awbl_bu21029muv *p_this)
{
    uint8_t cmd = __mkcmd_fun(1, 0, 1, 1);
    aw_i2c_write(&p_this->iic_bus_nosubaddr, 0, &cmd, 1);

    cmd = __mkcmd_fun(0, 0, 1, 0);
    return aw_i2c_write(&p_this->iic_bus_nosubaddr, 0, &cmd, 1);
}
static aw_err_t __software_reset(struct awbl_bu21029muv *p_this)
{
    uint8_t cmd = __mkcmd_reg(0, 0, 1, 1);
    aw_i2c_write(&p_this->iic_bus_nosubaddr, 0, &cmd, 1);
    cmd = __mkcmd_reg(1, 0, 1, 1);
    return aw_i2c_write(&p_this->iic_bus_nosubaddr, 0, &cmd, 1);
}

static aw_err_t __i2c_read(struct awbl_bu21029muv *p_this,
                           uint32_t subaddr,
                           uint8_t *p_buf,
                           size_t  nbytes)
{
    aw_err_t err;
    int retry = 0;

    struct awbl_bu21029muv_cfg * p_dev;
    p_dev = (struct awbl_bu21029muv_cfg *)p_this->super.p_devhcf->p_devinfo;

    /* 若连续三次i2c读取失败，提供触摸复位 */
    do {
            err = aw_i2c_read(&p_this->iic_bus, subaddr, p_buf, nbytes);
    } while ((err != AW_OK) && (retry++ < 3));

    if(err == AW_OK) {
        return err;
    }

    if(p_dev->rst_pin < 0) {
        return err;
    } else {
        aw_gpio_set(p_dev->rst_pin, 0);
        aw_mdelay(10);
        aw_gpio_set(p_dev->rst_pin, 1);
        aw_mdelay(10);
    }

    return err;
}

#if 0
static aw_err_t __read_regs(struct awbl_bu21029muv *p_this, 
                            int                     page, 
                            int                     regaddr, 
                            char                   *p_buff, 
                            int                     n)
{
    int cmd = __mkcmd_reg(page, regaddr, 0, 0);
    return __i2c_read(p_this, cmd, p_buff, n);
}


static aw_err_t __write_regs(struct awbl_bu21029muv      *p_this, 
                             struct bu21029_register_map *p_reg)
{
    int cmd;
    aw_err_t err;
    int page = 0;
    int start_address;
    int pdm = 0;
    int stp = 0;
    int page0_size = 0x10;
    int page1_size = 0x10;


    page = 0;
    start_address = 0;
    cmd = __mkcmd_reg(page, start_address, pdm, stp);
    err = aw_i2c_write(&p_this->iic_bus, cmd, p_reg, page0_size);
    page = 1;
    start_address = 0;
    cmd = __mkcmd_reg(page, start_address, pdm, stp);
    err = aw_i2c_write(&p_this->iic_bus, cmd, (char*)p_reg+page0_size, page1_size);
    return err;
}

#endif

static aw_err_t __calibration(struct awbl_bu21029muv *p_this)
{
    uint8_t cmd = __mkcmd_fun(5, 0, 0, 0);
    int err;
    err = aw_i2c_write(&p_this->iic_bus_nosubaddr, 0, &cmd, 1);
    aw_mdelay(5);
    return err;
}

static aw_err_t __read_regs_map(struct awbl_bu21029muv      *p_this,
                                struct bu21029_register_map *reg)
{
    int page0_size = 0x10;
    int page1_size = 0x10;
    aw_err_t err;

    memset(reg, 0x00, sizeof(*reg));
    err = __i2c_read(p_this,
                     __mkcmd_reg(0, 0, 0, 1),
                     (uint8_t*)reg,
                     page0_size);
    if (err){
        return err;
    }
    err = __i2c_read(p_this,
                     __mkcmd_reg(1, 0, 0, 1),
                     (uint8_t*)reg+page0_size,
                     page1_size);

    return err;
}



static int __get_touch(struct awbl_bu21029muv *p_this)
{
    uint8_t dat1 = 0;

    __i2c_read(p_this, __mkcmd_reg(0, 0x0d, 0, 1), &dat1, 1);

    return dat1 & 0x01;
}

static void __init_regs(struct awbl_bu21029muv *p_this)
{
#if 0
    struct bu21029_register_map reg;
    const char reg_bytes[]={
            0x20, 0xA6, 0x04, 0x10, 0x10, 0x10, 0x00, 0x00,
            0x00, 0x0F, 0x0F, 0x72, 0x00, 0x00, 0x02, 0x29,

            0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    memcpy( &reg, reg_bytes, sizeof(reg));


    reg.intvl_time = 0x04;  /* 0.256ms*/

    reg.time_st_adc = 0x0e; /* 400us */

    reg.calib   = 1; /* Not to use calibration result.*/
    reg.stretch = 0; /* stretch off;*/
    reg.mav     = 1; /* median average filter, 0=off, 1=on*/
    reg.ave     = 7; /* AVE+1, the number of average samples setting for MAV.*/
    reg.rm8     = 0; /* coordinate resolution setting. 0=12bit, 1=8bit*/
    reg.dual    = 1; /* dual touch detection function setting off.*/
    reg.pvdd    = 7; /* 2.000V*/
    reg.avdd    = 7; /* 2.000V*/
    reg.pu90k   = 0;
    reg.evr_x   = 0xe0;
    reg.evr_y   = 0x80;
    reg.evr_xy  = 0x40;
    reg._auto   = 1;

    __calibration(p_this);
    __write_regs(p_this, &reg);
#else
    uint32_t cmd;
    uint8_t  init_val;
    cmd = __mkcmd_reg(0, 0, 0, 0);
    init_val = 0x20;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x1, 0, 0);
    init_val = 0xa6;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x2, 0, 0);
    init_val = 0x04;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x3, 0, 0);
    init_val = 0x1F;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x4, 0, 0);
    init_val = 0x1F;//0x80;//
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x5, 0, 0);
    init_val = 0x10;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x6, 0, 0);
    init_val = 0x00;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x7, 0, 0);
    init_val = 0x00;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x8, 0, 0);
    init_val = 0x00;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0x9, 0, 0);
    init_val = 0x0F;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0xa, 0, 0);
    init_val = 0x0F;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0xb, 0, 0);
    init_val = 0x72;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    cmd = __mkcmd_reg(0, 0xc, 0, 0);
    init_val = 0x00;
    aw_i2c_write(&p_this->iic_bus, cmd, &init_val, 1);

    __calibration(p_this);
    aw_mdelay(3);
#endif
}

void awbl_bu21029muv_init(struct awbl_bu21029muv *p_this, int addr, int busid)
{
    aw_i2c_mkdev(&p_this->iic_bus, 
                  busid, 
                  addr, 
                  AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
    aw_i2c_mkdev(&p_this->iic_bus_nosubaddr,
                    p_this->iic_bus.busid,
                    p_this->iic_bus.addr,
                    AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_NONE
                    );
    __software_reset(p_this);
    __init_regs(p_this);
}

int awbl_bu21029muv_is_valid(struct awbl_bu21029muv *p_this)
{
    struct bu21029_register_map reg;
    memset(&reg, 0x00, sizeof(reg));

    if ( AW_OK != __read_regs_map(p_this, &reg) ){
        return AW_ERROR;
    }

    return (reg.hw_idh == 0x02) && (reg.hw_idl == 0x29);
}

int awbl_bu21029muv_read_result(struct awbl_bu21029muv    *p_this, 
                                struct output_data_format *p_out)
{
    uint8_t cmd = __mkcmd_fun(0, 1, 1, 0);
    int err = __i2c_read(p_this, cmd, (uint8_t*)p_out, sizeof(*p_out));
    return err;
}


#if 0

void do_man_scan(struct awbl_bu21029muv *p_this)
{
    uint8_t cmd = __mkcmd_fun(0, 1, 0, 0);
    unsigned char buff[64];
    int i;

    memset(buff, 0x00, sizeof(buff));


    cmd = __mkcmd_fun(0x00, 0x01, 1, 0);
    __i2c_read(p_this, cmd, (uint8_t*)buff, 14);

    for (i=0; i<14; i++){
        aw_kprintf("%02X ", buff[i]);
    }
    aw_kprintf("\n");
}


void do_bu21029_test(void)
{
    struct bu21029_register_map reg;
    struct output_data_format out;
    struct awbl_bu21029muv obj;

    aw_gpio_pin_cfg(GP6_1, AW_GPIO_OUTPUT);
    aw_gpio_set(GP6_1, 0);
    aw_mdelay(100);

    awbl_bu21029muv_init(&obj);
    __init_regs(&obj);
    aw_mdelay(200);
    __read_regs_map(&obj, &reg);

    memset(&reg, 0x00, sizeof(reg));
    memset(&out, 0x00, sizeof(out));
    __start_automatic_scan(&obj);
    while (1){
        int i;
//        __start_automatic_scan(&obj);
        aw_mdelay(50);
        __read_regs_map(&obj, &reg);
        aw_kprintf(" touch=%d cd=%d ", reg.touch, reg.calib_done );
        do_man_scan(&obj);
        aw_kprintf("\n");
    }
}
#endif

#if 1
struct output_data_combined
{
    int16_t x;
    int16_t y;
    int16_t z1;
    int16_t z2;
    int16_t px;
    int16_t py;
    int16_t gh;
};

static void __combine_output_data(struct output_data_format *in,
                                  struct output_data_combined *out)
{
    out->x = (((int16_t)in->x_h << 8) | (in->x_l))>>4;
    out->y = (((int16_t)in->y_h << 8) | (in->y_l))>>4;
    out->z1 = (((int16_t)in->z1_h << 8) | (in->z1_l))>>4;
    out->z2 = (((int16_t)in->z2_h << 8) | (in->z2_l))>>4;

    if ((in->px_l & 0x01) == 0x01)
    {
        out->px = ((int16_t)in->px_h << 2) + ((in->px_l & 0xC0) >> 6) - 1024;
    }
    else
    {
        out->px = ((int16_t)in->px_h << 2) + ((in->px_l & 0xC0) >> 6);
    }

    if ((in->py_l & 0x01) == 0x01)
    {
        out->py = ((int16_t)in->py_h << 2) + ((in->py_l & 0xC0) >> 6) - 1024;
    }
    else
    {
        out->py = ((int16_t)in->py_h << 2) + ((in->py_l & 0xC0) >> 6);
    }

    if ((in->gh_l & 0x01) == 0x01)
    {
        out->gh = ((in->gh_h << 4) + ((in->gh_l & 0xF0)>>4))- 4096;
    }
    else
    {
        out->gh = ((in->gh_h << 4) + ((in->gh_l & 0xF0)>>4));
    }

}


#define  X_MINVAL          10
#define  X_MAXVAL           4000

#define  Y_MINVAL           10
#define  Y_MAXVAL          4000

/** \brief 触摸屏压力阀值 有 10欧的外接电阻 */
#define  TOUCH_Z1_THRESHOLD       90    

/** \brief 触摸屏压力阀值 有 10欧的外接电阻 */     
#define  TOUCH_Z2_THRESHOLD       40         
static uint8_t __get_touch_state (struct output_data_combined *data)
{
       if (((4095 - data->z2) > TOUCH_Z2_THRESHOLD) && (data->z1 > TOUCH_Z1_THRESHOLD))
    {
        /*  采样数据超过采集区间所有数据无效*/
        if ((data->x < X_MINVAL) || (data->x > X_MAXVAL) ||
            (data->y < Y_MINVAL) || (data->y > Y_MAXVAL))
        {
            return 0;
        }

        if ((data->px > 70) || (data->py > 70))
        {
            return 0;
        }

        /*  多点触摸及方向判断 */
        if ((data->px > 2) || (data->py > 2))
        {
            aw_mdelay(2);
            /*aw_kprintf("px=%d py=%d\n", px, py);*/
            if (data->gh >= 0)
            {

                return 0x3;                    /* detect two points*/
               } else {

                return 0x2;                    /* detect two points and judge y*/
               }
        }
        else
        {

            return 0x1;                        /* detect one point*/
        }
    }
    else
    {
        return 0;
    }
}


#define X_LIMIT  281273
#define Y_LIMIT  288076
static void __cal_touch_position (struct output_data_combined *pos,
                                  uint8_t   touch_state,
                                  void     *p_x_sample[],
                                  void     *p_y_sample[],
                                  int       max_slots,
                                  uint8_t  *index)
{
    uint8_t count = *index;

    uint16_t *p_i2c_xsample  = NULL;
    uint16_t *p_i2c_ysample  = NULL;

    p_i2c_xsample = (uint16_t *)p_x_sample[0];
    p_i2c_ysample = (uint16_t *)p_y_sample[0];
    if (touch_state > 1) {
        int16_t dx = (pos->px > 0) ? (int16_t)(2048 * \
                    ((double)g_bu21029_conver_tab[pos->px] / X_LIMIT)) : 0;   
        int16_t dy = (pos->py > 0) ? (int16_t)(2048 * \
                    ((double)g_bu21029_conver_tab[pos->py] / Y_LIMIT)) : 0; 

        if (0x2 == touch_state) {
            p_i2c_xsample[count] = pos->x + dx;
            p_i2c_ysample[count] = pos->y - dy;
            p_i2c_xsample = (uint16_t *)p_x_sample[1];
            p_i2c_ysample = (uint16_t *)p_y_sample[1];
            p_i2c_xsample[count] = pos->x - dx;
            p_i2c_ysample[count] = pos->y + dy;

        } else {
            p_i2c_xsample[count] = pos->x + dx;
            p_i2c_ysample[count] = pos->y + dy;
            p_i2c_xsample = (uint16_t *)p_x_sample[1];
            p_i2c_ysample = (uint16_t *)p_y_sample[1];
            p_i2c_xsample[count] = pos->x - dx;
            p_i2c_ysample[count] = pos->y - dy;
        }

    } else {
        p_i2c_xsample[count] = pos->x;
        p_i2c_ysample[count] = pos->y;
    }
}
#endif

/* 获取X Y 轴坐标，并且返回是否按下 */
static int __xy_get (struct awbl_ts_service *p_serv,
                     void                   *p_x_sample[],
                     void                   *p_y_sample[],
                     int                     max_slots,
                     int                     count)
{
    __DECL_THIS();
    struct output_data_format out;
    int i;
    uint16_t x;
    uint16_t y;
    uint16_t z1;
    uint16_t z2;
    int32_t res;
    uint8_t index = 0;
    uint8_t mt_cnt = 0;

    uint16_t *p_i2c_xsample  = NULL;
    uint16_t *p_i2c_ysample  = NULL;

    /* test */
    struct output_data_combined out_combine;
    uint8_t touch_state = 0;
    uint8_t first_touch_sta = 0;
    /* end of test */

    if (!__get_touch(p_this)){
        return 0;
    }
    __start_automatic_scan(p_this);
    aw_mdelay(10);
    awbl_bu21029muv_read_result(p_this, &out); /* dummy read*/
    for (i=0; i<count; i++){
        aw_mdelay(2);
        if (awbl_bu21029muv_read_result(p_this, &out) == AW_OK){
            x = ((uint16_t)out.x_h << 8) | (out.x_l);
            y = ((uint16_t)out.y_h << 8) | (out.y_l);
            z1 = (((uint16_t)out.z1_h << 8) | (out.z1_l))>>4;
            z2 = (((uint16_t)out.z2_h << 8) | (out.z2_l))>>4;
            x >>= 4;
            y >>= 4;
           
            if (x == 0 && y == 0 && ((z1 == 0) || (z2 == 0))){
                continue;
            }

            if ((z1 == 0) || (z2 == 0)) {

                continue;
            }


            res = ((((z2-z1)*x*(400/50))/z1 + 2047/50) >> 12);
            if (res > 50){
                continue;
            }

            __combine_output_data(&out, &out_combine);
            touch_state = __get_touch_state(&out_combine);
            if (touch_state == 0) {

                return 0;
            }
            if (i == 0) {

                first_touch_sta = touch_state;
            }
            if (touch_state != first_touch_sta) {

                return 0;
            }

            __cal_touch_position(&out_combine, touch_state, p_x_sample, p_y_sample, max_slots, &index);
            index++;
        }
    }
    if (index == 0){
        return 0;
    }

    if (index < count){
        for (i = 0; i < touch_state; i++) {

            if (mt_cnt <  max_slots) {

                p_i2c_xsample = (uint16_t *)p_x_sample[mt_cnt];
                p_i2c_ysample = (uint16_t *)p_y_sample[mt_cnt];

                for (i=index; i<count; i++){

                    p_i2c_xsample[i] = p_i2c_xsample[i-1];
                    p_i2c_ysample[i] = p_i2c_ysample[i-1];

                }

                mt_cnt++;
            }
        }
    } else {
        /* 如果多个点按下，只返回用户请求的触摸点数 */
        if (touch_state > max_slots) {
            mt_cnt = max_slots;
        } else {
            mt_cnt = touch_state;
        }
    }

    return mt_cnt;
}

/* 设置是否锁屏 */
static void __lock_set(struct awbl_ts_service *p_serv, aw_bool_t lock)
{
    /* __DECL_THIS();*/
}

/* 准备测量X轴坐标  */
static void __active_x(struct awbl_ts_service *p_serv)
{
}

/* 准备测量Y轴坐标  */
static void __active_y(struct awbl_ts_service *p_serv)
{
}

/* 测量X轴坐标  */
static void __measure_x (
        struct awbl_ts_service *p_serv,
        void                   *p_sample[],
        int                     max_slots,
        int                     count)
{

    __DECL_THIS();
    struct output_data_format out;
    uint16_t *p_i2_xsample = (uint16_t *)p_sample[0];
    if (awbl_bu21029muv_read_result(p_this, &out) == AW_OK){
        uint16_t y;
        int i;


        y = ((uint16_t)out.y_h << 8) | (out.y_l);

        for (i=0; i<count; i++){
            /* todo 暂时只取了一点*/
            p_i2_xsample[i] = y;
        }
    }

}

/* 测量Y轴坐标  */
static void __measure_y (
        struct awbl_ts_service *p_serv,
        void                   *p_sample[],
        int                     max_slots,
        int                     count)
{
    __DECL_THIS();
    struct output_data_format out;
    uint16_t *p_i2_ysample = (uint16_t *)p_sample[0];
    if (awbl_bu21029muv_read_result(p_this, &out) == AW_OK){
        uint16_t x;
        int i;
        x = ((uint16_t)out.x_h << 8) | (out.x_l);

        for (i=0; i<count; i++){
            /* todo 暂时只取了一点*/
            p_i2_ysample[i] = x;
        }
    }
}

static int __is_touch(struct awbl_ts_service *p_serv)
{
    __DECL_THIS();

    return __get_touch(p_this);
}

/* instance initializing stage 1 */
static void __inst_connect (struct awbl_dev *p_dev)
{
    aw_err_t err = 0;
    struct awbl_bu21029muv *p_this = AW_CONTAINER_OF(p_dev, 
                                                     struct awbl_bu21029muv, 
                                                     super);
    struct awbl_bu21029muv_cfg *cfg;
    cfg = (struct awbl_bu21029muv_cfg *)p_dev->p_devhcf->p_devinfo;

    /* 初始化触摸屏的中断引脚*/
    if(cfg->int_pin != -1)
    {
        err = aw_gpio_pin_request("bu21029muv_int_gpio", &(cfg->int_pin), 1);
        if (err == AW_OK){
            aw_gpio_pin_cfg(cfg->int_pin, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
        } else {
            AW_INFOF(("bu21029muv Request GPIO INT pin Failed %d！\r\n",err));
        }
    }

    /* 初始化触摸屏的复位引脚*/
    if(cfg->rst_pin != -1)
    {
        err = aw_gpio_pin_request("bu21029muv_rst_gpio", &(cfg->rst_pin), 1);
        if (err == AW_OK){
            aw_gpio_pin_cfg(cfg->rst_pin, AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PUSH_PULL);
        } else {
            AW_INFOF(("bu21029muv Request GPIO RST pin Failed %d！\r\n",err));
        }
    }


    awbl_bu21029muv_init(p_this, cfg->dev_addr, p_this->super.p_devhcf->bus_index);

    awbl_ts_service_init(&p_this->ts, &cfg->ts_serv_info);

    p_this->ts.drv_fun.pfn_xy_get    = __xy_get;
    p_this->ts.drv_fun.pfn_measure_y = __measure_y;
    p_this->ts.drv_fun.pfn_measure_x = __measure_x;
    p_this->ts.drv_fun.pfn_lock_set = __lock_set;
    p_this->ts.drv_fun.pfn_is_touch = __is_touch;
    p_this->ts.drv_fun.pfn_active_y = __active_y;
    p_this->ts.drv_fun.pfn_active_x = __active_x;

    /* 得到的是采样值，需要校准  */
    p_this->ts.ts_cal_att  = 1;
    awbl_ts_service_register( &p_this->ts );

    if (!awbl_bu21029muv_is_valid(p_this)){
        aw_kprintf("bu21029muv NOT found on iic bus %d with slaveaddress 0x%02x\r\n", 
                    p_this->super.p_devhcf->bus_index, cfg->dev_addr);
        return ;
    }
}

/** instance initializing stage 2 */
static void __inst_init1 (struct awbl_dev *p_dev)
{

}

/** instance initializing stage 3 */
static void __inst_init2 (struct awbl_dev *p_dev)
{

}


/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs= {
     __inst_init1,      /* pfunc_dev_init1 */
     __inst_init2,      /* pfunc_dev_init2 */
    __inst_connect      /* pfunc_dev_connect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo= {

    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_I2C,                 /* bus_id */
    AWBL_TOUCHSCREEN_NAME,          /* p_drvname */
    &__g_awbl_drvfuncs, /* p_busfuncs */
    NULL,                           /* p_methods */
    NULL                            /* pfunc_drv_probe */
};

/** \brief register touch screen driver */
void awbl_bu21029muv_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo);
}

/* end of file*/
