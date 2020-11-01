#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>

enum Error {
    OK = 0,
    WRONG_ARGUMENTS,
    MALFORMED
};

struct Node {
    size_t sum = 0;
    char symbol = '$';
    std::pair<Node*, Node*> nextNode;

    ~Node() {
        delete nextNode.first;
        delete nextNode.second;
    }
};

template<typename T>
inline void Read(T& val, std::istream* in) {
    in->read(reinterpret_cast<char*>(&val), sizeof(T));
}

template<typename T>
inline void Write(const T& val, std::ostream* out) {
    out->write(reinterpret_cast<const char*>(&val), sizeof(T));
}

// we should be able to pass map and unordered_map
template<typename ContainerType, typename Required = typename ContainerType::key_type>
void FillCodes(const Node& v, size_t code, uint8_t codeLen, ContainerType* codes) {
    if (v.nextNode.first == nullptr) {
        (*codes)[v.symbol] = {code, std::max(codeLen, static_cast<uint8_t>(1))};
    } else {
        ++codeLen;
        code <<= 1ull;
        FillCodes(*v.nextNode.first, code, codeLen, codes);
        code |= 1ull;
        FillCodes(*v.nextNode.second, code, codeLen, codes);
    }
}

// we should be able to pass map and unordered_map
template<typename ContainerType, typename Required = typename ContainerType::key_type>
void FillDecodes(const Node& v, size_t code, uint8_t codeLen, ContainerType* decodes) {
    if (v.nextNode.first == nullptr) {
        (*decodes)[{code, std::max(codeLen, static_cast<uint8_t>(1))}] = v.symbol;
    } else {
        ++codeLen;
        code <<= 1u;
        FillDecodes(*v.nextNode.first, code, codeLen, decodes);
        code |= 1u;
        FillDecodes(*v.nextNode.second, code, codeLen, decodes);
    }
}

template<typename ValueType, typename Required = std::enable_if_t<std::is_unsigned_v<ValueType>>>
uint8_t GetLength(ValueType value) {
    return value == 0 ? 1u : static_cast<uint8_t>(std::log2(value)) + 1u;
}

struct NodeCompare {
    bool operator()(const Node* first, const Node* second) const {
        return first->sum < second->sum;
    }
};

template<typename CounterContainer>
Node* buildTree(const CounterContainer& stats) {
    std::multiset<Node*, NodeCompare> nodesQueue;
    for (size_t i = 0; i != stats.size(); ++i) {
        if (stats[i] != 0) {
            nodesQueue.insert(new Node{
                stats[i],
                static_cast<char>(static_cast<int32_t>(i) + std::numeric_limits<char>::min()),
                {nullptr, nullptr}
            });
        }
    }

    while (nodesQueue.size() != 1) {
        Node* first = nodesQueue.extract(nodesQueue.begin()).value();
        Node* second = nodesQueue.extract(nodesQueue.begin()).value();

        Node* newNode = new Node{
            first->sum + second->sum,
            '$',
            {second, first}
        };
        nodesQueue.insert(newNode);
    }
    return *nodesQueue.begin();
}

inline size_t CharToIdx(char c) {
    return static_cast<size_t>(static_cast<int16_t>(c) - std::numeric_limits<char>::min());
}

template<typename KeyType, typename ValueType>
struct PairHasher {
    size_t operator()(const std::pair<KeyType, ValueType>& p) const {
        static std::hash<KeyType> keyHasher{};
        static std::hash<ValueType> valueHasher{};
        return keyHasher(p.first) * 31ull + valueHasher(p.second);
    }
};

int compress(std::istream* in, std::ostream* out) {
    if (in == nullptr || out == nullptr) {
        return Error::WRONG_ARGUMENTS;
    }
    if (in->peek() == EOF) {
        return Error::OK;
    }

    std::array<size_t, 256> stats{};
    char ch;
    while (in->peek() != EOF) {
        in->get(ch);
        ++stats[CharToIdx(ch)];
    }

    std::unique_ptr<Node> root(buildTree(stats));
    std::unordered_map<char, std::pair<size_t, uint8_t>> codes;
    FillCodes(*root, 0, 0, &codes);
    for (size_t i = 0; i != stats.size(); ++i) {
        Write(stats[i], out);
    }
    size_t size = 0;
    for (auto[symbol, code] : codes) {
        size += stats[CharToIdx(symbol)] * code.second;
    }

    unsigned char last = 0;
    size_t lastLen = 0;
    Write(size, out);

    in->seekg(0, std::ios::beg);
    while (in->peek() != EOF) {
        char i;
        in->get(i);
        auto code = codes[i];
        for (int at = code.second - 1; at >= 0; --at) {
            size_t bit = (code.first >> at) & 1ull;
            last |= bit << lastLen;
            ++lastLen;
            if (lastLen == 8) {
                Write(last, out);
                last = 0;
                lastLen = 0;
            }
        }
    }

    if (lastLen != 0) {
        Write(last, out);
    }

    return Error::OK;
}

int decompress(std::istream* in, std::ostream* out) {
    if (in == nullptr || out == nullptr) {
        return Error::WRONG_ARGUMENTS;
    }

    if (in->peek() == EOF) {
        return Error::OK;
    }

    std::array<size_t, 256> stats{};
    for (size_t i = 0; i != stats.size(); ++i) {
        try {
            Read(stats[i], in);
        } catch (...) {
            return Error::MALFORMED;
        }
    }

    std::unique_ptr<Node> root(buildTree(stats));

    std::unordered_map<std::pair<size_t, uint8_t>, char, PairHasher<size_t, uint8_t>> decodes;
    FillDecodes(*root, 0, 0, &decodes);

    size_t size;
    Read(size, in);
    size_t chars = (size >> 3ull) + ((size & 7ull) != 0);

    size_t maxCodeLen = 0;
    for (auto &[_code, _] : decodes) {
        if (maxCodeLen < _code.second) {
            maxCodeLen = _code.second;
        }
    }

    size_t code = 0, codeLen = 0;
    std::array<size_t, 256> decodedStats{};
    for (size_t i = 0; i != chars; ++i) {
        unsigned char cur;
        try {
            Read(cur, in);
        } catch (...) {
            return Error::MALFORMED;
        }
        for (size_t j = 0; j != 8; ++j) {
            bool bit = (cur >> j) & 1ull;
            code = (code << 1ull) | bit;
            ++codeLen;
            if (codeLen > maxCodeLen) {
                return Error::MALFORMED;
            }
            if (auto it = decodes.find({code, codeLen}); it != decodes.end()) {
                Write(it->second, out);
                ++decodedStats[CharToIdx(it->second)];
                code = 0;
                codeLen = 0;
            }
            --size;
            if (size == 0) {
                break;
            }
        }
    }

    if (code != 0 || in->peek() != EOF || decodedStats != stats) {
        return Error::MALFORMED;
    }

    return Error::OK;
}