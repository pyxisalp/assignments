`timescale 1ns / 1ps

module gen_even(
    input a, b, c, d,
    output e
    );
    assign e = a ^ b ^ c ^ d;
endmodule