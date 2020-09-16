	lw	0   7	iter	==== iter-th FIBONACCI ====
	lw	0   6	neg1
loop	lw	0   1	a1
	lw	0   2	a2
	add	1   2	3
	sw	0   3	a3	==== RESULT in a3 & reg3 ====
	add	7   6	7
	beq	0   7	done
	sw	0   2	a1
	sw	0   3	a2
	beq	0   0	loop
done	halt
a1	.fill	1
a2	.fill	1
a3	.fill	1
iter	.fill	25
neg1	.fill	-1
