module gcd
(
	input	[31:0] a,
	input	[31:0] b,
	output	[31:0] ap_return,
	input	ap_clk,
	input	ap_rst_n,
	input	ap_start,
	output	reg ap_idle,
	output	reg ap_done
);
	reg [31:0] reg_1;
	reg [31:0] reg_2;
	reg [31:0] reg_3;
	reg [31:0] reg_4;


	reg [31:0] Mem_c;
	reg [31:0] Mem_divisor;
	reg [31:0] Mem_d;
	reg [31:0] Mem_remainder;
	reg [31:0] Mem_b1;
	reg [31:0] Mem_a1;


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
			ap_done <= 1'b0;
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
		else if(CurrentState == state_start & branch_ready == 1'b1 & cond == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_cal;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_start & branch_ready == 1'b1 & cond == 1'b0)
		begin
			LastState <= CurrentState;
			CurrentState <= state_exchange;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_cal & branch_ready == 1'b1 & cond == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_ret;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_cal & branch_ready == 1'b1 & cond == 1'b0)
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


	reg[31:0] counter;
	always @(posedge ap_clk or negedge ap_rst_n)
	begin
	if(!ap_rst_n)
	begin
		counter <= 0;
	end
	else if(CurrentState == state_fiction_head && counter == 1 && ap_start == 1'b1)
	begin
		counter <= 0;
		branch_ready <= 1;
	end
	else if(CurrentState == state_0 && counter == 2)
	begin
		counter <= 0;
	end
	else if(CurrentState == state_start && counter == 6)
	begin
		counter <= 0;
	end
	else if(CurrentState == state_cal && counter == 8)
	begin
		counter <= 0;
	end
	else if(CurrentState == state_exchange && counter == 2)
	begin
		counter <= 0;
	end
	else if(CurrentState == state_ret && counter == 2)
	begin
		counter <= 0;
	end
	else
		counter <= counter + 1;
	end


	always@(counter)
	case(CurrentState)
	state_0: begin
		case(counter)
		32'd0: begin
		end
		32'd1: begin
		reg_1 <= a;
		reg_2 <= b;
		end
		32'd2: begin
		Mem_c <= reg_1;
		Mem_d <= reg_2;
		branch_ready <= 1;
		end
		endcase
	end
	state_start: begin
		case(counter)
		32'd0: begin
		reg_1 <= Mem_c;
		reg_2 <= Mem_divisor;
		reg_3 <= Mem_d;
		reg_4 <= Mem_remainder;
		end
		32'd1: begin
		if(LastState == state_0)
			reg_1 <= reg_1;
		else if(LastState == state_cal)
			reg_1 <= reg_2;
		if(LastState == state_0)
			reg_2 <= reg_3;
		else if(LastState == state_cal)
			reg_2 <= reg_4;
		end
		32'd2: begin
		end
		32'd3: begin
		reg_3 <= (reg_1 >= reg_2);
		end
		32'd4: begin
		end
		32'd5: begin
		end
		32'd6: begin
		Mem_b1 <= reg_2;
		Mem_a1 <= reg_1;
		branch_ready <= 1;
		end
		endcase
	end
	state_cal: begin
		case(counter)
		32'd0: begin
		reg_1 <= Mem_b1;
		reg_2 <= Mem_a1;
		end
		32'd1: begin
		if(LastState == state_start)
			reg_1 <= reg_1;
		else if(LastState == state_exchange)
			reg_1 <= reg_2;
		if(LastState == state_start)
			reg_2 <= reg_2;
		else if(LastState == state_exchange)
			reg_2 <= reg_1;
		end
		32'd2: begin
		end
		32'd3: begin
		reg_2 <= reg_2 - reg_1;
		end
		32'd4: begin
		end
		32'd5: begin
		reg_3 <= (reg_2 == 0);
		end
		32'd6: begin
		end
		32'd7: begin
		end
		32'd8: begin
		Mem_divisor <= reg_1;
		Mem_remainder <= reg_2;
		Mem_divisor <= reg_1;
		branch_ready <= 1;
		end
		endcase
	end
	state_exchange: begin
		case(counter)
		32'd0: begin
		end
		32'd1: begin
		end
		32'd2: begin
		branch_ready <= 1;
		end
		endcase
	end
	state_ret: begin
		case(counter)
		32'd0: begin
		reg_1 <= Mem_divisor;
		end
		32'd1: begin
		end
		32'd2: begin
		branch_ready <= 1;
		end
		endcase
	end
	endcase
	assign ap_return = reg_1;
	assign cond = ((CurrentState == state_start) & reg_3) || ((CurrentState == state_cal) & reg_3);
endmodule
