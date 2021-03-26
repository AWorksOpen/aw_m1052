/**
 * File:   csv_file.h
 * Author: AWTK Develop Team
 * Brief:  csv file
 *
 * Copyright (c) 2020 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2020-06-08 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_CSV_FILE_H
#define TK_CSV_FILE_H

#include "tkc/types_def.h"

BEGIN_C_DECLS

typedef struct _csv_row_t {
  char* buff;
  uint32_t size : 31;
  uint32_t should_free_buff : 1;
} csv_row_t;

typedef struct _csv_rows_t {
  csv_row_t* rows;
  uint32_t size;
  uint32_t capacity;
} csv_rows_t;

/**
 *@class csv_file_t
 * 操作CSV文件。
 */
typedef struct _csv_file_t {
  /**
   * @property {bool_t} has_title
   * 是否有标题。
   */
  bool_t has_title;
  /*private*/
  char* buff;
  uint32_t size;
  csv_rows_t rows;

  uint32_t cols;
  char sep;

  char* filename;
} csv_file_t;

/**
 * @method csv_file_create
 *
 * 根据文件创建csv对象。
 *
 * @param {const char*} filename 文件名。
 * @param {char} sep 分隔符。
 * 
 * @return {csv_file_t} 返回csv对象。
 */
csv_file_t* csv_file_create(const char* filename, char sep);

/**
 * @method csv_file_reload
 *
 * 丢弃内存中的修改，重新加载文件。
 *
 * @param {csv_file_t*} csv csv对象。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t csv_file_reload(csv_file_t* csv);

/**
 * @method csv_file_create_with_buff
 *
 * 根据buff创建csv对象。
 *
 * @param {const char*} buff 数据。
 * @param {uint32_t} size 数据长度。
 * @param {bool_t} 是否释放buff。
 * @param {char} sep 分隔符。
 * 
 * @return {csv_file_t} 返回csv对象。
 */
csv_file_t* csv_file_create_with_buff(const char* buff, uint32_t size, bool_t should_free,
                                      char sep);

/**
 * @method csv_file_get
 *
 * 获取指定行列的数据。
 *
 * @param {csv_file_t*} csv csv对象。
 * @param {uint32_t} row 行号。
 * @param {uint32_t} col 列号。
 * 
 * @return {const char*} 返回数据。
 */
const char* csv_file_get(csv_file_t* csv, uint32_t row, uint32_t col);

/**
 * @method csv_file_set
 *
 * 修改指定行列的数据。
 *
 * @param {csv_file_t*} csv csv对象。
 * @param {uint32_t} row 行号。
 * @param {uint32_t} col 列号。
 * @param {const char*} value 值。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t csv_file_set(csv_file_t* csv, uint32_t row, uint32_t col, const char* value);

/**
 * @method csv_file_get_title
 *
 * 获取标题(不存在则返回NULL)。
 *
 * @param {csv_file_t*} csv csv对象。
 * 
 * @return {const char*} 返回标题。
 */
const char* csv_file_get_title(csv_file_t* csv);

/**
 * @method csv_file_get_rows
 *
 * 获取行数(包括标题)。
 *
 * @param {csv_file_t*} csv csv对象。
 * 
 * @return {uint32_t} 返回行数。
 */
uint32_t csv_file_get_rows(csv_file_t* csv);

/**
 * @method csv_file_get_cols
 *
 * 获取列数。
 *
 * @param {csv_file_t*} csv csv对象。
 * 
 * @return {uint32_t} 返回列数。
 */
uint32_t csv_file_get_cols(csv_file_t* csv);

/**
 * @method csv_file_remove_row
 *
 * 删除指定行。
 *
 * @param {csv_file_t*} csv csv对象。
 * @param {uint32_t} row 行号。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t csv_file_remove_row(csv_file_t* csv, uint32_t row);

/**
 * @method csv_file_append_row
 *
 * 追加一行。
 *
 * @param {csv_file_t*} csv csv对象。
 * @param {const char*} data 数据。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t csv_file_append_row(csv_file_t* csv, const char* data);

/**
 * @method csv_file_insert_row
 *
 * 插入一行。
 *
 * @param {csv_file_t*} csv csv对象。
 * @param {uint32_t} row 行号。
 * @param {const char*} data 数据。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t csv_file_insert_row(csv_file_t* csv, uint32_t row, const char* data);

/**
 * @method csv_file_save
 *
 * 保存。
 *
 * @param {csv_file_t*} csv csv对象。
 * @param {const char*} filename 文件名。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t csv_file_save(csv_file_t* csv, const char* filename);

/**
 * @method csv_file_destroy
 *
 * 销毁csv对象。
 *
 * @param {csv_file_t*} csv csv对象。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t csv_file_destroy(csv_file_t* csv);

/*public for test*/
const char* csv_file_get_by_name(csv_file_t* csv, uint32_t row, const char* name);
int32_t csv_file_get_col_of_name(csv_file_t* csv, const char* name);
csv_row_t* csv_file_get_row(csv_file_t* csv, uint32_t row);

uint32_t csv_row_count_cols(csv_row_t* row);
const char* csv_row_get(csv_row_t* row, uint32_t col);
ret_t csv_row_set(csv_row_t* row, uint32_t col, const char* value);
ret_t csv_row_init(csv_row_t* row, char* buff, uint32_t size, bool_t should_free_buff);
ret_t csv_row_reset(csv_row_t* row);

ret_t csv_rows_init(csv_rows_t* rows, uint32_t init_capacity);
ret_t csv_rows_remove(csv_rows_t* rows, uint32_t row);
csv_row_t* csv_rows_append(csv_rows_t* rows);
csv_row_t* csv_rows_insert(csv_rows_t* rows, uint32_t row);
csv_row_t* csv_rows_get(csv_rows_t* rows, uint32_t row);
ret_t csv_rows_reset(csv_rows_t* rows);
ret_t csv_row_set_data(csv_row_t* row, const char* data, char sep);

END_C_DECLS

#endif /*TK_CSV_FILE_H*/
