module HP_Counter (

  input  logic clk,
  input  logic resetN,
  input  logic collision_mp,
  input	logic stop_gameN,
  output logic [1:0] HP
);
	parameter int INITIAL_HP = 3;
	int HP_reg;

	always_ff @( posedge clk or negedge resetN or negedge stop_gameN) begin
	
		if ( (!resetN) || (!stop_gameN)) begin
			if (!resetN) begin
				HP_reg <= INITIAL_HP;	
			end // if reset
		end
		
		else begin
			if (collision_mp == 1'b1) begin //pacman and red monster collided
				if (HP_reg > 0) begin
					HP_reg <= HP_reg - 1;
				end
			end
		end
	end
	
  assign HP = HP_reg;

  initial begin
    HP_reg = INITIAL_HP;
  end

endmodule






