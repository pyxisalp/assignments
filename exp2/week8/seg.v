`timescale 1ns / 1ps

module seg(
    input w, x, y, z,
    output a, b, c, d, e, f, g, dp, digit
    );
    assign a = (~x & ~z) | (~w & y) | (~w & x & z) | (w & ~z) | (x & y) | (w & ~x & ~y);
    assign b = (~w & ~x) | ( ~w & ~y & ~z) | (~w & y & z) | (w & ~y & z) | (~x & ~z);
    assign c = (~w & ~y) | (~w & z) | (~w & x) | (~y & z) | (w & ~x);
    assign d = (~w & ~x & ~z) | (~w & ~x & y) | (x & ~y & z) | (x & y & ~z) | (w & ~y & ~z) | (~x & y & z);
    assign e = (~x & ~z) | (y & ~z) | (w & x) | (w & y);
    assign f = (~y & ~z) | (x & ~z) | (~w & x & ~y) | (w & y) | (w & ~x);
    assign g = (~x & y) | (~w & x & ~y) | (y & ~z) | (w & z) | (w & ~x);
    assign dp = 1;
    assign digit = a | b | c | d | e | f | g | dp;
endmodule