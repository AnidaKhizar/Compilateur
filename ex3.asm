	inc	1
	jp	main
myfunc:
	libp	-4
	libp	-3
	mts
	libp	-2
	mts
	add
	stm
	ret
paola:
	libp	0
	pushr	1.000000
	stm
	libp	1
	libp	0
	mts
	pushr	1.000000
	add
	stm
	libp	-5
	libp	-4
	mts
	libp	-3
	mts
	add
	libp	-2
	mts
	add
	libp	1
	mts
	add
	stm
	ret
main:
	push	0
	pushr	1.000000
	stm
L000:
	push	0
	mts
	pushr	5.000000
	ls
	jf	L001
	inc	1
	push	0
	mts
	push	0
	mts
	pushr	1.000000
	add
	savebp
	inc	0
	call	myfunc
	dec	0
	rstrbp
	dec	2
	output
	push	0
	push	0
	mts
	pushr	1.000000
	add
	stm
	jp	L000
L001:
	halt
	end
