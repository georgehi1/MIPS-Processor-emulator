        .section .text



addi $2, $0, 0     # F0
addi $3, $0, 1     # F1
addi $4, $0, 1     # Counter

fibonacci_loop:
        add $5, $2, $3    # $11 = F0 + F1 (Next Fibonacci number)
        addi $6, $0, 22   # Load 22 into $12 for comparison
        slt $7, $4, $6  # Check if Counter < 22 (28657 requires 22 Fibonacci numbers)
        sw $5, 0x7f08($0) # print number
        beq $7, $0, hang_processor   # If Counter = 22, hang processor 

        add $2, $3, $0     # F0 = F1
        add $3, $5, $0    # F1 = Next Fibonacci number
        addi $4, $4, 1   # Increment Counter
        j fibonacci_loop
        sw $8, 0x7f00($0)             

hang_processor:
         
        j hang_processor
        NOP          # Enter infinite loop to halt processor
