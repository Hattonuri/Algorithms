#include <algorithm>
#include <future>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

template <typename K, typename V>
class ConcurrentMap {
 public:
    static_assert(std::is_integral_v<K>, "ConcurrentMap supports only integer keys");

    struct Access {
        std::lock_guard<std::mutex> lock;
        V& ref_to_value;
    };

    explicit ConcurrentMap(size_t bucket_count) : bucket(bucket_count), mut(bucket_count) {}

    Access operator[](const K &key) {
        size_t z = key % bucket.size();
        return {std::lock_guard(mut[z]), bucket[z][key]};
    }

    std::map<K, V> BuildOrdinaryMap() {
        std::map<K, V> result;
        size_t idx = 0;
        for (const std::unordered_map<K, V> &node : bucket) {
            std::lock_guard lock(mut[idx++]);
            for (auto &[key, value] : node) {
                result[key] += value;
            }
        }
        return result;
    }
 private:
    std::vector<std::unordered_map<K, V>> bucket;
    std::vector<std::mutex> mut;
};