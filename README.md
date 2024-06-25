# notepad--

[中文 ](README.md) | [English](README_EN.md)

## 项目简介

这是一个使用C++编写的文本编辑器Notepad--,可以支持Win/Linux/Mac平台。

我们的目标是要发展文本编辑类软件的国产可替代，重点在国产Uos系统、Mac 系统上发展。

一个支持windows/linux/mac的文本编辑器，目标是要国产替换同类软件，来自中国。

对比其它竞品Notepad类软件而言，我们的优势是可以跨平台，支持linux mac操作系统。

 **鉴于某些Notepad竞品作者的不当言论，Notepad--的意义在于：减少一点错误言论，减少一点自以为是。** 

 **Notepad--的目标，致力于国产软件的可替代，专心做软件。** 

 **Notepad--不涉及政治，Ndd作者没有政治立场。请不要在本项目讨论政治，发现一律删帖！** 

您可以在这个项目提交bug或反馈问题。

本仓库与https://gitee.com/cxasm/notepad-- 是同一个软件。
中国内使用https://gitee.com/cxasm/notepad-- 。国外使用本github库，该库会定期同步国内gitee库。
由于作者在中国经常网络无法访问github，所以该库可能不会及时更新，还请见谅。

最新版本下载地址：https://gitee.com/cxasm/notepad--/releases/tag/v2.17

由于github访问经常断网，几乎没法提交代码。国内用户请访问 https://gitee.com/cxasm/notepad--
NDD 支持插件编写功能，希望广大的CPP/QT开发者加入我们，插件功能均可以留上您的大名和捐赠渠道，希望
开发者参与插件功能开发。

**鉴于Notepad--遭到反动竞品同类软件的匿名发帖诋毁，ndd项目申明：** 

**Notepad--不涉及任何政治，更不会随意发布政治言论。** 

**最后强烈建议全体海内外同胞，务必禁用发表错误言论的软件，您可以选择用其它同类软件进行替代，比如vscode,notepad3,sublime都行。大家用实际行动拒绝错误言论软件, 不去评论它，不去关注它，不给它带来任何流量，则发布错误言论的人，终将自讨没趣而闭嘴。** 

 **做中国人自己的免费编辑器，离不开您的支持，请通过微信捐赠我们。**

![6688](https://user-images.githubusercontent.com/42246867/202892430-e4738634-4e37-4c4a-9120-a1665af41eb4.png)

## 联络方式

QQ群：213027225(新开) 用户群，做NDD的问题反馈、功能建议等。

## 编译

**CMake工具链编译说明:**

- Ubuntu/Debian

1. 安装编译环境 `sudo apt-get install g++ make cmake`
1. 安装qt工具和库 `sudo apt-get install qtbase5-dev qt5-qmake qtbase5-dev-tools libqt5printsupport5 libqt5xmlpatterns5-dev `
1. 配置 `cmake -B build -DCMAKE_BUILD_TYPE=Release`
1. 编译 `cd build && make -j` 
1. 打包 `cpack`

- ArchLinux

1. 安装编译环境 `sudo pacman -S gcc cmake make ninja`
1. 安装 qt 工具和库 `sudo pacman -S qt5-tools qt5-base qt5-xmlpatterns`
1. 配置 `cmake -S . -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release  -DCMAKE_INSTALL_PREFIX=/usr -W no-dev`
1. 编译 `ninja -C build && ninja -C build install`
1. 打包: 使用 [AUR/notepad---git](https://aur.archlinux.org/packages/notepad---git) `yay -S notepad---git`
1. 安装：
    - 预编译包添加 [ArchLinuxCN/notepad---git](https://github.com/archlinuxcn/repo) 镜像 `yay -S archlinuxcn/notepad---git`
    - 预编译包 [Debuginfod/notepad---git-debug](https://wiki.archlinux.org/title/Debuginfod) 包 `yay -S archlinuxcn/notepad---git-debug`

- openSUSE Tumbleweed

1. 安装编译环境和Qt工具库 `sudo zypper in -t pattern devel_C_C++ devel_basis devel_qt5 `
1. 配置 `cmake -B build -DCMAKE_BUILD_TYPE=Release `
1. 编译 `cd build && make -j `
1. 打包使用 OBS (Open Build Service)


最新版本下载地址：https://gitee.com/cxasm/notepad--/releases/tag/v2.15


**Qt工程编译说明：** 

1）使用qtcreator 或 vs  先打开qscint/src/qscintilla.pro 。先编译出这个qscintlla的依赖库。

2）再打开RealCompare.pro 加载后编译。

3）由于编译的平台较多，涉及windows/linux/mac，有任何编译问题，还请加qq群 959439826 。欢迎广大网友实现新功能后提交代码给我们。

代码上线不久，删除了商业的对比功能和注册功能（这部分有商业原因，请理解），除此以外，所有功能全部保留。

4）Arch Linux 及其衍生版可以通过 AUR 仓库安装：[notepad---git](https://aur.archlinux.org/packages/notepad---git)
```
yay -S notepad---git
```

ndd在深度国产uos运行效果：
![ntscreenshot_20230805_134025](https://github.com/cxasm/notepad--/assets/42246867/ec0b5243-3c9d-46d0-bba6-348617236829)


ndd在windows运行效果：
![ndd在windows](https://user-images.githubusercontent.com/42246867/235033753-058ef39a-f815-4404-b5d1-de4a834dec1d.png)

![ndd 在windows](https://user-images.githubusercontent.com/42246867/218263319-3e78b0a9-9fb1-45d1-8f1c-2e850e073425.png)

ndd 在macOs运行效果：

![ndd 在 mac Os](https://user-images.githubusercontent.com/42246867/221394341-743f71a9-11bd-4dc8-aa0d-c24316493dcf.png)

![ndd 在mac OS 代码对比](https://user-images.githubusercontent.com/42246867/221394356-89cf463c-8dcf-4697-aedd-33700b947653.png)

subtwo在redhat运行效果：

![subtwo 在redhat 运行效果](https://user-images.githubusercontent.com/42246867/235033326-b2fffca5-625c-4c96-a661-1072f8a8b3ee.png)
