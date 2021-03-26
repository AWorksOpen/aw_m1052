#include "usrclass1.h"
#include <string.h>

#if MICROPY_PY_USRMOD

/*
 * \brief usrclass1��
 */
typedef struct _usrclass1_obj_t {
    mp_obj_base_t base;
    mp_uint_t id;
    char name[16];
    mp_uint_t age;
    char post[16];
} usrclass1_obj_t;


/*
 * \brief ��ӡusrclass1���Ͷ���������Ϣ
 *
 * \param self_in ����ʵ��
 *
 */
STATIC void usrmod_usrclass1_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    usrclass1_obj_t *self = self_in;
    mp_printf(print, "<usrclass1 %d>", self->id);
}


/*
 * \brief usrclass1�����ʼ��
 *
 * \param[in] self ����ʵ��
 * \param[in] n_args ��ͨ����������name�����Ǳ�ѡ�age������int��,Ĭ��ֵ��30��Ϊ��ѡ��
 * \param[in] pos_args ��ͨ������ʼλ��
 * \param[in] kw_args ���ؼ��ֲ�����post�����Ǵ��ؼ��ֵĲ�����Ϊ��ѡ��
 *
 * \retun NULL
 *
 */
STATIC mp_obj_t usrmod_usrclass1_obj_init_helper (usrclass1_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_name, ARG_age, ARG_post };

    /*�����Ϸ��Զ���*/
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_name, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_age, MP_ARG_INT, {.u_int = 30}},
        { MP_QSTR_post, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *name = mp_obj_str_get_str(args[ARG_name].u_obj);
    strncpy(self->name, name, sizeof(name));

    self->age = args[ARG_age].u_int;
    // check age
    if (self->age < 0 || self->age >100) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "age in range 0~100"));
    }

    if(args[ARG_post].u_obj != MP_OBJ_NULL){
        const char *post = mp_obj_str_get_str(args[ARG_post].u_obj);
        strncpy(self->post, post, sizeof(post));
    }

    return mp_const_none;
}

/* \brief ����usrclass1_obj_t���Ͷ��󣬲���ʼ��id
 *
 * \param type usrmod_usrclass1_type�������
 * \param n_args ��ͨ��������
 * \param n_kw ���ؼ��ֲ�������
 * \param args ������ʼ��ַ
 *
 * \return usrclass1���Ͷ���
 *
 * example: from usrmod import *
 *          usr1 = usrclass1(1)
 *          usr2 = usrclass1(2,"zhang",post  = "test")
 *          usr3 = usrclass1(3,"wang",20, post = "soft")
 */
STATIC mp_obj_t mp_usrmod_usrclass1_make_new(const mp_obj_type_t *type, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    usrclass1_obj_t *self = m_new_obj(usrclass1_obj_t);

    if (!self) {
        mp_raise_OSError(ENOMEM);
    }

    self->base.type = &usrmod_usrclass1_type;
    self->id = mp_obj_get_int(args[0]);

    if (n_args > 1 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        usrmod_usrclass1_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t) self;
}

/*
 * \brief usrclass1�����ʼ��
 *
 * \param n_args ����������name�����Ǳ�ѡ�age������int��,Ĭ��ֵ��30��Ϊ��ѡ��
 * \param args ������ʼ��ַ��args[0]Ϊ����ʵ��
 * \param kw_args ���ؼ��ֲ�����post�����Ǵ��ؼ��ֵĲ�����Ϊ��ѡ��
 *
 * example��from usrmod import *
 *         usr = usrclass1(1)
 *         usr.init("zhang",22,post = "test")
 */
STATIC mp_obj_t usrmod_usrclass1_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return usrmod_usrclass1_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(usrmod_usrclass1_obj_init_obj, 1, usrmod_usrclass1_obj_init);

/*
 * \brief
 *
 * \param arg1 ʵ������
 * \param[in] arg2 ����1��array����
 * \param[in] arg3 ����2��array����
 *
 * example: from usrmod import *
 *          usr = usrclass1(1)
 *          usr.init('zhangsan',33,post = 'soft')
 *          array1 = bytearray('123')
 *          array2 = bytearray('456')
 *
 *          usr.usrfun4(array1,array2)
 */
STATIC mp_obj_t usrmod_usrclass1_obj_usrfun4(mp_obj_t arg1,mp_obj_t arg2,mp_obj_t arg3){

    usrclass1_obj_t *self = arg1;

    printf("self->name is %s\r\n",self->name);
    printf("self->age is %d\r\n",self->age);
    printf("self->post is %s\r\n",self->post);

    /*���������ж�,arg2��arg3������ bytearray ��*/
    if (!MP_OBJ_IS_TYPE(arg2, &mp_type_bytearray) || !MP_OBJ_IS_TYPE(arg3, &mp_type_bytearray)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "arg must be bytearray"));
    }

    mp_obj_array_t *array1 = MP_OBJ_TO_PTR(arg2);
    mp_obj_array_t *array2 = MP_OBJ_TO_PTR(arg3);

    printf("array1 = %s\r\n",(char*)array1->items);
    printf("array2 = %s\r\n",(char*)array2->items);

    /*�������ܴ����*/

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(usrmod_usrclass1_obj_usrfun4_obj,usrmod_usrclass1_obj_usrfun4);

/*
 * \brief
 *
 * \param arg1 ʵ������
 * \param[in] arg2 ����1��dict����
 * \param[in] arg3 ����2��dict����
 *
 * example: from usrmod import *
 *          usr = usrclass1(1)
 *          usr.init('zhangsan',33,post = 'soft')
 *
 *          usr.usrfun5()
 *          usr.usrfun5({'a':1,'b':2})
 *          usr.usrfun5({'a':1,'b':2},2)
 *          usr.usrfun5({'a':1,'b':2},2,'a')
 */
STATIC mp_obj_t usrmod_usrclass1_obj_usrfun5(uint n_args, const mp_obj_t *args){


    usrclass1_obj_t *self = args[0];

    printf("self->name is %s\r\n",self->name);
    printf("self->age is %d\r\n",self->age);
    printf("self->post is %s\r\n",self->post);

    if(n_args > 1){
        /*���������жϣ�args[1]�������ֵ�����*/
        if (!MP_OBJ_IS_TYPE(args[1], &mp_type_dict)) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "args[1] must be dict"));
        }

        /*�ֵ�����ת��,��������Ҫ����{string:int,string:int...}���͵��ֵ�,����ת�����ݻ����*/
        mp_obj_dict_t *d = MP_OBJ_TO_PTR(args[1]);
        printf("dict = {'%s':%d,'%s':%d}\r\n",
                mp_obj_str_get_str(d->map.table[0].key),
                mp_obj_get_int(d->map.table[0].value),
                mp_obj_str_get_str(d->map.table[1].key),
                mp_obj_get_int(d->map.table[1].value));
    }

    /*�������ܴ����*/

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(usrmod_usrclass1_obj_usrfun5_obj,1,usrmod_usrclass1_obj_usrfun5);


/*
 * \brief
 *
 * \param arg1��ʵ������
 * \param[in] arg2������1��list����
 * \param[in] arg3������2��list����
 *
 * example: from usrmod import *
 *          usr = usrclass1(1)
 *          usr.init('zhangsan',33,post = 'soft')
 *
 *          usr.usrfun6()
 *          usr.usrfun6([1,'a','bc'])
 *          usr.usrfun6([1,'a','bc'],const)
 */
STATIC mp_obj_t usrmod_usrclass1_obj_usrfun6(uint n_args, const mp_obj_t *args){

    usrclass1_obj_t *self = args[0];

    printf("self->name is %s\r\n",self->name);
    printf("self->age is %d\r\n",self->age);
    printf("self->post is %s\r\n",self->post);

    if(n_args > 1){
        /*���������ж�*/
        if (!MP_OBJ_IS_TYPE(args[1], &mp_type_list)) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "args[1] must be list"));
        }

        /*��������Ҫ����[int,string,string...]���͵��б������������ת�������*/
        mp_obj_list_t *ret = MP_OBJ_TO_PTR(args[1]);

        printf("list = [%d,'%s','%s']\r\n",
                mp_obj_get_int(ret->items[0]),
                mp_obj_str_get_str(ret->items[1]),
                mp_obj_str_get_str(ret->items[2]));
    }

    /*�������ܴ����*/

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usrmod_usrclass1_obj_usrfun6_obj,1,3,usrmod_usrclass1_obj_usrfun6);


/*
 * brief usrclass1�����ú�����
 */
STATIC const mp_rom_map_elem_t usrmod_usrclass1_locals_dict_table[] = {
    // instance methods

    //��ʼ������
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&usrmod_usrclass1_obj_init_obj) },

    //����
    { MP_ROM_QSTR(MP_QSTR_usrfun4),    MP_ROM_PTR(&usrmod_usrclass1_obj_usrfun4_obj) },
    { MP_ROM_QSTR(MP_QSTR_usrfun5),    MP_ROM_PTR(&usrmod_usrclass1_obj_usrfun5_obj) },
    { MP_ROM_QSTR(MP_QSTR_usrfun6),    MP_ROM_PTR(&usrmod_usrclass1_obj_usrfun6_obj) },

    //����
    { MP_ROM_QSTR(MP_QSTR_const),    MP_ROM_INT(USRCLASS_CONST_AGE) },
};
STATIC MP_DEFINE_CONST_DICT(usrmod_usrclass1_locals_dict, usrmod_usrclass1_locals_dict_table);

/*
 * \brief usrclass1���Ͷ���Ļ������
 */
const mp_obj_type_t usrmod_usrclass1_type = {
    { &mp_type_type },
    .name = MP_QSTR_usrclass1,
    .print = usrmod_usrclass1_print,
    .make_new = mp_usrmod_usrclass1_make_new,
    .locals_dict = (mp_obj_t)&usrmod_usrclass1_locals_dict,
};
#endif
