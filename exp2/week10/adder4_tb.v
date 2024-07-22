`timescale 1ns / 1ps

module adder4_tb;
    reg [3:0] a, b;
    reg c0;
    
    wire c4;
    wire [3:0] s;
    
    adder4 hi(
    .a(a),
    .b(b),
    .c0(c0),
    .s(s),
    .c4(c4)
    );
    
    initial begin
    a = 4'b0000;
    b = 4'b0000;
    c0 = 1'b0;
    end
    
    always a[3] = #256 ~a[3];
    always a[2] = #128 ~a[2];
    always a[1] = #64 ~a[1];
    always a[0] = #32 ~a[0];
    always b[3] = #16 ~b[3];
    always b[2] = #8 ~b[2];
    always b[1] = #4 ~b[1];
    always b[0] = #2 ~b[0];
    always c0 = #1 ~c0;
    
    initial begin
    #512
    $finish;
    end
endmodule