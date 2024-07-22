`timescale 1ns / 1ps

module adder4(
    input [3:0] a, [3:0] b, 
    input c0,
    output [3:0] s, 
    output c4
    );
    
    wire c3, c2, c1;
    
    adder1 fa1(a[0], b[0], c0, s[0], c1);
    adder1 fa2(a[1], b[1], c1, s[1], c2);
    adder1 fa3(a[2], b[2], c2, s[2], c3);
    adder1 fa4(a[3], b[3], c3, s[3], c4);
endmodule