.arch armv8-a
.text
.align 2
.syntax unified
.arm
.global main

main:
main.main0:
	push	{fp,lr}
	mov	fp, sp
	sub	sp, sp, #4
	mov	r0, #0
	add	sp, sp, #4
	mov	sp, fp
	pop	{fp,lr}
	bx	lr

.data
.align 2
