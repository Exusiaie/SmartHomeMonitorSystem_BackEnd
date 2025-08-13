#include "MyLogger.hpp"   
#include "Configuration.hpp"         //配置文件类
#include <iostream>

#include <log4cpp/Category.hh>         //日志记录，核心功能
#include <log4cpp/PatternLayout.hh>    //定义日志输出格式

#include <log4cpp/OstreamAppender.hh>  //输出日志到控制台
#include <log4cpp/FileAppender.hh>     //输出日志到文件

#include <log4cpp/Priority.hh>        //定义日志级别（过滤规则）

using std::cout;
using std::endl;
using std::string;

using namespace log4cpp;

Mylogger * Mylogger::_pInstance = nullptr;  //指针初始化
Mylogger::AutoRelease Mylogger::_ar;        //自动释放对象

//构造函数-------------------
Mylogger::Mylogger()
:_cat(log4cpp::Category::getInstance("wd"))     //？
{
    cout << "Mylogger()" << endl;

    //1.创建日志布局对象------------------------------------------
    auto ptn1 = new PatternLayout();            
    ptn1->setConversionPattern("%d %c [%p] %m%n");

    auto ptn2 = new PatternLayout();
    ptn2->setConversionPattern("%d %c [%p] %m%n");

    //2.创建日志目的地对象-----------------------------------------
    auto pOsAppender = new OstreamAppender("console",&cout);
    pOsAppender->setLayout(ptn1);

    //                  配置文件类      指针          存放配置信息的map 根据log_file查找
    string logfile = wd::Configuration::getInstance()->getConfigMap()["log_file"];  
    cout << "set log file:" <<logfile << endl;
    auto pFileAppender = new FileAppender("FileAppender",logfile);
    pFileAppender->setLayout(ptn2);

    //3.设置Category对象------------------
    _cat.setPriority(Priority::DEBUG);    //设置日志记录器，最低日志级别=debug
    _cat.addAppender(pOsAppender);        //将控制台输出目标，添加到日志记录器中
    _cat.addAppender(pFileAppender);      //写入文件
}

//析构函数
Mylogger::~Mylogger(){
    cout << "~Mylogger" <<endl;
    Category::shutdown();       //回收日志布局，目的地-堆对象
}

    //报错------------------------------------------
    //基础版本
void Mylogger::error(const char * msg){
    _cat.error(msg);
}

    //警告------------------------------------------
void Mylogger::warn(const char * msg){
    _cat.warn(msg);
}

    //调试-------------------------------------------
void Mylogger:: debug(const char * msg){
    _cat.debug(msg);
}

    //信息-------------------------------------------
void Mylogger::info(const char * msg){
    _cat.debug(msg);
}




