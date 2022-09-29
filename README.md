# notepad--
一个支持windows/linux/mac的文本编辑器，目标是要替换notepad++，来自中国。

对比Notepad++而言，我们的优势是可以跨平台，支持linux mac操作系统。

<h4>鉴于Notepad++作者的错误言论，Notepad--的意义在于：减少一点错误言论，减少一点自以为是。</h4>

<h4>严正声明，台湾是中国的一部分。</h4>

当前最新版预览版本：https://github.com/cxasm/notepad--/releases/tag/notepad-v1.16

当前最新发布版本：https://github.com/cxasm/notepad--/releases/tag/notepad-v1.15

国内用户访问gitee https://gitee.com/cxasm/notepad--

<h3>说明1：windows下关联文件右键菜单“以Notepad--打开”的操作。</h3>

对于非安装版本，现在是个单文件，可以随意存放位置。关联文件右键打开菜单的操作如下：

1）你只需要把 Notepad--放在一个固定位置，然后不要移动程序位置。
2）找到notepad--的位置，右键，以管理员权限运行，运行一次后，就会把当前右键位置注册到右键菜单中区。

![image](https://user-images.githubusercontent.com/42246867/188557489-bc033327-3ca9-4ede-a5b8-f9a36005c13b.png)

只需要这样执行一次后，windows下就自动关联所有文件了。
如果你移动了notepad--位置，都以管理员权限运行一次即可。

<h3>说明2：打开yaml后缀类型文件时显示看起来乱码，是以16进制打开的，怎么办？</h3>

这是因为Notepad--检测到该文件可能是二进制，以二进制只读打开，避免修改二进制文件而破坏文件。
两个解决方式：
1）右键文件标签，选择“重新以文本模式”打开。可以解决。
2）永远解决办法：在选项-文件关联-中，把后缀yaml的自定义添加到关联中去。下次遇到yaml的文件，软件会自动识别为文本模式。


支持mac m1 x64系统：
![2022-09-04 16 02 04](https://user-images.githubusercontent.com/42246867/188304259-5ef01164-c1c5-44a1-826e-4e657f3293d7.png)

支持皮肤切换：
![ntscreenshot_20220808_141458](https://user-images.githubusercontent.com/42246867/183382151-32237f08-417f-4c4c-8601-89831ee970be.png)

![20220802](https://user-images.githubusercontent.com/42246867/183382198-64516dc7-cf0a-461a-8be6-08540f36f02d.png)
