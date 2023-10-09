import os
import time
from subprocess import call

import matplotlib.pyplot as plt

ORIGINAL_PATH = os.getcwd()
os.chdir("..")
os.chdir("..")
os.chdir("cmake-build-debug")
CMAKE_BUILD_DIR = os.getcwd()
TESTS_WORKING_DIR = os.path.join(CMAKE_BUILD_DIR, "database", "test", "tests")
RESULTS_DIR = os.path.join(ORIGINAL_PATH, "results")


def clear_dir():
    for root, dirs, files in os.walk(os.getcwd()):
        for file in files:
            os.remove(os.path.join(root, file))


def prepare_common(batch_num: int, test_name: str):
    os.chdir(TESTS_WORKING_DIR)
    os.chdir(test_name)
    clear_dir()


def prepare_with_populate(batch_num: int, test_name: str):
    prepare_common(batch_num, test_name)
    call(["../../llp_lab_database_test_stress_insert", str(batch_num)])


def finish_test():
    clear_dir()
    os.chdir("..")


def perform_test(batch_num: int, test_name: str, name: str, prepare_func, constant: bool):
    prepare_func(batch_num, test_name)
    start_time = time.time()
    print("\nPerforming test '{0}'; number of records {1}".format(name, batch_num * 1000))
    print("Program output:\n----------------------------")
    call(["../../llp_lab_database_test_" + test_name, str(10 if constant else batch_num)])
    print("----------------------------")
    file_size = os.path.getsize("database")
    run_time = time.time() - start_time
    print("Program runtime: %s seconds" % run_time)
    print("Database file size: %s bytes" % file_size)
    finish_test()
    return run_time, file_size


def test(test_name: str, prepare_func, constant: bool, name=''):
    if name == '':
        name = test_name

    x = list(range(1, 100))
    y = list(map(lambda num: perform_test(num, test_name, name, prepare_func, constant), x))

    finish_test()

    y_time = list(map(lambda values: values[0], y))
    y_size = list(map(lambda values: values[1], y))

    os.chdir(RESULTS_DIR)
    plt.xlabel("batch number (records * 1000)")
    plt.ylabel("execution time (seconds)")
    plt.title(name + "_time")
    plt.axis([0, x[len(x) - 1], 0, max(y_time)])
    plt.plot(x, y_time)
    plt.savefig(name + "_time")

    plt.clf()
    plt.xlabel("batch number (records * 1000)")
    plt.ylabel("file size (bytes)")
    plt.title(name + "_size")
    plt.axis([0, x[len(x) - 1], 0, max(y_size)])
    plt.plot(x, y_size)
    plt.savefig(name + "_size")


os.chdir(TESTS_WORKING_DIR)
print(os.getcwd())

test("stress_insert", prepare_common, False)
test("stress_update", prepare_with_populate, False)
test("stress_delete", prepare_with_populate, False)
test("stress_query", prepare_with_populate, False)
test("stress_insert_delete", prepare_common, False)
test("stress_insert", prepare_with_populate, True, "const_insert")
