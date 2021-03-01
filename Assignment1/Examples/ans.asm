.data
    n : .asciiz "Number of Points: "
    point : .asciiz "Point "
    xCoordinate : .asciiz " X Co-ordinate: "
    yCoordinate : .asciiz " Y co-ordinate: "
    finalstr : .asciiz "Area under the curve formed by joining given points with straight line is: "
    invalidN : .asciiz "INVALID-N : Number of points should be greater than 0 to compute area. "
    unsorted : .asciiz "UNSORTED-X : X Co-ordinates should be provided sorted. "
    decimal : .asciiz ".5"
    br : .asciiz "\n"
    dash : .asciiz "-0.5"
.text

main:

    # t0,t1 -> x1,x2
    # t2,t3 -> y1,y2
    # t4 -> saving the ans
    # t5 -> saving int division of t4/2
    # s0 -> n (number of points)
    # s1 -> j (increment once i (x,y) pair is found, range = [1,n(s0)])
    # s2 -> 0 if first point is stored else 1

    # initialize all registors needed
    la $t0,0
    la $t1,0
    la $t2,0
    la $t3,0
    la $t4,0
    la $s0,0
    la $s1,1
    la $s2,1
 
    # telling the user to enter number of points
    li $v0, 4
    la $a0, n
    syscall

    # read the value of n and store in s0
    li $v0,5
    syscall
    move $s0, $v0

    ble $s0, 0, invalidExit 


    # taking points and computing area

    while:
        
        bgt $s1, $s0, exit
        move $t0, $t1
        move $t2, $t3

        # asking for x coordinae

            # printing "point"
            li $v0, 4
            la $a0, point
            syscall

            # printing point number
            li $v0, 1
            add $a0, $s1, $zero
            syscall
            
            # printing x coordinae
            li $v0, 4
            la $a0, xCoordinate
            syscall

        # taking x coordinate and storing in t1
        li $v0,5
        syscall
        move $t1, $v0

        # asking for y coordinae

            # printing "point"
            li $v0, 4
            la $a0, point
            syscall

            # printing point number
            li $v0, 1
            add $a0, $s1, $zero
            syscall
            
            # printing y coordinae
            li $v0, 4
            la $a0, yCoordinate
            syscall

        # taking y coordinate and storing in t3
        li $v0,5
        syscall
        move $t3, $v0

        add $s1, $s1, 1

        beqz $s2,area
        beq $s2, 1, firstInput


        j while

    exit:

        # priting final msg
        li $v0, 4
        la $a0, finalstr
        syscall

        # printing the fina area
        div $t5, $t4, 2

        beq $t4, -1,negZero

        li $v0, 1
        add $a0, $zero, $t5
        syscall

        mul $t5, $t5, 2
        bne $t5, $t4, point5

        # breaking the line
        li $v0, 4
        la $a0, br
        syscall

        # Exit the program
        li $v0, 10
        syscall

    point5:

        # printing ".5"
        li $v0, 4
        la $a0, decimal
        syscall

        # breaking the line
        li $v0, 4
        la $a0, br
        syscall

        # Exit the program
        li $v0, 10
        syscall

    negZero:
        # printing "-0.5"
        li $v0, 4
        la $a0, dash
        syscall

        # breaking the line
        li $v0, 4
        la $a0, br
        syscall

        # Exit the program
        li $v0, 10
        syscall

    invalidExit:

        # priting msg -> n invalid
        li $v0, 4
        la $a0, invalidN
        syscall

        # breaking the line
        li $v0, 4
        la $a0, br
        syscall

        # Exit the program
        li $v0, 10
        syscall

    unsortedExit:

        # priting msg ->  not sorted 
        li $v0, 4
        la $a0, unsorted
        syscall

        # breaking the line
        li $v0, 4
        la $a0, br
        syscall

        # Exit the program
        li $v0, 10
        syscall



# find area under line joining two points s.t x1 = $t0, y1 = $t1, x2 = $t2, y2 = $t3
# stores the output in f12

# areaabs:
#     mul $t4, $t2, $t3

area:

    bgt $t0, $t1, unsortedExit
    
    sub $t0, $t1, $t0 #(x2-x1)
    add $t2, $t2, $t3 #(y2+y1) 
    mul $t2, $t0, $t2

    add $t4, $t4, $t2


    j while

firstInput:

    la $s2, 0
    j while

