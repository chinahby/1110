使用须知; 本文件为了解决应用的资源文件重复引发的编译问题。
相关文件  res_rename.sh

使用说明 
1. 将res_rename.sh和你要更改的文件夹放同一级目录
2. 文件夹dos路径， ../res_rename.sh执行后，所有的文件名字除了mod等几个文件，其他将会该给为$(目录名)_$(文件名)
3. staticbrewapp.min 里面这样写  
CAH006_EFS_FILES = \
    $(notdir $(wildcard $(CAH006_DIRS)/*.xml)) \
    $(notdir $(wildcard $(CAH006_DIRS)/*.html)) \
    $(notdir $(wildcard $(CAH006_DIRS)/*.fwm))
$(foreach filename, $(CAH006_EFS_FILES), $(filename)|fs:/mod/cah006/$(subst cah006_,,$(filename))) 



如有疑问请联系eric0593@gmail.com