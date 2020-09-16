	lw	0	1	five
	lw	0	3	neg1
	lw	0	4	pos1
big	beq	0	1	done
	add	1	3	1
	lw	0	2	four
small	beq	0	2	big
	add	2	3	2
	add	5	4	5
	beq	0	0	small
done	halt
five	.fill	5
four	.fill	4
neg1	.fill	-1
pos1	.fill	1
