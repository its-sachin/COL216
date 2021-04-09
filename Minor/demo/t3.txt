addi $t0, $zero, 1
sw $t0, 1000($zero)
sw $t0, 1024($zero)
lw $t0, 1000($zero)
lw $t1, 1024($zero)
add $t0, $t0, $t1
