// System-Verilog 'written by Alex Grinshpun May 2018
// New bitmap dudy February 2021
// (c) Technion IIT, Department of Electrical Engineering 2021 



module	pacmanBitMap	(	
					input	logic	clk,
					input	logic	resetN,
					input logic	[10:0] offsetX,// offset from top left  position 
					input logic	[10:0] offsetY,
					input	logic	InsideRectangle, //input that the pixel is within a bracket 
					input logic [1:0] direction, //up,down,left or right

					output	logic	drawingRequest, ///output for collisions
					output	logic	drawingRequest2,//output that the pixel should be displayed(for invisible circle around pacman)
					output	logic	[7:0] RGBout,  //rgb value from the bitmap 
					output	logic	[3:0] HitEdgeCode //one bit per edge 
 ) ;

// this is the devider used to access the right pixel 
localparam  int OBJECT_NUMBER_OF_Y_BITS = 5;  // 2^5 = 32 
localparam  int OBJECT_NUMBER_OF_X_BITS = 5;  // 2^5 = 32 


localparam  int OBJECT_HEIGHT_Y = 1 <<  OBJECT_NUMBER_OF_Y_BITS ;
localparam  int OBJECT_WIDTH_X = 1 <<  OBJECT_NUMBER_OF_X_BITS;

// this is the devider used to acess the right pixel 
localparam  int OBJECT_HEIGHT_Y_DIVIDER = OBJECT_NUMBER_OF_Y_BITS - 2; //how many pixel bits are in every collision pixel
localparam  int OBJECT_WIDTH_X_DIVIDER =  OBJECT_NUMBER_OF_X_BITS - 2;

// generating a smiley bitmap

localparam logic [7:0] TRANSPARENT_ENCODING = 8'hFF ;// RGB value in the bitmap representing a transparent pixel
localparam logic [7:0] TRANSPARENT_ENCODING2 = 8'h00 ;// RGB value in the bitmap representing a transparent pixel for hit

logic [0:OBJECT_HEIGHT_Y-1] [0:OBJECT_WIDTH_X-1] [7:0] object_colors = {
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hfd,8'hf9,8'hf9,8'hf9,8'hfe,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hfd,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hfe,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hfe,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hfe,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf9,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf9,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'h00,8'h00,8'h20,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hd0,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf9,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf9,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hfe,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf9,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hfe,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hfe,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hfd,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hfe,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hf9,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hfe,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hfd,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff,8'hf9,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hf8,8'hfd,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'hff,8'hff,8'hff,8'hf9,8'hf9,8'hf8,8'hf9,8'hfd,8'hff,8'hff,8'hff,8'hff,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff}};


//////////--------------------------------------------------------------------------------------------------------------=
//hit bit map has one bit per edge:  hit_colors[3:0] =   {Left, Top, Right, Bottom}	
//there is one bit per edge, in the corner two bits are set  


logic [0:3] [0:3] [3:0] hit_colors = 
		  {16'hc446,     
			16'h8c62,    
			16'h8932,
			16'h9113};

 

// pipeline (ff) to get the pixel color from the array 	 

//////////--------------------------------------------------------------------------------------------------------------=
always_ff@(posedge clk or negedge resetN)
begin
	if(!resetN) begin
		RGBout <=	8'h00;
		HitEdgeCode <= 4'h0;

	end

	else begin
		RGBout <= TRANSPARENT_ENCODING ; // default  
		HitEdgeCode <= 4'h0;

		if (InsideRectangle == 1'b1 ) 
		begin // inside an external bracket 
			HitEdgeCode <= hit_colors[offsetY >> OBJECT_HEIGHT_Y_DIVIDER][offsetX >> OBJECT_WIDTH_X_DIVIDER];	//get hitting edge from the colors table  
			
			
			if(direction == 2'b00) //up
				RGBout <= object_colors[offsetX][-offsetY];
				
			else if(direction == 2'b01) //down
				RGBout <= object_colors[-offsetX][offsetY];
			
			else if(direction == 2'b10) //left
				RGBout <= object_colors[offsetY][OBJECT_WIDTH_X - offsetX - 1];

			else begin
				RGBout <= object_colors[offsetY][offsetX]; //right and default
			end	
				
		end  	
	end
		
end

//////////--------------------------------------------------------------------------------------------------------------=
// decide if to draw the pixel or not 
assign drawingRequest = (RGBout != TRANSPARENT_ENCODING) ? 1'b1 : 1'b0 ; // get optional transparent command from the bitmpap includes circle   
assign drawingRequest2 = (RGBout != TRANSPARENT_ENCODING && RGBout != TRANSPARENT_ENCODING2) ? 1'b1 : 1'b0 ; // get optional transparent command from the bitmpap   

endmodule