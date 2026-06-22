# my-project

嵌入式系统实验项目

## 目录结构
- `ex01/`：第一个练习程序（Hello World）
- `lab01/`：实验一 — Arduino IDE + ESP32 开发环境配置
- `ex02/`：作业2 — 用millis()控制LED 1Hz闪烁
- `ex03/`：作业3 — 用millis()控制LED产生SOS信号
- `ex03/`：作业3 — PWM

## 实验内容

### 实验一：环境配置
- [x] Arduino IDE 安装
- [x] ESP32 开发板支持包安装
- [x] CH340 USB 驱动安装
- [x] 上传测试程序（LED 闪烁）

### 作业2：1Hz LED闪烁
- [x] 查阅millis()函数
- [x] 实现1Hz稳定闪烁
- [x] 使用millis()替代delay()

### 作业3：SOS信号
- [x] 实现SOS闪烁模式（三短三长三短）
- [x] 每次循环后有长停顿
- [x] 使用millis()实现非阻塞控制

### 作业4：PWM
- [x]PWM 原理： 通过调节占空比（Duty Cycle）控制 LED 平均亮度。
- [x]ESP32 PWM 配置：
          ledcSetup()：配置 PWM 通道参数（频率、分辨率）。
          ledcAttachPin()：绑定引脚到 PWM 通道。
          ledcWrite()：设置通道的占空比值。
- [x] 呼吸灯算法： 通过循环结构（for循环）逐渐递增和递减占空比，配合 delay() 实现平滑过渡。

## 提交记录
- 每个作业3次以上commit
- 包含代码、文档和实验结果
