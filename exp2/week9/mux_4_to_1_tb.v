`timescale 1ns / 1ps

module mux_4_to_1_tb;
    reg b, a, D, C, B, A;
    
    wire out;
    
    mux_4_to_1 hi(
    .b(b ),
    .a(a ),
    .D(D ),
    .C(C ),
    .B(B ),
    .A(A ),
    .out(out )
    );
    
    initial begin
    D = 1'b0;
    C = 1'b0;
    B = 1'b0;
    A = 1'b0;
    b = 1'b0;
    a = 1'b0;
    end
    
    always D = #320 ~D;
    always C = #160 ~C;
    always B = #80 ~B;
    always A = #40 ~A;
    always b = #20 ~b;
    always a = #10 ~a;
    
    initial begin
    #640
    $finish;
    end
endmodule