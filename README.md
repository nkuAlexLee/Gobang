# 五子棋简介 #
> 
五子棋是世界智力运动会竞技项目之一，是一种两人对弈的纯策略型棋类游戏，是世界智力运动会竞技项目之一，通常双方分别使用黑白两色的棋子，下在棋盘直线与横线的交叉点上，先形成5子连线者获胜。

## 一.行棋规则 ##
1. 在双人对弈中，规定黑棋先行
2. 在人机对弈中，人决定执黑棋或白棋
3. 在机器对弈中，人决定黑棋第一步落子<br>
![1.png](https://s2.loli.net/2022/05/17/sG8kNBP1YmZ5hgQ.png)

## 二.胜负判断 ##
最先在棋盘横向、竖向、斜向形成连续的相同色五个棋子的一方为胜。<br>
双方均无禁手。如分不出胜负，则定为平局。

![2.png](https://s2.loli.net/2022/05/17/gvo57KjSwBbWEOI.png)

## 三.代码分析 

### 1. 整体函数分析<br> 
&nbsp; 1）代码主要分为三个类<br>
`Logic` 主要负责处理游戏逻辑和页面信息处理
`Draw` 主要负责游戏页面绘制和刷新
`chessAI` 主要分析页面局势并计算出对电脑有利的点<br>
&nbsp; 2）`main`函数调用绘制页面开始运行
![3.png](https://s2.loli.net/2022/05/17/SkqtreCBD8Oygox.png)
### 2. `Logic`分析

　&emsp; 1）对玩家的点击做出回应

　&emsp;　&emsp;如果玩家点击棋盘则需要判断是否该玩家落子，如果是正常落子并判断输赢；当玩家点击的是右侧工具栏时，做出相应的回应。

　&emsp; 2）玩家落子<br>
　&emsp;　&emsp;玩家落子不需要电脑做出过多的判断，程序中只需要判断玩家是否点击棋盘进行落子、落子位置是否合理、落子后玩家是否会赢等情况即可，其他情况均有玩家做出判断。

　&emsp;3）机器落子

　&emsp;　&emsp;当玩家落子完成后，其根据算法判断落子位置。

  &emsp;&emsp;4）模式切换

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;在人机模式和双人模式以及机器对弈进行切换。

&emsp;&emsp;5）新开一局

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;重置棋盘，重新开始游戏。

&emsp;&emsp;6）悔一步棋

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;在双人模式下回顾一个棋子，在人机模式下会回顾两个棋子（若棋盘上只有一个棋子则只回顾一个）

&emsp;&emsp;7）先后手切换

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;在人机模式下可以切换先后手

&emsp;&emsp;8）提示功能

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;在人机模式下可以让AI代替玩家落子

&emsp;&emsp;9）退出功能

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;点击可退出游戏

&emsp;&emsp;9）音效功能

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;游戏开始播放背景音乐《沧海一声笑》，胜负分出后背景音乐停止，新开游戏后再次播放。

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;落子时播放落子音效，黑白棋落子音效有所不同。

  &nbsp;&nbsp;&nbsp;&emsp;　&emsp;点击非落子区域（包含已有子区域）和非菜单区域播放错误音效，提醒无效操作。
### 3. `Draw`分析

#### &emsp;1）打印菜单栏

&emsp;&emsp;&emsp;&emsp;利用EasyX将提前准备好的菜单图片显示在显示器，用户点击相应功能运行。<br> 
&emsp;&emsp;&emsp;![7.png](https://s2.loli.net/2022/05/17/rVKQqDNZ8lWG25O.png)
![8.png](https://s2.loli.net/2022/05/17/yGkl73eNSv2EDn9.png)

#### &emsp;2）打印棋盘
&emsp;&emsp;&emsp;&emsp;![9.png](https://s2.loli.net/2022/05/17/TOt2gzVPyc3xL6p.png)
#### &emsp;3）打印棋子
&emsp;&emsp;&emsp;&emsp;新落子的棋子有红点提醒<br> 
&emsp;&emsp;&emsp;&emsp;![10.png](https://s2.loli.net/2022/05/17/CJt5efAn2l3Nwyo.png)
#### &emsp;4）打印信息栏
&emsp;&emsp;&emsp;&emsp;开发者信息；当前落子颜色（或者哪方胜利）；步数；最近一次落子位置<br> 
&emsp;&emsp;&emsp;&emsp;![6.png](https://s2.loli.net/2022/05/17/1K7YjVO4kE9sHWc.png)
## 四.安装包 ##
### 1.图标
![11.png](https://s2.loli.net/2022/05/17/nqrSJLyipdWgUfH.png)
### 2.安装文件
![12.png](https://s2.loli.net/2022/05/17/oGBdIt9M2ZEPbqV.png)
### 3.安装程序
![13.png](https://s2.loli.net/2022/05/17/28FEjeTJHBsb5gK.png)![14.png](https://s2.loli.net/2022/05/17/QAbofiR8csqk7LY.png)
## 五.源代码
见附件
