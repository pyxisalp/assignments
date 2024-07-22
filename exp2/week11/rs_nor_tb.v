`timescale 1ns / 1ps

module rs_nor_tb;
    reg r, s, clk;
    
    wire Q, notQ;
    
    rs_nor hi(
    .r(r),
    .s(s),
    .clk(clk),
    .Q(Q),
    .notQ(notQ)
    );
    
    initial begin
    r = 1'b0;
    s = 1'b1;
    clk = 1'b0;
    #50
    
    r = 1'b0;
    s = 1'b0;
    #50
    
    r = 1'b1;
    s = 1'b0;
    #50
    
    r = 1'b0;
    s = 1'b0;
    #50   

    r = 1'b1;
    s = 1'b0;
    #50

    r = 1'b1;
    s = 1'b1;
    #50
    $finish;
    end
    
    always clk = #10 ~clk;
    
    initial begin
    #400   
    $finish;
    end
endmodule