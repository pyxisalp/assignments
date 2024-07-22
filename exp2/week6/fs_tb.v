`timescale 1ns / 1ps

module fs_tb;
    reg a, b, bi;
    
    wire d, bo;
    
    fs hi(
    .a(a ),
    .b(b ),
    .bi(bi ),
    .d(d ),
    .bo(bo )
    );
    
    initial begin
    a = 1'b0;
    b = 1'b0;
    bi = 1'b0;
    end
    
    always a = #40 ~a;
    always b = #20 ~b;
    always bi = #10 ~bi;
    
    initial begin
    #80
    $finish;
    end
endmodule