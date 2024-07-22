`timescale 1ns / 1ps

module cmp2_tb;
    reg a, b, c, d;
    
    wire f1, f2, f3;
    
    cmp2 hi(
    .a(a ),
    .b(b ),
    .c(c ),
    .d(d ),
    .f1(f1 ),
    .f2(f2 ),
    .f3(f3 )
    );
    
    initial begin
    a = 1'b0;
    b = 1'b0;
    c = 1'b0;
    d = 1'b0;
    end
    
    always a = #80 ~a;
    always b = #40 ~b;
    always c = #20 ~c;
    always d = #10 ~d;
    
    initial begin
    #160
    $finish;
    end
endmodule