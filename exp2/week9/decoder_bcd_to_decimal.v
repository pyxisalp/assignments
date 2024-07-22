`timescale 1ns / 1ps

module decoder_bcd_to_decimal(
    input a3, a2, a1, a0,
    output b1, b2, b3, b4, b5, b6, b7, b8, b9
    );
    assign b1 = ~a3 & ~a2 & ~a1 & a0;
    assign b2 = ~a3 & ~a2 & a1 & ~a0;
    assign b3 = ~a3 & ~a2 & a1 & a0;
    assign b4 = ~a3 & a2 & ~a1 & ~a0;
    assign b5 = ~a3 & a2 & ~a1 & a0;
    assign b6 = ~a3 & a2 & a1 & ~a0;
    assign b7 = ~a3 & a2 & a1 & a0;
    assign b8 = a3 & ~a2 & ~a1 & ~a0;
    assign b9 = a3 & ~a2 & ~a1 & a0;
endmodule