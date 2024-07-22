`timescale 1ns / 1ps

module subtractor4(
    input [3:0] A, B, 
    input b0,
    output [3:0] d, 
    output b4
    );
    
    wire b3, b2, b1;
    
    subtractor1 fs1(A[0], B[0], b0, d[0], b1);
    subtractor1 fs2(A[1], B[1], b1, d[1], b2);
    subtractor1 fs3(A[2], B[2], b2, d[2], b3);
    subtractor1 fs4(A[3], B[3], b3, d[3], b4);
endmodule