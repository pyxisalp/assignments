`timescale 1ns / 1ps

module encoder_4_to_2(
    input a, b, c, d,
    output e0, e1
    );
    assign e0 = a | b;
    assign e1 = a | c;
endmodule