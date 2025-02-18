  # analyze.asm
  # This file written 2015 by F Lundevall
  # Copyright abandoned - this file is in the public domain.
andi $t6, $a1,0xff
bnez $t6, normal
nop
beqz $t6, hour
nop

hour: 	
	li $t0, 58
    	sb $t0, 0($a0)
    	li $t0, 58
        sb $t0, 1($a0)
        li $t0, 58
        sb $t0, 2($a0)
        li $t0, 58
	sb $t0, 3($a0)
	
	j end
	nop
	.text
main:
	li	$s0,0x30
loop:
	move	$a0,$s0		# copy from s0 to a0
	
	li	$v0,11		# syscall with v0 = 11 will print out
	syscall			# one byte from a0 to the Run I/O window

	addi	$s0,$s0,3	# what happens if the constant is changed?
	
	li	$t0,0x5b
	slt	$t1,$s0,$t0
    	bne	$t1,$zero,loop

	nop			# delay slot filler (just in case)

stop:	j	stop		# loop forever here
	nop			# delay slot filler (just in case)

#line 14 had to be changed tofrom incrementing 1 to incrimenting 3.
#line 17 had to be added. Since we are not goung thru each ASCII  character now ther is a chance that Z could be missed.
#line 18 had to be changed to check on the value given by slt instaid. If it  is noequal to 0. The loop contnieus.
