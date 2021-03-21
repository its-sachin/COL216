main:
	addi $t0, $zero, 1
	addi $t1, $zero, 2
	sw $t0, 1000
	sw $t1, 1024
	
	lw $t2, 1000
	lw $t3, 1024
	
	add $t3, $t3, $t2
		
	sw $t3, 1028
	
	

exit:
