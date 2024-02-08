module Time_Counter (

  input  logic clk,
  input  logic resetN,
  input  logic collision_pc,
  output logic [11:0] Score

);

	int Score_reg1;
	int Score_reg2;
	int Score_reg3;

	always_comb begin
	
		if ( !resetN ) begin
			Score_reg1 = 0;
			Score_reg2 = 0;
			Score_reg3 = 0;	
		end // if reset
		
		else begin
		
			if (collision_pc == 1'b1) begin
				
				if(Score_reg1 == 9) begin
				
					Score_reg1 <= 0;

					if (Score_reg2 == 9) begin
						Score_reg2 <= 0;
						Score_reg3 <= Score_reg3+1;
					end
					
					else begin
						Score_reg2 <= Score_reg2+1;
					end
					
				end
				
				else begin
					Score_reg1 <= Score_reg1+1;
				end
				
		  end
		end
	end
	
   assign Score[3:0] = Score_reg1;
	assign Score[7:4] = Score_reg2;
	assign Score[11:8] = Score_reg3;
	
  initial begin
    Score_reg1 = -2;
	 Score_reg2 = 0;
	 Score_reg3 = 0;
  end

endmodule






