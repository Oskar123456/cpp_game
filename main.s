	.file	"main.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.section	.text._ZNKSt5ctypeIcE8do_widenEc,"axG",@progbits,_ZNKSt5ctypeIcE8do_widenEc,comdat
	.align 2
	.p2align 4
	.weak	_ZNKSt5ctypeIcE8do_widenEc
	.type	_ZNKSt5ctypeIcE8do_widenEc, @function
_ZNKSt5ctypeIcE8do_widenEc:
.LFB2453:
	.cfi_startproc
	endbr64
	movl	%esi, %eax
	ret
	.cfi_endproc
.LFE2453:
	.size	_ZNKSt5ctypeIcE8do_widenEc, .-_ZNKSt5ctypeIcE8do_widenEc
	.text
	.p2align 4
	.globl	SDL_main
	.type	SDL_main, @function
SDL_main:
.LFB78:
	.cfi_startproc
	endbr64
	leaq	SDL_AppQuit(%rip), %r9
	leaq	SDL_AppEvent(%rip), %r8
	leaq	SDL_AppIterate(%rip), %rcx
	leaq	SDL_AppInit(%rip), %rdx
	jmp	SDL_EnterAppMainCallbacks@PLT
	.cfi_endproc
.LFE78:
	.size	SDL_main, .-SDL_main
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"ft: %lums (avg. ft: %lums)\n"
	.text
	.p2align 4
	.globl	SDL_AppIterate
	.type	SDL_AppIterate, @function
SDL_AppIterate:
.LFB4588:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	leaq	_ZL5t_now(%rip), %rdi
	call	SDL_GetCurrentTime@PLT
	movslq	_ZL10t_hist_idx(%rip), %rdx
	movq	_ZL5t_now(%rip), %rax
	pxor	%xmm0, %xmm0
	movq	%rdx, %rcx
	movq	%rax, %rdi
	subq	_ZL13t_last_update(%rip), %rdi
	movq	%rax, _ZL13t_last_update(%rip)
	leal	1(%rdx), %eax
	movl	%ecx, %esi
	imulq	$-2004318071, %rdx, %rdx
	sarl	$31, %esi
	movl	%eax, _ZL10t_hist_idx(%rip)
	leaq	_ZL6t_hist(%rip), %rax
	shrq	$32, %rdx
	addl	%ecx, %edx
	sarl	$5, %edx
	subl	%esi, %edx
	imull	$60, %edx, %edx
	subl	%edx, %ecx
	leaq	480(%rax), %rdx
	movslq	%ecx, %rcx
	movq	%rdi, (%rax,%rcx,8)
	.p2align 4,,10
	.p2align 3
.L5:
	paddq	(%rax), %xmm0
	addq	$32, %rax
	paddq	-16(%rax), %xmm0
	cmpq	%rax, %rdx
	jne	.L5
	movabsq	$-8130577079664715991, %rax
	movdqa	%xmm0, %xmm1
	psrldq	$8, %xmm1
	paddq	%xmm1, %xmm0
	movq	%xmm0, %rsi
	imulq	%rsi
	movabsq	$4835703278458516699, %rax
	leaq	(%rdx,%rsi), %rcx
	sarq	$63, %rsi
	imulq	%rdi
	sarq	$25, %rcx
	sarq	$63, %rdi
	xorl	%eax, %eax
	subq	%rsi, %rcx
	leaq	.LC0(%rip), %rsi
	sarq	$18, %rdx
	subq	%rdi, %rdx
	movl	$2, %edi
	call	__printf_chk@PLT
	movss	.LC1(%rip), %xmm3
	movaps	%xmm3, %xmm2
	movaps	%xmm3, %xmm1
	movaps	%xmm3, %xmm0
	call	*glad_glClearColor(%rip)
	movl	$16384, %edi
	call	*glad_glClear(%rip)
	movl	sp(%rip), %edi
	call	*glad_glUseProgram(%rip)
	movl	vao(%rip), %edi
	call	*glad_glBindVertexArray(%rip)
	movl	$3, %edx
	xorl	%esi, %esi
	movl	$4, %edi
	call	*glad_glDrawArrays(%rip)
	movq	(%rbx), %rdi
	call	SDL_GL_SwapWindow@PLT
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE4588:
	.size	SDL_AppIterate, .-SDL_AppIterate
	.section	.rodata.str1.1
.LC2:
	.string	"SDL_AppQuit... (%d)\n"
	.text
	.p2align 4
	.globl	SDL_AppQuit
	.type	SDL_AppQuit, @function
SDL_AppQuit:
.LFB4590:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%esi, %edx
	movq	%rdi, %rbx
	leaq	.LC2(%rip), %rsi
	movl	$2, %edi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	movq	8(%rbx), %rdi
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	SDL_GL_DestroyContext@PLT
	.cfi_endproc
.LFE4590:
	.size	SDL_AppQuit, .-SDL_AppQuit
	.p2align 4
	.globl	SDL_AppEvent
	.type	SDL_AppEvent, @function
SDL_AppEvent:
.LFB4589:
	.cfi_startproc
	endbr64
	movl	(%rsi), %eax
	cmpl	$256, %eax
	je	.L17
	cmpl	$768, %eax
	je	.L22
.L19:
	xorl	%edx, %edx
	movl	%edx, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L22:
	cmpl	$68, 24(%rsi)
	jne	.L19
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movq	app_state(%rip), %rdi
	call	SDL_GetWindowFlags@PLT
	movq	app_state(%rip), %rdi
	notq	%rax
	movq	%rax, %rsi
	andl	$1, %esi
	call	SDL_SetWindowFullscreen@PLT
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L17:
	movl	$1, %edx
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE4589:
	.size	SDL_AppEvent, .-SDL_AppEvent
	.section	.rodata.str1.1
.LC3:
	.string	"Failed to initialize GL ("
.LC4:
	.string	")"
.LC5:
	.string	"Cube Render"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC6:
	.string	"Error: SDL_CreateWindow(): %s\n"
	.align 8
.LC7:
	.string	"Error: SDL_GL_CreateContext(): %s\n"
	.section	.rodata.str1.1
.LC8:
	.string	"Failed to initialize GLAD"
.LC9:
	.string	"shaders/simp.fs"
.LC10:
	.string	"shaders/simp.vs"
	.text
	.p2align 4
	.globl	SDL_AppInit
	.type	SDL_AppInit, @function
SDL_AppInit:
.LFB4587:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	leaq	app_state(%rip), %rax
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	%rax, (%rdi)
	movl	$32, %edi
	call	SDL_InitSubSystem@PLT
	xorl	%edi, %edi
	call	SDL_GL_LoadLibrary@PLT
	testb	%al, %al
	je	.L49
	xorl	%esi, %esi
	movl	$19, %edi
	call	SDL_GL_SetAttribute@PLT
	movl	$1, %esi
	movl	$20, %edi
	call	SDL_GL_SetAttribute@PLT
	movl	$3, %esi
	movl	$17, %edi
	call	SDL_GL_SetAttribute@PLT
	movl	$3, %esi
	movl	$18, %edi
	call	SDL_GL_SetAttribute@PLT
	movl	$1, %esi
	movl	$5, %edi
	call	SDL_GL_SetAttribute@PLT
	movl	$24, %esi
	movl	$6, %edi
	call	SDL_GL_SetAttribute@PLT
	movl	$8, %esi
	movl	$7, %edi
	call	SDL_GL_SetAttribute@PLT
	movl	$34, %ecx
	movl	$600, %edx
	leaq	.LC5(%rip), %rdi
	movl	$800, %esi
	call	SDL_CreateWindow@PLT
	movq	%rax, app_state(%rip)
	movq	%rax, %rdi
	testq	%rax, %rax
	je	.L50
	movl	$805240832, %esi
	movl	$805240832, %edx
	call	SDL_SetWindowPosition@PLT
	movq	app_state(%rip), %rdi
	call	SDL_GL_CreateContext@PLT
	movq	%rax, 8+app_state(%rip)
	movq	%rax, %rsi
	testq	%rax, %rax
	je	.L51
	movq	app_state(%rip), %rdi
	leaq	_ZSt4cout(%rip), %rbx
	call	SDL_GL_MakeCurrent@PLT
	movl	$1, %edi
	call	SDL_GL_SetSwapInterval@PLT
	movq	SDL_GL_GetProcAddress@GOTPCREL(%rip), %rdi
	call	gladLoadGLLoader@PLT
	movl	$25, %edx
	leaq	.LC8(%rip), %rsi
	testl	%eax, %eax
	je	.L48
	leaq	vao(%rip), %rsi
	movl	$1, %edi
	call	*glad_glGenVertexArrays(%rip)
	movl	vao(%rip), %edi
	call	*glad_glBindVertexArray(%rip)
	leaq	vbo(%rip), %rsi
	movl	$1, %edi
	call	*glad_glGenBuffers(%rip)
	movl	vbo(%rip), %esi
	movl	$34962, %edi
	call	*glad_glBindBuffer(%rip)
	movl	$35044, %ecx
	movl	$36, %esi
	leaq	vs(%rip), %rdx
	movl	$34962, %edi
	call	*glad_glBufferData(%rip)
	xorl	%r9d, %r9d
	movl	$12, %r8d
	xorl	%ecx, %ecx
	movl	$5126, %edx
	movl	$3, %esi
	xorl	%edi, %edi
	call	*glad_glVertexAttribPointer(%rip)
	xorl	%edi, %edi
	call	*glad_glEnableVertexAttribArray(%rip)
	xorl	%edi, %edi
	call	*glad_glBindVertexArray(%rip)
	leaq	.LC9(%rip), %rsi
	leaq	.LC10(%rip), %rdi
	call	_Z16util_shader_loadPKcS0_@PLT
	leaq	_ZL13t_last_update(%rip), %rdi
	movl	%eax, sp(%rip)
	call	SDL_GetCurrentTime@PLT
	xorl	%eax, %eax
.L23:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L49:
	.cfi_restore_state
	leaq	_ZSt4cout(%rip), %rbx
	movl	$25, %edx
	leaq	.LC3(%rip), %rsi
	movq	%rbx, %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	call	SDL_GetError@PLT
	movq	%rax, %rbp
	testq	%rax, %rax
	je	.L52
	movq	%rax, %rdi
	call	strlen@PLT
	movq	%rbp, %rsi
	movq	%rbx, %rdi
	movq	%rax, %rdx
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
.L26:
	movl	$1, %edx
	leaq	.LC4(%rip), %rsi
.L48:
	movq	%rbx, %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	movq	_ZSt4cout(%rip), %rax
	movq	-24(%rax), %rax
	movq	240(%rbx,%rax), %rbp
	testq	%rbp, %rbp
	je	.L53
	cmpb	$0, 56(%rbp)
	je	.L36
	movsbl	67(%rbp), %esi
.L37:
	movq	%rbx, %rdi
	call	_ZNSo3putEc@PLT
	movq	%rax, %rdi
	call	_ZNSo5flushEv@PLT
.L30:
	movl	$2, %eax
	jmp	.L23
	.p2align 4,,10
	.p2align 3
.L36:
	movq	%rbp, %rdi
	call	_ZNKSt5ctypeIcE13_M_widen_initEv@PLT
	movq	0(%rbp), %rax
	movl	$10, %esi
	leaq	_ZNKSt5ctypeIcE8do_widenEc(%rip), %rdx
	movq	48(%rax), %rax
	cmpq	%rdx, %rax
	je	.L37
	movq	%rbp, %rdi
	call	*%rax
	movsbl	%al, %esi
	jmp	.L37
	.p2align 4,,10
	.p2align 3
.L52:
	movq	_ZSt4cout(%rip), %rax
	movq	-24(%rax), %rdi
	addq	%rbx, %rdi
	movl	32(%rdi), %esi
	orl	$1, %esi
	call	_ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate@PLT
	jmp	.L26
	.p2align 4,,10
	.p2align 3
.L50:
	call	SDL_GetError@PLT
	leaq	.LC6(%rip), %rsi
	movl	$2, %edi
	movq	%rax, %rdx
	xorl	%eax, %eax
	call	__printf_chk@PLT
	jmp	.L30
	.p2align 4,,10
	.p2align 3
.L51:
	call	SDL_GetError@PLT
	leaq	.LC7(%rip), %rsi
	movl	$2, %edi
	movq	%rax, %rdx
	xorl	%eax, %eax
	call	__printf_chk@PLT
	jmp	.L30
.L53:
	call	_ZSt16__throw_bad_castv@PLT
	.cfi_endproc
.LFE4587:
	.size	SDL_AppInit, .-SDL_AppInit
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB79:
	.cfi_startproc
	endbr64
	xorl	%ecx, %ecx
	leaq	SDL_main(%rip), %rdx
	jmp	SDL_RunApp@PLT
	.cfi_endproc
.LFE79:
	.size	main, .-main
	.text
	.p2align 4
	.globl	_Z12key_callback17SDL_KeyboardEvent
	.type	_Z12key_callback17SDL_KeyboardEvent, @function
_Z12key_callback17SDL_KeyboardEvent:
.LFB4586:
	.cfi_startproc
	endbr64
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	cmpl	$68, 40(%rsp)
	jne	.L56
	movq	app_state(%rip), %rdi
	call	SDL_GetWindowFlags@PLT
	movq	app_state(%rip), %rdi
	notq	%rax
	movq	%rax, %rsi
	andl	$1, %esi
	call	SDL_SetWindowFullscreen@PLT
.L56:
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE4586:
	.size	_Z12key_callback17SDL_KeyboardEvent, .-_Z12key_callback17SDL_KeyboardEvent
	.globl	metadata
	.section	.rodata.str1.1
.LC11:
	.string	"SDL.app.metadata.url"
	.section	.rodata.str1.8
	.align 8
.LC12:
	.string	"https://examples.libsdl.org/SDL3/demo/01-snake/"
	.section	.rodata.str1.1
.LC13:
	.string	"SDL.app.metadata.creator"
.LC14:
	.string	"SDL team"
.LC15:
	.string	"SDL.app.metadata.copyright"
.LC16:
	.string	"Placed in the public domain"
.LC17:
	.string	"SDL.app.metadata.type"
.LC18:
	.string	"game"
	.section	.data.rel.local,"aw"
	.align 32
	.type	metadata, @object
	.size	metadata, 64
metadata:
	.quad	.LC11
	.quad	.LC12
	.quad	.LC13
	.quad	.LC14
	.quad	.LC15
	.quad	.LC16
	.quad	.LC17
	.quad	.LC18
	.globl	app_state
	.bss
	.align 16
	.type	app_state, @object
	.size	app_state, 16
app_state:
	.zero	16
	.globl	sp
	.align 4
	.type	sp, @object
	.size	sp, 4
sp:
	.zero	4
	.globl	ebo
	.align 4
	.type	ebo, @object
	.size	ebo, 4
ebo:
	.zero	4
	.globl	vbo
	.align 4
	.type	vbo, @object
	.size	vbo, 4
vbo:
	.zero	4
	.globl	vao
	.align 4
	.type	vao, @object
	.size	vao, 4
vao:
	.zero	4
	.globl	vs
	.data
	.align 32
	.type	vs, @object
	.size	vs, 36
vs:
	.long	-1090519040
	.long	-1090519040
	.long	0
	.long	1056964608
	.long	-1090519040
	.long	0
	.long	0
	.long	1056964608
	.long	0
	.local	_ZL10t_hist_idx
	.comm	_ZL10t_hist_idx,4,4
	.local	_ZL6t_hist
	.comm	_ZL6t_hist,480,32
	.local	_ZL5t_now
	.comm	_ZL5t_now,8,8
	.local	_ZL13t_last_update
	.comm	_ZL13t_last_update,8,8
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC1:
	.long	1065353216
	.ident	"GCC: (Ubuntu 13.2.0-23ubuntu4) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
