#pragma once

class NonCopyable
{
  public:
    NonCopyable (const NonCopyable &) = delete;
    NonCopyable & operator = (const NonCopyable &) = delete;

    // Also disallow move semantics
    NonCopyable(NonCopyable &&) = delete;
    NonCopyable & operator=(NonCopyable &&) = delete;

  protected:
    NonCopyable () = default;
    ~NonCopyable () = default;
};
