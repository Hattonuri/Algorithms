#include <iostream>
#include <vector>

void outputDfs(const std::vector<std::vector<std::pair<uint64_t, size_t>>> &dp,
               const size_t l, const size_t r) {
    if (l == r) {
        return;
    }

    size_t lr = dp[l][r].second;
    outputDfs(dp, l, lr);
    outputDfs(dp, lr+1, r);
    std::cout << "[" << l + 1 << ":" << lr + 1 << "] * [" << lr + 2 << ":" << r + 1 <<
                 "] -> +" << dp[l][r].first - dp[l][lr].first - dp[lr+1][r].first << '\n';
}

void getMultiplicationOrder(const std::vector<std::pair<uint64_t, uint64_t>> &p) {
    size_t n = p.size();
    std::vector<std::vector<std::pair<uint64_t, size_t>>> dp(n);

    // l, r -> value, from which combined
    for (size_t l = 0; l < n; ++l) {
        dp[l].resize(n, {static_cast<uint64_t>(1e18), {}});
        dp[l][l] = {0, l};
    }

    for (size_t len1 = 1; len1 < n; ++len1) {
        for (size_t len2 = 1; len1 + len2 <= n; ++len2) {
            for (size_t l = 0; l + len1 < n; ++l) {
                size_t r = l + len1 - 1;
                size_t k = r + len2;
                if (k < n) {
                    uint64_t curAns = dp[l][r].first + dp[r+1][k].first +
                                      p[l].first * p[r].second * p[k].second;
                    if (curAns < dp[l][k].first) {
                        dp[l][k] = {curAns, r};
                    }
                } else {
                    break;
                }
            }
        }
    }

    outputDfs(dp, 0, n-1);
    std::cout << "Operations done -> " << dp[0][n-1].first << '\n';
}

int main() {
    size_t n;
    std::cin >> n;
    std::vector<std::pair<uint64_t, uint64_t>> matrices(n);
    for (auto &[rows, columns] : matrices) {
        std::cin >> rows >> columns;
    }
    getMultiplicationOrder(matrices);
}
