import textwrap
from SC import mark

def indent(n):
    return textwrap.indent(str(n), '  ')
    
class Var:
    def __init__(self, tp):
        self.tp = tp
    def __str__(self):
        return 'var ' + str(getattr(self, 'name', '')) + ' lev ' + \
               str(getattr(self, 'lev', '')) + ':\n' + indent(self.tp)

class Ref:
    def __init__(self, tp):
        self.tp = tp
    def __str__(self):
        return 'ref ' + str(getattr(self, 'name', '')) + ' lev ' + \
               str(getattr(self, 'lev', '')) + ': ' + str(self.tp)

class Const:
    def __init__(self, tp, val):
        self.tp, self.val = tp, val
    def __str__(self):
        return 'const ' + str(getattr(self, 'name', '')) + ': ' + \
               str(self.tp) + ' = ' + str(self.val)

class Type:
    def __init__(self, tp):
        self.tp, self.val = None, tp
    def __str__(self):
        return 'type ' + str(getattr(self, 'name', '')) + indent(self.val)

class Proc:
    def __init__(self, par):
        self.tp, self.par = None, par
    def __str__(self):
        return 'proc ' + self.name + ' lev ' + str(self.lev) + \
               '(' + str([str(s) for s in self.par]) + ')'

class StdProc:
    def __init__(self, par):
        self.tp, self.par = None, par
    def __str__(self):
        return 'stdproc ' + self.name + ' lev ' + str(self.lev) + ' par\n' + \
               indent([str(s) for s in self.par])
               
               
class Int: pass

class Bool: pass

class Enum: pass # for adding enumeration types

class Record:
    def __init__(self, fields):
        self.fields = fields
    def __str__(self):
        return 'record\n' + \
               indent('\n'.join(str(f) for f in self.fields))

class Array:
    def __init__(self, base, lower, length):
        self.base, self.lower, self.length = base, lower, length
    def __str__(self):
        return 'array lower ' + str(self.lower) + ' length ' + \
               str(self.length) + ' base\n' + indent(self.base)
               
               
def init():
    global symTab
    symTab = [[]]

def printSymTab():
    for l in symTab:
        for e in l: print(e)
        print()

def newDecl(name, entry):
    top, entry.lev, entry.name = symTab[0], len(symTab) - 1, name
    for e in top:
        if e.name == name:
            mark("multiple definition"); return
    top.append(entry)

def find(name):
    for l in symTab:
        for e in l:
            if name == e.name: return e
    mark('undefined identifier ' + name)
    return Const(None, 0)

def openScope():
    symTab.insert(0, [])

def topScope():
    return symTab[0]

def closeScope():
    symTab.pop(0)
