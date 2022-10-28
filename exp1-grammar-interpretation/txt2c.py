import os
try:
    for i in range(1, 7):
        oldName = "testfile{}.txt".format(i)
        newName = "testfile{}.c".format(i)
        os.rename(oldName, newName)
except FileNotFoundError:
    print("execution failed!")
else:
    print("execution succeeded!")
