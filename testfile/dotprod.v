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


	reg [31:0] Mem_i_inc;
	reg [31:0] Mem_cl;
	reg [31:0] Mem_c;
	reg [31:0] Mem_cr;
	reg [31:0] Mem_i;


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
			CurrentState <= state_ret;
			branch_ready <= 1'b0;
		end
		else if(CurrentState == state_start & branch_ready == 1'b1 & cond == 1'b0)
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
	else if(CurrentState == state_calc && counter == 14)
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
		reg_1 <= 0;
		end
		32'd2: begin
		Mem_c <= reg_1;
		branch_ready <= 1;
		end
		endcase
	end
	state_start: begin
		case(counter)
		32'd0: begin
		reg_1 <= Mem_i_inc;
		reg_2 <= Mem_c;
		reg_3 <= Mem_cr;
		end
		32'd1: begin
		if(LastState == state_0)
			reg_1 <= 0;
		else if(LastState == state_calc)
			reg_1 <= reg_1;
		if(LastState == state_0)
			reg_2 <= reg_2;
		else if(LastState == state_calc)
			reg_2 <= reg_3;
		end
		32'd2: begin
		if(LastState == state_0)
			reg_1 <= 0;
		else if(LastState == state_calc)
			reg_1 <= reg_1;
		if(LastState == state_0)
			reg_2 <= reg_2;
		else if(LastState == state_calc)
			reg_2 <= reg_3;
		end
		32'd3: begin
		reg_3 <= (reg_1 >= n);
		end
		32'd4: begin
		end
		32'd5: begin
		end
		32'd6: begin
		Mem_i <= reg_1;
		Mem_cl <= reg_2;
		Mem_cl <= reg_2;
		branch_ready <= 1;
		end
		endcase
	end
	state_calc: begin
		case(counter)
		32'd0: begin
		reg_1 <= Mem_i;
		reg_2 <= Mem_cl;
		end
		32'd1: begin
		a_ce0 <= 1;
		a_address0 <= reg_1;
		reg_3 <= reg_1 + 1;
		end
		32'd2: begin
		a_ce0 <= 0;
		reg_4 <= a_q0;
		end
		32'd3: begin
		a_ce0 <= 1;
		a_address0 <= reg_1;
		end
		32'd4: begin
		b_ce0 <= 1;
		b_address0 <= reg_1;
		end
		32'd5: begin
		b_ce0 <= 0;
		reg_1 <= b_q0;
		end
		32'd6: begin
		b_ce0 <= 1;
		b_address0 <= reg_1;
		end
		32'd7: begin
		reg_1 <= reg_4 * reg_1;
		end
		32'd8: begin
		end
		32'd9: begin
		end
		32'd10: begin
		end
		32'd11: begin
		end
		32'd12: begin
		reg_1 <= reg_2 + reg_1;
		end
		32'd13: begin
		end
		32'd14: begin
		Mem_i_inc <= reg_3;
		Mem_cr <= reg_1;
		branch_ready <= 1;
		end
		endcase
	end
	state_ret: begin
		case(counter)
		32'd0: begin
		reg_1 <= Mem_cl;
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
	assign cond = ((CurrentState == state_start) & reg_3);
endmodule
