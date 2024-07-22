`timescale 1ns / 1ps

module check_even_tb;
    reg a, b, c, d, p;
    
    wire pec;
    
    check_even hi(
    .a(a ),
    .b(b ),
    .c(c ),
    .d(d ),
    .p(p ),
    .pec(pec )
    );
    
    initial begin
    a = 1'b0;
    b = 1'b0;
    c = 1'b0;
    d = 1'b0;
    p = 1'b0;
    end
    
    always a = #160 ~a;
    always b = #80 ~b;
    always c = #40 ~c;
    always d = #20 ~d;
    always p = #10 ~p;
    
    initial begin
    #320
    $finish;
    end
endmodule