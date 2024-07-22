`timescale 1ns / 1ps

module cmp2(
    input a, b, c, d,
    output f1, f2, f3
    );
    assign f1 = (a & b & ~d) | (a & ~c) | (b & ~c & ~d);
    assign f2 = (a & b & c & d) | (a & ~b & c & ~d) | (~a & b & ~c & d) | (~a & ~b & ~c & ~d);
    assign f3 = (~a & ~b & d) | (~a & c) | (~b & c & d);
endmodule