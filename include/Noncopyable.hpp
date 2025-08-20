#ifndef _Noncopyable_H_
#define _Noncopyable_H_ 

namespace wd{

//不可复制、赋值类
class Noncopyable{
protected:
    //构造函数
    Noncopyable(){}

    //析构函数
    ~Noncopyable(){}

    //将拷贝构造函数，删除
    Noncopyable(const Noncopyable &) = delete;

    //赋值函数，删除
    Noncopyable & operator=(const Noncopyable &) = delete;
};

}
#endif

