		.text
		addu 	$s0,$zero,$zero 		# s0 = 0
		addu 	$s1,$zero,$zero 		# s1 = 0
		addiu 	$v0,$zero,5 			# v0 = read value cmd
		syscall 						# v0 = read()
		sw 		$v0,n($gp) 				# M[n] = v0
L1: 	lw 		$s2,n($gp) 				# s2 = M[n]
		slt 	$t0,$s1,$s2 			# if s1 >= s2 then
		beq 	$t0,$zero,L2 			# goto L2
		addiu 	$v0,$zero,5 			# v0 = read value cmd
		syscall 						# v0 = read()
		addu 	$s0,$s0,$v0 			# s0 += v0
		addiu 	$s1,$s1,1				# s1 += 1
		j 		L1 						# goto L1
L2:		addu 	$a0,$s0,$zero 			# a0 = s0
		addiu 	$v0,$zero,1 			# v0 = print value cmd
		syscall 						# print(a0)
		addiu 	$v0,$zero,10 			# v0 = exit cmd
		syscall 						# exit()
		mult	$t2,$t3					# testtesttesttest
		mflo	$s4						# another comment			
		.data
n:		.word 	0,5,45,2,0,1
num:	.space	20
stuff:	.word	69,34,0,12