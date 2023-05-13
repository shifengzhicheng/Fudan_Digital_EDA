`timescale 1ns / 1ps

module tb_Sum;
    // Declare inputs and outputs
    wire [31:0] a_q0;
    wire [31:0] a_address0;
    wire [31:0] a_ad0;
    wire a_ce0;
    wire a_we0;
    
    wire [31:0] b_q0;
    wire [31:0] b_address0;
    wire [31:0] b_ad0;
    wire b_ce0;
    wire b_we0;

    reg [31:0] n;
    wire [31:0] ap_return;
    reg ap_clk = 0;
    reg ap_rst_n = 0;
    reg ap_start;
    wire ap_idle;
    wire ap_done;

    reg wa;
    reg [31:0] ad;
    wire [31:0] a_ad1;
    reg [31:0] aadr;
    wire [31:0] a_address1;

    reg wb;
    reg [31:0] bd;
    wire [31:0] b_ad1;
    reg [31:0] badr;
    wire [31:0] b_address1;

    // Instantiate the DUT
    assign a_ad1 = ap_start ? a_ad0 : ad;
    assign b_ad1 = ap_start ? b_ad0 : bd;
    assign a_address1 = ap_start ? a_address0 : aadr;
    assign b_address1 = b_address0;
    SRAM a (
        .address0(a_address1),
        .d0(a_ad1),
        .ce0(a_ce0),
        .we0(a_we0 || wa),
        .q0(a_q0)
    );
    SRAM b (
        .address0(b_address1),
        .d0(b_ad1),
        .ce0(b_ce0),
        .we0(b_we0 || wb),
        .q0(b_q0)
    );
    Sum sum(
        .a_q0(a_q0),
        .a_address0(a_address0),
        .a_ad0(a_ad0),
        .a_ce0(a_ce0),
        .a_we0(a_we0),
        .b_q0(b_q0),
        .b_address0(b_address0),
        .b_ad0(b_ad0),
        .b_ce0(b_ce0),
        .b_we0(b_we0),
        .n(n),
        .ap_return(ap_return),
        .ap_clk(ap_clk),
        .ap_rst_n(ap_rst_n),
        .ap_start(ap_start),
        .ap_idle(ap_idle),
        .ap_done(ap_done)
    );

    // Clock generation
    always #5 ap_clk = ~ap_clk;

    // Reset generation
    initial begin
        ap_rst_n = 0;
        ap_start = 0;
        wa = 1;
        wb = 1;
        
        #2
        aadr = 0;
        ad = 1;
        
        #2
        aadr = 1;
        ad = 2;
        
        #2
        aadr = 2;
        ad = 3;
        
        #2
        aadr = 3;
        ad = 4;
        
        #2
        aadr = 4;
        ad = 5;
        
        #2
        aadr = 5;
        ad = 6;
        
        #2
        aadr = 6;
        ad = 7;
        
        #2
        aadr = 7;
        ad = 8;
        
        #2
        aadr = 8;
        ad = 9;
        
        #2
        aadr = 9;
        ad = 10;      
        
        #20;
        wa = 0;
        wb = 0;
        ap_start = 1;
        ap_rst_n = 1;
        n = 32'd10;
    end

    // Test case

endmodule

