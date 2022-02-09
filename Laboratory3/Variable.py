import numpy as np


class Variable:

    def __init__(self, type=None, name=None, value=None):
        self.type = type
        self.name = name
        self.value = value

    def __repr__(self):
        return f'{self.type} {self.name} -> {self.value}'


class Array:

    def __init__(self, type=None, name=None, size=None):
        if size is None:
            size = []
        self.type = type
        self.name = name
        self.size = size
        self.array = np.empty(shape=0)
        # self.matr = [[var.value] * self.size for _ in range(self.size)]

    def __repr__(self):
        return f'{self.type} {self.name}:{self.size}'


if __name__ == '__main__':
    arr = Array('bool_arr', 3, [2, 3, 4])
    var = Variable('bool', 4, True)
    print(arr)
    print(var)