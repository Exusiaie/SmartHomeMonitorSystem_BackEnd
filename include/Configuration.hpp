#ifndef _CONFIGURATION_H_ 
#define _CONFIGURATION_H_  

#include <map>
#include <string>

using namespace std;
namespace wd
{

class Configuration{

    //自动释放类-----------------------------------------
    class AutoRelease
    {
    public:
        //构造函数--------------------
        AutoRelease(){}

        //析构函数----------------------
        ~AutoRelease(){
            if(_pInstance){            //如果指针存在
                delete _pInstance;    //删除
                _pInstance = nullptr; //置空
            }//if
        }//析构函数
    };//class-Auto

    public:
        //静态函数，获取实例
        static Configuration * getInstance();

        //获取配置信息
        map<string,string> & getConfigMap();

        //显示
        void display() const;
    private:
        //构造函数
        Configuration();
        
        //析构函数
        ~Configuration();

        //读取配置文件
        void readConfiguration();
    private:
        map<string,string> _configMap;      //map
        static Configuration *_pInstance;   //配置指针
        static AutoRelease _ar;             //自动释放
};//Class-Configuration

}//wd
#endif

