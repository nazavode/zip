	.text
	.file	"Point3dSoaZipSum.cpp"
	.globl	Point3dSoaZipSum        # -- Begin function Point3dSoaZipSum
	.p2align	4, 0x90
	.type	Point3dSoaZipSum,@function
Point3dSoaZipSum:                       # @Point3dSoaZipSum
	.cfi_startproc
# %bb.0:
	movq	(%rdi), %r11
	movq	8(%rdi), %r8
	cmpq	%r8, %r11
	je	.LBB0_1
# %bb.2:
	movq	(%rsi), %r9
	movq	(%rdx), %r10
	subq	%r11, %r8
	addq	$-4, %r8
	shrq	$2, %r8
	incq	%r8
	cmpq	$15, %r8
	ja	.LBB0_7
# %bb.3:
	xorl	%edi, %edi
	movq	%r11, %rcx
	xorl	%eax, %eax
	xorl	%edx, %edx
	xorl	%esi, %esi
	jmp	.LBB0_4
.LBB0_1:
	xorl	%edx, %edx
	xorl	%eax, %eax
	xorl	%ecx, %ecx
	orq	%rcx, %rax
                                        # kill: def $edx killed $edx killed $rdx
	retq
.LBB0_7:
	movq	%r8, %rsi
	andq	$-16, %rsi
	leaq	-16(%rsi), %rcx
	movq	%rcx, %rax
	shrq	$4, %rax
	incq	%rax
	movl	%eax, %edx
	andl	$1, %edx
	testq	%rcx, %rcx
	je	.LBB0_8
# %bb.9:
	movq	%rdx, %rdi
	subq	%rax, %rdi
	vpxor	%xmm0, %xmm0, %xmm0
	xorl	%eax, %eax
	vpxor	%xmm1, %xmm1, %xmm1
	vpxor	%xmm2, %xmm2, %xmm2
	vpxor	%xmm3, %xmm3, %xmm3
	vpxor	%xmm4, %xmm4, %xmm4
	vpxor	%xmm5, %xmm5, %xmm5
	.p2align	4, 0x90
.LBB0_10:                               # =>This Inner Loop Header: Depth=1
	vpaddd	(%r11,%rax,4), %ymm0, %ymm0
	vpaddd	32(%r11,%rax,4), %ymm1, %ymm1
	vpaddd	(%r9,%rax,4), %ymm2, %ymm2
	vpaddd	32(%r9,%rax,4), %ymm3, %ymm3
	vpaddd	(%r10,%rax,4), %ymm4, %ymm4
	vpaddd	32(%r10,%rax,4), %ymm5, %ymm5
	vpaddd	64(%r11,%rax,4), %ymm0, %ymm0
	vpaddd	96(%r11,%rax,4), %ymm1, %ymm1
	vpaddd	64(%r9,%rax,4), %ymm2, %ymm2
	vpaddd	96(%r9,%rax,4), %ymm3, %ymm3
	vpaddd	64(%r10,%rax,4), %ymm4, %ymm4
	vpaddd	96(%r10,%rax,4), %ymm5, %ymm5
	addq	$32, %rax
	addq	$2, %rdi
	jne	.LBB0_10
# %bb.11:
	testq	%rdx, %rdx
	je	.LBB0_13
.LBB0_12:
	vpaddd	32(%r10,%rax,4), %ymm5, %ymm5
	vpaddd	(%r10,%rax,4), %ymm4, %ymm4
	vpaddd	32(%r9,%rax,4), %ymm3, %ymm3
	vpaddd	(%r9,%rax,4), %ymm2, %ymm2
	vpaddd	32(%r11,%rax,4), %ymm1, %ymm1
	vpaddd	(%r11,%rax,4), %ymm0, %ymm0
.LBB0_13:
	vpaddd	%ymm0, %ymm1, %ymm0
	vpaddd	%ymm2, %ymm3, %ymm1
	vpaddd	%ymm4, %ymm5, %ymm2
	vextracti128	$1, %ymm2, %xmm3
	vpaddd	%xmm3, %xmm2, %xmm2
	vpshufd	$78, %xmm2, %xmm3       # xmm3 = xmm2[2,3,0,1]
	vpaddd	%xmm3, %xmm2, %xmm2
	vpshufd	$229, %xmm2, %xmm3      # xmm3 = xmm2[1,1,2,3]
	vpaddd	%xmm3, %xmm2, %xmm2
	vmovd	%xmm2, %edx
	vextracti128	$1, %ymm1, %xmm2
	vpaddd	%xmm2, %xmm1, %xmm1
	vpshufd	$78, %xmm1, %xmm2       # xmm2 = xmm1[2,3,0,1]
	vpaddd	%xmm2, %xmm1, %xmm1
	vpshufd	$229, %xmm1, %xmm2      # xmm2 = xmm1[1,1,2,3]
	vpaddd	%xmm2, %xmm1, %xmm1
	vmovd	%xmm1, %eax
	vextracti128	$1, %ymm0, %xmm1
	vpaddd	%xmm1, %xmm0, %xmm0
	vpshufd	$78, %xmm0, %xmm1       # xmm1 = xmm0[2,3,0,1]
	vpaddd	%xmm1, %xmm0, %xmm0
	vpshufd	$229, %xmm0, %xmm1      # xmm1 = xmm0[1,1,2,3]
	vpaddd	%xmm1, %xmm0, %xmm0
	vmovd	%xmm0, %edi
	cmpq	%rsi, %r8
	je	.LBB0_5
# %bb.14:
	leaq	(%r11,%rsi,4), %rcx
	.p2align	4, 0x90
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	addl	(%rcx), %edi
	addl	(%r9,%rsi,4), %eax
	addl	(%r10,%rsi,4), %edx
	leaq	(%r11,%rsi,4), %rcx
	addq	$4, %rcx
	incq	%rsi
	cmpq	%rsi, %r8
	jne	.LBB0_4
.LBB0_5:
	shlq	$32, %rax
	movl	%edi, %ecx
	orq	%rcx, %rax
                                        # kill: def $edx killed $edx killed $rdx
	vzeroupper
	retq
.LBB0_8:
	vpxor	%xmm0, %xmm0, %xmm0
	xorl	%eax, %eax
	vpxor	%xmm1, %xmm1, %xmm1
	vpxor	%xmm2, %xmm2, %xmm2
	vpxor	%xmm3, %xmm3, %xmm3
	vpxor	%xmm4, %xmm4, %xmm4
	vpxor	%xmm5, %xmm5, %xmm5
	testq	%rdx, %rdx
	jne	.LBB0_12
	jmp	.LBB0_13
.Lfunc_end0:
	.size	Point3dSoaZipSum, .Lfunc_end0-Point3dSoaZipSum
	.cfi_endproc
                                        # -- End function

	.ident	"clang version 9.0.0 (tags/RELEASE_900/final)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __gxx_personality_v0
