from SC import TIMES, DIV, MOD, AND, PLUS, MINUS, OR, EQ, NE, LT, GT, LE, \
     GE, NOT, TILDE, AMP, BAR, mark
from ST import Var, Ref, Const, Type, Proc, StdProc, Int, Bool


GPregs = {'$t0', '$t1', '$t2', '$t3', '$t4', '$t5', '$t6', '$t7', '$t8'}

def genProgStart():
    global asm, curlev, label, regs
    asm, curlev, label = [], 0, 0
    regs = set(GPregs) # make copy
    putInstr('.data')
    

R0 = '$0'; FP = '$fp'; SP = '$sp'; LNK = '$ra'

def obtainReg():
    if len(regs) == 0: mark('out of registers'); return R0
    else: return regs.pop()

def releaseReg(r):
    if r in GPregs: regs.add(r)
    

def putLab(lab, instr = ''):
    """Emit label lab with optional instruction; lab may be a single
    label or a list of labels"""
    if type(lab) == list:
        for l in lab[:-1]: asm.append((l, '', ''))
        asm.append((lab[-1], instr, ''))
    else: asm.append((lab, instr, ''))

def putInstr(instr, target = ''):
    """Emit an instruction"""
    asm.append(('', instr, target))

def putOp(op, a, b, c):
    """Emit instruction op with three operands, a, b, c; c can be register or immediate"""
    putInstr(op + ' ' + a + ', ' + b + ', ' + str(c))

def putBranchOp(op, a, b, c):
    putInstr(op + ' ' + a + ', ' + b, str(c))

def putMemOp(op, a, b, c):
    """Emit load/store instruction at location or register b + offset c"""
    if b == R0: putInstr(op + ' ' + a + ', ' + str(c))
    else: putInstr(op + ' ' + a + ', ' + str(c) + '(' + b + ')')
    

def genBool(b):
    b.size = 4; return b

def genInt(i):
    i.size = 4; return i

def genRec(r):
    # r is Record
    s = 0
    for f in r.fields:
        f.offset, s = s, s + f.tp.size
    r.size = s
    return r

def genArray(a):
    # a is Array
    a.size = a.length * a.base.size
    return a
    
def genGlobalVars(sc, start):
    for i in range(len(sc) - 1, start - 1, - 1):
        if type(sc[i]) == Var:
            sc[i].reg, sc[i].adr = R0, sc[i].name + '_'
            putLab(sc[i].adr, '.space ' + str(sc[i].tp.size))
            
def genProgEntry(ident):
    putInstr('.text')
    putInstr('.globl main')
    putInstr('.ent main')
    putLab('main')


def assembly(l, i, t):
    """Convert label l, instruction i, target t to assembly format"""
    return (l + ':\t' if l else '\t') + i + (', ' + t if t else '')

def genProgExit(x):
    putInstr('li $v0, 10')
    putInstr('syscall')
    putInstr('.end main')
    return '\n'.join(assembly(l, i, t) for (l, i, t) in asm)
    
def newLabel():
    global label
    label += 1
    return 'L' + str(label)
    
    
class Reg:
    def __init__(self, tp, reg):
        # tp is Bool or Int
        self.tp, self.reg = tp, reg

class Cond:
    # labA, labB are lists of branch targets for when the result is true or false
    def __init__(self, cond, left, right):
        self.tp, self.cond, self.left, self.right = Bool, cond, left, right
        self.labA, self.labB = [newLabel()], [newLabel()]
        
def testRange(x):
    if x.val >= 0x8000 or x.val < -0x8000: mark('value too large')

def loadItemReg(x, r):
    if type(x) == Var: 
        putMemOp('lw', r, x.reg, x.adr); releaseReg(x.reg)
    elif type(x) == Const:
        testRange(x); putOp('addi', r, R0, x.val)
    elif type(x) == Reg: # move to register r
        putOp('add', r, x.reg, R0)
    else: assert False
    
    
def loadItem(x):
    if type(x) == Const and x.val == 0: r = R0 # use R0 for "0"
    else: r = obtainReg(); loadItemReg(x, r)
    return Reg(x.tp, r)

def loadBool(x):
    if type(x) == Const and x.val == 0: r = R0 # use R0 for "false"
    else: r = obtainReg(); loadItemReg(x, r)
    return Cond(NE, r, R0)


def put(cd, x, y):
    if type(x) != Reg: x = loadItem(x)
    if x.reg in (R0, '$a0', '$a1', '$a2', '$a3'): # find new destination register
        r = x.reg; x.reg = obtainReg()
    else: r = x.reg # r is source, x.reg is destination
    if type(y) == Const:
        testRange(y); putOp(cd, x.reg, r, y.val)
    else:
        if type(y) != Reg: y = loadItem(y)
        putOp(cd, x.reg, r, y.reg); releaseReg(y.reg)
    return x
    
    
"""  
def genVar(x): # version not supporting parameters in registers
    if 0 < x.lev < curlev: mark('level!')
    y = Var(x.tp); y.lev = x.lev
    if type(x) == Ref: # reference is loaded into register
        y.reg, y.adr = obtainReg(), 0 # variable at M[y.reg]
        putMemOp('lw', y.reg, x.reg, x.adr)
    elif type(x) == Var:
        y.reg, y.adr = x.reg, x.adr 
    else: assert False
    return y
"""
def genVar(x): # version supporting parameters in registers
    if 0 < x.lev < curlev: mark('level!')
    if type(x) == Ref:
        y = Var(x.tp); y.lev = x.lev
        if x.reg in ('$a0', '$a1', '$a2', '$a3'): # reference already in register, use it
            y.reg, y.adr = x.reg, 0 # variable at M[y.reg]
        else: # reference is loaded into register
            y.reg, y.adr = obtainReg(), 0 # variable at M[y.reg]
            putMemOp('lw', y.reg, x.reg, x.adr)
    elif type(x) == Var:
        if x.reg in ('$a0', '$a1', '$a2', '$a3'): # value already in register, use it
            y = Reg(x.tp, x.reg) #; y.lev, x.adr = x.lev, x.adr
        else:
            y = Var(x.tp); y.lev, y.reg, y.adr = x.lev, x.reg, x.adr
    else: assert False
    return y
    

def genConst(x):
    # x is Const
    return x
    
    
def negate(cd):
    return {EQ: NE, NE: EQ, LT: GE, LE: GT, GT: LE, GE: LT}[cd]

def condOp(cd):
    return {EQ: 'beq', NE: 'bne', LT: 'blt', LE: 'ble', GT: 'bgt', GE: 'bge'}[cd]

#in CGmips, extend genUnaryOp to generate code for bitwise complement when op is TILDE.
#The MIPS architecture does not have an instruction for bitwise complement, only for r nor s,
#which is defined as (r | s). When taking s to be r, the result is r. Follow the pattern for
#generating the code for MINUS.

def genUnaryOp(op, x):
    if op == MINUS: # subtract from 0
        if type(x) == Var: x = loadItem(x)
        putOp('sub', x.reg, R0, x.reg)
    elif op == NOT: # switch condition and branch targets, no code
        if type(x) != Cond: x = loadBool(x)
        x.cond = negate(x.cond); x.labA, x.labB = x.labB, x.labA
    elif op == TILDE: # switch condition and branch targets, no code
        if type(x) == Var: x = loadItem(x)
        putOp('nor', x.reg, R0, x.reg)
    elif op == AND: # load first operand into register and branch
        if type(x) != Cond: x = loadBool(x)
        putBranchOp(condOp(negate(x.cond)), x.left, x.right, x.labA[0])
        releaseReg(x.left); releaseReg(x.right); putLab(x.labB)
    elif op == OR: # load first operand into register and branch
        if type(x) != Cond: x = loadBool(x)
        putBranchOp(condOp(x.cond), x.left, x.right, x.labB[0])
        releaseReg(x.left); releaseReg(x.right); putLab(x.labA)
    else: assert False
    return x
    
#extend genBinaryOp to generate code for & and | when op is AMP and BAR. Follow the pattern
#for arithmetic operations. AMP, BAR
def genBinaryOp(op, x, y):
    if op == PLUS: y = put('add', x, y)
    elif op == MINUS: y = put('sub', x, y)
    elif op == TIMES: y = put('mul', x, y)
    elif op == DIV: y = put('div', x, y)
    elif op == MOD: y = put('mod', x, y)
    elif op == AMP: y = put('and', x, y)
    elif op == BAR: y = put('or', x, y)
    elif op == AND: # load second operand into register 
        if type(y) != Cond: y = loadBool(y)
        y.labA += x.labA # update branch targets
    elif op == OR: # load second operand into register
        if type(y) != Cond: y = loadBool(y)
        y.labB += x.labB # update branch targets
    else: assert False
    return y
    

def genRelation(op, x, y):
    if type(x) != Reg: x = loadItem(x)
    if type(y) != Reg: y = loadItem(y)
    return Cond(op, x.reg, y.reg)
    

def genSelect(x, f):
    x.tp, x.adr = f.tp, x.adr + f.offset if type(x.adr) == int else \
                        x.adr + '+' + str(f.offset)
    return x
    

def genIndex(x, y):
    if type(y) == Const:
        offset = (y.val - x.tp.lower) * x.tp.base.size
        x.adr = x.adr + (offset if type(x.adr) == int else '+' + str(offset))
    else:
        if type(y) != Reg: y = loadItem(y)
        putOp('sub', y.reg, y.reg, x.tp.lower)
        putOp('mul', y.reg, y.reg, x.tp.base.size)
        if x.reg != R0:
            putOp('add', y.reg, x.reg, y.reg); releaseReg(x.reg)
        x.reg = y.reg
    x.tp = x.tp.base
    return x


def genAssign(x, y):
    if type(x) == Var:
        if type(y) == Cond:
            putBranchOp(condOp(negate(y.cond)), y.left, y.right, y.labA[0])
            releaseReg(y.left); releaseReg(y.right); r = obtainReg()
            putLab(y.labB); putOp('addi', r, R0, 1) # load true
            lab = newLabel()
            putInstr('b', lab)
            putLab(y.labA); putOp('addi', r, R0, 0) # load false 
            putLab(lab)
        elif type(y) != Reg: y = loadItem(y); r = y.reg
        else: r = y.reg
        putMemOp('sw', r, x.reg, x.adr); releaseReg(r)
    else: assert False
    

def genLocalVars(sc, start):
    s = 0 # local block size
    for i in range(start, len(sc)):
        if type(sc[i]) == Var:
            s = s + sc[i].tp.size
            sc[i].reg, sc[i].adr = FP, - s - 8
    return s
    

def genProcStart():
    global curlev
    curlev = curlev + 1
    putInstr('.text')
    


def genFormalParams(sc):
    n = len(sc) # parameter block length
    for i in range(n):
        if sc[i].tp in (Int, Bool) or type(sc[i]) == Ref:
            sc[i].reg, sc[i].adr = FP, (n - i - 1) * 4
        else: mark('no structured value parameters')
    return n * 4


def genProcEntry(ident, parsize, localsize):
    putInstr('.globl ' + ident)            # global declaration directive
    putInstr('.ent ' + ident)              # entry point directive
    putLab(ident)                          # procedure entry label
    putMemOp('sw', FP, SP, - parsize - 4)  # push frame pointer
    putMemOp('sw', LNK, SP, - parsize - 8) # push return address
    putOp('sub', FP, SP, parsize)          # set frame pointer
    putOp('sub', SP, FP, localsize + 8)    # set stack pointer

def genProcExit(x, parsize, localsize):
    global curlev
    curlev = curlev - 1
    putOp('add', SP, FP, parsize) # restore stack pointer
    putMemOp('lw', LNK, FP, - 8)  # pop return address
    putMemOp('lw', FP, FP, - 4)   # pop frame pointer
    putInstr('jr $ra')            # return


def genActualPara(ap, fp, n):
    if type(fp) == Ref:  #  reference parameter, assume p is Var
        if ap.adr != 0:  #  load address in register
            r = obtainReg(); putMemOp('la', r, ap.reg, ap.adr)
        else: r = ap.reg  #  address already in register
        putMemOp('sw', r, SP, - 4 * (n + 1)); releaseReg(r)
    else:  #  value parameter
        if type(ap) != Cond:
            if type(ap) != Reg: ap = loadItem(ap)
            putMemOp('sw', ap.reg, SP, - 4 * (n + 1)); releaseReg(ap.reg)
        else: mark('unsupported parameter type')
        

def genCall(pr, ap):
    putInstr('jal', pr.name)
    

def genRead(x):
    putInstr('li $v0, 5'); putInstr('syscall')
    putMemOp('sw', '$v0', x.reg, x.adr)

def genWrite(x):
    loadItemReg(x, '$a0'); putInstr('li $v0, 1'); putInstr('syscall')

def genWriteln():
    putInstr('li $v0, 11'); putInstr("li $a0, '\\n'"); putInstr('syscall')
    

def genSeq(x, y):
    pass

def genCond(x):
    if type(x) != Cond: x = loadBool(x)
    putBranchOp(condOp(negate(x.cond)), x.left, x.right, x.labA[0])
    releaseReg(x.left); releaseReg(x.right); putLab(x.labB)
    return x

def genIfThen(x, y):
    putLab(x.labA)

def genThen(x, y):
    lab = newLabel()
    putInstr('b', lab)
    putLab(x.labA); 
    return lab

def genIfElse(x, y, z):
    putLab(y)

def genTarget():
    lab = newLabel()
    putLab(lab)
    return lab

def genWhile(lab, x, y):
    putInstr('b', lab)
    putLab(x.labA); 
    

