	.text
	.global zfunction

zfunction:
	pushl	%ebp
	movl	8(%esp),%eax
    andl    $0xffc01fff, %eax
	shl 	$4,%eax
	popl	%ebp
	ret
