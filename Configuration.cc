#include "Configuration.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace wd
{
//使用饿汉模式?
Configuration * Configuration::_pInstance = getInstance();  //静态函数，用指针存放
Configuration::AutoRelease Configuration::_ar;              //静态对象

//静态函数，类加载时，完成实例化=饿汉模式---------------------------------------
Configuration * Configuration::getInstance(){
    if(nullptr == _pInstance){          //初始为空
        _pInstance = new Configuration(); //实例化
    }
    return _pInstance;
}

//获取配置信息，返回map内容---------------------------------
map<string,string> & Configuration::getConfigMap(){
    return _configMap;
}

//显示------------------------------------------------------
void Configuration::display() const{
    //循环遍历map里的内容，并打印输出
    for(auto & elem : _configMap){
        cout << elem.first << "--->" << elem.second << endl;
    }//for
    cout << endl;
}

//构造函数----------------------
Configuration::Configuration(){
    cout << "Configuration()" << endl;
    readConfiguration();       //调用读配置信息
}
        
//构函数-----------------------
Configuration::~Configuration(){
    cout << "~Configuration()" << endl;
}

//读取配置文件--------------------------------------------------
void Configuration::readConfiguration(){
    string configfile("./conf/server.conf");    //指定配置文件位置
    ifstream ifs(configfile);                    //连接文件
    if(ifs.good()){
        string line;
        while(getline(ifs,line)){          //循环获取文件中的一行，放入line
            istringstream iss(line);       //分隔单词
            string key;
            string value;
            iss >> key >> value;            //分别存到变量中
            _configMap[key] = value;        //存入map中
        }//while

        cout << "reading config file" << configfile << "is over" << endl;
    }else{
        cout << "reading config file" << configfile << "error" << endl;
    }//if-else
}//void-read
 
}//wd

