`timescale 1ns / 1ps

module fs(
    input a, b, bi,
    output d, bo
    );
    assign d = a ^ b ^ bi;
    assign bo = (~(a ^ b) & bi) | (~a & b);
endmodule