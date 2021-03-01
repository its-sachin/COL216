.data
    decimal: .asciiz ".5"

.text

main:
    li $v0, 5
    syscall
    move $t4, $v0

    div $t5, $t4, 2
    # srl $t5, $t4, 1

    li $v0, 1
    add $a0, $zero, $t5
    syscall

    mul $t5,$t5,2

    bne $t5,$t4,point5
    
    li $v0, 10
        syscall


point5:
    li $v0, 4
    la $a0, decimal
    syscall

    li $v0, 10
        syscall

