.text

	addi $a0, $0, 8     # set $a0 to 4
	addi $v0, $0, 1



countDown:
    beq $a0, $0, end
    nop
    
    addi $a0, $a0, -1 
    add $a1, $a0, $0
    add $v1, $v0, $0
    
    multiply:
    	beq $a1, $0, countDown 
    	nop
    
    	add $v0, $v0, $v1
    	addi $a1, $a1, -1
    	    
    	beq, $0, $0, multiply
    	nop    
end:
     add  $0,$0,$0	  






#countDown:
#    beq $a0, $0, end
#    nop
#    
#    addi $a0, $a0, -1    
#    beq, $0, $0, countDown
#    nop    


#multiply:
#    beq $a2, $0, end 
#    nop
#    
#    add $v0, $v0, $v1
#    
#    addi $a2, $a2, -1    
#    beq, $0, $0, multiply
#    nop    
