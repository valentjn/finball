import random
import math


class Object:
    def __init__(self, pos_x, pos_y, angle=0):
        self.pos_x = pos_x
        self.pos_y = pos_y
        self.angle = angle

    def write_object(self, flag_field):
        flag_field[self.pos_x][self.pos_y] = '1'

    def check_overlap(self, flag_field):
        if flag_field[self.pos_x][self.pos_y] == '1':
            return True
        return False


class Rectangle(Object):
    def __init__(self, pos_x, pos_y, angle, size_x, size_y):

        super(Rectangle, self).__init__(pos_x, pos_y, angle)

        self.size_x = size_x
        self.size_y = size_y

    def write_object(self, flag_field):
        for xs in range(4*self.size_x):
            x=xs/4
            for ys in range(4*self.size_y):
                y=ys/4
                flag_field[int(self.pos_x + math.cos(self.angle) * x - math.sin(self.angle) * y + 0.5)] \
                [int(self.pos_y + math.sin(self.angle) * x + math.cos(self.angle) * y + 0.5)] = '1'

    def check_overlap(self, flag_field):
        for xs in range(4*self.size_x):
            x=xs/4
            for ys in range(4*self.size_y):
                y=ys/4
                obs_x = int(self.pos_x + math.cos(self.angle) * x - math.sin(self.angle) * y + 0.5)
                obs_y = int(self.pos_y + math.sin(self.angle) * x + math.cos(self.angle) * y + 0.5)
                if not (0 < obs_x < len(flag_field) and 0 < obs_y < len(flag_field[0])):
                    return True
                if flag_field[obs_x][obs_y] == '1' or flag_field[obs_x][obs_y] == 'B':
                    return True
        return False


class Circle(Object):
    def __init__(self, pos_x, pos_y, radius):

        super(Circle, self).__init__(pos_x, pos_y)

        self.radius = radius

    def write_object(self, flag_field):
        for x in range(-self.radius, self.radius):
            for y in range(-self.radius, self.radius):
                if x**2+y**2 < self.radius**2:
                    flag_field[self.pos_x+x][self.pos_y+y] = '1'

    def check_overlap(self, flag_field):
        for x in range(-self.radius, self.radius):
            for y in range(-self.radius, self.radius):
                if x**2+y**2 < self.radius**2:
                    obs_x = self.pos_x+x
                    obs_y = self.pos_y+y
                    if not (0 < obs_x < len(flag_field) and 0 < obs_y < len(flag_field[0])):
                        return True
                    if flag_field[obs_x][obs_y] == '1' or flag_field[obs_x][obs_y] == 'B':
                        return True
        return False


class Line(Object):
    def __init__(self, pos_x, pos_y, angle, length):

        super(Line, self).__init__(pos_x, pos_y, angle)

        self.length = length

    def write_object(self, flag_field):
        for ls in range(4*self.length):
            l = ls/4
            flag_field[int(self.pos_x + math.cos(self.angle) * l + 0.5)] \
            [int(self.pos_y + math.sin(self.angle) * l + 0.5)] = '1'

    def check_overlap(self, flag_field):
        for ls in range(4*self.length):
            l = ls/4
            obs_x = int(self.pos_x + math.cos(self.angle) * l + 0.5)
            obs_y = int(self.pos_y + math.sin(self.angle) * l + 0.5)
            if not (0 < obs_x < len(flag_field) and 0 < obs_y < len(flag_field[0])):
                return True
            if flag_field[obs_x][obs_y] == '1' or flag_field[obs_x][obs_y] == 'B':
                return True
        return False


class Triangle(Object):
    def __init__(self, pos_x, pos_y, angle, length, angle_1, length_1):

        super(Triangle, self).__init__(pos_x, pos_y, angle)

        self.angle_1 = angle_1
        self.length = length
        self.length_1 = length_1

    def write_object(self, flag_field):
        h = math.sin(self.angle_1) * self.length_1
        b = self.length - math.cos(self.angle_1) * self.length_1
        l = math.sqrt(h ** 2 + b ** 2)
        alpha = math.acos(b / l)

        for lens in range(4*self.length):
            len = lens/4
            for iss in range(4*int(l*len/self.length)):
                i = iss/4
                x = int(math.cos(self.angle)*len - math.cos(alpha+self.angle)*i)
                y = int(math.sin(self.angle)*len - math.sin(alpha+self.angle)*i)
                flag_field[self.pos_x + x][self.pos_y + y] = '1'

    def check_overlap(self, flag_field):
        h = math.sin(self.angle_1) * self.length_1
        b = self.length - math.cos(self.angle_1) * self.length_1
        l = math.sqrt(h ** 2 + b ** 2)
        alpha = math.acos(b / l)

        for lengs in range(4*self.length):
            leng = lengs/4
            for iss in range(4*int(l * leng / self.length)):
                i = iss/4
                obs_x = self.pos_x + int(math.cos(self.angle) * leng - math.cos(alpha + self.angle) * i)
                obs_y = self.pos_y + int(math.sin(self.angle) * leng - math.sin(alpha + self.angle) * i)
                if not (0 < obs_x < len(flag_field) and 0 < obs_y < len(flag_field[0])):
                    return True
                if flag_field[obs_x][obs_y] == '1' or flag_field[obs_x][obs_y] == 'B':
                    return True
        return False


def inflow(size_x, size_y, flag_field):
    for i in range(4):
        rand_x = random.randint(0, 1)
        rand_y = random.randint(0, 3)

        while flag_field[rand_x*(size_x-1)][int(size_y/4)*rand_y] == '2':
            rand_x = random.randint(0, 1)
            rand_y = random.randint(0, 3)

        for i in range(int(size_y/4)):
            flag_field[rand_x*(size_x-1)][int(size_y/4)*rand_y + i] = '2'


def draw_object(flag_field):
    rand = random.randint(1,4)
    rand_x = random.randint(0, size_x-1)
    rand_y = random.randint(0, size_y-1)
    rand_angle = random.random()*2*math.pi
    if rand == 1:
        rand_size_x = random.randint(2, 20)
        rand_size_y = random.randint(2, 20)

        obj = Rectangle(rand_x, rand_y, rand_angle, rand_size_x, rand_size_y)
    elif rand == 2:
        rand_radius = random.randint(2, 10)
        obj = Circle(rand_x, rand_y, rand_radius)
    elif rand == 3:
        rand_length = random.randint(2, 20)
        rand_length_1 = random.randint(2, 20)
        rand_angle_1 = random.random()*math.pi
        obj = Triangle(rand_x, rand_y, rand_angle, rand_length, rand_angle_1, rand_length_1)
    else:
        rand_length = random.randint(2, 20)
        obj = Line(rand_x, rand_y, rand_angle, rand_length)

    while obj.check_overlap(flag_field):
        rand_x = random.randint(0, size_x - 1)
        rand_y = random.randint(0, size_y - 1)
        obj.pos_x = rand_x
        obj.pos_y = rand_y

    return obj


def write_file(file, size_x, size_y, flag_field):
    with open(file, 'w') as f:
        f.write("{} {}".format(size_x, size_y))
        for y in range(size_x):
            f.write("\n")
            for x in range(size_y):
                f.write(str(flag_field[x][y]))

if __name__ == "__main__":
    size_x = 64
    size_y = 64

    flag_field = []
    objects = []

    for x in range(size_x):
        row = []
        for y in range(size_y):
            if y == 0:
                if not int(size_x/16) < x < int(15*size_x/16):
                    row.append('2')
                elif int(3*size_x/8) < x < int(5*size_x/8):
                    row.append('2')
                else:
                    row.append('1')
            elif y == size_y - 1:
                if not int(size_x/16) < x < int(15*size_x/16):
                    row.append('2')
                elif int(2*size_x/8) < x < int(6*size_x/8):
                    row.append('3')
                else:
                    row.append('1')
            elif x == 0 or x == size_x - 1:
                row.append('1')
            else:
                row.append('0')
        flag_field.append(row)

    inflow(size_x, size_y, flag_field)

    flag_field[32][6] = 'B'

    for i in range(random.randint(4, 7)):
        draw_object(flag_field).write_object(flag_field)

    write_file('./data/testLevel3.txt', size_x, size_y, flag_field)
