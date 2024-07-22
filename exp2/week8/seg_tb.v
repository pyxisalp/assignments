`timescale 1ns / 1ps

module seg_tb;
    reg w, x, y ,z;
    
    wire a, b, c, d, e, f, g, dp, digit;
    
    seg hi(
    .w(w ),
    .x(x ),
    .y(y ),
    .z(z ),
    .a(a ),
    .b(b ),
    .c(c ),
    .d(d ),
    .e(e ),
    .f(f ),
    .g(g ),
    .dp(dp ),
    .digit(digit )
    );
    
    initial begin
    w = 1'b0;
    x = 1'b0;
    y = 1'b0;
    z = 1'b0;
    end
    
    always w = #80 ~w;
    always x = #40 ~x;
    always y = #20 ~y;
    always z = #10 ~z;
    
    initial begin
    #160
    $finish;
    end
endmodule