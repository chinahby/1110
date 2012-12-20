@echo off

rem window默认字符编码GBK,用于解决某些系统字符编码为UTF-8出现乱码
set OUTPUT_CHARSET=GBK

call %1 preobjs
call %1 genobjs -j8
call %1







