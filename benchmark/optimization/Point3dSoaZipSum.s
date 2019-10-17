	.text
	.file	"Point3dSoaZipSum.cpp"
	.globl	Point3dSoaZipSum        # -- Begin function Point3dSoaZipSum
	.p2align	4, 0x90
	.type	Point3dSoaZipSum,@function
Point3dSoaZipSum:                       # @Point3dSoaZipSum
	.cfi_startproc
# %bb.0:
	movq	(%rdi), %r10
	movq	8(%rdi), %r8
	cmpq	%r8, %r10
	je	.LBB0_1
# %bb.2:
	movq	(%rsi), %r9
	movq	(%rdx), %r11
	subq	%r10, %r8
	addq	$-4, %r8
	shrq	$2, %r8
	incq	%r8
	cmpq	$63, %r8
	ja	.LBB0_5
# %bb.3:
	xorl	%edi, %edi
	movq	%r10, %rcx
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
.LBB0_5:
	movq	%r8, %rsi
	andq	$-64, %rsi
	leaq	(%r10,%rsi,4), %rcx
	vpxor	%xmm12, %xmm12, %xmm12
	xorl	%eax, %eax
	vpxor	%xmm1, %xmm1, %xmm1
	vpxor	%xmm2, %xmm2, %xmm2
	vpxor	%xmm3, %xmm3, %xmm3
	vpxor	%xmm4, %xmm4, %xmm4
	vpxor	%xmm5, %xmm5, %xmm5
	vpxor	%xmm6, %xmm6, %xmm6
	vpxor	%xmm7, %xmm7, %xmm7
	vpxor	%xmm8, %xmm8, %xmm8
	vpxor	%xmm9, %xmm9, %xmm9
	vpxor	%xmm10, %xmm10, %xmm10
	vpxor	%xmm11, %xmm11, %xmm11
	.p2align	4, 0x90
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	vpaddd	(%r10,%rax,4), %zmm12, %zmm12
	vpaddd	64(%r10,%rax,4), %zmm1, %zmm1
	vpaddd	128(%r10,%rax,4), %zmm2, %zmm2
	vpaddd	192(%r10,%rax,4), %zmm3, %zmm3
	vpaddd	(%r9,%rax,4), %zmm4, %zmm4
	vpaddd	64(%r9,%rax,4), %zmm5, %zmm5
	vpaddd	128(%r9,%rax,4), %zmm6, %zmm6
	vpaddd	192(%r9,%rax,4), %zmm7, %zmm7
	vpaddd	(%r11,%rax,4), %zmm8, %zmm8
	vpaddd	64(%r11,%rax,4), %zmm9, %zmm9
	vpaddd	128(%r11,%rax,4), %zmm10, %zmm10
	vpaddd	192(%r11,%rax,4), %zmm11, %zmm11
	addq	$64, %rax
	cmpq	%rax, %rsi
	jne	.LBB0_6
# %bb.7:
	vpaddd	%zmm8, %zmm9, %zmm8
	vpaddd	%zmm8, %zmm10, %zmm8
	vpaddd	%zmm8, %zmm11, %zmm8
	vextracti64x4	$1, %zmm8, %ymm9
	vpaddd	%zmm9, %zmm8, %zmm8
	vextracti128	$1, %ymm8, %xmm0
	vpaddd	%xmm0, %xmm8, %xmm8
	vpshufd	$78, %xmm8, %xmm0       # xmm0 = xmm8[2,3,0,1]
	vpaddd	%xmm0, %xmm8, %xmm8
	vpshufd	$229, %xmm8, %xmm0      # xmm0 = xmm8[1,1,2,3]
	vpaddd	%xmm0, %xmm8, %xmm0
	vmovd	%xmm0, %edx
	vpaddd	%zmm4, %zmm5, %zmm0
	vpaddd	%zmm0, %zmm6, %zmm0
	vpaddd	%zmm0, %zmm7, %zmm0
	vextracti64x4	$1, %zmm0, %ymm4
	vpaddd	%zmm4, %zmm0, %zmm0
	vextracti128	$1, %ymm0, %xmm4
	vpaddd	%xmm4, %xmm0, %xmm0
	vpshufd	$78, %xmm0, %xmm4       # xmm4 = xmm0[2,3,0,1]
	vpaddd	%xmm4, %xmm0, %xmm0
	vpshufd	$229, %xmm0, %xmm4      # xmm4 = xmm0[1,1,2,3]
	vpaddd	%xmm4, %xmm0, %xmm0
	vmovd	%xmm0, %eax
	vpaddd	%zmm12, %zmm1, %zmm0
	vpaddd	%zmm0, %zmm2, %zmm0
	vpaddd	%zmm0, %zmm3, %zmm0
	vextracti64x4	$1, %zmm0, %ymm1
	vpaddd	%zmm1, %zmm0, %zmm0
	vextracti128	$1, %ymm0, %xmm1
	vpaddd	%xmm1, %xmm0, %xmm0
	vpshufd	$78, %xmm0, %xmm1       # xmm1 = xmm0[2,3,0,1]
	vpaddd	%xmm1, %xmm0, %xmm0
	vpshufd	$229, %xmm0, %xmm1      # xmm1 = xmm0[1,1,2,3]
	vpaddd	%xmm1, %xmm0, %xmm0
	vmovd	%xmm0, %edi
	cmpq	%rsi, %r8
	je	.LBB0_9
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	addl	(%rcx), %edi
	addl	(%r9,%rsi,4), %eax
	addl	(%r11,%rsi,4), %edx
	leaq	(%r10,%rsi,4), %rcx
	addq	$4, %rcx
	incq	%rsi
	cmpq	%rsi, %r8
	jne	.LBB0_4
.LBB0_9:
	shlq	$32, %rax
	movl	%edi, %ecx
	orq	%rcx, %rax
                                        # kill: def $edx killed $edx killed $rdx
	vzeroupper
	retq
.Lfunc_end0:
	.size	Point3dSoaZipSum, .Lfunc_end0-Point3dSoaZipSum
	.cfi_endproc
                                        # -- End function

	.ident	"clang version 9.0.0 (tags/RELEASE_900/final)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __gxx_personality_v0
