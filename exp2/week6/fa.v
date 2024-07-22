`timescale 1ns / 1ps

module fa(
    input a, b, ci,
    output s, co
    );
    assign s = (a ^ b) ^ ci;
    assign co = (ci & (a ^ b)) | (a & b);
endmodule