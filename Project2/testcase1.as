	lw	0	3	mplier	  
	lw	0	4	neg1	    
	lw	0	5	fifteen	 
	lw	0	6	neg1	   
loop	add	6	4	7	 
	lw	0	2	mcand	
	nor	2	7	2	  
	beq	0	2	action	    
	beq	0	0	noact	   
action	add	1	3	1	    
noact	add	3	3	3	    // data dependency here
	add	5	4	5	
	add	6	6	6	    
	beq	0	5	done	    // branch dependency here
	beq	0	0	loop
done	halt
mcand	.fill	15894
mplier	.fill	10783
neg1	.fill	-1
fifteen	.fill	15			    fifteen digits
