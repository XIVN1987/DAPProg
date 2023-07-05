#! python3
import os


for file in os.listdir(os.getcwd()):
    if os.path.isfile(file) and file.endswith('.bin'):
        name, exe = os.path.splitext(file)

        bs = open(file, 'rb').read()

        ss = 'uint8_t %s[%d] = {' %(name, len(bs))
        for i in range(len(bs)):
            if i%16 == 0: ss += '\n\t'
            ss += '0x%02X, ' %bs[i]
        ss += '\n};\n'

        open(f'{name}.c', 'w').write(ss)
