`timescale 1ns / 1ps

module cmp_tb;
    reg a, b;
    
    wire c, d, e, f;
    
    cmp hi(
    .a(a ),
    .b(b ),
    .c(c ),
    .d(d ),
    .e(e ),
    .f(f )
    );
    
    initial begin
    a = 1'b0;
    b = 1'b0;
    end
    
    always a = #20 ~b;
    always b = #10 ~b;
    
    initial begin
    #40
    $finish;
    end
endmodule