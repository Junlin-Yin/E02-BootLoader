# E02-BootLoader
Implement a bootloader for STM32F103C8

**0.preparation**

Step:

1). In your app project (which will be loaded in "load" phase below), adjust "Options for Target" like this:
![Options for Target in App project](http://gwjyhs.com/t6/702/1556379097x2890149745.jpg "Options for Target in App project")

2). Generate .bin file of this project. Reference website: https://blog.csdn.net/u014563989/article/details/51127519

**1.load**

Step:

1). Open Secure CRT and serial port;

2). Press the reset button on 103 board;

3). In command line, input "load";

4). In address line, input proper address. Following addresses are recommmanded: [0x20000800, 0x20003000);

5). Menu 传输->发送Ymodom，choose the bin file you want to load, and then CRT will automatically send it to 103 board.
![load in SecureCRT](http://gwjyhs.com/t6/702/1556378401x2890149745.jpg "load in SecureCRT")

**2.run**

Step:

1). In command line, input "run";

2). In address line, input the address where you load the .bin file.
![run in SecureCRT](http://gwjyhs.com/t6/702/1556379664x2890149865.jpg "run in SecureCRT")
