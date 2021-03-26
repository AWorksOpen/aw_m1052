#include "usrmod.h"
#if MICROPY_PY_USRMOD

/*
 * \brief
 * \param[in] arg1 tuple���Ͳ���
 * \param[in] arg2 tuple���Ͳ���
 *
 * example: import usrmod
 *          tuple1 = (string,int)
 *          tuple2 = (string,int)
 *          usrmod.usrfun3(tuple1,tuple2)
 *
 */
STATIC mp_obj_t usrmod_usrfun3(mp_obj_t arg1,mp_obj_t arg2) {

    /*���������ж�*����������������Ԫ������*/
    if (!MP_OBJ_IS_TYPE(arg1, &mp_type_tuple) || !MP_OBJ_IS_TYPE(arg2, &mp_type_tuple)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "arg must be tuple"));
    }

    /*Ԫ������ת��*/
    mp_obj_t *items1;
    mp_obj_get_array_fixed_n(arg1, 2, &items1);
    const char *buf1 = mp_obj_str_get_str(items1[0]);
    int id1 = mp_obj_get_int(items1[1]);

    mp_obj_t *items2;
    mp_obj_get_array_fixed_n(arg1, 2, &items2);
    const char *buf2 = mp_obj_str_get_str(items2[0]);
    int id2 = mp_obj_get_int(items2[1]);

    printf("tuple1 = (%s,%d)\r\n",buf1,id1);
    printf("tuple2 = (%s,%d)\r\n",buf2,id2);

    printf("-----------------------------------------\r\n");
    printf("wait add function...\r\n");
    printf("-----------------------------------------\r\n");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(usrmod_usrfun3_obj,usrmod_usrfun3);


/*
 * \brief
 * \param[in] arg int���Ͳ���
 *
 * example: import usrmod
 *          usrmod.usrfun2(1)
 *
 */
STATIC mp_obj_t usrmod_usrfun2(mp_obj_t arg) {

    /*���������ж�*/
    if(!MP_OBJ_IS_INT(arg)){
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "arg must be int"));
    }

    /*�� mp_obj_t ����ת���� int ��*/
    int data = mp_obj_get_int(arg);
    printf("data = %d\r\n",data);

    printf("-----------------------------------------\r\n");
    printf("wait add function...\r\n");
    printf("-----------------------------------------\r\n");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(usrmod_usrfun2_obj, usrmod_usrfun2);


/*
 * \brief
 * \param ��
 *
 * \example import usrmod
 *          usrmod.usrfun1()
 *
 */
STATIC mp_obj_t usrmod_usrfun1(void) {

    /*���������û����������*/

    printf("-----------------------------------------\r\n");
    printf("wait add function...\r\n");
    printf("-----------------------------------------\r\n");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(usrmod_usrfun1_obj,usrmod_usrfun1);

/*
 * \brief usrmodģ�����ú��������
 */
STATIC const mp_rom_map_elem_t mp_module_usrmod_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usrmod) },

    //usrmodģ���µ����
    { MP_ROM_QSTR(MP_QSTR_usrclass1),           MP_ROM_PTR(&usrmod_usrclass1_type) },

    //usrmodģ������ú���
    { MP_ROM_QSTR(MP_QSTR_usrfun1),           MP_ROM_PTR(&usrmod_usrfun1_obj) },
    { MP_ROM_QSTR(MP_QSTR_usrfun2),           MP_ROM_PTR(&usrmod_usrfun2_obj) },
    { MP_ROM_QSTR(MP_QSTR_usrfun3),           MP_ROM_PTR(&usrmod_usrfun3_obj) },

};

STATIC MP_DEFINE_CONST_DICT(mp_module_usrmod_globals, mp_module_usrmod_globals_table);

/*
 * \brief usrmodģ��ʵ��
 */
const mp_obj_module_t mp_module_usrmod = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_usrmod_globals,
};
#endif
