module SRAM (
    input [31:0] address0,
    input [31:0] d0,
    input ce0,
    input we0,
    output reg [31:0] q0
);
  reg [31:0] DATA[9:0];
  always @(*) begin
    if (ce0 == 1'b1) begin
      q0 <= DATA[address0];
    end
    if (we0 == 1'b1) begin
      DATA[address0] <= d0;
    end
  end
endmodule
