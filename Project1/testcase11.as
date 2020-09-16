	lw	0   1	one
	nor	0   1	2
	add	1   2	3
	nor	1   3	2
	beq	1   3	-2
done	halt
one	.fill	1
