`timescale 1ns / 1ps

module de1_b(
    input a, b, c,
    output d
    );
    assign d = ~((a & b) | c);
endmodule