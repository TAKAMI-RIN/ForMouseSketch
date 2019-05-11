#include <Mouse.h>
#include <Keyboard.h>
//Author:TOOSAKA
//20180216
//Ver:1.3.1020.1

int b0,b1,b2,b3,b4,b5,b6,i;                             //初始化必要的变量
long summer;                                            //初始化变量summer，用于校验算法
byte bufferer[32];                                      //初始化串口缓冲数组
byte heb0,heb1,heb2,heb3,heb4,heb5;                     //初始化byte变量，用于串口通信
void setup(){                                           //setup函数，该函数在Arduino初始化时运行一次
  pinMode(2,INPUT_PULLUP);                              //初始化2号脚，用于附属功能按键
  pinMode(3,INPUT_PULLUP);                              //初始化3号脚，用于附属功能按键
  pinMode(8,INPUT);                                     //初始化8号脚，与HC-05的state脚相连，用于读取蓝牙连接状态
  Mouse.begin();                                        //开启HID鼠标模拟功能
  Keyboard.begin();                                     //开启HID键盘模拟功能
  Keyboard.releaseAll();                                //键盘状态初始化
  //Serial.begin(115200);
}

void loop(){
  if(digitalRead(8)==1){                                //如果8号脚发现蓝牙state脚为高电平
    heb3 = 0;                                           //将heb3置为0
    Serial1.begin(115200);                              //与蓝牙相连的串口打开
    Keyboard.releaseAll();                              //键盘状态初始化（即将模拟键盘的状态设为所有键放开）
    while(Serial1.read() >=0){}                         //清空串口1缓存空间（Serial1.read（）函数有此功能）
    while(1){                                               //此时进入不断读取串口数据并响应的状态
      while(Serial1.available()>0){                     //若串口1有值输入
        heb4 = 0;                                       //将heb4置0
        delay(2);                                       //等待2ms，等串口数据包传输完毕
        for(i = 0;i<=26;i++){                           //此for函数用于将串口1缓存空间内收到的数据包传至缓存数组
          bufferer[i] = Serial1.read();
        }
        for(i = 0;i<24;i++){                            //heb4将串口数据包前24位进行累加（第0-第23）
          heb4 += bufferer[i];
        }
        //sum up part                                   //串口数据包结构详见安卓app工程中TsControl.java的TimerTask send_task（205行左右）中
        if(bufferer[24] == heb4%256){                   //若串口数据包中第24位与前24位累加后对256取余后的校验结果相等，则数据包校验通过
        	heb0 = bufferer[5];                         //使用heb0、heb1、heb2分别存储关键数据位（当安卓app中相应按键被按下，关键数据位将变化）
       		heb1 = bufferer[9];
       		heb2 = bufferer[7];
        	if(heb0 == 205){         //A                //当安卓app中按键A被按下，则heb0将会是205，此时模拟按下键盘f键
        	  Keyboard.press(102);    //f               //Keyboard.press即为模拟键盘中模拟按下对应键
        	  Keyboard.release(119);   //w              //Keyboard.release即为模拟键盘中模拟放开对应键
        	  Keyboard.release(115);    // s   
        	  Keyboard.release(32);   //space
        	}
        	else if(heb0 == 215){    //B
        	  Keyboard.release(102);    //f
        	  Keyboard.press(119);   //w
        	  Keyboard.release(115);    // s   
        	  Keyboard.release(32);   //space
        	}
        	else if(heb0 == 235){    //C
        	  Keyboard.release(102);    //f
        	  Keyboard.release(119);   //w
        	  Keyboard.press(115);    // s   
        	  Keyboard.release(32);   //space
        	}
        	else if(heb0 == 245){    //D
        	  Keyboard.release(102);    //f
        	  Keyboard.release(119);   //w
        	  Keyboard.release(115);    // s   
        	  Keyboard.press(32);   //space
        	}
        	else if(heb0 == 225){    //no input         //当安卓app中无按键被按下，则heb0将会是225，放开所有heb0所能表达的所有虚拟键盘按键
        	  Keyboard.release(102);    //f
        	  Keyboard.release(119);   //w
        	  Keyboard.release(115);    // s   
        	  Keyboard.release(32);   //space
        	}

        	if(heb1 == 225){         // no input            //与heb0 同理
        	  Keyboard.release(97);    //a
        	  Keyboard.release(100);   //d
        	}
        	else if(heb1 == 180){    //E
        	  Keyboard.press(97);    //a
        	  Keyboard.release(100);   //d
        	}
        	else if(heb1 == 15){     //F
        	  Keyboard.release(97);    //a
        	  Keyboard.press(100);   //d
        	}

        	if(heb2 == 225){                                 
        	  Keyboard.release(KEY_UP_ARROW);    //up
        	  Keyboard.release(KEY_DOWN_ARROW);   //down	
        	}
        	else if(heb2 == 180 && heb3 != 180){             //在此次数据包中heb2=180，而上一数据包中的heb2（由heb3存储）！=180，即可判断对应控制键在安卓app中刚被按下，此时响应，否则不响应
        	  Keyboard.press(KEY_UP_ARROW);    //up
        	  Keyboard.release(KEY_DOWN_ARROW);   //down	
        	  delay(5);
        	  Keyboard.release(KEY_UP_ARROW);    //up
        	  Keyboard.release(KEY_DOWN_ARROW);   //down  
        	}
        	else if(heb2 == 15 && heb3 != 15){               
        	  Keyboard.release(KEY_UP_ARROW);    //up
        	  Keyboard.press(KEY_DOWN_ARROW);   //down	
        	  delay(5);
        	  Keyboard.release(KEY_UP_ARROW);    //up
        	  Keyboard.release(KEY_DOWN_ARROW);   //down  
        	}
        	heb3 = heb2;                                    //存储这个循环heb2的值
        }
        else{}                                              //与38行if对应，若校验不通过则清除缓存区
        Serial1.flush();
      }

      if(digitalRead(8)==0){                                //若8号脚发现低电平，则代表蓝牙未连接，则跳出不断读取串口数据并响应的状态(27行while1)
        Keyboard.releaseAll();
        break;
      }

    }
  }
  else{                                                      //在蓝牙未连接的情况下：
    if(digitalRead(2)==0 && digitalRead(3)==1){        //front key   //若前键（2号脚上）被按下
      Keyboard.press(KEY_LEFT_CTRL);                                 //按一下ctrl+c，复制命令
      Keyboard.press(99);
      delay(5);
      Keyboard.releaseAll();
      while(digitalRead(2)==0){delay(1);}
    }
    else if(digitalRead(3)==0 && digitalRead(2)==1){      //rear     //若后键(3号脚上)被按下
      Keyboard.press(KEY_LEFT_CTRL);                                 //按一下ctrl+v，粘贴命令
      Keyboard.press(118);
      delay(5);
      Keyboard.releaseAll();
      while(digitalRead(3)==0){delay(1);}
    }
  }
}
