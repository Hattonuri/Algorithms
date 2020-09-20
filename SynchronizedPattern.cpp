#include <mutex>

template <typename ValueType>
class Synchronized {
 public:
    explicit Synchronized(ValueType initial = ValueType()) {
        value = move(initial);
    }

    struct Access {
        ValueType& ref_to_value;
        std::lock_guard<std::mutex> lock;
    };

    Access GetAccess() {
        return {value, std::lock_guard(m)};
    }
 private:
    ValueType value;
    std::mutex m;
};
