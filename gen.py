"""Generate individuals from GP/ML."""

import os

OUTPUTS = 'gp_outputs'
TEMP = 'temp.txt'

def main():
    if not os.path.exists(OUTPUTS):
        os.makedirs(OUTPUTS)

    best = float('-inf')

    while True:
        try:
            score = float(os.popen('bin/GP -o {}'.format(TEMP)).read())
            print('Just got', score)
            if score > best:
                print('^ New best!')
                best = score
                os.rename(TEMP, os.path.join(OUTPUTS, '{}.inp'.format(score)))
        except KeyboardInterrupt:
            os.remove(TEMP)
            break

if __name__ == '__main__':
    main()
