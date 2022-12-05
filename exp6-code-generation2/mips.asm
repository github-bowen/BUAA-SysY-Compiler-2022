.data 0x10010000
temp:  .space  160000


# string tokens: 


.text 0x00400000

# main function
# local array defition:
li $t1, 268500992
li $t0, 1
sw $t0, 0($t1)
li $t0, 2
sw $t0, 4($t1)
li $t0, 3
sw $t0, 8($t1)
li $t0, 4
sw $t0, 12($t1)
li $t0, 0
li $t8, 1
li $t9, 1
mul $t8, $t8, 2
addu $t9, $t8, $t9
sll $t9, $t9, 2
addiu $t9, $t9, 268500992
sw $t0, 0($t9)
li $t8, 0
li $t9, 0
mul $t8, $t8, 2
addu $t9, $t8, $t9
sll $t9, $t9, 2
addiu $a0, $t9, 268500992
lw $a0, 0($a0)
li $v0, 1
syscall

li $v0, 10
syscall




# self defined functions

