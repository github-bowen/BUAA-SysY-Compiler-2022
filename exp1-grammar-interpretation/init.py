import os

for i in range(1, 7):
    create_testfile = "touch testfile{}.txt".format(i)
    create_inputfile = "touch input{}.txt".format(i)
    create_outputfile = "touch output{}.txt".format(i)
    os.system(create_testfile)
    os.system(create_inputfile)
    os.system(create_outputfile)
