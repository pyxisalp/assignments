`timescale 1ns / 1ps

module updown(
    input clk, reset, u0d1,
    output reg [3:0] out,
    output reg [6:0] dp,
    output reg digit
    );
    initial out = 4'b0000;
    always @(posedge clk) begin
        if (reset == 1'b1) begin
            out <= 1'b0000;
        end
        else if (u0d1 == 1'b0) begin
                out <= out + 1;
                dp[0] <= 0;
                dp[1] <= 1;
                dp[2] <= 1;
                dp[3] <= 1;
                dp[4] <= 1;
                dp[5] <= 1;
                dp[6] <= 0;
        end
        else if (u0d1 == 1'b1) begin
                out <= out - 1;
                dp[0] <= 0;
                dp[1] <= 1;
                dp[2] <= 1;
                dp[3] <= 1;
                dp[4] <= 1;
                dp[5] <= 0;
                dp[6] <= 1;
        end
        digit <= dp[0] | dp[1] | dp[2] | dp[3] | dp[4] | dp[5] | dp[6];
    end
endmodule