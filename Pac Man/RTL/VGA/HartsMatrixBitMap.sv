// HartsMatrixBitMap File 
// A two level bitmap. dosplaying harts on the screen Apr  2023  
// (c) Technion IIT, Department of Electrical Engineering 2023 



module	HartsMatrixBitMap	(	
					input	logic	clk,
					input	logic	resetN,
					input logic	[10:0] offsetX,// offset from top left  position 
					input logic	[10:0] offsetY,
					input	logic	InsideRectangle, //input that the pixel is within a bracket 
					input logic [1:0]	HP,

					output	logic	drawingRequest, //output that the pixel should be dispalyed 
					output	logic	[7:0] RGBout  //rgb value from the bitmap 
 ) ;
 

// Size represented as Number of X and Y bits 
localparam logic [7:0] TRANSPARENT_ENCODING = 8'hFF ;// RGB value in the bitmap representing a transparent pixel 

localparam logic [0:14] [0:19] [3:0] MAZE_DEFAULT = 
{{4'h01, 4'h01, 4'h01, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00},
 {4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00, 4'h00}};




logic [0:14] [0:19] [3:0]  MazeBiMapMask = MAZE_DEFAULT; //3 hearts



 logic[0:31] [0:31] [7:0]  object_colors  = 
{{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'h00,8'h00,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'hff,8'hff,8'hff,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'hff,8'hff,8'hff,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'hff,8'hff,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'he1,8'he1,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'he1,8'he1,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'h00,8'h00,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff},
	{8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff,8'hff}
};
 

// pipeline (ff) to get the pixel color from the array 	 

//==----------------------------------------------------------------------------------------------------------------=
always_ff@(posedge clk or negedge resetN)
begin
	if(!resetN) begin
		RGBout <=	TRANSPARENT_ENCODING;
		MazeBiMapMask= MAZE_DEFAULT;
		
	end
	else begin
		RGBout <= TRANSPARENT_ENCODING ; // default
		
		if ( HP == 3'b010) begin
			MazeBiMapMask[0][2] = 4'h00; //delete 3rd heart

		end 
		if ( HP == 3'b001) begin
			MazeBiMapMask[0][1] = 4'h00; //delete 2nd heart
	
		end
		
		if ( HP == 3'b000) begin
			MazeBiMapMask[0][0] = 4'h00; //delete 1st heart

		end
		if ((InsideRectangle == 1'b1 )		& 	// only if inside the external bracket 
		   (MazeBiMapMask[offsetY[8:5] ][offsetX[9:5]] == 4'h01 )) // take bits 5,6,7,8,9,10 from address to select  position in the maze    
						RGBout <= object_colors[offsetY[4:0]][offsetX[4:0]] ; 
		end 
end

//==----------------------------------------------------------------------------------------------------------------=
// decide if to draw the pixel or not 
assign drawingRequest = (RGBout != TRANSPARENT_ENCODING ) ? 1'b1 : 1'b0 ; // get optional transparent command from the bitmpap   
endmodule

