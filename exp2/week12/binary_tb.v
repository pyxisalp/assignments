`timescale 1ns / 1ps

module binary_tb;
    reg clk, reset, x;
    wire z;
    wire [1:0] state;
    
    binary test(
    .clk(clk),
    .reset(reset),
    .x(x),
    .z(z),
    .state(state)
    );
    
    initial begin
    clk = 1'b1;
    reset = 1'b0;
    x = 1'b0;
    end
    
    always reset = #300 ~reset;
    always x = #20 ~x;
    always clk = #10 ~clk;
    
    initial begin
    #400
    $finish;
    end
endmodule