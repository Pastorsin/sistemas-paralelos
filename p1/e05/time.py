import os
import subprocess

TIMES = 10

os.system('gcc -O3 quadatric2.c -o opt-quadatric2.out -lm')
os.system('gcc -O3 quadatric3.c -o opt-quadatric3.out -lm')

os.system('gcc -O0 quadatric2.c -o nopt-quadatric2.out -lm')
os.system('gcc -O0 quadatric3.c -o nopt-quadatric3.out -lm')


def run(cmd):
    result = subprocess.run(
        cmd,
        stdout=subprocess.PIPE
    )

    return result.stdout.decode('utf-8')


def get_double_time(output):
    return output.split(':')[1].split('\n')[0]


def get_float_time(output):
    return output.split(':')[-1].split('\n')[0]


def get_time(cmd):
    output = run(cmd)
    double_time = get_double_time(output)
    float_time = get_float_time(output)

    return float(double_time), float(float_time)


def total_time(cmd):
    total_double = 0
    total_float = 0

    for i in range(TIMES):
        double_time, float_time = get_time(cmd)

        total_double += double_time
        total_float += float_time

    return total_double, total_float

# Run with optimization


if __name__ == '__main__':

    print(f'Run scripts {TIMES} times')
    print('-' * 20)

    print('- With optimization (-O3):')

    print('-- Quadatric2')
    total_double, total_float = total_time('./opt-quadatric2.out')

    print(f'--- Double: {total_double/TIMES}')
    print(f'--- Float: {total_float/TIMES}')

    print('-- Quadatric3')
    total_double, total_float = total_time('./opt-quadatric3.out')

    print(f'--- Double: {total_double/TIMES}')
    print(f'--- Float: {total_float/TIMES}')

    print('-' * 20)

    print('- Without optimization (-O0):')

    print('-- Quadatric2')
    total_double, total_float = total_time('./nopt-quadatric2.out')

    print(f'--- Double: {total_double/TIMES}')
    print(f'--- Float: {total_float/TIMES}')

    print('-- Quadatric3')

    total_double, total_float = total_time('./nopt-quadatric3.out')

    print(f'--- Double: {total_double/TIMES}')
    print(f'--- Float: {total_float/TIMES}')
