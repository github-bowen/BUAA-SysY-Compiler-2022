# BUAA-SysY-Compiler-2022

[中文版](./README_CN.md)

> Note: BUAA course team strictly check the code, this repository code is for reference only, please don't copy it!

## Introduction

This project is a compiler for the BUAA compilation course major assignment - SysY language (a subset of C) based on the MIPS architecture. The compiler converts input SysY language code into MIPS language assembly code that can run on MARS.

[Design Document](./docs/System_Design.md)

## Usage

Windows system users can download `compiler.zip` compressed file in [GitHub Release](https://github.com/NormalLLer/BUAA-SysY-Compiler-2022/releases/tag/v1.0), which contains the Execute the file `compiler.exe` and the sample SysY language source code `testfile.txt`, run `compiler.exe` directly in `cmd`:

```
compiler.exe
```

This program compiles the SysY language code from ``testfile.txt`` and outputs the MIPS language assembly code to ``mips.txt``.

### Example

The SysY language codes in `testfile.txt` are as follows:

```c
int a,b,c,d,e,i=0;

int func0(int a,int b) {
    int i = 0;
    int c = 128;
    return (a+b)%c;
}

int func1(int a) {
    i = i + 1;
    return i;
}

int func2(int a,int b) {
    if (a % b == 0) {
        return 1;
    }
    return 0;
}

int func3() {
    printf("glo_i = %d\n",i);
    int tt0,tt1,t2,tt3,tt4,v=1906;
    while (i < 10000) {
        int v = a * 4 * 32 * a / a / 32;
        b = func0(b,v);
        tt0 = a*4 + b + c ;
        tt1 = a*4 + b + c + d;
        t2 = a*4 + b + c + d + e;
        tt3 = a*4 + b + c + d + e;
        tt4 = a*4 + b + c + d + e;
        if (func2(i,1000)) {
            printf("sum = %d\n", tt0 + tt1 + t2 + tt3 + tt4);
        }
        func1(i);
    }
    return tt0 + tt1 + t2 + tt3 + tt4;
}

int main() {
    int i = 0;
    a = getint();
    b = getint();
    c = getint();
    d = getint();
    e = getint();
    i = getint();
    printf("main_i = %d\n",i);
    printf("%d\n",func3());
    return 0;
}
```

The MIPS code output after running `compiler.exe` is as follows:

```assembly
.data 0x10010000
temp:  .space  160000


#a
global_var1: .word  0

#b
global_var2: .word  0

#c
global_var3: .word  0

#d
global_var4: .word  0

#e
global_var5: .word  0

#i
global_var6: .word  0

# string tokens: 
str_1:  .asciiz   "glo_i = "
str_3:  .asciiz   "\n"
str_4:  .asciiz   "sum = "
str_6:  .asciiz   "\n"
str_7:  .asciiz   "main_i = "
str_9:  .asciiz   "\n"
str_11:  .asciiz   "\n"


.text 0x00400000

# main function
# local var defition:
li $t0, 0
sw $t0, 268500992($zero)

li $v0, 5
syscall
la $t9, global_var1
sw $v0, 0($t9)

li $v0, 5
syscall
la $t9, global_var2
sw $v0, 0($t9)

li $v0, 5
syscall
la $t9, global_var3
sw $v0, 0($t9)

li $v0, 5
syscall
la $t9, global_var4
sw $v0, 0($t9)

li $v0, 5
syscall
la $t9, global_var5
sw $v0, 0($t9)

li $v0, 5
syscall
sw $v0, 268500992($zero)
la $a0, str_7
li $v0, 4
syscall
lw $a0, 268500992($zero)
li $v0, 1
syscall
la $a0, str_9
li $v0, 4
syscall


# Pushing Function Real Params:
addiu $sp, $sp, -30000
sw $ra, 0($sp)
# Call function!
jal Label_5
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 30000

sw $v0, 268500996($zero)
lw $a0, 268500996($zero)
li $v0, 1
syscall
la $a0, str_11
li $v0, 4
syscall

li $v0, 10
syscall




# self defined functions

# ---------------- FUCNTION: func0 ------------------
Label_1:

# local var defition:
li $t0, 0
sw $t0, 12($sp)
# local var defition:
li $t0, 128
sw $t0, 16($sp)
lw $t0, 4($sp)
lw $t1, 8($sp)
addu $t2, $t0, $t1
sw $t2, 20($sp)
lw $t0, 20($sp)
lw $t1, 16($sp)
div $t0, $t1
mfhi $t2
sw $t2, 24($sp)
lw $v0, 24($sp)
jr $ra
jr $ra



# ---------------- FUCNTION: func1 ------------------
Label_2:

la $t0, global_var6
lw $t0, 0($t0)
li $t1, 1
addu $t2, $t0, $t1
sw $t2, 8($sp)
lw $t0, 8($sp)
la $t9, global_var6
sw $t0, 0($t9)
la $v0, global_var6
lw $v0, 0($v0)
jr $ra
jr $ra



# ---------------- FUCNTION: func3 ------------------
Label_5:

la $a0, str_1
li $v0, 4
syscall
la $a0, global_var6
lw $a0, 0($a0)
li $v0, 1
syscall
la $a0, str_3
li $v0, 4
syscall
# local var defition:
# local var defition:
# local var defition:
# local var defition:
# local var defition:
# local var defition:
li $t0, 1906
sw $t0, 24($sp)
Label_6:
la $t0, global_var6
lw $t0, 0($t0)
li $t1, 10000
slt $t2, $t0, $t1
sw $t2, 28($sp)
lw $t0, 28($sp)
beqz $t0, Label_7
la $t0, global_var1
lw $t0, 0($t0)
li $t1, 4
mul $t2, $t0, $t1
sw $t2, 32($sp)
lw $t0, 32($sp)
li $t1, 32
mul $t2, $t0, $t1
sw $t2, 36($sp)
lw $t0, 36($sp)
la $t1, global_var1
lw $t1, 0($t1)
mul $t2, $t0, $t1
sw $t2, 40($sp)
lw $t0, 40($sp)
la $t1, global_var1
lw $t1, 0($t1)
div $t0, $t1
mflo $t2
sw $t2, 44($sp)
lw $t0, 44($sp)
li $t1, 32
div $t0, $t1
mflo $t2
sw $t2, 48($sp)
# local var defition:
lw $t0, 48($sp)
sw $t0, 52($sp)


# Pushing Function Real Params:
addiu $sp, $sp, -30000
sw $ra, 0($sp)
la $t0, global_var2
lw $t0, 0($t0)
sw $t0, 4($sp)
lw $t0, 30052($sp)
sw $t0, 8($sp)
# Finished Pushing Params!
# Call function!
jal Label_1
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 30000

sw $v0, 56($sp)
lw $t0, 56($sp)
la $t9, global_var2
sw $t0, 0($t9)
la $t0, global_var1
lw $t0, 0($t0)
li $t1, 4
mul $t2, $t0, $t1
sw $t2, 60($sp)
lw $t0, 60($sp)
la $t1, global_var2
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 64($sp)
lw $t0, 64($sp)
la $t1, global_var3
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 68($sp)
lw $t0, 68($sp)
sw $t0, 4($sp)
la $t0, global_var1
lw $t0, 0($t0)
li $t1, 4
mul $t2, $t0, $t1
sw $t2, 72($sp)
lw $t0, 72($sp)
la $t1, global_var2
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 76($sp)
lw $t0, 76($sp)
la $t1, global_var3
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 80($sp)
lw $t0, 80($sp)
la $t1, global_var4
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 84($sp)
lw $t0, 84($sp)
sw $t0, 8($sp)
la $t0, global_var1
lw $t0, 0($t0)
li $t1, 4
mul $t2, $t0, $t1
sw $t2, 88($sp)
lw $t0, 88($sp)
la $t1, global_var2
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 92($sp)
lw $t0, 92($sp)
la $t1, global_var3
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 96($sp)
lw $t0, 96($sp)
la $t1, global_var4
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 100($sp)
lw $t0, 100($sp)
la $t1, global_var5
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 104($sp)
lw $t0, 104($sp)
sw $t0, 12($sp)
la $t0, global_var1
lw $t0, 0($t0)
li $t1, 4
mul $t2, $t0, $t1
sw $t2, 108($sp)
lw $t0, 108($sp)
la $t1, global_var2
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 112($sp)
lw $t0, 112($sp)
la $t1, global_var3
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 116($sp)
lw $t0, 116($sp)
la $t1, global_var4
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 120($sp)
lw $t0, 120($sp)
la $t1, global_var5
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 124($sp)
lw $t0, 124($sp)
sw $t0, 16($sp)
la $t0, global_var1
lw $t0, 0($t0)
li $t1, 4
mul $t2, $t0, $t1
sw $t2, 128($sp)
lw $t0, 128($sp)
la $t1, global_var2
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 132($sp)
lw $t0, 132($sp)
la $t1, global_var3
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 136($sp)
lw $t0, 136($sp)
la $t1, global_var4
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 140($sp)
lw $t0, 140($sp)
la $t1, global_var5
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 144($sp)
lw $t0, 144($sp)
sw $t0, 20($sp)


# Pushing Function Real Params:
addiu $sp, $sp, -30000
sw $ra, 0($sp)
la $t0, global_var6
lw $t0, 0($t0)
sw $t0, 4($sp)
li $t0, 1000
sw $t0, 8($sp)
# Finished Pushing Params!
# Call function!
jal Label_3
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 30000

sw $v0, 148($sp)
lw $t0, 148($sp)
beqz $t0, Label_8
lw $t0, 4($sp)
lw $t1, 8($sp)
addu $t2, $t0, $t1
sw $t2, 152($sp)
lw $t0, 152($sp)
lw $t1, 12($sp)
addu $t2, $t0, $t1
sw $t2, 156($sp)
lw $t0, 156($sp)
lw $t1, 16($sp)
addu $t2, $t0, $t1
sw $t2, 160($sp)
lw $t0, 160($sp)
lw $t1, 20($sp)
addu $t2, $t0, $t1
sw $t2, 164($sp)
la $a0, str_4
li $v0, 4
syscall
lw $a0, 164($sp)
li $v0, 1
syscall
la $a0, str_6
li $v0, 4
syscall
Label_8:


# Pushing Function Real Params:
addiu $sp, $sp, -30000
sw $ra, 0($sp)
la $t0, global_var6
lw $t0, 0($t0)
sw $t0, 4($sp)
# Finished Pushing Params!
# Call function!
jal Label_2
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 30000

j Label_6
Label_7:
lw $t0, 4($sp)
lw $t1, 8($sp)
addu $t2, $t0, $t1
sw $t2, 168($sp)
lw $t0, 168($sp)
lw $t1, 12($sp)
addu $t2, $t0, $t1
sw $t2, 172($sp)
lw $t0, 172($sp)
lw $t1, 16($sp)
addu $t2, $t0, $t1
sw $t2, 176($sp)
lw $t0, 176($sp)
lw $t1, 20($sp)
addu $t2, $t0, $t1
sw $t2, 180($sp)
lw $v0, 180($sp)
jr $ra
jr $ra



# ---------------- FUCNTION: func2 ------------------
Label_3:

lw $t0, 4($sp)
lw $t1, 8($sp)
div $t0, $t1
mfhi $t2
sw $t2, 12($sp)
lw $t0, 12($sp)
li $t1, 0
seq $t2, $t0, $t1
sw $t2, 16($sp)
lw $t0, 16($sp)
beqz $t0, Label_4
li $v0, 1
jr $ra
Label_4:
li $v0, 0
jr $ra
jr $ra
```

