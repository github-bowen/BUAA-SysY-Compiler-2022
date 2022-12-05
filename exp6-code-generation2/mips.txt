.data 0x10010000
temp:  .space  160000


# string tokens: 
str_2:  .asciiz   " "


.text 0x00400000

# main function
# local array defition:
# local var defition:
li $t0, 0
sw $t0, 268501012($zero)
Label_1:
lw $t0, 268501012($zero)
li $t1, 5
slt $t2, $t0, $t1
sw $t2, 268501016($zero)
lw $t0, 268501016($zero)
beqz $t0, Label_2

li $v0, 5
syscall
lw $t9, 268501012($zero)
sll $t9, $t9, 2
addiu $t9, $t9, 268500992
sw $v0, 0($t9)
lw $t0, 268501012($zero)
li $t1, 1
addu $t2, $t0, $t1
sw $t2, 268501020($zero)
lw $t0, 268501020($zero)
sw $t0, 268501012($zero)
j Label_1
Label_2:
li $t0, 0
sw $t0, 268501012($zero)
Label_3:
lw $t0, 268501012($zero)
li $t1, 5
slt $t2, $t0, $t1
sw $t2, 268501024($zero)
lw $t0, 268501024($zero)
beqz $t0, Label_4
lw $t9, 268501012($zero)
sll $t9, $t9, 2
addiu $a0, $t9, 268500992
lw $a0, 0($a0)
li $v0, 1
syscall
la $a0, str_2
li $v0, 4
syscall
lw $t0, 268501012($zero)
li $t1, 1
addu $t2, $t0, $t1
sw $t2, 268501028($zero)
lw $t0, 268501028($zero)
sw $t0, 268501012($zero)
j Label_3
Label_4:

li $v0, 10
syscall




# self defined functions

