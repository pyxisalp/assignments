`timescale 1ns / 1ps

module rs_nor(
    input r, s, clk,
    output reg Q, notQ
    );
    always @(*) begin
        Q <= ~((r & clk) | notQ);
        notQ <= ~((s & clk) | Q);
    end
endmodule