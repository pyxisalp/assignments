`timescale 1ns / 1ps

module ha_tb;
    reg a, b;
    
    wire s, c;
    
    ha hi(
    .a(a ),
    .b(b ),
    .s(s ),
    .c(c )
    );
    
    initial begin
    a = 1'b0;
    b = 1'b0;
    end
    
    always a = #20 ~a;
    always b = #10 ~b;
    
    initial begin
    #40
    $finish;
    end
endmodule