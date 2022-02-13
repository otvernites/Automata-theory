import sys
sys.setrecursionlimit(10000)
import numpy as np
from multipledispatch import dispatch
import sys
from Errors import *
from Parser.Parser import Parser
from Parser.SyntaxTree import Node
from Robot.Robot import Robot
from Variable import Variable, Array


class Interpreter:

    def __init__(self, parser=Parser(), robot=None):
        self.parser = parser
        self.root = None
        self.symbol_table = dict()  # здесь все переменные, метки (int - разная информация)
        self.declaration = dict()  # процедуры, массивы процедур и метки (int - node или variable/array)
        self.id_table = dict()  # переменная - список связанных процедур (int-list)

        self.program = None
        self.robot = robot

        self.current_cycle = None
        self.current_proc = None
        self.current_label = None
        self.tmp_variables = []
        self.exit = False

    def processing(self, program=None, robot=None):
        self.program = program
        self.robot = robot
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
            except RecursionError:
                print("Recursion error!")
            except Exception:
                raise

    def interpreter(self, node):
        """
        if self.exit or isinstance(self.exit, Node) and self.exit != node:
            return
        elif isinstance(self.exit, Node) and self.exit == node:
            self.exit = False
        """

        if self.exit:
            return

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
            """
            if node is self.current_label:  # label call
                return
            """

            for child in node.children:
                res = self.interpreter(child)
                if res is not True and res is not False and isinstance(res, int):
                    self.declaration[res] = (node, self.current_proc)  # label decl
                    self.symbol_table[res].value = (node, self.current_proc)

        # выражение языка
        elif node.type == "statement":
            """
            if node is self.current_label:  # label call
                return
            """

            res = self.interpreter(node.children)
            if res is not True and res is not False and isinstance(res, int):
                self.declaration[res] = (node, self.current_proc)
                self.symbol_table[res].value = (node, self.current_proc)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ таблица символов и процедур

        # ,1 .1 $1 + см. arr_call_id, int_arr_var, bool_arr_var
        elif node.type == "declaration":
            return self.interpreter(node.children)

        # SIMPLE VARIABLES
        # ~5
        elif node.type == "label":
            var = Variable("label", node.value)

            if not (node.value in self.parser.declaration.keys()):
                self.parser.declaration[var.name] = var

            """
            if self.find_duplicate(node.value):
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists
            """
            self.symbol_table[var.name] = var

            if self.current_proc is not None:
                self.tmp_variables.append(var.name)
            return var.name

        # ,2
        elif node.type == "int_var":
            if self.find_duplicate(node.value):
                var = Variable("int", node.value)
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

            if self.current_proc is not None:
                self.tmp_variables.append(var.name)

        # .3
        elif node.type == "bool_var":
            if self.find_duplicate(node.value):
                var = Variable("bool", node.value)
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

            if self.current_proc is not None:
                self.tmp_variables.append(var.name)

        # $4
        elif node.type == "procedure":
            var = Variable("proc", node.value)
            var.value = "np"

            if not (node.value in self.parser.declaration.keys()):
                self.parser.declaration[var.name] = var
            self.parser.declaration[var.name].value = "np"

            if self.find_duplicate(var.name):
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

            if self.current_proc is not None:
                self.tmp_variables.append(var.name)

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
            self.parser.declaration[var.name].size = self.symbol_table[var.name].size

            if self.current_proc is not None:
                self.tmp_variables.append(var.name)

        # ,3:[то же, что и выше]
        elif node.type == "int_arr_var":
            var = Array("int_arr", node.value)

            if self.find_duplicate(var.name):
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

            self.array_indexes(node, 0)

            if self.current_proc is not None:
                self.tmp_variables.append(var.name)

        # .3:[то же, что и выше]
        elif node.type == "bool_arr_var":
            var = Array("bool_arr", node.value)

            if self.find_duplicate(var.name):
                self.symbol_table[var.name] = var
            else:
                raise VariableAlreadyExists

            self.array_indexes(node, 0)

            if self.current_proc is not None:
                self.tmp_variables.append(var.name)

        # np
        elif node.type == "pass":
            pass
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for assignment
        # 6
        elif node.type == "var_call":
            if self.find_duplicate(node.value):  # если такая не существует
                raise NonExistentVariable

            if self.id_table.get(node.value) is not None:
                proc_list = self.id_table[node.value]
                for proc in proc_list:
                    action = self.procedure_call_chain(proc.name)
                    if action == "np":
                        pass
                    elif type(action) is Node:
                        self.current_proc = proc.name
                        self.interpreter(action)
                        self.exit = False
                        self.current_proc = None
                        for tmp_var in self.tmp_variables:
                            del self.symbol_table[tmp_var]
                        self.tmp_variables.clear()
                    else:
                        raise ProcedureError

        # 7:[3,] | 7:[6] | 7:[8:[2]]
        elif node.type == "arr_call":  # если это вызов
            return self.array_indexes(node, 1)
            # arr_index_validation - хз могу не могу, т.к. запретила вызывать в себе # 3:[3:[1,]]

        elif node.type == "literal":
            return node

        # ,2<-3, |
        # 3 <- {
        #  .#5
        # }
        elif node.type == "assignment":
            # если присваиваем инструкции процедуре (временные переменные не создаем)
            self.assign(node)
        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for identification

        # .3 @ 4 | 3 @ 4 | .4 % 3 | 3 % 3
        elif node.type == "identification":
            return self.id_processing(node)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for math

        # ,#4
        elif node.type == "incr":
            self.math(node, 1)

        # ,*4
        elif node.type == "decr":
            self.math(node, 0)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for conditional (пока напишу только для label без cond)
        # [.#5] please 6 | [T] please 6
        # [F]  6
        # please 6
        # 6
        elif node.type == "conditionals":  # вызов метки
            if len(node.children) == 2:  # метка c условием
                # левое значение
                log_value = None
                child = node.children[0]

                if child.type == "logics" or child.type == "log_type" or child.type == "comparison" or \
                        child.type == "identification" or child.type == "robot":
                    log_value = self.interpreter(child)
                elif child.type == "var_call":
                    self.interpreter(child)  # id
                    if self.symbol_table.get(child.value) is None:
                        raise NonExistentVariable
                    if self.symbol_table[child.value].type == "bool":
                        log_value = self.symbol_table[child.value].value
                elif child.type == "arr_call":
                    if self.symbol_table.get(child.value) is None:
                        raise NonExistentVariable
                    indices = self.interpreter(child)
                    if self.symbol_table[child.value].type == "bool_arr":
                        log_value = self.get_array_value(self.symbol_table[child.value], indices)
                elif child.type == "literal":
                    log_value = child.value
                else:
                    raise LabelError

                if log_value is True:
                    self.label_jump(node.children[1])
                elif log_value is False:
                    #self.label_jump(node.children[1])
                    self.current_label = None
                    return
                else:
                    raise LabelError
            else:  # метка без условия
                self.label_jump(node.children)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for logics
        # .#T-F-7:[3,]
        elif node.type == "logics":
            bool_values = []
            for child in node.children:
                res = self.interpreter(child)
                bool_values = bool_values + res if isinstance(res, list) \
                                                            else bool_values + [res]

            iter_count = len(bool_values) - 1
            if iter_count <= 0:
                raise LogicError

            last_value = bool_values.pop(0)
            for i in range(iter_count):
                last_value = self.pier_arrow(last_value, bool_values[i])
            return last_value

        # разные комбинации log_type T-F-T
        elif node.type == "logic_expr":

            bool_list = []

            for child in node.children:

                if child.type == "logic_expr":
                    bool_list = bool_list + self.interpreter(child)
                elif child.type == "comparison" or child.type == "identification" or child.type == "robot":
                    bool_list.append(self.interpreter(child))
                elif child.type == "log_type":
                    bool_list.append(self.interpreter(child))
                elif self.symbol_table.get(child.value) is not None:
                    if child.type == "arr_call":
                        if self.symbol_table[child.value].type == "bool_arr":
                            indices = self.interpreter(child)
                            bool_list = bool_list + [self.get_array_value(self.symbol_table[child.value], indices)]
                        else:
                            raise NotBooleanVariable
                    elif child.type == "var_call":
                        self.interpreter(child)  # id
                        var = self.symbol_table[child.value]
                        if var.type == "bool":
                            bool_list = bool_list + [var.value]
                        else:
                            raise NotBooleanVariable
                else:
                    raise NonExistentVariable
            return bool_list

        # T | F  + см. arr_call_id и var_call_id
        elif node.type == "log_type":
            if node.value is True or node.value is False:
                return node.value
            elif self.symbol_table.get(node.value) is not None:
                var = self.symbol_table.get(node.value)
                if var.type == "bool":
                    return var.value
                elif var.type == "bool_arr":
                    indexes = self.interpreter(node)
                    return self.get_array_value(var, indexes)
                else:
                    raise NotBooleanVariable
            else:
                raise NonExistentVariable

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for comparison

        # 4 eq np | ,#5 eq 6,
        elif node.type == "comparison":
            return self.compare(node)

        # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for cycle

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
            self.current_cycle = node

            while self.check_cycle(node.children[0]):
                self.interpreter(node.children[1])
                self.exit = False

            log_value = self.check_cycle(node.children[0])
            if log_value is False:
                self.current_cycle = None
                return
            else:
                raise CycleError


        # mf mb mr ml tp
        elif node.type == "robot":
            if node.value == "mf":
                return self.robot.move_forward()
            if node.value == "mb":
                return self.robot.move_back()
            if node.value == "mr":
                return self.robot.move_right()
            if node.value == "ml":
                return self.robot.move_left()
            if node.value == "tp":
                return self.robot.teleport()
            if node.value == "tp?":
                print(robot.tp_skill)
                if self.robot.tp_skill:
                    return True
                else:
                    return False
            if node.value == "sf":
                return self.robot.step_forward()
            if node.value == "sb":
                return self.robot.step_back()
            if node.value == "sr":
                return self.robot.step_right()
            if node.value == "sl":
                return self.robot.step_left()


    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~additional~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

                self.interpreter(leaf)  # id
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

    # for indexes
    @dispatch(Node, list)
    def get_array_value(self, leaf, indexes):
        if self.symbol_table.get(leaf.value) is None:
            raise NonExistentVariable

        type = self.symbol_table[leaf.value].type

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

    # for r_values
    @dispatch(Array, list)
    def get_array_value(self, leaf, indexes):
        if len(leaf.size) == 0:
            raise IndexOutOfRange

        delta = len(leaf.size) - len(indexes)
        if delta < 0:  # если новая длина больше
            raise IndexOutOfRange
        if delta > 0:
            for i in range(delta):
                indexes.append(0)
        for i in range(len(indexes)):
            if leaf.size[i] < indexes[i]:
                raise IndexOutOfRange

        value = leaf.array[tuple(indexes)[::-1]]
        if value is None:
            raise UninitializedVariable

        return value

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

        # вызываем присваивание для процедур (но! присваивнаие процедура-блок выражений обр в assign)
        if (node.children[0].type == "var_call" or node.children[0].type == "arr_call") and \
                (node.children[1].type == "var_call" or node.children[1].type == "arr_call"):
            l_type = self.symbol_table.get(node.children[0].value).type
            r_type = self.symbol_table.get(node.children[1].value).type
            if self.declaration.get(node.children[0].value) is not None \
                    and self.declaration.get(node.children[1].value) is not None:
                if (l_type == "proc" or l_type == "proc_arr") and (r_type == "proc" or r_type == "proc_arr"):
                    self.assign_for_procedure(node)
                    return


        left_child = node.children[0]
        l_indexes = self.interpreter(left_child)  # если слева - вызов массива # id
        r_value = None

        # чтоб не выполнялась процедура
        if node.children[1].type != "statement_block" and node.children[1].type != "statement":

            if node.children[1].type == "logics" or node.children[1].type == "comparison" or \
                    node.children[1].type == "identification" or node.children[1].type == "robot":  # statement
                r_value = self.interpreter(node.children[1])

            elif node.children[1].type != "var_call" and node.children[1].type != "arr_call":  # литерал
                right = self.interpreter(node.children[1])  # возвращаемое значение сложной операции справа
                r_value = right.value if isinstance(right, Node) else right

            elif node.children[1].type == "var_call":  # если переменная
                self.interpreter(node.children[1])  # id
                right = self.symbol_table.get(node.children[1].value)
                if right is not None:
                    r_value = right.value
                else:
                    raise NonExistentVariable
            elif node.children[1].type == "arr_call":  # если массив
                right = self.interpreter(node.children[1])  # indexes
                r_value = self.get_array_value(self.symbol_table[node.children[1].value], right)

        # записываю в таблицу символов значение
        if left_child.type == "var_call" or left_child.type == "declaration":

            if left_child.type == "declaration":
                left_child = left_child.children[0]

            #  здесь вставить условия, что этой переменной можно присваивать такое значение (напр, инт булу нельзя)
            id = int(left_child.value)  # имя переменной
            type = self.symbol_table[id].type

            is_num = lambda x: x.isdigit() if x[:1] != '-' else x[1:].isdigit()
            #  int bool  int_arr bool_arr = const или когда переменная справа -  возвращает значение T или F
            if (((type == "int" or type == "int_arr") and is_num(r_value)) or
                    ((type == "bool" or type == "bool_arr") and
                     (r_value is True or r_value is False or r_value == "True" or r_value == "False"))):
                if type == "int" or type == "bool":
                    self.symbol_table[id].value = r_value
                    return
                else:  # массивы
                    size = (self.symbol_table[id]).size
                    (self.symbol_table[id]).array = np.empty(shape=[i + 1 for i in size][::-1], dtype=object)
                    (self.symbol_table[id]).array[tuple(size)[::-1]] = r_value
                    return
            elif type == "proc":  # левое значение всегда var_call
                if node.children[1].type == "statement_block" or node.children[1].type == "statement":
                    self.declaration[id] = node.children[1]
                    self.symbol_table[id].value = node.children[1]
                    return
                else:
                    raise AssignmentErrorRight
            else:
                raise AssignmentErrorRight

        elif left_child.type == "arr_call":

            id = int(left_child.value)  # имя переменной
            type = self.symbol_table[id].type

            is_num = lambda x: x.isdigit() if x[:1] != '-' else x[1:].isdigit()
            if ((type == "int_arr" and is_num(r_value)) or
                    (type == "bool_arr" and (r_value is True or r_value is False))):  # добавить, что это мб процедурой

                delta = len((self.symbol_table[id]).size) - len(l_indexes)

                for i in range(abs(delta)):
                    if delta > 0:
                        l_indexes.append(0)
                    elif delta < 0:
                        self.symbol_table[id].size.append(0)

                # новые индексы
                expanded_array_indexes = []
                for i in range(len(l_indexes)):
                    if self.symbol_table[id].size[i] < l_indexes[i]:
                        expanded_array_indexes.append(l_indexes[i])
                    else:
                        expanded_array_indexes.append(self.symbol_table[id].size[i])

                old_size = self.symbol_table[id].size
                if expanded_array_indexes != old_size:  # расширяю
                    (self.symbol_table[id]).size = expanded_array_indexes
                    tmp_array = np.empty(shape=[i + 1 for i in expanded_array_indexes][::-1], dtype=object).flatten()
                    (self.symbol_table[id]).array = (self.symbol_table[id]).array.flatten()

                    tmp_array = tmp_array[len((self.symbol_table[id]).array):]
                    (self.symbol_table[id]).array = np.concatenate((self.symbol_table[id].array, tmp_array), axis=0)

                    (self.symbol_table[id]).array = (self.symbol_table[id]).array.reshape(
                        [i + 1 for i in expanded_array_indexes][::-1])
                    (self.symbol_table[id]).array[tuple(expanded_array_indexes)[::-1]] = r_value
                    return

                else:  # просто записываю значение
                    if (self.symbol_table[id]).array.size == 0:
                        size = (self.symbol_table[id]).size
                        (self.symbol_table[id]).array = np.empty(shape=[i + 1 for i in size][::-1], dtype=object)
                        (self.symbol_table[id]).array[tuple(l_indexes)[::-1]] = r_value
                    else:
                        (self.symbol_table[id]).array[tuple(l_indexes)[::-1]] = r_value
                    return
            else:
                raise AssignmentErrorRight
        else:
            raise AssignmentErrorLeft

    def assign_for_procedure(self, node):

        left_child = node.children[0]
        l_indexes = self.interpreter(left_child)  # если слева - вызов массива # id
        id = int(left_child.value)  # имя переменной
        type = self.declaration[id].type

        if node.children[1].type != "arr_call":  # если переменная
            self.interpreter(node.children[1])  # id
            r_value = self.symbol_table[node.children[1].value]
        else:  # если массив
            right = self.interpreter(node.children[1])  # indexes
            r_value = self.get_array_value(self.declaration[node.children[1].value], right)

        # записываю в таблицу символов значение
        if left_child.type == "var_call":
            self.declaration[id] = r_value
            self.symbol_table[id] = r_value

        elif left_child.type == "arr_call":

            delta = len((self.declaration[id]).size) - len(l_indexes)

            for i in range(abs(delta)):
                if delta > 0:
                    l_indexes.append(0)
                elif delta < 0:
                    self.declaration[id].size.append(0)
                    self.symbol_table[id].size.append(0)

            # новые индексы
            expanded_array_indexes = []
            for i in range(len(l_indexes)):
                if self.declaration[id].size[i] < l_indexes[i]:
                    expanded_array_indexes.append(l_indexes[i])
                else:
                    expanded_array_indexes.append(self.declaration[id].size[i])

            old_size = self.declaration[id].size
            if expanded_array_indexes != old_size:  # расширяю
                (self.declaration[id]).size = expanded_array_indexes
                (self.symbol_table[id]).size = expanded_array_indexes

                tmp_array = np.empty(shape=[i + 1 for i in expanded_array_indexes][::-1], dtype=object).flatten()
                (self.declaration[id]).array = (self.declaration[id]).array.flatten()

                tmp_array = tmp_array[len((self.declaration[id]).array):]
                (self.declaration[id]).array = np.concatenate((self.declaration[id].array, tmp_array), axis=0)

                (self.declaration[id]).array = (self.declaration[id]).array.reshape(
                    [i + 1 for i in expanded_array_indexes][::-1])

                (self.declaration[id]).array[tuple(expanded_array_indexes)[::-1]] = r_value
                (self.symbol_table[id]).array = (self.declaration[id]).array
                return

            else:  # просто записываю значение
                if (self.declaration[id]).array.size == 0:
                    size = (self.declaration[id]).size
                    (self.declaration[id]).array = np.empty(shape=[i + 1 for i in size][::-1], dtype=object)
                    (self.declaration[id]).array[tuple(l_indexes)[::-1]] = r_value
                    (self.symbol_table[id]).array = (self.declaration[id]).array
                else:
                    (self.declaration[id]).array[tuple(l_indexes)[::-1]] = r_value
                    (self.symbol_table[id]).array = (self.declaration[id]).array
                return
        else:
            raise AssignmentErrorLeft

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for identification
    def id_processing(self, node):

        operation = 0 if node.value == "@" else 1

        left = self.symbol_table.get(node.children[0].value)
        l_name = left.name
        r_name = self.symbol_table.get(node.children[1].value)

        if left is None or r_name is None:
            raise NonExistentVariable

        if left.type == "label" or r_name.type != "proc":
            raise IdentificationError

        list_of_procedures = self.id_table.get(l_name)
        if operation == 0:
            # link
            if list_of_procedures is not None:
                if r_name not in list_of_procedures:
                    if not self.recursive_linking_check(l_name, r_name):
                        self.id_table[l_name].append(r_name)
                        return True
            else:
                if not self.recursive_linking_check(l_name, r_name):
                    self.id_table[l_name] = [r_name]
                    return True
            return False
        else:
            # link break
            if self.id_table.get(l_name):
                if r_name in self.id_table[l_name]:
                    self.id_table[l_name].remove(r_name)
            return True

    def recursive_linking_check(self, id, proc):
        res = False
        if self.symbol_table.get(id) == proc:
            return True
        list_of_procedures = self.id_table.get(proc.name)
        if list_of_procedures:
            for p in list_of_procedures:
                res = self.recursive_linking_check(id, p)
        return res

    def procedure_call_chain(self, proc_name):
        action = self.declaration[proc_name]
        if isinstance(action, tuple):
            action = action[0]
        while not (type(action) is Node or proc_name == action.name):
            self.procedure_call_chain(action.name)
        if type(action) is not Node and proc_name == action.name:
            return action.value  # np
        else:
            return action

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for math operation
    def math(self, node, flag):
        # id
        if self.id_table.get(int(node.value)) is not None:
            proc_list = self.id_table[int(node.value)]
            for proc in proc_list:
                action = self.procedure_call_chain(proc.name)
                if action == "np":
                    pass
                elif type(action) is Node:
                    self.current_proc = proc.name
                    self.interpreter(action)
                    self.exit = False
                    self.current_proc = None
                    for tmp_var in self.tmp_variables:
                        del self.symbol_table[tmp_var]
                    self.tmp_variables.clear()
                else:
                    raise ProcedureError

        variable = self.symbol_table.get(int(node.value))


        if variable is None:
            raise NonExistentVariable

        if variable.type != "int":
            raise ArithmeticError

        if variable.value is None:
            raise UninitializedVariable

        if flag == 0:
            self.symbol_table[variable.name].value = str(int(variable.value) - 1)
        else:
            self.symbol_table[variable.name].value = str(int(variable.value) + 1)

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for logical operations
    def pier_arrow(self, next_value, prev_result):
        if not next_value and not prev_result:
            return True
        else:
            return False

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for comparison
    def compare(self, node):
        return_values = []

        for child in node.children:
            if isinstance(child, str) or isinstance(child, int):
                return_values.append(child)
            else:
                if child.type == "incr" or child.type == "decr":
                    self.interpreter(child)
                    return_values.append(self.symbol_table[int(child.value)].value)
                elif child.type == "logics" or child.type == "log_type" or child.type == "comparison" or \
                        child.type == "identification" or child.type == "robot":
                    return_values.append(self.interpreter(child))
                elif child.type == "var_call":
                    self.interpreter(child)  # id
                    if self.symbol_table.get(child.value) is None:
                        raise NonExistentVariable
                    return_values.append(self.symbol_table[child.value].value)
                elif child.type == "arr_call":
                    if self.symbol_table.get(child.value) is None:
                        raise NonExistentVariable
                    indices = self.interpreter(child)
                    return_values.append(self.get_array_value(self.symbol_table[child.value], indices))
                elif child.type == "literal":
                    return_values.append(child.value)
                else:
                    raise ComparisonError

        if len(return_values) != 2:
            raise ComparisonError

        if type(return_values[0]) is Node and type(return_values[1]) is Node:
            raise ComparisonError
        elif (type(return_values[0]) is Node and return_values[1] == 'np') or \
                (return_values[0] == 'np' and type(return_values[1]) is Node):
            return False
        elif type(return_values[0]) != type(return_values[1]):
            raise ComparisonError

        return return_values[0] == return_values[1]

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for label
    def label_jump(self, node):

        if isinstance(node, list):
            node = node[0]

        if self.declaration.get(node.value) is None:
            raise NonExistentVariable

        if isinstance(self.declaration[node.value], tuple) or self.declaration[
            node.value].value is not None:  # уже объявлена (а-ля statement_label_decl, name_curr_proc)
            decl_proc = self.declaration[int(node.value)][1]
            if decl_proc != self.current_proc:
                raise LabelError

            self.current_label = self.declaration[int(node.value)][0]
            leaf_label = None
            for child in self.current_label.children:
                if child.type == "declaration":
                    leaf_label = child.children[0]

            if decl_proc is None and self.current_cycle is None:  # нет процедуры
                #self.interpreter(self.root)
                self.exit = True
                self.interpreter_shell(self.root, leaf_label)
                self.exit = True
            elif decl_proc and self.current_cycle is None:
                self.exit = True
                self.interpreter_shell(self.procedure_call_chain(decl_proc), leaf_label)
                self.exit = True
            elif decl_proc is None and self.current_cycle:
                self.exit = True
                self.interpreter_shell(self.current_cycle, leaf_label)
                self.exit = True

        else:  # вызвана раньше объявления
            self.find_label_declaration(node.value, self.root, self.current_proc)
            self.label_jump(node)
            """
            if self.current_proc is not None:
                self.exit = self.procedure_call_chain(self.current_proc)
            else:
                self.exit = self.root
            return
            """

    def find_label_declaration(self, label_name, root, curr_proc):
        statement_node = []  # узел объявления

        def _find_label_declaration(label_name, node, curr_proc):
            if not node:
                return 0
            if isinstance(node, str):
                return 0
            elif node.type == "statement" or node.type == "statement_block":
                for child in node.children:
                    if child.type == "declaration":
                        for c in child.children:
                            if c.value == label_name:
                                statement_node.append(node)
                                return 1
            for child in node.children:
                if _find_label_declaration(label_name, child, curr_proc):
                    return 1

        _find_label_declaration(label_name, root, curr_proc)

        if len(statement_node) == 0:
            raise LabelError

        statement_node = statement_node[0]

        self.declaration[int(label_name)] = (statement_node, curr_proc)
        self.symbol_table[int(label_name)] = (statement_node, curr_proc)

        return

    def interpreter_shell(self, node, leaf_label):
        if self.exit is True:
            self.bypass(node, leaf_label)
        else:
            self.interpreter(node)

    def bypass(self, node, leaf_label):
        if not node:
            return
        if isinstance(node, str):
            return
        elif node == leaf_label:
            self.exit = False
            return

        for child in node.children:
            if self.interpreter_shell(child, leaf_label):
                return

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ for cycle
    def check_cycle(self, child):
        # вычисляю левое значение
        log_value = None

        if child.type == "logics" or child.type == "log_type" or child.type == "comparison" or \
                child.type == "identification" or child.type == "robot":
            log_value = self.interpreter(child)
        elif child.type == "var_call":
            self.interpreter(child)  # id
            if self.symbol_table.get(child.value) is None:
                raise NonExistentVariable
            if self.symbol_table[child.value].type == "bool":
                log_value = self.symbol_table[child.value].value
        elif child.type == "arr_call":
            if self.symbol_table.get(child.value) is None:
                raise NonExistentVariable
            indices = self.interpreter(child)
            if self.symbol_table[child.value].type == "bool_arr":
                log_value = self.get_array_value(self.symbol_table[child.value], indices)
        elif child.type == "literal":
            log_value = child.value
        else:
            raise CycleError

        return log_value



if __name__ == '__main__':
    s = open("bypass1.txt", "r")
    #s = open("Tests/TEST_var_already_exist.txt", "r")
    it = Interpreter()

    robot = Robot("Robot/Maps/map2.txt")
    it.processing(s.read(), robot)
    print(robot.memorized_path)
    robot.vizualize()

