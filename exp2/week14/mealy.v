`timescale 1ns / 1ps

module mealy(
    input clk, reset, x,
    output reg [3:0] pat,
    output reg z
    );
    initial pat = 4'b0000;
    always @(posedge clk) begin
        if (reset == 1'b1) begin
            z = 0;
            pat = 1'b0000;
        end
        else begin
            pat[3] = pat[2];
            pat[2] = pat[1];
            pat[1] = pat[0];
            pat[0] = x;
            if (pat == 4'b1101)
                z = 1;
            else
                z = 0;
        end
    end
endmodule