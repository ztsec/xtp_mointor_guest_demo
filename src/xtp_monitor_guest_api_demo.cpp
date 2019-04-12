
#include <string>
#include <map>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#include "FileUtils.h"
#include "xhost_md5.h"
#include "xtp_monitor_guest_api.h"

std::string host_monitor_username;
std::string host_monitor_password;
std::string host_server_ip;
int32_t host_server_port;
std::string host_username;

//策略是否运行状态的标识
bool strategy_status = false;

//监控客户登录时，验证账户有效性的回调函数
int32_t CheckLogin(const char* username, const char* password, const char* mac_add, const char* ip)
{
	std::cout << "CheckLogin. name:" << username << ", password:" << password << ",mac_add:" << mac_add << ",ip:" << ip << std::endl;

	//由于用户密码是经过MD5加密的，因此需要将您设定的监控账户密码加密后，与传入的密码进行校验
	char temp[1024] = { 0 };
	calc_md5(host_monitor_password.c_str(), strlen(host_monitor_password.c_str()), temp);//MD5加密
	std::cout << temp << std::endl;

	if ((strcmp(password, temp) == 0) && (strcmp(username, host_monitor_username.c_str()) == 0))
	{
		//用户名、密码相同，可以视为验证通过
		return 0;
	}

	return 10001; //错误code，用户自定义
}

//监控客户端启动策略
int32_t StrategyStart()
{
	std::cout << "StrategyStart." << std::endl;
	strategy_status = true;
	return 0;
}

//监控客户端停止策略
int32_t StrategyStop()
{
	std::cout << "StrategyStop." << std::endl;
	strategy_status = false;
	return 0;
}

//与monitor服务器断线回调函数
void MonitorDisconnected()
{
	std::cout << "Disconnected from monitor server." << std::endl;

	//可以重新登录monitor server
	int login_count = 0;
	while (login_count < 5) //如果5次登录都不成功，就不再登录了
	{
		int32_t ret = ConnectToMonitor(host_server_ip.c_str(), host_server_port, host_username.c_str(), strategy_status);
		if (ret == 0)
		{
			//登录成功
			std::cout << host_username << " login to monitor server success." << std::endl;
			return;
		}
		login_count++;
		std::cout << host_username << " login to monitor server failed. ------ " << login_count << " times." << std::endl;

#ifdef _WIN32
		Sleep(3000);
#else
		sleep(3);
#endif // WIN32
	}

}

int32_t ClientSetParameter(const char* key, const char* value)
{
	std::cout << "Client set --- Key:" << key << ", Value:" << value << std::endl;
	return 0;
}

int main()
{

	FileUtils* fileUtils = new FileUtils();
	if (!fileUtils->init())
	{
		std::cout << "The config.json file parse error." << std::endl;
#ifdef _WIN32
		system("pause");
#endif

		return 0;
	}

	//读取host guest配置
	host_server_ip = fileUtils->stdStringForKey("host_ip");
	host_server_port = fileUtils->intForKey("host_port");
	host_username = fileUtils->stdStringForKey("host_user");
	host_monitor_username = fileUtils->stdStringForKey("host_monitor_name");
	host_monitor_password = fileUtils->stdStringForKey("host_monitor_password");

	//注册Host的响应函数
	RegisterMonitorClientLoginFunc(&CheckLogin);
	RegisterStartFunc(&StrategyStart);
	RegisterStopFunc(&StrategyStop);
	RegisterDisconnectedFunc(&MonitorDisconnected);
	RegisterSetParameterFunc(&ClientSetParameter);

	//登录至Monitor服务器
	int32_t ret = ConnectToMonitor(host_server_ip.c_str(), host_server_port, host_username.c_str(), strategy_status);
	if (ret == 0)
	{
		//host guest user login success
		std::cout << host_username << " login to monitor server success." << std::endl;

		//循环等待监控客户端启动策略
		while (!strategy_status)
		{
#ifdef _WIN32
			Sleep(1000);
#else
			sleep(1);
#endif // WIN32
		}
	}
	else
	{
		std::cout << host_username << " login to monitor server failed." << std::endl;

#ifdef _WIN32
		system("pause");
#endif

		return 0;
	}


	//等待策略停止
	while (strategy_status)
	{
#ifdef _WIN32
		Sleep(1000);
#else
		sleep(1);
#endif // WIN32
	}

#ifdef _WIN32
	system("pause");
#endif

	return 0;
}
