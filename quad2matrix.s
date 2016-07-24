.text

# Decodes a quadtree to the original matrix
#
# Arguments:
#     quadtree (qNode*)
#     matrix (void*)
#     matrix_width (int)
#
# Recall that quadtree representation uses the following format:
#     struct qNode {
#         int leaf;
#         int size;
#         int x;
#         int y;
#         int gray_value;
#         qNode *child_NW, *child_NE, *child_SE, *child_SW;
#     }

quad2matrix:

		bne $a0 $zero Leaf    #if qNode is a leaf jump to Leaf	
		
		addiu $sp $sp -24 
		
		lw $t2 20($a0)
		lw $t6 24($a0)
		lw $t7 28($a0)
		lw $t8 32($a0)
		
		li $t9 4
		divu $a2 $t9
		mflo $t3
		add $a2 $t3 $zero 
		
		sw $t2 0($sp)
		sw $t6 4($sp)
		sw $t7 8($sp)
		sw $t8 12($sp)
		sw $ra 16($sp)
		sw $a2 20($sp)
		
		lw $t2 0($sp)
		lw $a2 20($sp)
		lw $a0 0($t2)
		jal quad2matrix
		
		lw $t6 4($sp)
		lw $a2 20($sp)
		lw $a0 0($t6)
		jal quad2matrix
		
		lw $t7 8($sp)
		lw $a2 20($sp)
		lw $a0 0($t7)
		jal quad2matrix
		
		lw $t8 12($sp)
		lw $a2 20($sp)
		lw $a0 0($t8)
		jal quad2matrix
		
		j Fin6
		
Leaf:   lw $t0 8($a0)      #load x value into $t0
		add $t1 $t0 $a2    #add x by the width. Store in $t1
		#don't use registers $t0, $t4, $t1, and $t2 below as temps unless incrementing $t0 and $t4
		
XLoop:  slt $t3 $t0 $t1    #if x is smaller than (x + width)
		beq $t3 $zero Fin5 #if x is equal to or greater than (x + width), jump to Fin5
		
		lw $t4 12($a0)     #load y value into $t4
		add $t2 $t4 $a2    #add y by the width. Store in $t2
		
YLoop:  slt $t5 $t4 $t2    #if y is smaller than (y + width)
		beq $t5 $zero DaLoop  #if y is equal to or greater than (y + width), jump to DaLoop
						
		multu $t4 $a2      #multiply y by width
		mflo $t3
		add $t4 $t3 $t0    #$t4 = (x + width * y)
		
		lw $t8 16($a0)     #load gray value of (x, y) into $t8
		
		add $t9 $a1 $t4    #add matrix pointer to what index this (x, y) should be stored
		sb $t8 0($t9)      #store the gray_value in the correct index of matrix
		
		addi $t4 $t4 1     #increment y by 1
		j YLoop            #jump back to YLoop
		
DaLoop: addi $t0 $t0 1     #increment x by 1
        j XLoop            #jump back to XLoop
		
Fin5:   jr $ra

Fin6:   lw $ra 16($sp)
		addiu $sp $sp 24
		jr $ra