#!/usr/bin/env python3

import re
from collections import namedtuple

Instruction = namedtuple('Instruction', ['opcode', 'tokens'])
LABEL = r'\.(?P<label>[a-zA-Z_][a-zA-Z_0-9]*)'
ADDR = r'0[xX]([0-9a-fA-F]{3})'
BYTE = r'0[xX]([0-9a-fA-F]{2})'
NIBBLE = r'0[xX]([0-9a-fA-F])'
REG = r'V([0-9a-fA-F])'
START_ADDR = 0x200
BYTES_PER_OP = 0x2
instruction_table = (
    Instruction('0%s',     ('SYS',  ADDR)),
    Instruction('00E0',    ('CLS',)),
    Instruction('00EE',    ('RET',)),
    Instruction('1%s',     ('JMP',  ADDR)),
    Instruction('2%s',     ('CALL', ADDR)),
    Instruction('3%s%s',   ('SEQ',  REG,  BYTE)),
    Instruction('4%s%s',   ('SNE',  REG,  BYTE)),
    Instruction('5%s%s0',  ('SEQ',  REG,  REG)),
    Instruction('6%s%s',   ('LD',   REG,  BYTE)),
    Instruction('7%s%s',   ('ADD',  REG,  BYTE)),
    Instruction('8%s%s0',  ('LD',   REG,  REG)),
    Instruction('8%s%s1',  ('AND',  REG,  REG)),
    Instruction('8%s%s2',  ('OR',   REG,  REG)),
    Instruction('8%s%s3',  ('XOR',  REG,  REG)),
    Instruction('8%s%s4',  ('ADD',  REG,  REG)),
    Instruction('8%s%s5',  ('SUB',  REG,  REG)),
    Instruction('8%s%s6',  ('SHR',  REG,  REG)),
    Instruction('8%s%s7',  ('SUBN', REG,  REG)),
    Instruction('8%s%sE',  ('SHL',  REG,  REG)),
    Instruction('9%s%s0',  ('SNE',  REG,  REG)),
    Instruction('A%s',     ('LD',   'I',  ADDR)),
    Instruction('B%s',     ('JMP',  'V0', ADDR)),
    Instruction('C%s%s',   ('RND',  REG,  BYTE)),
    Instruction('D%s%s%s', ('DRAW', REG,  REG, NIBBLE)),
    Instruction('E%s9E',   ('SKP',  REG)),
    Instruction('E%sA1',   ('SKNP', REG)),
    Instruction('F%s07',   ('LD',   REG,  'DT')),
    Instruction('F%s0A',   ('LD',   'K',  REG)),
    Instruction('F%s15',   ('LD',   'DT', REG)),
    Instruction('F%s18',   ('LD',   'ST', REG)),
    Instruction('F%s1E',   ('ADD',  'I',  REG)),
    Instruction('F%s29',   ('LD',   'F',  REG)),
    Instruction('F%s33',   ('LD',   'B',  REG)),
    Instruction('F%s55',   ('LD',   r'\[I\]', REG)),
    Instruction('F%s65',   ('LD',   REG,  r'\[I\]')),
)


class Parser:

    def __init__(self):
        self.line_no = 0
        self.instructions = []
        self.labels = {}
        self.re_table = [
            re.compile(r'[ \t]+'.join(i.tokens)) for i in instruction_table
        ]

    def parse_line(self, line):

        self.line_no += 1

        line = line.strip().upper()

        if not line or line.startswith(';'):
            # ignore blank lines and comments
            return

        elif line.startswith('.'):
            # store label and index of next instruction
            match = re.match(LABEL, line)
            if not match:
                print('invalid label at line %d'
                      % (self.line_no))
                return

            label = match.group('label')
            if label in self.labels:
                print('label %s at line %d already defined'
                      % (label, self.line_no))
                return
            self.labels[label] = len(self.instructions)

        else:
            self.instructions.append(line)

    def resolve_labels(self):
        for i in range(len(self.instructions)):
            match = re.match(r'(JMP|CALL)[ \t]+%s'
                             % LABEL, self.instructions[i])
            if match:
                cmd = match.group(1)
                label = match.group('label')
                addr = 0x200 + self.labels[label] * 2
                self.instructions[i] = '%s %#03X' % (cmd, addr)

    def generate_opcodes(self):
        opcodes = []
        for instruction in self.instructions:
            for i in range(len(self.re_table)):

                match = self.re_table[i].match(instruction)
                if match:
                    op = instruction_table[i].opcode % match.groups()
                    opcodes.append(op)
                    break
        return opcodes


def encode(opcodes):
    hex = ' '.join(opcodes)
    return bytes.fromhex(hex)


def test():
    p = Parser()
    lines = '''
; look, a comment
LD V0 0x00
.START
ADD V0 0x01
SNE V0 0xff
JMP .START
.END
RET
    '''.split('\n')
    p.parse_lines(lines)

    print(p.instructions)
    p.resolve_labels()
    print(p.instructions)

    opcodes = p.generate_opcodes()
    print(opcodes)


def main(args):
    p = Parser()
    pass

if __name__ == '__main__':
    import sys
    args = sys.argv
    main(args)
