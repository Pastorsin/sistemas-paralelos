import subprocess

THREADS = 3
ARRAY_SIZE = 524288
SCRIPTS = ['a.out', 'o.out', 's.out']

TIMES = 1000


def run(cmd):
    result = subprocess.run(
        cmd,
        shell=True,
        check=True,
        stdout=subprocess.PIPE
    )

    return result.stdout.decode('utf-8')


def get_time(output):
    first_line = output.split('\n')[0]
    return float(first_line)


def main():
    result = {}

    for script in SCRIPTS:
        result[script] = []

        for i in range(TIMES):
            cmd = f"./{script} {ARRAY_SIZE} {THREADS}"
            output = run(cmd)
            time = get_time(output)

            result[script].append(time)

        average = sum(result[script]) / TIMES

        print(f'{script} --> {average}')


if __name__ == '__main__':
    main()
