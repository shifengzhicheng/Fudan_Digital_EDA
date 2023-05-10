`timescale 1ns / 1ps

module tb_dotprod;

  // Declare inputs and outputs
    reg ap_clk = 0;
    reg ap_rst_n = 0;
    reg [31:0] a_q0;
	wire [31:0] a_address0;
	wire [31:0] a_ad0;
	wire a_ce0;
	wire a_we0;
	reg [31:0] b_q0;
	wire [31:0] b_address0;
	wire [31:0] b_ad0;
	wire b_ce0;
	wire b_we0;
	reg [31:0] n;
	wire ap_return;
	reg ap_start;
	wire ap_idle;
	wire ap_done;
  // Instantiate the DUT
  dotprod dut (
	.a_q0(a_q0),
	.a_address0(a_address),
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
    ap_start = 1;
    #10;
    ap_rst_n = 1;
  end

  // Test case

endmodule
