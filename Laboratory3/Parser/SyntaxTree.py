import pydot

count = 0


class Node:

    def __init__(self, type="unspecified", value=None, children=None, lineno=None):

        global count
        self.type = type
        self.value = value
        self.lineno = lineno
        self.id = str(count)
        count += 1

        if not children:
            self.children = []
        elif hasattr(children, '__len__'):
            self.children = children
        else:
            self.children = [children]

    def __str__(self):
        message = f'{self.id} '
        if self.type != "unspecified":
            message += f'{self.type} : '
        if self.value is not None:
            message += f'{self.value}'
        if self.lineno is not None:
            message += f' (l:{self.lineno})'

        return message

    def print_tree(self, dot=None, parent=None):
        if not dot:
            dot = pydot.Dot(graph_type='digraph')

            dot.set_node_defaults(color='lightpink',
                                style='filled',
                                shape='ellipse',
                                fontname='colibri',
                                fontsize='10')
        else:
            # dot.add_node(pydot.Node(str(self)))
            for node in self.children:
                dot.add_edge(pydot.Edge(str(self), str(node))) # было parent self

        for c in self.children:
            if isinstance(c, Node):
                c.print_tree(dot=dot, parent=self)
            """
            else:
                n = Node(value=c)
                n.print_tree(dot=dot, parent=self) # здесь написать строковое представление для узла
            """

        dot.write_png('tree.png')
        return dot
