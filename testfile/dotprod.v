module dotprod
(
	input	[31:0] a_q0,
	output	reg [31:0] a_address0,
	output	reg [31:0] a_ad0,
	output	reg a_ce0,
	output	reg a_we0,
	input	[31:0] b_q0,
	output	reg [31:0] b_address0,
	output	reg [31:0] b_ad0,
	output	reg b_ce0,
	output	reg b_we0,
	input	[31:0] n,
	output	ap_return,
	input	ap_clk,
	input	ap_rst_n,
	input	ap_start,
	output	reg ap_idle,
	output	reg ap_done
);

	reg [4:0] CurrentState;
	reg [4:0] LastState;
	wire cond;
	reg branch_ready;


	parameter state_fiction_head = 5'b10000;
	parameter state_0 = 5'b01000;
	parameter state_start = 5'b00100;
	parameter state_calc = 5'b00010;
	parameter state_ret = 5'b00001;


	always @(posedge ap_clk or negedge ap_rst_n)
	begin
		if(!ap_rst_n)
		begin
			LastState <= state_fiction_head;
			CurrentState <= state_fiction_head;
		end
		else if(CurrentState == state_fiction_head & branch_ready == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_0;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_0 & branch_ready == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_start;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_start & branch_ready == 1'b1 & cond == 1'b0)
		begin
			LastState <= CurrentState;
			CurrentState <= state_ret;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_start & branch_ready == 1'b1 & cond == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_calc;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_calc & branch_ready == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_start;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_ret & branch_ready == 1'b1)
		begin
			ap_done <= 1'b1;
			branch_ready <= 1'b0;
		end
	end


reg[31:0] counter
	always @(posedge ap_clk or negedge ap_rst_n)
	begin
	if(!ap_rst_n)
		counter <= 0
	if(CurrentState == state_fiction_head && counter == 0)
		counter <= 0
	if(CurrentState == state_0 && counter == 8)
		counter <= 0
	if(CurrentState == state_start && counter == 20)
		counter <= 0
	if(CurrentState == state_calc && counter == 36)
		counter <= 0
	if(CurrentState == state_ret && counter == 1)
		counter <= 0
	else
		counter <= counter + 1
	end
	always@(counter)
	case(CurrentState)
	state_0: begin
		case(counter)
		32'b1: begin
test  test  test
		end
		32'b2: begin
test  test  test
		end
		32'b3: begin
test  test  test
		end
		32'b4: begin
test  test  test
		end
		32'b5: begin
test  test  test
		end
		32'b6: begin
test  test  test
		end
		32'b7: begin
test  test  test
		end
		32'b8: begin
test  test  test
		end
		endcase
	end
	state_start: begin
		case(counter)
		32'b1: begin
test  test  test
		end
		32'b2: begin
test  test  test
		end
		32'b3: begin
test  test  test
		end
		32'b4: begin
test  test  test
		end
		32'b5: begin
test  test  test
		end
		32'b6: begin
test  test  test
		end
		32'b7: begin
test  test  test
		end
		32'b8: begin
test  test  test
		end
		32'b9: begin
test  test  test
		end
		32'b10: begin
test  test  test
		end
		32'b11: begin
test  test  test
		end
		32'b12: begin
test  test  test
		end
		32'b13: begin
test  test  test
		end
		32'b14: begin
test  test  test
		end
		32'b15: begin
test  test  test
		end
		32'b16: begin
test  test  test
		end
		32'b17: begin
test  test  test
		end
		32'b18: begin
test  test  test
		end
		32'b19: begin
test  test  test
		end
		32'b20: begin
test  test  test
		end
		endcase
	end
	state_calc: begin
		case(counter)
		32'b1: begin
test  test  test
		end
		32'b2: begin
test  test  test
		end
		32'b3: begin
test  test  test
		end
		32'b4: begin
test  test  test
		end
		32'b5: begin
test  test  test
		end
		32'b6: begin
test  test  test
		end
		32'b7: begin
test  test  test
		end
		32'b8: begin
test  test  test
		end
		32'b9: begin
test  test  test
		end
		32'b10: begin
test  test  test
		end
		32'b11: begin
test  test  test
		end
		32'b12: begin
test  test  test
		end
		32'b13: begin
test  test  test
		end
		32'b14: begin
test  test  test
		end
		32'b15: begin
test  test  test
		end
		32'b16: begin
test  test  test
		end
		32'b17: begin
test  test  test
		end
		32'b18: begin
test  test  test
		end
		32'b19: begin
test  test  test
		end
		32'b20: begin
test  test  test
		end
		32'b21: begin
test  test  test
		end
		32'b22: begin
test  test  test
		end
		32'b23: begin
test  test  test
		end
		32'b24: begin
test  test  test
		end
		32'b25: begin
test  test  test
		end
		32'b26: begin
test  test  test
		end
		32'b27: begin
test  test  test
		end
		32'b28: begin
test  test  test
		end
		32'b29: begin
test  test  test
		end
		32'b30: begin
test  test  test
		end
		32'b31: begin
test  test  test
		end
		32'b32: begin
test  test  test
		end
		32'b33: begin
test  test  test
		end
		32'b34: begin
test  test  test
		end
		32'b35: begin
test  test  test
		end
		32'b36: begin
test  test  test
		end
		endcase
	end
	state_ret: begin
		case(counter)
		32'b1: begin
test  test  test
		end
		endcase
	end
	endcase
endmodule
