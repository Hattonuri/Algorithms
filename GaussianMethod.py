from random import random

EPS = 1e-8


def multiply_list(ar, mul):
    return [i * mul for i in ar]


def subtract_list(a, b):
    return [a[i] - b[i] for i in range(len(a))]


class Matrix:
    def __init__(self, matrix):
        self.matrix = matrix
        self.size_x = len(matrix[0])
        self.size_y = len(matrix)

    def help_transform(self, ans):
        for y in range(self.size_y):
            mult = random()
            self.matrix[y] = multiply_list(self.matrix[y], mult)
            ans[y] *= mult
        su = [0 for _ in range(self.size_x)]
        for row in self.matrix:
            for x in range(self.size_x):
                su[x] += row[x]

        ans_sum = sum(ans)
        for y in range(self.size_y):
            for x in range(self.size_x):
                self.matrix[y][x] += su[x]
            ans[y] += ans_sum
        for x in su:
            if abs(x) < EPS:
                return False
        return True

    def solve(self, ans):
        if self.size_y < self.size_x or not self.help_transform(ans):
            raise Exception

        # subtract upper from lower rows
        for from_idx in range(self.size_x - 1):
            for to_idx in range(from_idx + 1, self.size_y):
                diff = self.matrix[to_idx][from_idx] / \
                       self.matrix[from_idx][from_idx]
                ans[to_idx] -= ans[from_idx] * diff
                self.matrix[to_idx] = subtract_list(
                    self.matrix[to_idx], multiply_list(
                        self.matrix[from_idx], diff))

        for idx in range(self.size_x):
            if abs(self.matrix[idx][idx]) < EPS:
                raise Exception

        # check unnecessary equations
        for idx in range(self.size_x - 1, self.size_y):
            ans[idx] /= self.matrix[idx][self.size_x - 1]
            self.matrix[idx][self.size_x - 1] = 1
            if abs(ans[idx] - ans[self.size_x - 1]) > EPS:
                raise Exception

        for x in range(self.size_x - 1, 0, -1):
            for y in range(x - 1, -1, -1):
                mult = self.matrix[y][x] / self.matrix[x][x]
                ans[y] -= ans[x] * mult
                self.matrix[y][x] = 0

        keys = [round(ans[i] / self.matrix[i][i], 8) for i in range(
            self.size_x)]
        return keys
