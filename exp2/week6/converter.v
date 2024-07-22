`timescale 1ns / 1ps

module converter(
    input a, b, c, d,
    output w, x, y, z
    );
    assign w = ~(~a & (~(b & c)) & (~(b & d)));
    assign x = ~(~a & (~(b & c)) & (~(b & ~d)));
    assign y = ~(~a & (~(~b & c)) & (~(b & ~c & d)));
    assign z = d;
endmodule