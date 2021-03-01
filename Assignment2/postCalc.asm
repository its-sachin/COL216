# @SACHIN
#  2019CS10722


# Decleration of originality :
# complete code written solely by me.



.data
    input : .asciiz "Enter the postfix expression: "
    numChars : .word 1024
    userinput :	  .space 1024
    inputInvalid : .asciiz "INVALID-INPUT: Input is not in postfix\n"
    output : .asciiz "Computation of given postfix expression is: "
    partial : .asciiz "Partial computation: "
    partialWar : .asciiz "WARNING: Your input is not complete postfix, ans above is partial answer.\n"
    unboundWar : .asciiz "WARNING: Your input is too big, tanswer might be frivolous.\n"
    br : .asciiz "\n"
.text

main:

    # t0 -> storing address of input string
    # t1 -> bit vise value
    # t2,t3 -> calculation in steps
    # t4 -> stack entries count
    # t5 -> counter
    # $s0 -> max value

    la $s0, numChars
    la $t0, 0
    la $t1, 0
    la $t2, 0
    la $t3, 0
    la $t4, 0
    la $t5, 0

    # telling the user to enter number of points
    li $v0, 4
    la $a0, input
    syscall

    # taking input from the user
    li $v0, 8
    la $a0, userinput
    add $a1, $zero, $s0
    syscall

    add $t0, $zero, $a0
    add $t1, $zero, $a0

compute:

    lb $t1, 0($t0)
    beq $t1, 10, end
    beqz $t1, end

    bge $t1, 48, number

    beq $t1, 42, multiply
    beq $t1, 43, addition
    beq $t1, 45, subtract

    j invalidend

number:

    bgt $t1, 57, invalidend

    add $t2, $t0, -1
    lb $t3, 0($t2)

    beq $t3, 42, invalidend
    beq $t3, 43, invalidend
    beq $t3, 45, invalidend

    addi $sp, $sp, -4
    add $t4, $t4, 1

    addi $t1, $t1, -48
    sw $t1, 0($sp)


    add $t0, $t0, 1
    add $t5, $t5, 1
    j compute

multiply:

    lw $t2, 0($sp)
    lw $t3, 4($sp)
    mul $t2, $t2, $t3

    addi $sp, $sp, 4
    sw $t2, 0($sp)

    add $t4, $t4, -1

    add $t0, $t0, 1
    add $t5, $t5, 1
    j compute

addition:

    lw $t2, 0($sp)
    lw $t3, 4($sp)
    add $t2, $t2, $t3

    addi $sp, $sp, 4
    sw $t2, 0($sp)

    add $t4, $t4, -1

    add $t0, $t0, 1
    add $t5, $t5, 1
    j compute

subtract:

    lw $t2, 0($sp)
    lw $t3, 4($sp)
    sub $t2, $t3, $t2

    addi $sp, $sp, 4
    sw $t2, 0($sp)
    
    add $t4, $t4, -1

    add $t0, $t0, 1
    add $t5, $t5, 1
    j compute




invalidend:

    li $v0, 4
    la $a0, inputInvalid
    syscall

    li $v0, 10
    syscall

partialAns:

    li $v0, 4
    la $a0, partial
    syscall

    li $v0, 1
    lw $a0, 0($sp)
    syscall

    li $v0, 4
    la $a0, br
    syscall

    li $v0, 4
    la $a0, partialWar
    syscall

    bge $t5, $s0, unBound

    li $v0, 10
    syscall

unBound:

    li $v0, 4
    la $a0, unboundWar
    syscall

    li $v0, 10
    syscall

end: 

    add $t5, $t5, 1
    bne $t4, 1, partialAns

    li $v0, 4
    la $a0, output
    syscall

    li $v0, 1
    lw $a0, 0($sp)
    syscall

    li $v0, 4
    la $a0, br
    syscall

    bge $t5, $s0, unBound

    li $v0, 10
    syscall    










