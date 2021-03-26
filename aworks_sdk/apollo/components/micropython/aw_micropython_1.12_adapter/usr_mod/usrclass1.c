#include "usrclass1.h"
#include <string.h>

#if MICROPY_PY_USRMOD

/*
 * \brief usrclass1类
 */
typedef struct _usrclass1_obj_t {
    mp_obj_base_t base;
    mp_uint_t id;
    char name[16];
    mp_uint_t age;
    char post[16];
} usrclass1_obj_t;


/*
 * \brief 打印usrclass1类型对象属性信息
 *
 * \param self_in 对象实例
 *
 */
STATIC void usrmod_usrclass1_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    usrclass1_obj_t *self = self_in;
    mp_printf(print, "<usrclass1 %d>", self->id);
}


/*
 * \brief usrclass1对象初始化
 *
 * \param[in] self 对象实例
 * \param[in] n_args 普通参数个数，name参数是必选项，age参数是int型,默认值是30，为可选项
 * \param[in] pos_args 普通参数起始位置
 * \param[in] kw_args 带关键字参数，post参数是带关键字的参数，为可选项
 *
 * \retun NULL
 *
 */
STATIC mp_obj_t usrmod_usrclass1_obj_init_helper (usrclass1_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_name, ARG_age, ARG_post };

    /*参数合法性定义*/
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

/* \brief 创建usrclass1_obj_t类型对象，并初始化id
 *
 * \param type usrmod_usrclass1_type基类对象
 * \param n_args 普通参数个数
 * \param n_kw 带关键字参数个数
 * \param args 参数起始地址
 *
 * \return usrclass1类型对象
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
 * \brief usrclass1对象初始化
 *
 * \param n_args 参数个数，name参数是必选项，age参数是int型,默认值是30，为可选项
 * \param args 参数起始地址，args[0]为对象实例
 * \param kw_args 带关键字参数，post参数是带关键字的参数，为可选项
 *
 * example：from usrmod import *
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
 * \param arg1 实例对象
 * \param[in] arg2 参数1，array类型
 * \param[in] arg3 参数2，array类型
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

    /*参数类型判断,arg2和arg3必须是 bytearray 型*/
    if (!MP_OBJ_IS_TYPE(arg2, &mp_type_bytearray) || !MP_OBJ_IS_TYPE(arg3, &mp_type_bytearray)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "arg must be bytearray"));
    }

    mp_obj_array_t *array1 = MP_OBJ_TO_PTR(arg2);
    mp_obj_array_t *array2 = MP_OBJ_TO_PTR(arg3);

    printf("array1 = %s\r\n",(char*)array1->items);
    printf("array2 = %s\r\n",(char*)array2->items);

    /*函数功能待添加*/

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(usrmod_usrclass1_obj_usrfun4_obj,usrmod_usrclass1_obj_usrfun4);

/*
 * \brief
 *
 * \param arg1 实例对象
 * \param[in] arg2 参数1，dict类型
 * \param[in] arg3 参数2，dict类型
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
        /*参数类型判断，args[1]必须是字典类型*/
        if (!MP_OBJ_IS_TYPE(args[1], &mp_type_dict)) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "args[1] must be dict"));
        }

        /*字典类型转换,该例程需要定义{string:int,string:int...}类型的字典,否则转化数据会出错*/
        mp_obj_dict_t *d = MP_OBJ_TO_PTR(args[1]);
        printf("dict = {'%s':%d,'%s':%d}\r\n",
                mp_obj_str_get_str(d->map.table[0].key),
                mp_obj_get_int(d->map.table[0].value),
                mp_obj_str_get_str(d->map.table[1].key),
                mp_obj_get_int(d->map.table[1].value));
    }

    /*函数功能待添加*/

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(usrmod_usrclass1_obj_usrfun5_obj,1,usrmod_usrclass1_obj_usrfun5);


/*
 * \brief
 *
 * \param arg1：实例对象
 * \param[in] arg2：参数1，list类型
 * \param[in] arg3：参数2，list类型
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
        /*参数类型判断*/
        if (!MP_OBJ_IS_TYPE(args[1], &mp_type_list)) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "args[1] must be list"));
        }

        /*该例程需要定义[int,string,string...]类型的列表，否则参数类型转换会出错*/
        mp_obj_list_t *ret = MP_OBJ_TO_PTR(args[1]);

        printf("list = [%d,'%s','%s']\r\n",
                mp_obj_get_int(ret->items[0]),
                mp_obj_str_get_str(ret->items[1]),
                mp_obj_str_get_str(ret->items[2]));
    }

    /*函数功能待添加*/

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usrmod_usrclass1_obj_usrfun6_obj,1,3,usrmod_usrclass1_obj_usrfun6);


/*
 * brief usrclass1类内置函数表
 */
STATIC const mp_rom_map_elem_t usrmod_usrclass1_locals_dict_table[] = {
    // instance methods

    //初始化函数
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&usrmod_usrclass1_obj_init_obj) },

    //方法
    { MP_ROM_QSTR(MP_QSTR_usrfun4),    MP_ROM_PTR(&usrmod_usrclass1_obj_usrfun4_obj) },
    { MP_ROM_QSTR(MP_QSTR_usrfun5),    MP_ROM_PTR(&usrmod_usrclass1_obj_usrfun5_obj) },
    { MP_ROM_QSTR(MP_QSTR_usrfun6),    MP_ROM_PTR(&usrmod_usrclass1_obj_usrfun6_obj) },

    //常量
    { MP_ROM_QSTR(MP_QSTR_const),    MP_ROM_INT(USRCLASS_CONST_AGE) },
};
STATIC MP_DEFINE_CONST_DICT(usrmod_usrclass1_locals_dict, usrmod_usrclass1_locals_dict_table);

/*
 * \brief usrclass1类型对象的基类对象
 */
const mp_obj_type_t usrmod_usrclass1_type = {
    { &mp_type_type },
    .name = MP_QSTR_usrclass1,
    .print = usrmod_usrclass1_print,
    .make_new = mp_usrmod_usrclass1_make_new,
    .locals_dict = (mp_obj_t)&usrmod_usrclass1_locals_dict,
};
#endif
