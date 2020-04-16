# ProjectAC::VM

## 简介

HustPC3.0版对应的虚拟机。和可以运行在FPGA板上的HustPC3.0具有几乎完全一致的表现。  

## 模块

分为以下主要模块：  

1. CPU  

单核（带Delay Slot虚拟功能的）Mips架构 CPU。可以实例化多个对象来构建多核CPU。  

2. Address Manager

地址译码器。可以根据需求地址确定地址空间对应的设备，并译码到该设备内部。  
可以操控Memory及其所有派生类对象。支持跨设备访问，即对于一个4-Bytes Word，可以有地址连续的1个字节在A设备，3个字节在B设备。  

3. Memory

存储器基类。只有继承Memory类的对象可以被加入Address Manager的地址空间。  
Monitor（VGA显示控制器）和KeyManager（PS/2键盘缓冲区）都继承了此类。  
本基类也可以实例化成为普通的RAM内存，也可以预先加载二进制内容来虚拟ROM。  

4. Monitor

利用OpenGL进行逐像素绘制模拟的的VGA显示控制器。自带可切换的字符/图形模式并且配有对应的显存模块。
可以在main.cpp中设置默认刷新频率（过大会引起卡顿，过小会导致画面不连续，默认1刷新：10000条指令）。

5. KeyManager

利用glut函数模拟键盘按下/抬起事件的PS/2接口键盘缓冲模拟。
当一个键盘码被从队列中读取之后会出队，指针移动到下一个键盘码。
将来会使用interrupt和syscall来监测键盘行为。
