`timescale 1ns / 1ps

module decoder_2_to_4_and_tb;
    reg a, b;
    
    wire d0, d1, d2, d3;
    
    decoder_2_to_4_and hi(
    .a(a ),
    .b(b ),
    .d0(d0 ),
    .d1(d1 ),
    .d2(d2 ),
    .d3(d3 )
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