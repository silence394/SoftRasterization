# SoftRender
这是一个简易的可编程的软件渲染器。重点实现了软件渲染相关的部分，没有对应用层做过多的封装。
接口模仿了DX11，清晰明了，可以编写shader做想要的效果。

## 效果图
正常效果：

![img](https://i.loli.net/2018/05/31/5b0f65b77bfd8.bmp)

背面拣选：

![img](https://i.loli.net/2018/05/31/5b0f65b7af5f1.bmp)

线框模式:

![img](https://i.loli.net/2018/05/31/5b0f65b784b81.bmp)

## 技术点
- 创建窗口，渲染设备并显示颜色
- 矩阵的位移、旋转、缩放变换
- 摄像机矩阵变换，摄像机控制
- 投影矩阵变换
- 拣选模式，支持全拣选，正面拣选，背面拣选
- 近裁剪面裁剪
- 视口变换
- 视口裁剪
- 线框模式，Bresenham画线算法
- 扫描线光栅化，左上填充规则
- 透视矫正
- 深度测试
- 自定义shader
- 材质
- Blinn-Phong 光照模型

## TODO
- Rasterization on Larrabee
- 多线程渲染
- Shader编译