`timescale 1ns / 1ps

module decade8421_tb;
    reg clk, reset, x;
    wire z;
    wire [3:0] state;
    
    decade8421 test(
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
    
    always reset = #700 ~reset;
    always x = #20 ~x;
    always clk = #10 ~clk;
    
    initial begin
    #800
    $finish;
    end
endmodule