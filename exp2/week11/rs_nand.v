`timescale 1ns / 1ps

module rs_nand(
    input r, s, clk,
    output reg Q, notQ
    );
    always @(*) begin
        Q <= ~(~(s & clk) & notQ);
        notQ <= ~(~(r & clk) & Q);
    end
endmodule