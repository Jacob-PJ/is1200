.text

	addi $a0, $0, 8     # set $a0 to 4
	addi $v0, $0, 1



countDown:
    beq $a0, $0, end
    nop
    
    mul $v0, $v0, $a0
    
    addi $a0, $a0, -1 
    beq $0, $0, countDown 
    nop   
end:
     add  $0,$0,$0	  