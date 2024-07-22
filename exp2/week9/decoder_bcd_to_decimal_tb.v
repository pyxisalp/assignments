`timescale 1ns / 1ps

module decoder_bcd_to_decimal_tb;
    reg a3, a2, a1, a0;
    
    wire b1, b2, b3, b4, b5, b6, b7, b8, b9;
    
    decoder_bcd_to_decimal hi(
    .a3(a3 ),
    .a2(a2 ),
    .a1(a1 ),
    .a0(a0 ),
    .b1(b1 ),
    .b2(b2 ),
    .b3(b3 ),
    .b4(b4 ),
    .b5(b5 ),
    .b6(b6 ),
    .b7(b7 ),
    .b8(b8 ),
    .b9(b9 )
    );
    
    initial begin
    a3 = 1'b0;
    a2 = 1'b0;
    a1 = 1'b0;
    a0 = 1'b0;    
    end
    
    always a3 = #80 ~a3;
    always a2 = #40 ~a2;
    always a1 = #20 ~a1;
    always a0 = #10 ~a0;
    
    initial begin
    #160
    $finish;
    end
endmodule