`timescale 1ns / 1ps

module and_4input_b_tb;
    reg a, b, c, d;
    wire e, f, g;
    
    and_4input_b hi(
    .a(a ),
    .b(b ),
    .c(c ),
    .d(d ),
    .e(e ),
    .f(f ),
    .g(g )
    );
    
    initial begin
    a = 1'b0;
    b = 1'b0;
    c = 1'b0;
    d = 1'b0;
    end
    
    always begin
    a = #20 ~a;
    b = #30 ~b;
    c = #40 ~c;
    d = #50 ~d;
    end
    
    initial begin
     #1000
     $finish;
    end
endmodule