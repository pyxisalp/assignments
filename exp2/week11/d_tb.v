`timescale 1ns / 1ps

module d_tb;
    reg d, clk;
    
    wire Q, notQ;
    
    d hi(
    .d(d),
    .clk(clk),
    .Q(Q),
    .notQ(notQ)
    );
    
    initial begin
    d = 1'b0;
    clk = 1'b0;
    #50
    
    d = 1'b0;
    #50
    
    d = 1'b1;
    #50
    
    d = 1'b0;
    #50   

    d = 1'b1;
    #50

    d = 1'b1;
    #50
    $finish;
    end
    
    always clk = #10 ~clk;
    
    initial begin
    #400   
    $finish;
    end
endmodule