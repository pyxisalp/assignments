`timescale 1ns / 1ps

module mux_4_to_1(
    input b, a, D, C, B, A,
    output out
    );
    assign out = (~b & ~a & A) | (~b & a & B) | (b & ~a & C) | (b & a & D);
endmodule