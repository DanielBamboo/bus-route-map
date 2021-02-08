# Bus Route Map

这是一个qt新手项目。我在这个项目中倾注了很多的心血，它也是我第一个相对完整的软件作品，在发布到GitHub之前我对它进行了一些调整，但代码可能还是很难阅读，我想把它分享出来，希望对有些朋友能有所启发

## 外观

[\[预览gif\]](https://ftp.bmp.ovh/imgs/2021/02/da5f8dee730006bb.gif)

## 环境
在gbk编码的环境下运行

| 名称 | 版本 |
| -- | -- |
| system | Windows 10(home) |
| Qt |  5.14.1 |
| Qt Creator |  4.11.0 |

## 特点

* 模糊拼音/汉字搜索 [\[预览gif\]](https://gitee.com/DanielBamboo/images/raw/master/20210208123343.gif)
* 一张可以放大缩小的地图
* 对数据文件的加密(route.txt)
* 公交路线计算算法（基于Dijkstra最短路径）

## 感谢

我的这个项目受到到这位朋友在GitHub上分享的这个优秀的项目的启发--[BaiJiazm/SubwayTransferSystem](https://github.com/BaiJiazm/SubwayTransferSystem)

谢谢豆子写的这份精彩的Qt入门指南 -- [qt学习之路](https://www.devbean.net/2012/08/qt-study-road-2-catelog/)

这个程序的核心算法来自于一篇论文《最优公交线路选择问题的数学模型及算法》，作者是：周文峰, 李珍萍, 刘洪伟, 王吉光，算法的描述很精彩也让我收获很多。

感谢所有在网络上无私贡献的博主们