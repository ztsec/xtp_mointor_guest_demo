# xtp_mointor_guest_demo
C++，只包含monitor guest api   
此Demo仅包含monitor guest api的文本文件上传、断线后最多尝试5次重连功能   

##配置文件
测试用户可自行修改配置文件config.json进行测试：  
`host_ip`为xtp monitor server所对应的ip地址  
`host_port` 为xtp monitor guest所应连的端口，通常为7750  
`host_user` 为xtp monitor guest端的用户名  
`host_monitor_name` 为xtp monitor client端的用户名   
`host_monitor_password` 为xtp monitor client端登录的密码，由用户自行设定，请设置不小于8位的密码  

用户测试xtp monitor的时候，需要按如下步骤操作（以此demo程序为例）：   
（1）修改配置`config.json`文件，并运行guest端程序  
（2）等提示登录monitor server成功后，再运行monitor client程序  
（3）monitor client程序设置网络ip和端口（通常为7749）  
（4）用`host_monitor_name`以及自行设定的密码进行登录   
（5）登录成功后，可以上传文本文件  
  
注意：monitor guest程序断线的时候，会导致monitor client断线  


##使用CMake生成工程文件：   
linux下：     
（1）安装cmake   
（2）进入demo文件夹，新建build目录  
（3）进入build目录   
（4）命令行键入：cmake ..  
注意：如果是Debug模式，需要使用 cmake .. -DCMAKE_BUILD_TYPE=Debug   
（5）命令行键入：make 执行编译  
（6）可进入api目录下运行生成的demo程序

