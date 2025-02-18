.data
	prompt: .asciiz "Enter delay in ms: "
	done: .asciiz "Done"
	
	milSec: .word 4711
.text

delay:
	li $v0, 4
	la $a0, prompt
	syscall
	
	li $v0, 5
	syscall
	
	move $a0, $v0
	
	move $t0, $a0 
	la $s0, milSec
	
	loop:
		subi $t0, $t0, 1 # ms input
		
		li $t1, 0 #i =0
		nestedLoop:
			addi $t1, $t1, 1 #i+1
			slt $t3, $t1, $s0 #i<4711
			beqz $t3, nestedLoop
		

		slti $t2, $t0, 1 # ms´= 0?
		beqz $t2, loop
	
	li $v0, 4
	la $a0, done
	syscall
	
	li $v0, 10
	syscall
