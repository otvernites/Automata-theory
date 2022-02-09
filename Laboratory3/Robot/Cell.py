class Cell:
    def __init__(self, x=0, y=0, is_filled=False):
        self.x = x
        self.y = y
        self.is_filled = is_filled

    def __repr__(self):
        message = f'cell({self.x}, {self.y}) -> '
        if self.is_filled is True:
            message += f'{"filled"} : '
        else:
            message += f'{"not filled"} : '
        return message

    def __eq__(self, other):
        if isinstance(other, Cell):
            return self.x == other.x and self.y == other.y
        return False

