.data 0x10010000
temp:  .space  160000


# a[2][2]
global_const_arr1:  .word  1, 2, 4, 3

# string tokens: 
str_2:  .asciiz   "\n"


.text 0x00400000

# main function
li $t8, 0
li $t9, 1
mul $t8, $t8, 2
addu $t9, $t8, $t9
sll $t9, $t9, 2
la $a0, global_const_arr1
addu $a0, $a0, $t9
lw $a0, 0($a0)
li $v0, 1
syscall
la $a0, str_2
li $v0, 4
syscall

li $v0, 10
syscall




# self defined functions

