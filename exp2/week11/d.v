`timescale 1ns / 1ps

module d(
    input d, clk,
    output reg Q, notQ
    );
    always @(*) begin
        Q <= ~((clk & ~d) | notQ);
        notQ <= ~((clk & d) | Q);
    end
endmodule