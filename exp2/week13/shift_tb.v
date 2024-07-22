`timescale 1ns / 1ps

module shift_tb;
    reg clk, reset, in;
    
    wire [3:0] out;
    
    shift test(
    .clk(clk),
    .reset(reset),
    .in(in),
    .out(out)
    );
    
    initial begin
    clk = 1'b0;
    reset = 1'b0;
    in = 1'b0;
    end
    
    always reset = #300 ~reset;
    always in = #80 ~in;
    always clk = #10 ~clk;
    
    initial begin
    #400   
    $finish;
    end
endmodule