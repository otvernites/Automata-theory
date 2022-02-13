import random
import sys, pygame
import numpy as np
from Errors import *


class Robot:
    def __init__(self, filename):
        self.x = 0
        self.y = 0
        self.tp_skill = 0
        self.map = np.array([])
        self.memorized_map = np.array([])
        self.memorized_path = []
        self.initialize(filename)

    def initialize(self, filename):
        with open(filename, 'r') as file:
            data = file.read().split('\n')
        self.x = int(data[0].split(',')[0])
        self.y = int(data[0].split(',')[1])
        x_mas = [int(c.split(',')[0]) for c in data[1:]]
        y_mas = [int(c.split(',')[1]) for c in data[1:]]
        if self.x > max(x_mas) or self.y > max(y_mas):
            raise InputError
        self.tp_skill = random.randint(1, int((max(x_mas) + max(y_mas)) / 2)+1)
        self.map = np.zeros((max(x_mas) + 1, max(y_mas) + 1))
        self.memorized_map = np.zeros((self.map.shape[0], self.map.shape[1]))
        self.memorized_map[self.x, self.y] = 1
        self.memorized_path.append((self.x, self.y))
        for coords in list(zip(x_mas, y_mas)):
            self.map[coords[0], coords[1]] = 1
        if self.map[self.x, self.y] == 1:
            raise InputError

    def move_forward(self):
        if self.y == 0:
            return False
        elif self.map[self.x, self.y - 1] == 1 or self.memorized_map[self.x, self.y - 1] == 1:
            return False
        else:
            self.memorized_map[self.x, self.y - 1] = 1
            self.y -= 1
            self.memorized_path.append((self.x, self.y))
            return True

    def move_back(self):
        if self.y == self.map.shape[1] - 1:
            return False
        elif self.map[self.x, self.y + 1] == 1 or self.memorized_map[self.x, self.y + 1] == 1:
            return False
        else:
            self.memorized_map[self.x, self.y + 1] = 1
            self.y += 1
            self.memorized_path.append((self.x, self.y))
            return True

    def move_left(self):
        if self.x == 0:
            return False
        elif self.map[self.x - 1, self.y] == 1 or self.memorized_map[self.x - 1, self.y] == 1:
            return False
        else:
            self.memorized_map[self.x - 1, self.y] = 1
            self.x -= 1
            self.memorized_path.append((self.x, self.y))
            return True

    def move_right(self):
        if self.x == self.map.shape[0] - 1:
            return False
        elif self.map[self.x + 1, self.y] == 1 or self.memorized_map[self.x + 1, self.y] == 1:
            return False
        else:
            self.memorized_map[self.x + 1, self.y] = 1
            self.x += 1
            self.memorized_path.append((self.x, self.y))
            return True

    def step_forward(self):
        if self.y == 0 or self.map[self.x, self.y - 1] == 1:
            return False
        self.y -= 1
        self.memorized_path.append((self.x, self.y))

    def step_back(self):
        if self.y == self.map.shape[1] - 1 or self.map[self.x, self.y + 1] == 1:
            return False
        self.y += 1
        self.memorized_path.append((self.x, self.y))

    def step_left(self):
        if self.x == 0 or self.map[self.x - 1, self.y] == 1:
            return False
        self.x -= 1
        self.memorized_path.append((self.x, self.y))

    def step_right(self):
        if self.x == self.map.shape[0] - 1 or self.map[self.x + 1, self.y] == 1:
            return False
        self.x += 1
        self.memorized_path.append((self.x, self.y))

    def get_tp_skill(self):
        return self.tp_skill

    def teleport(self):
        if self.tp_skill == 0:
            return False
        allowed_cells = []
        for x in range(0, self.memorized_map.shape[0]):
            for y in range(0, self.memorized_map.shape[1]):
                if not self.memorized_map[x, y]:
                    allowed_cells.append((x, y))
        if len(allowed_cells) == 0:
            return False
        cell = random.randint(0, len(allowed_cells)-1)
        x_pos = allowed_cells[cell][0]
        y_pos = allowed_cells[cell][1]
        if self.map[x_pos, y_pos] == 1:
            self.tp_skill -= 1
            return False
        else:
            self.x = x_pos
            self.y = y_pos
            self.memorized_map = np.zeros((self.map.shape[0], self.map.shape[1]))
            self.tp_skill -= 1
            self.memorized_map[self.x, self.y] = 1
            self.memorized_path.append((self.x, self.y, 't'))
            return True

    def vizualize(self):
        delay_time = 10
        scale_f = 60
        black = 0, 0, 0
        robot_x = self.memorized_path[0][0]
        robot_y = self.memorized_path[0][1]
        pygame.init()
        pygame.mixer.music.load("./Robot/sound.mp3")  # signal of teleport
        size = width, height = self.map.shape[0] * scale_f, self.map.shape[1] * scale_f

        screen = pygame.display.set_mode(size)
        # set background
        bg = pygame.image.load("./Robot/images/3.jpg")
        bg = pygame.transform.scale(bg, (width, height))
        # set wall
        wall = pygame.image.load("./Robot/images/block.png")
        wall = pygame.transform.scale(wall, (scale_f, scale_f))
        # set robot
        robot_sprite = pygame.image.load("./Robot/images/bird.png")
        robot_sprite = pygame.transform.scale(robot_sprite, (scale_f, scale_f))
        robot_rect = robot_sprite.get_rect()
        # start position
        robot_rect.center = (robot_x * scale_f + scale_f * 0.5, robot_y * scale_f + scale_f * 0.5)
        idx = 0
        while True:
            screen.fill(black)
            for event in pygame.event.get():
                if event.type == pygame.QUIT: sys.exit()
            # draw map
            screen.blit(bg, (0, 0))
            for x in range(0, self.map.shape[0]):
                for y in range(0, self.map.shape[1]):
                    if self.map[x, y]:
                        screen.blit(wall, (scale_f * x, scale_f * y))
            # передвигаем на следующую клетку в пути
            if idx + 1 != len(self.memorized_path):
                x_next = self.memorized_path[idx + 1][0]
                y_next = self.memorized_path[idx + 1][1]
                # если телепорт
                if len(self.memorized_path[idx + 1]) == 3:
                    pygame.mixer.music.play()
                    robot_x = x_next
                    robot_y = y_next
                    pygame.time.delay(1200)
                    robot_rect.center = (robot_x * scale_f + scale_f * 0.5, robot_y * scale_f + scale_f * 0.5)
                    idx += 1
                    continue
                robot_rect = robot_rect.move(x_next - robot_x, y_next - robot_y)
                if robot_rect.x == x_next * scale_f and robot_rect.y == y_next * scale_f:
                    robot_x = x_next
                    robot_y = y_next
                    idx += 1

            screen.blit(robot_sprite, robot_rect)
            pygame.time.delay(delay_time)
            pygame.display.flip()  # отрисовка всего


if __name__ == '__main__':
    optimus_prime = Robot('map.txt')

    optimus_prime.vizualize()
