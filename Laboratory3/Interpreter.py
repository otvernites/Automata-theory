import numpy as np
from Errors import *
from Parser.Parser import Parser
from Parser.SyntaxTree import Node
from Variable import Variable, Array


class Interpreter:

    def __init__(self, parser=Parser()):
        self.parser = parser
        self.root = None
        self.symbol_table = dict()
        self.declaration = dict()
        self.program = None

    def processing(self, program=None):
        self.program = program
        self.root, self.declaration, error = self.parser.parse(self.program)

        """
        try:
            dot = self.root.print_tree()
            dot.write_png('Parser/tree.png')
        except Exception:
            sys.stderr.write(f'> Tree building error! Correct your program.\n')
        """

        if error is True:
            raise ProgramError
        else:
            try:
                self.interpreter(self.root)
                return 0
            except Exception:
                raise

    def interpreter(self, node):
        if isinstance(node, list):
            node = node[0]
        # дефолт (ошибка)
        if node.type == "None":
            return ''
        elif node.type == "error":
            raise ProgramError
        # основа
        elif node.type == "program":
            self.interpreter(node.children)
        # блок выражений
        elif node.type == "statement_block":
            for node in node.children:
                self.interpreter(node)
        # выражение языка
        elif node.type == "statement":
            self.interpreter(node.children)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ таблица символов и процедур

        # ,1 .1 $1 + см. arr_call_id, int_arr_var, bool_arr_var
        elif node.type == "declaration":
            self.interpreter(node.children)

        # SIMPLE VARIABLES
        # ~5
        elif node.type == "label":
            if self.find_duplicate(node.value):
                var = Variable("label", node.value)
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

        # ,2
        elif node.type == "int_var":
            if self.find_duplicate(node.value):
                var = Variable("int", node.value)
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

        # .3
        elif node.type == "bool_var":
            if self.find_duplicate(node.value):
                var = Variable("bool", node.value)
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

        # $4
        elif node.type == "procedure":

            var = Variable("proc", node.value)

            if not (node.value in self.parser.declaration.keys()):
                self.parser.declaration[var.name] = var

            if self.find_duplicate(var.name):
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

        # ARRAYS
        # $3:[то же, что и выше]
        elif node.type == "proc_arr_var":

            var = Array("proc_arr", node.value)

            if not (node.value in self.parser.declaration.keys()):
                self.parser.declaration[var.name] = var

            if self.find_duplicate(var.name):
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

            self.array_indexes(node, 0)

        # ,3:[то же, что и выше]
        elif node.type == "int_arr_var":
            var = Array("int_arr", node.value)

            if self.find_duplicate(var.name):
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

            self.array_indexes(node, 0)

        # .3:[то же, что и выше]
        elif node.type == "bool_arr_var":
            var = Array("bool_arr", node.value)

            if self.find_duplicate(var.name):
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

            self.array_indexes(node, 0)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for assignment
        # 6
        elif node.type == "var_call":
            if self.find_duplicate(node.value):  # если такая не существует
                raise NonExistentVariable

        # 7:[3,] | 7:[6] | 7:[8:[2]]
        elif node.type == "arr_call": # если это вызов
            return self.array_indexes(node, 1)  # arr_index_validation - хз могу не могу, т.к. запретила вызывать в себе # 3:[3:[1,]]

        elif node.type == "literal":
            return node

        # ,2<-3, |
        # 3 <- {
        #  .#5
        # }
        elif node.type == "assignment":
            self.assign(node)

    """
    
        # (1,) | 1, | T | F | -1,
    elif node.type == "literal":
    
    # 7:[8:[2] 6, 2] (см индексы)
    elif node.type == "id_list":
    
    # 7:[3,] | 7:[6] | 7:[8:[2]] (см индексы)
    elif node.type == "id":
    
    # ,#4
    elif node.type == "incr":
    
    # ,*4
    elif node.type == "decr":
    
    # T | F  + см. arr_call_id и var_call_id
    elif node.type == "log_type":
    
    # разные комбинации log_type через двоеточие T:F:T
    elif node.type == "logic_expr":
    
    # .#T:F:7:[3,] ?????????????????????????????????????????????????????????????
    elif node.type == "logics":
    
    # 4 eq np | ,#5 eq 6,
    elif node.type == "comparison":
    
    # (.#T) {
    #   statement
    #   statement
    # }
    #
    # (T) {
    #   statement
    #   statement
    # }
    elif node.type == "cycle":
    
    # [.#5] please 6 | [T] please 6
    # [F]  6
    # please 6
    # 6
    elif node.type == "conditionals":
    
    # np
    elif node.type == "pass":
    
    # .3 @ 4 | 3 @ 4 | .4 % 3 | 3 % 3
    elif node.type == "identification":
    
    # mf mb mr ml tp
    elif node.type == "robot":
    
    # enter
    elif node.type == "empty":
    
    """
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for declaration
    def array_indexes(self, node, flag):
        leafs, arrays_call = self.get_leaf_nodes(node)  # получила все листья-индексы
        new_leafs = self.split_leafs(leafs)

        for list_elem in new_leafs:
            for elem in list_elem:
                elem.pop(0)

        conformity_arr_id = dict(zip(arrays_call, new_leafs))  # узлу arr_call соотв его листья

        # Для индексов - вызовов массивов и переменных проверяю, что они существуют
        for call in arrays_call:
            if self.symbol_table.get(call.value) is None:
                raise NonExistentVariable

        return self.arr_index_validation(conformity_arr_id, node, flag)

    # проверяем корректность индексов
    def arr_index_validation(self, conformity_arr_id, node, flag):
        indexes = conformity_arr_id[node]  # все индексы-ноды вызываемого массива

        def _arr_index_validation(conformity, call_node, leaf, flag):

            if isinstance(leaf, list):
                leaf = leaf[0]

            # Вычисляю значения по индексам
            if int(leaf.value) < 0:
                raise ErrorIndexes

            if leaf.type == "var_call":
                id = leaf.value

                if self.find_duplicate(id):  # если обращаемся к несуществующей переменной
                    raise NonExistentVariable

                if (self.symbol_table.get(id)).value is None:  # если в ней нет значения
                    raise UninitializedVariable

                if (self.symbol_table[id]).type != "int":  # если она не целочисленная
                    raise NotIntVariable

                if flag == 0:  # декларация
                    (self.symbol_table[call_node.value]).size.append(int((self.symbol_table[id]).value))
                elif flag == 1:
                    return int((self.symbol_table[id]).value)

            elif leaf.type == "id":  # сразу дана размерность (в int)
                if flag == 0:
                    (self.symbol_table[call_node.value]).size.append(int(leaf.value))
                elif flag == 1:
                    return int(leaf.value)
            else:
                indexes = conformity[leaf]  # достаем значение из подмассива
                for i in range(0, len(indexes)):
                    indexes[i] = _arr_index_validation(conformity, leaf, indexes[i], 1)

                return self.get_array_value(leaf, indexes)

        for i in range(0, len(indexes)):
            indexes[i] = _arr_index_validation(conformity_arr_id, node, indexes[i], 1)

        if flag == 0:
            self.symbol_table[node.value].size = indexes

        return indexes

    def get_array_value(self, leaf, indexes):
        if len(self.symbol_table[leaf.value].size) == 0:
            raise IndexOutOfRange

        delta = len(self.symbol_table[leaf.value].size) - len(indexes)
        if delta < 0:  # если новая длина больше
            raise IndexOutOfRange
        if delta > 0:
            for i in range(delta):
                indexes.append(0)
        for i in range(len(indexes)):
            if self.symbol_table[leaf.value].size[i] < indexes[i]:
                raise IndexOutOfRange

        value = self.symbol_table[leaf.value].array[tuple(indexes)[::-1]]
        if value is None:
            raise UninitializedVariable

        return int(value)

    def split_leafs(self, leafs):
        splitted = [[]]
        for item in leafs:
            if not splitted[0]:
                splitted[-1].append(item)
            else:
                flag = 0
                for split in splitted:
                    if split[0][0] == item[0]:
                        split.append(item)
                        flag = 1
                        break
                if flag == 0:
                    splitted.append([item])

        return splitted

    # ищем, уникальный ли номер у новообъявленной переменной
    def find_duplicate(self, id):
        res = True  # уникальный
        if self.symbol_table.get(id):
            res = False  # уже существует
        return res

    # получили список индексов для поддерева
    def get_leaf_nodes(self, node):
        leafs = []
        nested_arrays_call = []  # массивы, кот вызываются внутри

        def _get_leaf_nodes(node, id):
            if node is not None:
                if isinstance(node, list):
                    node = node[0]
                if node.type == "arr_call" or node.type == "proc_arr_var" \
                        or node.type == "int_arr_var" or node.type == "bool_arr_var":
                    nested_arrays_call.append(node)
                    if node.type == "arr_call":
                        leafs.append([id, node])
                    _get_leaf_nodes(node.children, node.id)
                else:
                    if not node.children:
                        leafs.append([id, node])
                    for n in node.children:
                        _get_leaf_nodes(n, id)

        _get_leaf_nodes(node, node.id)

        leafs = [leaf for leaf in leafs if leaf[1] is not node]

        return leafs, nested_arrays_call
    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for assignment

    def assign(self, node):

        left_child = node.children[0]
        l_indexes = self.interpreter(left_child)

        if node.children[1].type != "var_call":
            right = self.interpreter(node.children[1])  # возвращаемое значение сложной операции справа
            if isinstance(right, Node):
                r_value = right.value
            else:
                r_value = right
        else:
            right = node.children[1]
            r_value = self.symbol_table[right.value].value

        # записываю в таблицу символов значение
        if left_child.type == "var_call" or left_child.type == "declaration":

            if left_child.type == "declaration":
                left_child = left_child.children[0]

            #  здесь вставить условия, что этой переменной можно присваивать такое значение (напр, инт булу нельзя)
            id = int(left_child.value)  # имя переменной
            type = self.symbol_table[id].type

            #  int bool  int_arr bool_arr = const или когда переменная справа -  возвращает значение T или F
            if (((type == "int" or type == "int_arr") and r_value.isdigit()) or
                    ((type == "bool" or type == "bool_arr") and (r_value is True or r_value is False))):
                if type == "int" or type == "bool":
                    self.symbol_table[id].value = r_value
                    return
                else:  # массивы
                    size = (self.symbol_table[id]).size
                    (self.symbol_table[id]).array = np.empty(shape=[i + 1 for i in size][::-1], dtype=object)
                    (self.symbol_table[id]).array[tuple(size)[::-1]] = r_value
                    return
            # label  proc  proc_arr ???
            else:
                raise AssignmentErrorRight

        elif left_child.type == "arr_call":

            indexes = self.interpreter(left_child)
            id = int(left_child.value)  # имя переменной
            type = self.symbol_table[id].type

            if ((type == "int_arr" and r_value.isdigit()) or
                    (type == "bool_arr" and (r_value is True or r_value is False))):  # добавить, что это мб процедурой

                delta = len((self.symbol_table[id]).size) - len(l_indexes)

                for i in range(abs(delta)):
                    if delta > 0:
                        l_indexes.append(0)
                    elif delta < 0:
                        self.symbol_table[id].size.append(0)

                expanded_array_indexes = []
                for i in range(len(l_indexes)):
                    if self.symbol_table[id].size[i] < indexes[i]:
                        expanded_array_indexes.append(indexes[i])
                    else:
                        expanded_array_indexes.append(self.symbol_table[id].size[i])

                old_size = self.symbol_table[id].size
                if expanded_array_indexes != old_size:  # расширяю
                    (self.symbol_table[id]).size = expanded_array_indexes
                    tmp_array = np.empty(shape=[i + 1 for i in expanded_array_indexes][::-1], dtype=object).flatten()
                    (self.symbol_table[id]).array = (self.symbol_table[id]).array.flatten()

                    tmp_array = tmp_array[len((self.symbol_table[id]).array):]
                    (self.symbol_table[id]).array = np.concatenate((self.symbol_table[id].array, tmp_array), axis=0)

                    (self.symbol_table[id]).array = (self.symbol_table[id]).array.reshape([i + 1 for i in expanded_array_indexes][::-1])
                    (self.symbol_table[id]).array[tuple(expanded_array_indexes)[::-1]] = r_value
                    return

                else:  # просто записываю значение
                    if (self.symbol_table[id]).array.size == 0:
                        size = (self.symbol_table[id]).size
                        (self.symbol_table[id]).array = np.empty(shape=[i + 1 for i in size][::-1], dtype=object)
                        (self.symbol_table[id]).array[tuple(size)[::-1]] = r_value
                    else:
                        (self.symbol_table[id]).array[tuple(l_indexes)[::-1]] = r_value
                    return
            else:
                raise AssignmentErrorRight
        else:
            raise AssignmentErrorLeft

        """
        if node.children[1].type == "declaration":  # и таких мб много (вдруг там statement block?)
            raise AssignmentErrorRight
        """


if __name__ == '__main__':
    s = """ .0:[1, 4,]
            0:[1, 4,]<-T
            $6
            ~19
            ,5 <- 1,
            ,2<- 7,
            ,4:[0, 1,]<-10,
            ,9:[2]<-44,
            9:[16,]<-13,
            ,10:[1, 7, 10,] <- 66,
            ,3:[2, 5 10:[1, 2 4:[0, 1,]] 9:[16,]]
            ,1:[1, 2,]<-10,
            1:[3, 5,]<-4,
            1:[2, 7, 9,]<-7,
            5<-67,
            5<-2
            ,7<-2
            2<-90,


        """
    it = Interpreter()
    it.processing(s)



"""

# написать все тонкости и нюансы
# создать таблицу символов
# при использовании переменных проверять, что они уже были созданы (если это не объявление). тип можно ,3<-5,
# если объявление, то все ок
# глобальной области видимости нет, таблица символов и процедур одна на всю программу
#
# ПРОЦЕДУРА: не возвращает значение (т.е. все новые переменные остаются внутри, лучше в таблицу символов их не вносить)
#   -> что делать с (обмен данными с процедурой осуществляется через переменные)
#   -> процедура используется используется только в assignment и в связывании
#   -> по метке нельзя переходить из процедуры, по метке нельзя заходить в процедуру
#   -> по умолчанию инициализируется np
# ЦЕЛОЧИСЛ. МАССИВ: индекс по умолчанию - 0
#   -> в качестве индексов могут использоваться целочисленные литералы, целочисленные переменные и элементы массивов
#   (допускается любой уровень вложенности и рекурсии);
#   -> память под элементы массива выделяется при первом обращении к соответствующему элементу
# ЛОГИЧ. МАССИВ: индекс по умолчанию - 0
#    -> в качестве индексов могут использоваться целочисленные литералы, целочисленные переменные и элементы массивов
#    (допускается любой уровень вложенности и рекурсии);
#    -> память под элементы массива выделяется при первом обращении к соответствующему элементу
# МАССИВ ПРОЦЕДУР: индекс по умолчанию - 0
#     -> в качестве индексов могут использоваться целочисленные литералы, целочисленные переменные и элементы массивов
#     (допускается любой уровень вложенности и рекурсии);
#     -> память под элементы массива выделяется при первом обращении к соответствующему элементу
# Номера целочисленной и логической переменной, массивов и процедур + МЕТОК в пределах одной программы
#   совпадать не могут !!!! (так что в парсере сделать ошибку при добавлении такого же обявления процедуры)
# АРИФМ ОПЕРАТОРЫ: переменная должна быть целочисленная
# ПРИСВАИВАНИЕ ДЛЯ ПРОЦЕДУР:
#           <имя процедуры><-<имя процедуры или элемент массива процедур>;
#           <имя процедуры><-{<предложения языка>};
#           при присвоении процедур ни присваиваемый код, ни сама процедура не выполняется.
# ЛОГИЧЕСКИЕ ОПЕРАТОРЫ: внутри логическое выражение
# ОПЕР. СРАВНЕНИЯ: арифм выраж-арифм конст, логич выраж-логич конст, переменная процедура-np
# ОПЕР. ЦИКЛА: в условии логич выражение!
# ОПЕР. УСЛ. ПЕРЕХОДА: в условии логич выражение и вызов существующей метки
# СВЯЗЫВАНИЕ ИДЕНТИФИКАТОРОВ С ПРОЦЕДУРАМИ:
#   -> Идентификатор - любая переменная, в том числе процедура; рекурсивное связывание не допускается,
#   при попытке связывания, которое образует рекурсию, оператор не выполняется, и возвращает F, иначе возвращает T;
#   -> При обращении к связанному идентификатору сначала вызывается связанная с ним процедура,
#   затем возвращается значение идентификатора (возможно модифицированное связанной процедурой);
#   -> C идентификатором может быть связано несколько процедур;
#   -> Процедура может быть связана с несколькими идентификаторами;
# РАЗРЫВ СВЯЗИ:
#   -> Если оператор вызывается не для связанных ранее переменных, то ничего не происходит;
#      в любом случае возвращается значение T
# ПЕРЕМЕЩЕНИЕ РОБОТА:
#   -> Перемещается на одну клетку в заданном направлении от-но текущего. Если оператор невозможно выполнить
#      из-за наличия препятствия, он возвращает логическое значение F. Иначе – T.
# ТЕЛЕПОРТАЦИЯ РОБОТА:
#   -> Телепортируется в случайную, еще не посещенную им клетку лабиринта
#   -> робот имеет ограниченное количество попыток телепортации, определяемой средой выполнения;
#   -> при успешной телепортации возвращается T, иначе F; после телепортации робот не знает свое местоположение.
# где-то там был еще пример с двумя двоеточиями
"""
