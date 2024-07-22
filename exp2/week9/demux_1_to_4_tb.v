`timescale 1ns / 1ps

module demux_1_to_4_tb;
    reg b, a, F;
    
    wire D, C, B, A;
    
    demux_1_to_4 hi(
    .b(b ),
    .a(a ),
    .F(F ),
    .D(D ),
    .C(C ),
    .B(B ),
    .A(A )
    );
    
    initial begin
    b = 1'b0;
    a = 1'b0;
    F = 1'b0;
    end
    
    always b = #40 ~b;
    always a = #20 ~a;
    always F = #10 ~F;
    
    initial begin
    #80
    $finish;
    end
endmodule