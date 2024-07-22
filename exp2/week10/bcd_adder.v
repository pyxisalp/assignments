`timescale 1ns / 1ps

module bcd_adder(
    input [3:0] a, b, 
    input c0,
    output [3:0] sum, 
    output dummy, carry
    );
    wire c4;
    wire [3:0] s, temp;
    wire [4:1] c;
    adder4 adder4_1(a, b, c0, s, c4);
    assign carry = c4 | (s[3] & s[2]) | (s[3] & s[1]);
    assign temp[3] = 0;
    assign temp[2] = carry;
    assign temp[1] = carry;
    assign temp[0] = 0;
    adder4 adder4_2(temp, s, 0, sum, dummy);
endmodule