`timescale 1ns / 1ps

module decade8421(
    input clk, reset, x,
    output reg z,
    output reg [3:0] state
    );
    
    initial begin
        z <= 1'b0;
        state <= 4'b0000;
    end
    
    always @(posedge clk) begin
        if (reset == 1'b1) begin
            state <= 4'b0000;
            z <= 1'b0;
        end
        else if (x == 1'b1 && state == 4'b1001) begin
            state <= 4'b0000;
            z <= 1'b1;
        end
        else if (x == 1'b1) begin
                state <= state + 1;
                z <= 1'b0;
        end
    end
endmodule