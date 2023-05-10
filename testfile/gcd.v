module gcd
(
	input	[31:0] a,
	input	[31:0] b,
	output	ap_return,
	input	ap_clk,
	input	ap_rst_n,
	input	ap_start,
	output	reg ap_idle,
	output	reg ap_done
);

	reg [5:0] CurrentState;
	reg [5:0] LastState;
	wire cond;
	reg branch_ready;


	parameter state_fiction_head = 6'b100000;
	parameter state_0 = 6'b010000;
	parameter state_start = 6'b001000;
	parameter state_cal = 6'b000100;
	parameter state_exchange = 6'b000010;
	parameter state_ret = 6'b000001;


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
			CurrentState <= state_cal;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_start & branch_ready == 1'b1 & cond == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_exchange;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_cal & branch_ready == 1'b1 & cond == 1'b0)
		begin
			LastState <= CurrentState;
			CurrentState <= state_ret;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_cal & branch_ready == 1'b1 & cond == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_start;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_exchange & branch_ready == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_cal;
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
	if(CurrentState == state_0 && counter == 2)
		counter <= 0
	if(CurrentState == state_start && counter == 3)
		counter <= 0
	if(CurrentState == state_cal && counter == 3)
		counter <= 0
	if(CurrentState == state_exchange && counter == 2)
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
		endcase
	end
	state_cal: begin
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
		endcase
	end
	state_exchange: begin
		case(counter)
		32'b1: begin
test  test  test
		end
		32'b2: begin
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
