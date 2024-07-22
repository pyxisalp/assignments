`timescale 1ns / 1ps

module ring_tb;
    reg clk, reset;
    
    wire [3:0] out;
    
    ring test(
    .clk(clk),
    .reset(reset),
    .out(out)
    );
    
    initial begin
    clk = 1'b0;
    reset = 1'b0;
    end
    
    always reset = #300 ~reset;
    always clk = #10 ~clk;
    
    initial begin
    #400   
    $finish;
    end
endmodule