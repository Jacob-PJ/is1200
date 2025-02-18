  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
  
.global time2string
time2string:

andi $t6, $a1, 0xff
bnez $t6, normal
nop
hour:
li $t6, 72
sb $t6, 0($a0)
li $t6, 79
sb $t6, 1($a0)
li $t6, 85
sb $t6, 2($a0)
li $t6, 82
sb $t6, 3($a0)
sb $0, 4($a0)
jr $ra
nop

normal:
    PUSH $ra
#digit 1
    PUSH $a0
    srl $a0, $a1, 12
    jal hexasc
    nop
    POP $a0
    sb $v0, 0($a0)

#digit 2
    PUSH $a0
    srl $a0, $a1, 8
    jal hexasc
    nop
    POP $a0
    sb $v0, 1($a0)

#colon
    li $t0, 58
    sb $t0, 2($a0)

#digit 3
    PUSH $a0
    srl $a0, $a1, 4 
    jal hexasc
    nop
    POP $a0
    sb $v0, 3($a0)

#digit 4
    PUSH $a0
    move $a0, $a1
    jal hexasc
    nop
    POP $a0
    sb $v0, 4($a0)
end:
#null terminator
    li $t0, 0
    sb $t0, 5($a0)
    POP $ra
    jr $ra
    nop
    jr $ra
    nop
		
.global delay
delay:
	li $t4, 38752000
	
	loop:
		addi $a0, $t0, -1 # ms input
		
		li $t1, 0 #i =0
		nestedLoop:
			addi $t1, $t1, 1 #i+1
			slt $t3, $t1, $t4 #i<4711
			bnez $t3, nestedLoop
			nop
		
		slti $t2, $t0, 1 # ms´= 0?
		beqz $t2, loop
		nop
	jr $ra
	nop

	
hexasc:
	andi $a0, $a0, 0x0F
	addi $t0, $a0, 0x30

	
	slti $t1, $a0, 0x0A
	beqz $t1, getChar
	nop
	bnez $t1, getInt
	nop
	
	getChar:
		addi $v0, $t0, 7
		jr $ra
		nop
		
	getInt:
	 	move $v0, $t0
	 	jr $ra
	 	nop	

