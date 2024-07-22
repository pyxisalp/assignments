`timescale 1ns / 1ps

module mealy_tb;
    reg clk, reset, x;
    
    wire [3:0] pat;
    wire z;
    
    mealy test(
    .clk(clk),
    .reset(reset),
    .x(x),
    .pat(pat),
    .z(z)
    );
    
    initial begin
    clk = 1'b0;
    reset = 1'b0;
    x = 1'b0;
    #10
    
    x = 1'b1;
    #10
    
    x = 1'b1;
    #10
    
    x = 1'b0;
    #10
    
    x = 1'b1;
    #10
    
    x = 1'b0;
    #10
    
    x = 1'b0;
    #10
    
    x = 1'b1;
    #10
    
    x = 1'b1;
    #10
    
    x = 1'b0;
    #10
    
    x = 1'b1;
    #10
    
    x = 1'b0;
    #100
    $finish;
    end
    
    always reset = #300 ~reset;
    always clk = #5 ~clk;
    
    initial begin
    #200   
    $finish;
    end
endmodule