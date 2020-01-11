#pragma once
#include <functional>
#include <memory>
#include <atomic>
class CancelAble
{
public:
    explicit CancelAble(std::function<void()> && cancelFunc) :
        cancelFunc_{ cancelFunc }
    { }

   /*
        取消任务--如果任务还未执行的话
   */
    void cancel()
    {
        if (!called_ || !cancelFunc_ || called_->test_and_set())
        {
            return;
        }
        if (cancelFunc_) cancelFunc_();
    }

    CancelAble() = default;

    
    CancelAble(CancelAble const&) = delete;

   
    auto operator=(CancelAble const&)->CancelAble & = delete;

    //! This class is move-constructible.
    CancelAble(CancelAble&&) = default;

  
    auto operator=(CancelAble&&)->CancelAble & = default;

    explicit operator bool() const noexcept {
        if (cancelFunc_) return true;
        return false;
    }
private:
    std::function<void()> cancelFunc_{ []() {} };

    std::unique_ptr<std::atomic_flag> called_{ std::make_unique<std::atomic_flag>() };
};
