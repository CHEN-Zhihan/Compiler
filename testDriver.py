from os import listdir
import os
import subprocess


def buildSuite(name):
    oracle = open(name, "r")
    lines = oracle.readlines()
    expected = []
    i = 0
    while i < len(lines):
        result = ""
        while i != len(lines) and lines[i][0] != '-'\
                and lines[i][0] != "\n":
            result += lines[i]
            i += 1
        i += 1
        if len(result) != 0:
            expected.append(result)
    oracle.close()
    return expected


def getResult(inputFile, code):
    inp = open(inputFile, "r")
    lines = inp.readlines()
    i = 0
    case = []
    result = []
    if len(lines) == 0:
        sp = subprocess.Popen(["./cvm", code],
                              stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        return [], [sp.communicate()[0].decode("utf8")]
    while i < len(lines):
        out = []
        test = ""
        sp = subprocess.Popen(["./cvm", code],
                              stdout=subprocess.PIPE, stdin=subprocess.PIPE)
        while i != len(lines) and lines[i][0] != '-':
            out.append(sp.communicate(input=lines[i].encode("utf8"))[0])
            test += lines[i] + "\n"
            i += 1
        i += 1
        result.append(b''.join(out).decode("utf8"))
        case.append(test)
    inp.close()
    return case, result


def testCorrect(case):
    out = open(case + ".as", "w")
    sp = subprocess.Popen(["./c6", "test/" + case + ".sc"], stdout=out)
    sp.wait()
    out.flush()
    oracle = buildSuite("test/out/" + case + ".out")
    test, result = getResult("test/in/" + case + ".in", case + ".as")
    for i in range(len(oracle)):
        try:
            assert(oracle[i] == result[i])
        except AssertionError:
            print("[ERROR] expect: ")
            print(oracle[i])
            print("got")
            print(result[i])
            if (i < len(test)):
                print("input")
                print(test[i])
            import pdb
            pdb.set_trace()
        else:
            print("{} {}/{} passed".format(case, i + 1, len(oracle)))
    out.close()
    os.remove(case + ".as")


def testError(case):
    sp = subprocess.Popen(
        ["./c6", "test/" + case + ".sc"], stderr=subprocess.PIPE)
    out = sp.communicate()[1]
    with open("test/out/" + case + ".out", "r") as f:
        line = f.readline()
        try:
            assert(line == out.decode("utf8"))
        except AssertionError:
            print("[ERROR] expect: ")
            print(line)
            print("got")
            print(out.decode("utf8"))
            import pdb
            pdb.set_trace()
        else:
            print(case + " passed")


def main():
    subprocess.run(["make"])
    cases = list(filter(lambda x: x[-3:] == ".sc", listdir("test")))
    for case in cases:
        temp = case[:-3]
        if case[:2] == "er":
            testError(temp)
        else:
            testCorrect(temp)


if __name__ == "__main__":
    main()
