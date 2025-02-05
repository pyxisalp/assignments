`timescale 1ns / 1ps

module de2_b_tb;
    reg a, b, c;
    
    wire d;
    
    de2_b hi(
    .a(a ),
    .b(b ),
    .c(c ),
    .d(d )
    );
    
    initial begin
    a = 1'b0;
    b = 1'b0;
    c = 1'b0;
    end
    
    always a = #40 ~a;
    always b = #20 ~b;
    always c = #10 ~c;
    
    initial begin
    #80
    $finish;
    end
endmodule