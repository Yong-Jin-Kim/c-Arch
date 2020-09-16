	lw	0	3	mplier	    load reg3 with mplier
	lw	0	4	neg1	    load reg4 with -1 (stationary)
	lw	0	5	fifteen	    load reg5 with 15
	lw	0	6	neg1	    load reg6 with -1
loop	add	6	4	7	    reg7 = reg6 + reg4
	lw	0	2	mcand	    load reg2 with mcand
	nor	2	7	2	    get the bit value
	beq	0	2	action	    if the bit was 1
	beq	0	0	noact	    if the bit was 0
action	add	1	3	1	    ==== CRITICAL SECTION! ====
noact	add	3	3	3	    mplier shift left
	add	5	4	5	    loop variable control
	add	6	6	6	    ==== A DEVIL's NUMBER? ====
	beq	0	5	done	    end the program
	beq	0	0	loop
done	halt
mcand	.fill	2766
mplier	.fill	14383
neg1	.fill	-1
fifteen	.fill	15			    fifteen digits
