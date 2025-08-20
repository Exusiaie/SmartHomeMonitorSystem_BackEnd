#include "SmartHomeMonitorServer.hpp"
#include "Configuration.hpp"
#include "MyLogger.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace wd;
using std::cout;
using std::endl;

int main() {
    // 初始化配置
    Configuration* config = Configuration::getInstance();
    auto& configMap = config->getConfigMap();
    
    // 日志系统初始化
    Mylogger* logger = Mylogger::getInstance();
    LogInfo("Server starting...");
    logger->info("Server starting");
    // 从配置获取参数
    unsigned short port = static_cast<unsigned short>(std::stoi(configMap["port"]));
    std::string ip = configMap["ip"];
    int threadNum = std::stoi(configMap["thread_num"]);
    int taskSize = std::stoi(configMap["task_num"]);
    
    LogInfo("Server config: port=%d, ip=%s, threadNum=%d, taskSize=%d", 
            port, ip.c_str(), threadNum, taskSize);
    
    try {
        // 创建服务器实例
        SmartHomeMonitorServer server(threadNum, taskSize, port, ip);
        
        // 启动服务器
        LogInfo("Starting server components...");
        server.start();
        LogInfo("Server started successfully");
        
        // 主循环（实际工作由事件循环处理）
        while (true) {
            // 此处可添加定期状态检查或维护任务
            sleep(60); // 每分钟检查一次
        }
    } catch (const std::exception& e) {
        LogError("Server fatal error: %s", e.what());
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
