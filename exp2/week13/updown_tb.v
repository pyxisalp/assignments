`timescale 1ns / 1ps

module updown_tb;
    reg clk, reset, u0d1;
    
    wire [3:0] out;
    
    updown test(
    .clk(clk),
    .reset(reset),
    .u0d1(u0d1),
    .out(out)
    );
    
    initial begin
    clk = 1'b0;
    reset = 1'b0;
    u0d1 = 1'b0;
    end
    
    always reset = #400 ~reset;
    always u0d1 = #200 ~u0d1;
    always clk = #5 ~clk;
    
    initial begin
    #600   
    $finish;
    end
endmodule