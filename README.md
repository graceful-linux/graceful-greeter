```
            ____                     __       _      ____               _            
           / ___|_ __ __ _  ___ ___ / _|_   _| |    / ___|_ __ ___  ___| |_ ___ _ __ 
          | |  _| '__/ _` |/ __/ _ \ |_| | | | |   | |  _| '__/ _ \/ _ \ __/ _ \ '__|
          | |_| | | | (_| | (_|  __/  _| |_| | |   | |_| | | |  __/  __/ ||  __/ |   
           \____|_|  \__,_|\___\___|_|  \__,_|_|    \____|_|  \___|\___|\__\___|_|   
```
> 一款基于 QT 的登录界面


## 使用说明
### archlinux
1. 安装`graceful-greeter`
```shell
yay -S graceful-greeter
```
> 如果本机**只有一个** 'lightdm-greeter' 直接跳过下面步骤，重新启动系统即可生效
2. 修改 lightdm 配置(/etc/lightdm/lightdm.conf)

```
[Seat:*]
graceful-session=graceful-greeter
```
> 注意：在 `[Seat:*]` 下修改，在 `[LightDM]` 下修改不起作用。
> 或者卸载掉多余的 greeter，只保留`graceful-greeter`也可

### 源码编译
1. 直接拉取源码编译，需要Qt环境 + lightdm 开发包 + 少量qt依赖模块
在源码目录下执行: `mkdir build && cd build && cmake .. && make -j8`
2. 编译完成之后执行 `sudo make install`
3. 配置同 `archlinux` 里第二步

