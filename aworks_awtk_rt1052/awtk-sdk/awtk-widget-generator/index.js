const fs = require('fs')
const fse = require('fs-extra')
const path = require('path')

class CodeGen {
  genIncludes(name) {
    let result = `#include "tkc/mem.h"\n`;
    result += `#include "tkc/utils.h"\n`;
    result += `#include "${name}.h"`;

    return result;
  }

  saveResult(json, header, content, relativePath) {
    const foldName = this.getFolderName(json);
    const target = this.getTargetRoot(foldName);
    const filenamePrefix = path.join(target, relativePath ? relativePath : `src/${json.name}/${json.name}`);
    const headerFileName = `${filenamePrefix}.h`
    const contentFileName = `${filenamePrefix}.c`

    fs.writeFileSync(headerFileName, '\ufeff' + header);
    fs.writeFileSync(contentFileName, '\ufeff' + content);

    console.log(`output to ${headerFileName} and ${contentFileName}`);
  }

  genSourceCode(json) {
    this.saveResult(json, this.genHeader(json), this.genContent(json));
    this.saveResult(json, this.genRegisterHeader(json), this.genRegisterContent(json),`src/${json.name}_register`);
  }

  getFolderName(json) {
    const name = json.name.replace(/_/g, '-');

    return name;
  }

  mkdirIfNotExist(name) {
    if (!fs.existsSync(name)) {
      fs.mkdirSync(name, {recursive : true});
    }
  }

  createFolders(json) {
    const name = json.name;
    const foldName = this.getFolderName(json);
    const target = this.getTargetRoot(foldName);

    this.mkdirIfNotExist(target);
    this.mkdirIfNotExist(path.join(target, 'bin'));
    this.mkdirIfNotExist(path.join(target, 'src'));
    this.mkdirIfNotExist(path.join(target, 'src', name));
    this.mkdirIfNotExist(path.join(target, 'tests'));
    this.mkdirIfNotExist(path.join(target, 'idl'));
    this.mkdirIfNotExist(path.join(target, 'scripts'));
    this.mkdirIfNotExist(path.join(target, 'demos'));
    this.mkdirIfNotExist(path.join(target, 'docs'));
  }

  fileReplaceContent(src, dst, items) {
    let filename = dst;
    let content = fs.readFileSync(src).toString();
    items.forEach(iter => {
      const searchRegExp = new RegExp(iter.from, 'g');
      content = content.replace(searchRegExp, iter.to);
    })

    console.log(`write file ${filename}`);
    fs.writeFileSync(filename, content);
  }

  getTargetRoot(foldName) {
    if (this.outputPath) {
      return path.join(this.outputPath, 'awtk-widget-' + foldName);
    }
    return path.join('..', 'awtk-widget-' + foldName);
  }

  createFiles(json) {
    const name = json.name;
    const foldName = this.getFolderName(json);
    const target = this.getTargetRoot(foldName);

    const items = [{
      from: "template",
      to: name
    }, {
      from: "template_folder",
      to: foldName
    }];

    const files = ['SConstruct',
      'project.json',
      'gen.sh',
      'README.md',
      'format.sh',
      '.clang-format',
      '.gitignore',
      'bin/README.md',
      'src/SConscript',
      'demos/SConscript',
      'demos/window_main.c',
      'demos/app_main.c',
      'scripts/README.md',
      'scripts/__init__.py',
      'scripts/app_helper.py',
      'scripts/awtk_locator.py',
      'scripts/update_res.py',
      'tests/SConscript',
      'tests/main.cc',
      'tests/template_test.cc',
      'design/default/ui/main.xml',
      'design/default/styles/main.xml'
    ];

    const folders = ['design']

    folders.forEach(iter => {
      const from = path.join('template', iter);
      const to = path.join(target, iter);
      fse.copySync(from, to);
      console.log(`copy ${from} => ${to}`);
    })

    files.forEach(iter => {
      const searchRegExp = new RegExp('template', 'g');
      const from = path.join('template', iter);
      const to = path.join(target, iter.replace(searchRegExp, name));
      this.fileReplaceContent(from, to, items);
    })

    if (files.indexOf('project.json') >= 0) {
      const now = new Date();
      const day = now.getDate();
      const year = now.getFullYear();
      const month = now.getMonth() + 1;
      const date = json.date || `${year}-${month}-${day}`
      const team = json.team;
      const author = json.author;
      const desc = json.desc || "";
      const copyright = json.copyright;
      const version = json.version || "1.0.0";
      const creatorVerison = (json.creator && json.creator.version) ? json.creator.version : "0.1.5";
      const items = [{
        from: "\"version\": \"1.0.0\",",
        to: `\"version\": \"${version}\",`
      }, {
        from: "\"date\": \"\",",
        to: `\"date\": \"${date}\",`
      }, {
        from: "\"team\": \"\",",
        to: `\"team\": \"${team}\",`
      }, {
        from: "\"author\": \"\",",
        to: `\"author\": \"${author}\",`
      }, {
        from: "\"desc\": \"\",",
        to: `\"desc\": \"${desc}\",`
      }, {
        from: "\"copyright\": \"\",",
        to: `\"copyright\": \"${copyright}\",`
      }, {
        from: "\"name\": \"AWTK Designer\",\n    \"version\": \"0.1.5\"",
        to: `\"name\": \"AWTK Designer\",\n    \"version\": \"${creatorVerison}\"`
      }];
      const from = path.join(target, 'project.json');
      const to = path.join(target, 'project.json');

      this.fileReplaceContent(from, to, items);
    }

    if (this.awtkPath) {
      const files = ['scripts/awtk_locator.py'];
      const items = [{
        from: '\'awtk\'',
        to: '\'' + path.basename(this.awtkPath) + '\''
      },{
        from: '    awtk_root = \'\\.\\./\' \\+ awtk',
        to: '    awtk_root = \'' + path.dirname(this.awtkPath) + '/\' + awtk'
      }];

      files.forEach(iter => {
        const from = path.join(target, iter);
        const to = path.join(target, iter);
        this.fileReplaceContent(from, to, items);
      })
    }
  }

  genProject(json) {
    this.createFolders(json);
    this.createFiles(json);
    this.genSourceCode(json);
  }


  genJson(json) {
    this.genProject(json);
  }

  genPropDecls(json) {
    let result = '';
    if (!json.props || !json.props.length) {
      return result;
    }

    result = json.props.map(iter => {
      let propDecl = `
  /**
   * @property {${iter.type}} ${iter.name}
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * ${iter.desc}。
   */
  ${iter.type} ${iter.name};
`
      return propDecl;
    }).join('');

    return result;
  }

  genPropSetterDecls(json) {
    let result = '';
    if (!json.props || !json.props.length) {
      return result;
    }

    const className = json.name;
    result = json.props.map(iter => {
      return `
/**
 * @method ${className}_set_${iter.name}
 * 设置 ${iter.desc}。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget widget对象。
 * @param {${this.mapType(iter.type)}} ${iter.name} ${iter.desc}。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t ${className}_set_${iter.name}(widget_t* widget, ${this.genParamDecl(iter.type, iter.name)});
`
    }).join('');

    return result;
  }

  genPropDefines(json) {
    let result = '';
    if (!json.props || !json.props.length) {
      return result;
    }

    const uclassName = json.name.toUpperCase();

    result = json.props.map(iter => {
      const uPropName = iter.name.toUpperCase();
      return `#define ${uclassName}_PROP_${uPropName} "${iter.name}"`
    }).join('\n');

    return result;
  }

  genHeader(json) {
    const desc = json.desc || "";
    const className = json.name;
    const escapeClassName = className.replace(/_/g, '\\_');
    const uclassName = className.toUpperCase();
    const propDecls = this.genPropDecls(json);
    const propSetterDecls = this.genPropSetterDecls(json);
    const propDefines = this.genPropDefines(json);

    let result = `${this.genFileComment(json, 'h')}

#ifndef TK_${uclassName}_H
#define TK_${uclassName}_H

#include "base/widget.h"

BEGIN_C_DECLS
/**
 * @class ${className}_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * ${desc}
 * 在xml中使用"${escapeClassName}"标签创建控件。如：
 *
 * \`\`\`xml
 * <!-- ui -->
 * <${className} x="c" y="50" w="100" h="100"/>
 * \`\`\`
 *
 * 可用通过style来设置控件的显示风格，如字体的大小和颜色等等。如：
 * 
 * \`\`\`xml
 * <!-- style -->
 * <${className}>
 *   <style name="default" font_size="32">
 *     <normal text_color="black" />
 *   </style>
 * </${className}>
 * \`\`\`
 */
typedef struct _${className}_t {
  widget_t widget;

${propDecls}
} ${className}_t;

/**
 * @method ${className}_create
 * @annotation ["constructor", "scriptable"]
 * 创建${className}对象
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} ${className}对象。
 */
widget_t* ${className}_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method ${className}_cast
 * 转换为${className}对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget ${className}对象。
 *
 * @return {widget_t*} ${className}对象。
 */
widget_t* ${className}_cast(widget_t* widget);

${propSetterDecls}

${propDefines}

#define WIDGET_TYPE_${uclassName} "${className}"

#define ${uclassName}(widget) ((${className}_t*)(${className}_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(${className});

END_C_DECLS

#endif /*TK_${uclassName}_H*/
`

    return result;
  }

  genFileComment(json, ext, name) {
    const now = new Date();
    const team = json.team;
    const author = json.author;
    const className = name ? name : json.name;
    const desc = json.desc || "";
    const copyright = json.copyright;

    const day = now.getDate();
    const year = now.getFullYear();
    const month = now.getMonth() + 1;
    const date = json.date || `${year}-${month}-${day}`

    return `/**
 * File:   ${className}.${ext}
 * Author: ${team}
 * Brief:  ${desc}
 *
 * Copyright (c) ${year} - ${year} ${copyright}
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
 * ${date} ${author} created
 *
 */
`
  }

  genAssignValue(type, name, value) {
    if (type.indexOf("char*") >= 0) {
      return `${name} = tk_str_copy(${name}, ${value});`;
    } else {
      return `${name} = ${value};`;
    }
  }

  mapType(type) {
    if (type.indexOf("char*") >= 0) {
      return `const char*`;
    } else {
      return type;
    }
  }

  genParamDecl(type, name) {
    return `${this.mapType(type)} ${name}`;
  }

  genPropSetterImpls(json) {
    let result = '';
    if (!json.props || !json.props.length) {
      return result;
    }

    const className = json.name;
    const uclassName = className.toUpperCase();
    result = json.props.map(iter => {
      let type = `${iter.type}`;
      const paramDecl = this.genParamDecl(iter.type, iter.name);
      const assignValue = this.genAssignValue(iter.type, `${className}->${iter.name}`, iter.name);

      let propSetter = `
ret_t ${className}_set_${iter.name}(widget_t* widget, ${paramDecl}) {
  ${className}_t* ${className} = ${uclassName}(widget);
  return_value_if_fail(${className} != NULL, RET_BAD_PARAMS);

  ${assignValue}

  return RET_OK;
}
`
      return propSetter;
    }).join('');

    return result;
  }

  genPropList(json) {
    let result = '';
    if (!json.props || !json.props.length) {
      return result;
    }

    const uclassName = json.name.toUpperCase();

    result = json.props.map(iter => {
      const uPropName = iter.name.toUpperCase();
      return `  ${uclassName}_PROP_${uPropName}`
    }).join(',\n');

    return result;
  }

  genToValue(clsName, type, name) {
    switch (type) {
      case 'int8_t':
      case 'int16_t':
      case 'int32_t':
      case 'int64_t':
      case 'uint8_t':
      case 'uint16_t':
      case 'uint32_t':
      case 'uint64_t':
      case 'bool_t':
      case 'float_t':
      case 'float':
      case 'double': {
        let typeName = type.replace(/_t$/, '');
        return `value_set_${typeName}(v, ${clsName}->${name});`;
      }
      case 'char*': {
        return `value_set_str(v, ${clsName}->${name});`;
      }
      case 'void*': {
        return `value_set_pointer(v, ${clsName}->${name});`;
      }
      default: {
        console.log(`not supported ${type} for ${name}`);
        process.exit(0);
      }
    }
  }

  genFromValue(clsName, type, name) {
    let typeName = '';
    switch (type) {
      case 'int8_t':
      case 'int16_t':
      case 'int32_t':
      case 'int64_t':
      case 'uint8_t':
      case 'uint16_t':
      case 'uint32_t':
      case 'uint64_t':
      case 'bool_t':
      case 'float_t':
      case 'float':
      case 'double': {
        typeName = type.replace(/_t$/, '');
        break;
      }
      case 'char*': {
        typeName = 'str';
        break;
      }
      case 'void*': {
        typeName = 'pointer';
        break;
      }
      default: {
        console.log(`not supported ${type} for ${name}`);
        process.exit(0);
      }
    }
    return `${clsName}_set_${name}(widget, value_${typeName}(v));`;
  }

  genSetPropDispatch(json) {
    let result = '';
    if (!json.props || !json.props.length) {
      return result;
    }

    const className = json.name;
    const uclassName = className.toUpperCase();

    result = json.props.map((iter, index) => {
      const uPropName = iter.name.toUpperCase();
      const propMacro = `${uclassName}_PROP_${uPropName}`
      let setProp = '';
      if (index) {
        setProp = `\n  } else if (tk_str_eq(${propMacro}, name)) {\n`
      } else {
        setProp = `  if (tk_str_eq(${propMacro}, name)) {\n`
      }
      setProp += `     ${this.genFromValue(className, iter.type, iter.name)}\n`;
      setProp += '     return RET_OK;';
      return setProp;
    }).join('');

    result += '\n  }';

    return result;
  }

  genPropInits(json) {
    let result = '';
    if (!json.props || !json.props.length) {
      return result;
    }

    const className = json.name;
    result = json.props.map((iter) => {
      if (iter.init_value) {
        if (iter.type === 'char*') {
          return `  ${className}->${iter.name} = tk_strdup("${iter.init_value}");\n`;
        } else {
          return `  ${className}->${iter.name} = ${iter.init_value};\n`;
        }
      }
    }).join('');

    return result;
  }

  genFreeProps(json) {
    let result = '';
    const className = json.name;
    if (!json.props || !json.props.length) {
      return result;
    }

    result = json.props.map((iter, index) => {
      if (iter.type.indexOf('char*') >= 0) {
        return `  TKMEM_FREE(${className}->${iter.name});\n`;
      } else {
        return '';
      }
    }).join('');

    return result;
  }

  genGetPropDispatch(json) {
    let result = '';
    if (!json.props || !json.props.length) {
      return result;
    }

    const className = json.name;
    const uclassName = className.toUpperCase();

    result = json.props.map((iter, index) => {
      const uPropName = iter.name.toUpperCase();
      const propMacro = `${uclassName}_PROP_${uPropName}`
      let setProp = '';
      if (index) {
        setProp = `\n  } else if (tk_str_eq(${propMacro}, name)) {\n`
      } else {
        setProp = `  if (tk_str_eq(${propMacro}, name)) {\n`
      }
      setProp += `     ${this.genToValue(className, iter.type, iter.name)}\n`;
      setProp += '     return RET_OK;';
      return setProp;
    }).join('');

    result += '\n  }';

    return result;
  }

  genContent(json) {
    const className = json.name;
    const uclassName = className.toUpperCase();
    const propList = this.genPropList(json);
    const propInits = this.genPropInits(json);
    const propSetterImpls = this.genPropSetterImpls(json);
    const propSetPropDispatch = this.genSetPropDispatch(json);
    const propGetPropDispatch = this.genGetPropDispatch(json);
    const freeProps = this.genFreeProps(json);
    let defaultInclude = this.genIncludes(className);

    if (json.includes) {
      defaultInclude += json.includes.map(iter => {
        return `#include "${iter}"`
      }).join('');
    }

    let result = `${this.genFileComment(json, 'c')}

${defaultInclude}
${propSetterImpls}

static ret_t ${className}_get_prop(widget_t* widget, const char* name, value_t* v) {
  ${className}_t* ${className} = ${uclassName}(widget);
  return_value_if_fail(${className} != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

${propGetPropDispatch}

  return RET_NOT_FOUND;
}

static ret_t ${className}_set_prop(widget_t* widget, const char* name, const value_t* v) {
  ${className}_t* ${className} = ${uclassName}(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

${propSetPropDispatch}

  return RET_NOT_FOUND;
}

static ret_t ${className}_on_destroy(widget_t* widget) {
  ${className}_t* ${className} = ${uclassName}(widget);
  return_value_if_fail(widget != NULL && ${className} != NULL, RET_BAD_PARAMS);

${freeProps}

  return RET_OK;
}

static ret_t ${className}_on_paint_self(widget_t* widget, canvas_t* c) {
  ${className}_t* ${className} = ${uclassName}(widget);

  (void)${className};

  return RET_OK;
}

static ret_t ${className}_on_event(widget_t* widget, event_t* e) {
  ${className}_t* ${className} = ${uclassName}(widget);
  return_value_if_fail(widget != NULL && ${className} != NULL, RET_BAD_PARAMS);

  (void)${className};

  return RET_OK;
}

const char* s_${className}_properties[] = {${propList.length <= 0 ? '': ('\n' + propList + ',')}
  NULL
};

TK_DECL_VTABLE(${className}) = {.size = sizeof(${className}_t),
                          .type = WIDGET_TYPE_${uclassName},
                          .clone_properties = s_${className}_properties,
                          .persistent_properties = s_${className}_properties,
                          .parent = TK_PARENT_VTABLE(widget),
                          .create = ${className}_create,
                          .on_paint_self = ${className}_on_paint_self,
                          .set_prop = ${className}_set_prop,
                          .get_prop = ${className}_get_prop,
                          .on_event = ${className}_on_event,
                          .on_destroy = ${className}_on_destroy};

widget_t* ${className}_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(${className}), x, y, w, h);
  ${className}_t* ${className} = ${uclassName}(widget);
  return_value_if_fail(${className} != NULL, NULL);

${propInits}

  return widget;
}

widget_t* ${className}_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, ${className}), NULL);

  return widget;
}
`
    return result;
  }

  genRegisterHeader(json) {
    const desc = json.desc || "";
    const className = json.name;
    const uclassName = className.toUpperCase();

    let result = `${this.genFileComment(json, 'h', `${className}_register`)}

#ifndef TK_${uclassName}_REGISTER_H
#define TK_${uclassName}_REGISTER_H

#include "base/widget.h"

BEGIN_C_DECLS

/**
 * @method  ${className}_register
 * 注册控件。
 *
 * @annotation ["global"]
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t ${className}_register(void);

/**
 * @method  ${className}_supported_render_mode
 * 获取支持的渲染模式。
 *
 * @annotation ["global"]
 *
 * @return {const char*} 返回渲染模式。
 */
const char* ${className}_supported_render_mode(void);

END_C_DECLS

#endif /*TK_${uclassName}_REGISTER_H*/
`

    return result;
  }

  genRegisterContent(json) {
    const className = json.name;
    const uclassName = className.toUpperCase();
    let defaultInclude = this.genIncludes(`${className}_register`);

    if (json.includes) {
      defaultInclude += json.includes.map(iter => {
        return `#include "${iter}"`
      }).join('');
    }

    let result = `${this.genFileComment(json, 'c')}

${defaultInclude}
#include "base/widget_factory.h"
#include "${className}/${className}.h"

ret_t ${className}_register(void) {
  return widget_factory_register(widget_factory(), WIDGET_TYPE_${uclassName}, ${className}_create);
}

const char* ${className}_supported_render_mode(void) {
  return "OpenGL|AGGE-BGR565|AGGE-BGRA8888|AGGE-MONO";
}
`
    return result;
  }

  genFile(filename) {
    this.genJson(JSON.parse(fs.readFileSync(filename).toString()));
  }

  static run(sourceIDL, outputPath, awtkPath) {
    const gen = new CodeGen();
    gen.outputPath = outputPath;
    gen.awtkPath = awtkPath;
    gen.genFile(sourceIDL);
  }
}

if (process.argv.length < 3) {
  console.log(`Usage: node index.js widget_desc.json`);
  process.exit(0);
}

CodeGen.run(process.argv[2], process.argv[3], process.argv[4]);
