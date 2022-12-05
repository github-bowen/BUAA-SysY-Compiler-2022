.data 0x10010000
temp:  .space  160000


# string tokens: 


.text 0x00400000

# main function


# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
# Call function!
jal Label_4
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000


li $v0, 10
syscall




# self defined functions

# ---------------- FUCNTION: cal ------------------
Label_1:

# local var defition:
li $t0, 0
sw $t0, 8($sp)
# local var defition:
li $t0, 0
sw $t0, 12($sp)
Label_2:
lw $t0, 8($sp)
li $t1, 3
slt $t2, $t0, $t1
sw $t2, 16($sp)
lw $t0, 16($sp)
beqz $t0, Label_3
lw $t0, 12($sp)
lw $t9, 8($sp)
sll $t9, $t9, 2
lw $t1, 4($sp)
addu $t1, $t1, $t9
lw $t1, 0($t1)
addu $t2, $t0, $t1
sw $t2, 20($sp)
lw $t0, 20($sp)
sw $t0, 12($sp)
lw $t0, 8($sp)
li $t1, 1
addu $t2, $t0, $t1
sw $t2, 24($sp)
lw $t0, 24($sp)
sw $t0, 8($sp)
j Label_2
Label_3:
lw $a0, 12($sp)
li $v0, 1
syscall
jr $ra



# ---------------- FUCNTION: f ------------------
Label_4:

# local array defition:
li $t0, 1
sw $t0, 4($sp)
li $t0, 2
sw $t0, 8($sp)
li $t0, 3
sw $t0, 12($sp)
li $t0, 4
sw $t0, 16($sp)
li $t0, 5
sw $t0, 20($sp)
li $t0, 6
sw $t0, 24($sp)


# Pushing Function Real Params:
addiu $sp, $sp, -1000
sw $ra, 0($sp)
li $t9, 1
mul $t9, $t9, 3
sll $t9, $t9, 2
addiu $t0, $sp, 1004
addu $t0, $t0, $t9
sw $t0, 4($sp)
# Finished Pushing Params!
# Call function!
jal Label_1
lw $ra, 0($sp)
# Pop params
addiu $sp, $sp, 1000

jr $ra



