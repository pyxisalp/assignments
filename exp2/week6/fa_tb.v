`timescale 1ns / 1ps

module fa_tb;
    reg a, b, ci;
    
    wire s, co;
    
    fa hi(
    .a(a ),
    .b(b ),
    .ci(ci ),
    .s(s ),
    .co(co )
    );
    
    initial begin
    a = 1'b0;
    b = 1'b0;
    ci = 1'b0;
    end
    
    always a = #40 ~a;
    always b = #20 ~b;
    always ci = #10 ~ci;
    
    initial begin
    #80
    $finish;
    end
endmodule