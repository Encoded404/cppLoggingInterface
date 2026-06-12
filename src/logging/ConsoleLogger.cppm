module;

export module logiface.ConsoleLogger;

import std;
import logiface;

export namespace Logiface {

// Example thread-safe console logger
struct ConsoleLogger : public Logger {
    explicit ConsoleLogger(Level lvl = Level::info,
                           std::ostream& out = std::clog,
                           std::ostream& err = std::cerr) noexcept
        : lvl_(lvl), out_(out), err_(err) {}

    void Log(const Record& r) override {
        std::lock_guard<std::mutex> const g(mutex_);
        auto& os = (r.lvl == Level::error || r.lvl == Level::critical || r.lvl == Level::warn)
                ? err_
                : out_;
        auto tt = std::chrono::system_clock::to_time_t(r.timestamp);
        os << "[" << std::put_time(std::localtime(&tt), "%F %T") << "] "
           << ToString(r.lvl) << " "
           << "(" << r.location << ":" << r.line << "): "
           << r.message << '\n';
    }
    void SetLevel(Level lvl) noexcept override { lvl_ = lvl; }
    [[nodiscard]] Level GetLevel() const noexcept override { return lvl_; }
private:
    Level lvl_;
    std::ostream& out_;
    std::ostream& err_;
    std::mutex mutex_;
};

} // namespace Logiface
