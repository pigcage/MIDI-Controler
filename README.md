# MIDI-Controler

一个简易的midi控制器，使用arduino pro mirco，按键使用机械键盘红轴按键。引脚连接等见代码

不要使用Nano之类的328P芯片开发板！首先无法作为MIDI设备直插，其次无法支持MIDIUSB之类的库，使用Serial.write()的话受限于USB协议的关系无法保证演奏的音符被第一时间送达。可使用各类32U4的芯片
