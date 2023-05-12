`timescale 1ns / 1ps

module tb_gcd;

  // Declare inputs and outputs
  reg [31:0] a;
  reg [31:0] b;
  wire [31:0] ap_return;
  reg ap_clk = 0;
  reg ap_rst_n;
  reg ap_start;
  wire ap_idle;
  wire ap_done;

  // Instantiate the DUT
  gcd gcd_test (
      .a(a),
      .b(b),
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
    ap_start = 1;
    a = 32'd24;
    b = 32'd56;
    #10;
    ap_rst_n = 1;

  end

  // Test case

endmodule
