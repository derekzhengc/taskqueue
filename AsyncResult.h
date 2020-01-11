#pragma once
#include <future>
#include <functional>
#include "CancelAble.h"

template<typename Ret>
class AsyncResult
{

public:
    using future_type = std::future<Ret>;
    AsyncResult(std::function<void()> && cancelFunc, future_type&& fu)
        :cancelFunc_(std::move(cancelFunc)), future_(std::move(fu))
    {
    }
    /*
        撤销任务
    */
    void cancel(){
        if(cancelFunc_)
            cancelFunc_.cancel();
    }

    /*
        获取任务结果
    */
    Ret get() {
        future_type _Local{ std::move(future_) };
        return _STD move(_Local.get());
    }

    AsyncResult() = default;
    AsyncResult(AsyncResult const&) = delete;
    auto operator=(AsyncResult const&)->AsyncResult & = delete;
    AsyncResult(AsyncResult&&) = default;
    auto operator=(AsyncResult&&)->AsyncResult & = default;

private:
    CancelAble cancelFunc_{ []() {} };
    future_type future_;
};
