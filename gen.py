"""Generate individuals from GP/ML."""

import os

OUTPUTS = 'gp_outputs'
TEMP = 'temp{}.txt'.format(os.getpid())

def main():
    if not os.path.exists(OUTPUTS):
        os.makedirs(OUTPUTS)

    best = float('-inf')

    while True:
        try:
            score = float(os.popen('bin/GP -o {}'.format(TEMP)).read())
            print('Just got', score, end='')
            if score > best:
                print(' -- New best!', end='')
                best = score
            if score > 0 or score >= best:
                print(' (saving)', end='')
                os.rename(TEMP, os.path.join(OUTPUTS, '{}.inp'.format(score)))
            print()
        except KeyboardInterrupt:
            os.remove(TEMP)
            break

if __name__ == '__main__':
    main()
