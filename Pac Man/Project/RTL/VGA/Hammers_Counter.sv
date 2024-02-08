module Hammers_Counter (

  input  logic clk,
  input  logic resetN,
  input  logic HammerKey,
  input  logic HammerIncrease,
  input	logic maintence_time,
  output logic [2:0] amount,
  output logic  use_hammer
);

	int amount_reg;
	parameter MAX_HAMMERS = 3;
	logic flag ; // a semaphore to set the output only once per frame / regardless of the number of collisions 

	always_ff @( posedge clk or negedge resetN ) begin
	
		if ( !resetN ) begin
			amount_reg <= MAX_HAMMERS;
			use_hammer <= 1'b0;
			flag	<= 1'b0;
			
		end // if reset
		
		else begin
			if ((HammerKey == 1'b1) && (amount_reg > 0) && (flag	== 1'b0)) begin  //hammer have been activated
					flag	<= 1'b1;
					amount_reg <= amount_reg - 1;
					use_hammer <= 1'b1;
			end
			
			else
				use_hammer <= 1'b0;
			
			if (HammerIncrease == 1'b1 && amount_reg < MAX_HAMMERS)  //green monster collided
					amount_reg <= amount_reg + 1;	
					
			if(maintence_time) // so won't use all hammers at once
				flag	<= 1'b0;
		end
	end
	
  assign amount = amount_reg;

  // Initial value of hammers amount is set to 3
  initial begin
    amount_reg = MAX_HAMMERS;
  end

endmodule
