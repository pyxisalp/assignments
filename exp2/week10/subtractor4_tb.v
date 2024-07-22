`timescale 1ns / 1ps

module subtractor4_tb;
    reg [3:0] A, B;
    reg b0;
    
    wire b4;
    wire [3:0] d;
    
    subtractor4 hi(
    .A(A),
    .B(B),
    .b0(b0),
    .b4(b4),
    .d(d)
    );
    
    initial begin
    A = 4'b0000;
    B = 4'b0000;
    b0 = 1'b0;
    end
    
    always A[3] = #256 ~A[3];
    always A[2] = #128 ~A[2];
    always A[1] = #64 ~A[1];
    always A[0] = #32 ~A[0];
    always B[3] = #16 ~B[3];
    always B[2] = #8 ~B[2];
    always B[1] = #4 ~B[1];
    always B[0] = #2 ~B[0];
    always b0 = #1 ~b0;
    
    initial begin
    #512
    $finish;
    end
endmodule