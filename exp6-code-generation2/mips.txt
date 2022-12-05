.data 0x10010000
temp:  .space  160000


# a1
global_const_var1:  .word  2

# d1
global_const_var2:  .word  3

# e1
global_const_var3:  .word  4

# h1
global_const_var4:  .word  3

# t1
global_const_var5:  .word  0

# t2
global_const_var6:  .word  1

# b1[5]
global_const_arr1:  .word  1, 2, 3, 4, 5

# c1[2][2]
global_const_arr2:  .word  1, 2, 3, 4

#test
global_var7: .word  0

# string tokens: 
str_1:  .asciiz   "20373275\n"
str_3:  .asciiz   "\n"
str_4:  .asciiz   "m:"
str_6:  .asciiz   "\n"
str_8:  .asciiz   "\n"
str_10:  .asciiz   "\n"


.text 0x00400000

# main function
la $a0, str_1
li $v0, 4
syscall
# local var defition:
# local var defition:
li $t0, 2
sw $t0, 268500996($zero)
# local var defition:
li $t0, 3
sw $t0, 268501000($zero)
# local var defition:
# local var defition:
# local var defition:
li $t0, 0
sw $t0, 268501012($zero)
# local array defition:
# local array defition:
li $t1, 268501036
li $t0, 1
sw $t0, 0($t1)
li $t0, 2
sw $t0, 4($t1)
li $t0, 3
sw $t0, 8($t1)
li $t0, 4
sw $t0, 12($t1)
li $t0, 5
sw $t0, 16($t1)
# local array defition:
li $t1, 268501056
li $t0, 1
sw $t0, 0($t1)
li $t0, 2
sw $t0, 4($t1)
li $t0, 3
sw $t0, 8($t1)
li $t0, 4
sw $t0, 12($t1)


# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
# Call function!
jal Label_1
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000



# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
li $t0, 268501036
sw $t0, 4($sp)
# Finished Pushing Params!
# Call function!
jal Label_5
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000



# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
li $t0, 268501056
sw $t0, 4($sp)
# Finished Pushing Params!
# Call function!
jal Label_6
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000



# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
li $t9, 1
sll $t9, $t9, 2
addiu $t0, $t9, 268501036
lw $t0, 0($t0)
sw $t0, 4($sp)
li $t9, 2
sll $t9, $t9, 2
addiu $t0, $t9, 268501036
lw $t0, 0($t0)
sw $t0, 8($sp)
li $t9, 3
sll $t9, $t9, 2
addiu $t0, $t9, 268501036
lw $t0, 0($t0)
sw $t0, 12($sp)
# Finished Pushing Params!
# Call function!
jal Label_7
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000



# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
li $t9, 0
sll $t9, $t9, 2
addiu $t0, $t9, 268501036
lw $t0, 0($t0)
sw $t0, 4($sp)
li $t9, 1
sll $t9, $t9, 2
addiu $t0, $t9, 268501036
lw $t0, 0($t0)
sw $t0, 8($sp)
# Finished Pushing Params!
# Call function!
jal Label_3
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000

sw $v0, 268501072($zero)
lw $a0, 268501072($zero)
li $v0, 1
syscall
la $a0, str_3
li $v0, 4
syscall
li $t8, 0
li $t9, 0
mul $t8, $t8, 2
addu $t9, $t8, $t9
sll $t9, $t9, 2
addiu $t0, $t9, 268501056
lw $t0, 0($t0)
li $t1, 1
seq $t2, $t0, $t1
sw $t2, 268501076($zero)
lw $t0, 268501076($zero)
beqz $t0, Label_8
# local var defition:

li $v0, 5
syscall
sw $v0, 268501080($zero)
la $a0, str_4
li $v0, 4
syscall
lw $a0, 268501080($zero)
li $v0, 1
syscall
la $a0, str_6
li $v0, 4
syscall
j Label_9
Label_8:
Label_9:
j Label_11


# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
# Call function!
jal Label_4
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000

sw $v0, 268501084($zero)
Label_11:
j Label_10
Label_10:
la $a0, global_var7
lw $a0, 0($a0)
li $v0, 1
syscall
la $a0, str_8
li $v0, 4
syscall
j Label_13


# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
# Call function!
jal Label_4
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000

sw $v0, 268501088($zero)
Label_13:
Label_12:
la $a0, global_var7
lw $a0, 0($a0)
li $v0, 1
syscall
la $a0, str_10
li $v0, 4
syscall
li $t0, -20
sw $t0, 268501092($zero)
lw $t0, 268501092($zero)
sw $t0, 268501000($zero)
li $t0, 30
sw $t0, 268501000($zero)
lw $t0, 268501000($zero)
li $t1, 1
addu $t2, $t0, $t1
sw $t2, 268501096($zero)
lw $t0, 268501096($zero)
li $t1, 1
addu $t2, $t0, $t1
sw $t2, 268501100($zero)
lw $t0, 268501100($zero)
sw $t0, 268501000($zero)
lw $t0, 268501012($zero)
li $t1, 1
subu $t2, $t0, $t1
sw $t2, 268501104($zero)
lw $t0, 268501104($zero)
sw $t0, 268501012($zero)
lw $t0, 268501012($zero)
li $t1, 1
mul $t2, $t0, $t1
sw $t2, 268501108($zero)
lw $t0, 268501108($zero)
sw $t0, 268501012($zero)
li $t0, 1
sw $t0, 268501012($zero)
lw $t0, 268501012($zero)
li $t1, 1
div $t0, $t1
mfhi $t2
sw $t2, 268501112($zero)
lw $t0, 268501112($zero)
sw $t0, 268501012($zero)
li $t0, 1
sw $t0, 268501012($zero)
lw $t0, 268501012($zero)
li $t1, 1
div $t0, $t1
mflo $t2
sw $t2, 268501116($zero)
lw $t0, 268501116($zero)
sw $t0, 268501012($zero)
lw $t0, 268500992($zero)
lw $t1, 268501012($zero)
sgt $t2, $t0, $t1
sw $t2, 268501120($zero)
lw $t0, 268501120($zero)
bnez $t0, Label_15
li $t9, 1
sll $t9, $t9, 2
addiu $t0, $t9, 268501036
lw $t0, 0($t0)
li $t9, 2
sll $t9, $t9, 2
addiu $t1, $t9, 268501036
lw $t1, 0($t1)
sge $t2, $t0, $t1
sw $t2, 268501120($zero)
Label_15:
lw $t0, 268501120($zero)
beqz $t0, Label_14
Label_14:
lw $t0, 268500992($zero)
lw $t1, 268501012($zero)
slt $t2, $t0, $t1
sw $t2, 268501124($zero)
lw $t0, 268501124($zero)
bnez $t0, Label_17
li $t9, 2
sll $t9, $t9, 2
addiu $t0, $t9, 268501036
lw $t0, 0($t0)
li $t9, 3
sll $t9, $t9, 2
addiu $t1, $t9, 268501036
lw $t1, 0($t1)
sle $t2, $t0, $t1
sw $t2, 268501124($zero)
Label_17:
lw $t0, 268501124($zero)
beqz $t0, Label_16
Label_16:
lw $t0, 268500992($zero)
lw $t1, 268501012($zero)
sne $t2, $t0, $t1
sw $t2, 268501128($zero)
lw $t0, 268501128($zero)
beqz $t0, Label_18
Label_18:
li $t0, 1
sw $t0, 268501132($zero)
lw $t0, 268501132($zero)
beqz $t0, Label_19
Label_19:
Label_20:
lw $t0, 268500992($zero)
lw $t1, 268501012($zero)
seq $t2, $t0, $t1
sw $t2, 268501136($zero)
lw $t0, 268501136($zero)
beqz $t0, Label_21
li $t0, 0
sw $t0, 268501140($zero)
lw $t0, 268501140($zero)
beqz $t0, Label_22
j Label_20
Label_22:
j Label_21
j Label_20
Label_21:

li $v0, 10
syscall




# self defined functions

# ---------------- FUCNTION: f2 ------------------
Label_2:

lw $t0, 4($sp)
li $t1, 1
addu $t2, $t0, $t1
sw $t2, 8($sp)
lw $t0, 8($sp)
sw $t0, 4($sp)
li $v0, 0
jr $ra



# ---------------- FUCNTION: f3 ------------------
Label_3:

# local var defition:
lw $t0, 4($sp)
lw $t1, 8($sp)
addu $t2, $t0, $t1
sw $t2, 16($sp)
lw $t0, 16($sp)
sw $t0, 12($sp)
lw $v0, 12($sp)
jr $ra



# ---------------- FUCNTION: f1 ------------------
Label_1:

jr $ra



# ---------------- FUCNTION: f6 ------------------
Label_6:

li $v0, 0
jr $ra



# ---------------- FUCNTION: f5 ------------------
Label_5:

li $v0, 1
jr $ra



# ---------------- FUCNTION: f4 ------------------
Label_4:

la $t0, global_var7
lw $t0, 0($t0)
li $t1, 1
addu $t2, $t0, $t1
sw $t2, 4($sp)
lw $t0, 4($sp)
la $t9, global_var7
sw $t0, 0($t9)
li $v0, 1
jr $ra



# ---------------- FUCNTION: f7 ------------------
Label_7:

li $v0, 0
jr $ra



