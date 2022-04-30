#!/bin/bash

# 安装 Xephyr 用于调试

# shellcheck disable=SC2046
workDir=$(dirname $(dirname $(realpath -- $0)))
cmd=$(which Xephyr 2>/dev/null)
wm=$(which mutter 2>/dev/null)
lightDM=$(which lightdm 2>/dev/null)

if [[ "x${cmd}" == "x" ]];
then
  echo "请安装 Xephyr 命令用于调试!"
  exit 1
fi

# 构建
buildDir="${workDir}/cmake-build-debug/"
[[ -f ${buildDir}/app/greeter/graceful-greeter ]] && rm -f ${buildDir}/app/greeter/graceful-greeter
[[ ! -d ${buildDir} ]] && mkdir -p ${workDir}

cd ${buildDir} && cmake .. && make -j 8 && sudo make install


# 运行 Xephyr
killall Xephyr
[[ -e ${buildDir}/app/greeter/graceful-greeter ]] && nohup ${cmd} -br -ac -r -resizeable -core -screen 800x600 :6&

# 窗口管理器
[[ "x" != "x${wm}" && -f ${wm} && -e ${wm} ]] && DISPLAY=:6 nohup ${wm} &


#lightdmDir="${buildDir}/lightDM"
#[[ ! -d "${lightdmDir}" ]] && mkdir -p "${lightdmDir}"
#[[ ! -d "${lightdmDir}/log" ]] && mkdir -p "${lightdmDir}/log"
#[[ ! -d "${lightdmDir}/run" ]] && mkdir -p "${lightdmDir}/run"
#[[ ! -d "${lightdmDir}/cache" ]] && mkdir -p "${lightdmDir}/cache"

#${lightDM} --show-config

#DISPLAY=:6 ${lightDM} --test-mode -d \
#    -c "${workDir}/scripts/graceful-greeter-debug.desktop" \
#    --log-dir="${lightdmDir}/log"           \
#    --run-dir="${lightdmDir}/run"           \
#    --cache-dir="${lightdmDir}/cache" 
    
# 运行 greeter
[[ -f ${wm} && -e ${buildDir}/app/greeter/graceful-greeter ]] && DISPLAY=:6 ${buildDir}/app/greeter/graceful-greeter
