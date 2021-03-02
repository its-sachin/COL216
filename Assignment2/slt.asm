.data
    numChars : .word 3
    userinput :	  .space 3
    notdone : .asciiz "not done\n"
    done : .asciiz "done\n"

.text

main:
    la $s0, numChars
    la $s1, 6

    # blt $s1, $s0, donef
    # slt $t0, $s0, $s1
    # bne $t0, $zero, donef

    # taking input from the user
    li $v0, 8
    la $a0, userinput
    add $a1, $zero, $s0
    syscall

    li $v0, 4
    la $a0, notdone
    syscall

    li $v0 ,10
    syscall

# donef:
#     li $v0, 4
#     la $a0, done
#     syscall

#     li $v0 ,10
#     syscall





