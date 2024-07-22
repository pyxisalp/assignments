`timescale 1ns / 1ps

module binary(
    input clk, reset, x,
    output reg z,
    output reg [1:0] state
    );
    
    initial begin
        z <= 1'b0;
        state <= 2'b00;
    end
    
    always @(posedge clk) begin
        if (reset == 1'b1) begin
            state <= 2'b00;
            z <= 1'b0;
        end
        else if (x == 1'b1) begin
            if (state == 2'b11)
                z <= 1'b1;
            else
                z <= 1'b0;
            state <= state + 1;
        end
    end
endmodule