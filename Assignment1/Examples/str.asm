.data
    str:  .asciiz "(2,4),(3,5),(0,9)" 
    inBr: .asciiz "("
    comma: .asciiz ","
    outBr: .asciiz ")"

.text  
main:  
    la $t4, str  # la means load address (so we load the address of str into $t4)  
    la $s4, inBr
    lb $s4,($s4) 
    la $s5, comma
    lb $s5,($s5) 
    la $s6, outBr
    lb $s6,($s6)  
    li $s1, 1
    li $s2, 0  #-> becomes 1 if first point found 
    li $s3, 0  # -> 1 if cominf coordinate is y

    while:  
        lb $t5, 0($t4)  # Load the first byte from address in $t4 

        beqz $t5, end   # if $t6 == 0 then go to label end  

        beq $t5, $s4, moveOn    # encounter in bracket

        beq $t5, $s5, moveOn    # encounter comma

        beq $t5, $s6, outMoveOn    # encounter out bracket

        beq $s3, 0, xFound

        move $t3, $t2
        sb $t5, ($t2)
        add $t4, $t4, 1
        li $s3, 0

        beq $s2, 1, area




        j while
    
    end:  
        li $v0, 10
        syscall


moveOn:

    add $t4, $t4, 1

    j while

outMoveOn: 
    add $s1, $s1, 1
    li $s2, 1
    add $t4, $t4,1

    j while

xFound:
    move $t1, $t0
    sb $t5, ($t0)
    add $t4, $t4,1 
    li $s3, 1

    j while

area:
    li $v0, 4
    la $a0, str
    syscall

    j while