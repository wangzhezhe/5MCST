import graphviz
g = graphviz.Digraph('hello',format='pdf')

g.node("Hello")
g.node("World")
g.edge('Hello', 'World', label='123')
g.edge('World', 'Hello', label='456')
print(g)

g.render()

g.render(directory='doctest-output',view=True)
