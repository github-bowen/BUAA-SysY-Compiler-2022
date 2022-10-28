import os
try:
    for i in range(1, 7):
        newName = "testfile{}.txt".format(i)
        oldName = "testfile{}.c".format(i)
        os.rename(oldName, newName)
except FileNotFoundError:
    print("execution failed!")
else:
    print("execution succeeded!")
