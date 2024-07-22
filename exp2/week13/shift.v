`timescale 1ns / 1ps

module shift(
    input clk, reset, in,
    output reg [3:0] out
    );
    initial out = 4'b0000;
    always @(posedge clk) begin
        if (reset == 1'b1) begin
            out <= 1'b0000;
        end
        else begin
            out[0] <= in;
            out[1] <= out[0];
            out[2] <= out[1];
            out[3] <= out[2];
        end
    end
endmodule