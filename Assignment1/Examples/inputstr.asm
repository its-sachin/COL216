.text
main:
    # Prompt for the string to enter
        li $v0, 4
        la $a0, prompt
        syscall

    # Read the x coordinate and save it in $s0
        li $v0, 5
        syscall
        move $t0, $v0

    # Prompt for the string to enter
        li $v0, 4
        la $a0, prompt
        syscall

    # Read the x coordinate and save it in $s0
        li $v0, 5
        syscall
        sub $t1, $t0, $v0

    # Output the text
        li $v0, 4
        la $a0, output
        syscall
    
    # Output the number
        li $v0, 1
        move $a0, $t1
        syscall
    # Exit the program
        li $v0, 10
        syscall
.data
    brac: .space 2
    bracSize: .word 1
    prompt: .asciiz "Please enter the point: "
    output: .asciiz "Sum of coordinates: "