#ifndef _WD_MYLOGGER_H_ 
#define _WD_MYLOGGER_H_

#include "Configuration.hpp"
#include <log4cpp/Category.hh>        //日志记录，核心功能
using std::cout;
using std::endl;
namespace wd{
class Mylogger{

    //自动释放类---------------------------------------
    class AutoRelease{
    public:
        //自动释放-构造函数
        AutoRelease(){}

        //自动释放-析构函数
        ~AutoRelease(){
            if(_pInstance){
                delete _pInstance;  //不置为null吗？
            }//if
        }//析构函数
    };//class-auto

public:
    //静态函数，类加载时，完成实例化=饿汉模式（单例模式）----------
    static Mylogger * getInstance(){
        if(nullptr == _pInstance){
            _pInstance = new Mylogger();
        }
        return _pInstance;
    }

    //销毁----------------------------------------
    static void destroy(){
        if(_pInstance){
            delete _pInstance;
            _pInstance = nullptr;
        }
    }

    //报错------------------------------------------
    //基础版本
    void error(const char * msg);

    //模板版本，可变参数
    template <class... Args>                    
    void error(const char * msg,Args... args){  //日志库封装
        _cat.error(msg,args...);                //将参数转发给 _cat.warn？
    }

    //警告------------------------------------------
    void warn(const char * msg);

    template <class... Args>
    void warn(const char * msg,Args... args){
        _cat.warn(msg,args...);
    }

    //调试-------------------------------------------
    void debug(const char * msg);

    template <class... Args>
    void debug(const char * msg,Args... args){
        _cat.debug(msg,args...);
    }

    //信息-------------------------------------------
    void info(const char * msg);

    template <class... Args>
    void info(const char * msg,Args... args){
        _cat.info(msg,args...);
    }

private:
    //构造函数
    Mylogger();

    //析构函数
    ~Mylogger();

private:
    static Mylogger * _pInstance;   //日志指针
    log4cpp::Category& _cat;       //？
    static AutoRelease _ar;         //自动释放
};//class Mylogger
  
//统一格式，宏
#define addprefix(msg) string("[")\
       .append(__FILE__).append(":")\
       .append(__func__).append(":")\
       .append(msg).c_str()

//快捷宏，四个日志级别                                为日志消息添加统一前缀    可变参数，格式化日志
#define LogInfo(msg,...) Mylogger::getInstance()->info(addprefix(msg),##__VA_ARGS__)
#define LogError(msg,...) Mylogger::getInstance()->error(addprefix(msg),##__VA_ARGS__)
#define LogWarn(msg,...) Mylogger::getInstance()->warn(addprefix(msg),##__VA_ARGS__)
#define LogDebug(msg,...) Mylogger::getInstance()->debug(addprefix(msg),##__VA_ARGS__)
}
#endif


