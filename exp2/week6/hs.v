`timescale 1ns / 1ps

module hs(
    input a, b,
    output d, bb
    );
    assign d = a ^ b;
    assign bb = ~a & b;
endmodule