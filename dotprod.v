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
	input	ap_rst,
	input	ap_start,
	output	reg ap_idle,
	output	reg ap_ready,
	output	reg ap_done
);

	reg [4:0] CurrentState;
	reg [4:0] LastState;
	 reg [1:0] condReg;

	parameter state_fiction_head = 5'b10000;
	parameter state_0 = 5'b01000;
	parameter state_start = 5'b00100;
	parameter state_calc = 5'b00010;
	parameter state_ret = 5'b00001;


	always @(posedge ap_clk or negedge ap_rst)
	begin
		if(!ap_rst)
		begin
			LastState <= state_fiction_head;
			CurrentState <= state_fiction_head;
		end
		else if(CurrentState == state_fiction_head & condReg[1] == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_0;
		end
	end


	always @(posedge ap_clk or negedge ap_rst)
	begin
		if(!ap_rst)
		begin
			LastState <= state_fiction_head;
			CurrentState <= state_fiction_head;
		end
		else if(CurrentState == state_0 & condReg[1] == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_start;
		end
	end


	always @(posedge ap_clk or negedge ap_rst)
	begin
		if(!ap_rst)
		begin
			LastState <= state_fiction_head;
			CurrentState <= state_fiction_head;
		end
		else if(CurrentState == state_start & condReg == 2'b10)
		begin
			LastState <= CurrentState;
			CurrentState <= state_ret;
		end
		else if(CurrentState == state_start & condReg == 2'b11)
		begin
			LastState <= CurrentState;
			CurrentState <= state_calc;
		end
	end


	always @(posedge ap_clk or negedge ap_rst)
	begin
		if(!ap_rst)
		begin
			LastState <= state_fiction_head;
			CurrentState <= state_fiction_head;
		end
		else if(CurrentState == state_calc & condReg[1] == 1'b1)
		begin
			LastState <= CurrentState;
			CurrentState <= state_start;
		end
	end


	always @(posedge ap_clk or negedge ap_rst)
	begin
		if(!ap_rst)
		begin
			LastState <= state_fiction_head;
			CurrentState <= state_fiction_head;
		end
		else if(CurrentState == state_ret & condReg[1] == 1'b1)
		begin
			ap_done <= 1'b1;
		end
	end


endmodule
