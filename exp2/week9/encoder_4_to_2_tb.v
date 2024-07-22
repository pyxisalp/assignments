`timescale 1ns / 1ps

module encoder_4_to_2_tb;
    reg a, b, c, d;
    
    wire e0, e1;
    
    encoder_4_to_2 hi(
    .a(a ),
    .b(b ),
    .c(c ),
    .d(d ),
    .e0(e0 ),
    .e1(e1 )
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