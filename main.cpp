#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;
#include "test.h"
#include <memory>
#include <type_traits>
#include "AsyncBase.h"

#include <iostream>

int func1( int ret)
{
    //std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "func1:" << ret << std::endl;
    return ret;
}

int threadFunc(AsyncBase& base, int ret)
{
    int retT = ret;
    if (ret % 2)
    {
        //同步获取执行结果
        retT = base.emplace(func1, ret).get();
    }
    else
    {
        std::cout << "cancel:" << ret << std::endl;
        //取消执行
        base.emplace(func1, ret).cancel();
    }
   
    return ret;
}
int main()
{
    AsyncBase taskQ(1);
    std::thread th[10];
    for (int i = 0; i < 10; ++i)
    {
        th[i] = std::thread(threadFunc, std::ref( taskQ), i);
    }
    taskQ.emplace(func1, 1);
    
    for (int i = 0; i < 10; ++i)
    {
        th[i].join();
    }


    system("pause");
    return 0;
}