#!/bin/bash

rm -rf fs-check
cp -a fs fs-check
cd fs-check
rm -rf *.php */*.php
rmdir status

echo "step 1"
filelist=`ls *.html */*.html *.js */*/*.xml */*/*/*.xml`
for file in $filelist
do
	echo "handling ${file}"
	mv ${file} ${file}.tmp
	# 行首空白
	# 行尾空白
	# ;后面空白
	# 单行注释
	# 行尾注释
	# 原html单行注释
	# == 两侧空白
	# != 两侧空白
	# || 两侧空白
	# && 两侧空白
	# = 两侧空白
	# ) 两侧空白
	# [ 两侧空白
	# ] 两侧空白
	# + 两侧空白
	# - 两侧空白
	# * 两侧空白
	# % 两侧空白
	# < 两侧空白
	# > 两侧空白
	# , 两侧空白
	# . 两侧空白
	# { 两侧空白
	# } 两侧空白
	# while 两侧空白
	# for 两侧空白
	# if 两侧空白
	# "这里的空白/>
	# '这里的空白/>
	# console.log
	# console.warn
	# console.clear
	# console.debug
	# console.error
	# 原js多行注释
	# 空行
	cat ${file}.tmp | tr -d '\r' | sed -e "s:^[[:space:]]*::g" \
	-e "s:[[:space:]]*$::g" \
	-e "s:;[[:space:]]:;:g" \
	-e "/^[[:space:]]*\/\//d" \
	-e "s:[[:space:]]*//.*$::g" \
	-e "s:<\!--.*-->::g" \
	-e "s:\([^[:space:]]\)[[:space:]]*==[[:space:]]*\([^[:space:]]\):\1==\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*!=[[:space:]]*\([^[:space:]]\):\1!=\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*||[[:space:]]*\([^[:space:]]\):\1||\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*&&[[:space:]]*\([^[:space:]]\):\1\&\&\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*=[[:space:]]*\([^[:space:]]\):\1=\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*)[[:space:]]*\([^[:space:]]\):\1)\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*\[[[:space:]]*\([^[:space:]]\):\1[\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*\][[:space:]]*\([^[:space:]]\):\1]\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*+[[:space:]]*\([^[:space:]]\):\1+\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*-[[:space:]]*\([^[:space:]]\):\1-\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*\*[[:space:]]*\([^[:space:]]\):\1\*\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*%[[:space:]]*\([^[:space:]]\):\1%\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*<[[:space:]]*\([^[:space:]]\):\1<\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*>[[:space:]]*\([^[:space:]]\):\1>\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*,[[:space:]]*\([^[:space:]]\):\1,\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*\.[[:space:]]*\([^[:space:]]\):\1.\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*{[[:space:]]*\([^[:space:]]\):\1{\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*}[[:space:]]*\([^[:space:]]\):\1}\2:g" \
	-e "s:[[:space:]]*while[[:space:]]*\((.*)\)[[:space:]]*{:while\1{:g" \
	-e "s:[[:space:]]*for[[:space:]]*\((.*)\)[[:space:]]*{:for\1{:g" \
	-e "s:else[[:space:]]*if[[:space:]]*:else-javascript-if:g" -e "s:[[:space:]]*if[[:space:]]*:if:g" -e "s:else-javascript-if:else if:g" \
	-e "s:\"[[:space:]]*/>:\"/>:g" \
	-e "s:\'[[:space:]]*/>:\'/>:g" \
	-e "/console\.log/d" -e "/console\.warn/d" -e "/console\.clear/d" -e "/console\.debug/d" -e "/console\.error/d" | sed -e "s:/\*.*\*/::g" > ${file}
	rm -f ${file}.tmp
	sed -i "/^[[:space:]]*$/d" ${file}
done

echo "step 2"
# echo "handling index.htm"
# # 特殊情况特殊处理
# mv index.html index.html.tmp
# cat index.html.tmp | sed -e "s:(var i=0;i<lis\.length\-1;i++):(var i in lis):g" \
# 	-e "s:if(i%2){lis.item(i)\.className=\"\";}:lis.item(i)\.className=\"\";:g" \
# 	-e "s:lis\.item(p\*2+1)\.className=\"active\";:lis.item(p).className=\"active\";:g" > index.html
# rm -f index.html.tmp

echo "handling css"
filelist=`ls *.css`
for file in $filelist
do
	mv ${file} ${file}.tmp
	# 行首空白
	# 行尾空白
	# 单行注释
	# 行尾注释
	# : 两侧空白
	# , 两侧空白
	# { 左侧空白
	# < 两侧空白
	# > 两侧空白
	# 空行
	# 合并为一行
	# 原多行注释
	cat ${file}.tmp | tr -d '\r' | sed -e "s:^[[:space:]]*::g" \
	-e "s:[[:space:]]*$::g" \
	-e "/^[[:space:]]*\/\//d" \
	-e "s:[[:space:]]*//.*$::g" \
	-e "s:\([^[:space:]]\)[[:space:]]*{\(.*\):\1{\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*,[[:space:]]*\([^[:space:]]\):\1,\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*<[[:space:]]*\([^[:space:]]\):\1<\2:g" \
	-e "s:\([^[:space:]]\)[[:space:]]*>[[:space:]]*\([^[:space:]]\):\1>\2:g" \
	-e "s/\([^[:space:]]\)[[:space:]]*:[[:space:]]*\([^[:space:]]\)/\1:\2/g" \
	-e "s:/\*.*\*/::g" > ${file}
	cat ${file} | sed -e "/^[[:space:]]*$/d" > ${file}.tmp
	cat ${file}.tmp | tr -d '\n' | sed -e "s:/\*.*\*/::g" > ${file}
	rm -f ${file}.tmp
done

echo "handling cfg.shtml"
file="dev/cfg.shtml"
mv ${file} ${file}.tmp
# 行首空白
# 行尾空白
# 单行注释
# 行尾注释
# == 两侧空白
# != 两侧空白
# || 两侧空白
# && 两侧空白
# = 两侧空白
# ) 两侧空白
# [ 两侧空白
# ] 两侧空白
# + 两侧空白
# < 两侧空白
# > 两侧空白
# , 两侧空白
# . 两侧空白
# { 两侧空白
# } 两侧空白
# while 两侧空白
# for 两侧空白
# if 两侧空白
# "这里的空白/>
# '这里的空白/>
# console.log
# console.warn
# console.clear
# console.debug
# console.error
# 合并为一行
# 原js多行注释
# 空行
cat ${file}.tmp | tr -d '\r' | sed -e "s:^[[:space:]]*::g" \
-e "s:[[:space:]]*$::g" \
-e "/^[[:space:]]*\/\//d" \
-e "s:[[:space:]]*//.*$::g" \
-e "s:\([^[:space:]]\)[[:space:]]*==[[:space:]]*\([^[:space:]]\):\1==\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*!=[[:space:]]*\([^[:space:]]\):\1!=\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*||[[:space:]]*\([^[:space:]]\):\1||\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*&&[[:space:]]*\([^[:space:]]\):\1\&\&\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*=[[:space:]]*\([^[:space:]]\):\1=\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*)[[:space:]]*\([^[:space:]]\):\1)\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*\[[[:space:]]*\([^[:space:]]\):\1[\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*\][[:space:]]*\([^[:space:]]\):\1]\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*+[[:space:]]*\([^[:space:]]\):\1+\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*<[[:space:]]*\([^[:space:]]\):\1<\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*>[[:space:]]*\([^[:space:]]\):\1>\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*,[[:space:]]*\([^[:space:]]\):\1,\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*\.[[:space:]]*\([^[:space:]]\):\1.\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*{[[:space:]]*\([^[:space:]]\):\1{\2:g" \
-e "s:\([^[:space:]]\)[[:space:]]*}[[:space:]]*\([^[:space:]]\):\1}\2:g" \
-e "s:[[:space:]]*while[[:space:]]*\((.*)\)[[:space:]]*{:while\1{:g" \
-e "s:[[:space:]]*for[[:space:]]*\((.*)\)[[:space:]]*{:for\1{:g" \
-e "s:else[[:space:]]*if[[:space:]]*:else-javascript-if:g" -e "s:[[:space:]]*if[[:space:]]*:if:g" -e "s:else-javascript-if:else if:g" \
-e "s:\"[[:space:]]*/>:\"/>:g" \
-e "s:\'[[:space:]]*/>:\'/>:g" \
-e "/console\.log/d" -e "/console\.warn/d" -e "/console\.clear/d" -e "/console\.debug/d" -e "/console\.error/d" | sed -e "s:/\*.*\*/::g" > ${file}
rm -f ${file}.tmp
sed -i "/^[[:space:]]*$/d" ${file}

echo "handling upgrade.html"
version=`date +"%F %T"`
sed -i "s/___version___/${version}/g" up/upgrade.html

# echo "step 3"
# filelist=`ls *.css *.html */*.html *.js */*/*.xml */*/*/*.xml`
# for file in $filelist
# do
# 	echo "handling ${file}"
# 	gzip -9nv ${file}
# 	mv ${file}.gz ${file}
# done

cd ..

# ./makefsdata.exe fs-build -e

# touch *.c
