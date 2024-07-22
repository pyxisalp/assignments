`timescale 1ns / 1ps

module demux_1_to_4(
    input b, a, F,
    output D, C, B, A
    );
    assign D = b & a & F;
    assign C = b & ~a & F;
    assign B = ~b & a & F;
    assign A = ~b & ~a & F;
endmodule