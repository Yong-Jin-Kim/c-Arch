	lw	0	5	mcand	    load reg5 with mcand
	lw	0	6	mcand	    load reg6 with mcand
	lw	0	7	mcand	    load reg7 with mcand
	lw	0	3	plow	    *load reg3 with low parser
	nor	5	3	5
	nor	5	3	5	    *parse reg5 ..... ..... xxxxx
	lw	0	3	pmid	    *load reg3 with mid parser
	nor	6	3	6
	nor	6	3	6	    *parse reg6 ..... xxxxx .....
	lw	0	3	phigh	    *load reg3 with high parser
	nor	7	3	7
	nor	7	3	7	    *parse reg7 xxxxx ..... .....
	lw	0	2	mplier	    load reg2 with mplier
	lw	0	3	nlow	    load reg3 with -1
loop1	beq	5	0	go1	    ====jump out====
	add	1	2	1	    add mplier to reg1
	add	5	3	5	    reg5 = reg5 - 1	
	beq	0	0	loop1	    goto loop1
go1	add	2	2	2	    
	add	2	2	2
	add	2	2	2
	add	2	2	2
	add	2	2	2	    mplierX32
	lw	0	3	nmid	    load reg3 with -32
loop2	beq	6	0	go2	    ====jump out====
	add	1	2	1	    add mplierX32 to reg1
	add	6	3	6	    reg6 = reg6 - 32
	beq	0	0	loop2	    goto loop2
go2	add	2	2	2
	add	2	2	2
	add	2	2	2
	add	2	2	2
	add	2	2	2	    mplierX1024
	lw	0	3	nhigh	    load reg3 with -1024
loop3	beq	7	0	done	    ====jump out====
	add	1	2	1	    add mplierX1024 to reg1
	add	7	3	7	    reg7 = reg7 - 1024
	beq	0	0	loop3	    goto loop3
done	halt				    finish the program
five	.fill	5
mcand	.fill	32766
mplier	.fill	10383
nlow	.fill	-1
nmid	.fill	-32
nhigh	.fill	-1024
plow	.fill	-32
pmid	.fill	-993
phigh	.fill	-31745
