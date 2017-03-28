//--------------------------------------------------------------------------------------
// A basic shader with colour and texture fetch and a simple light
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	matrix rot;
	float4 lightCol;
	float4 ambientCol;
	float3 lightPos;
	float height = 255;
}

//--------------------------------------------------------------------------------------
// texture
//--------------------------------------------------------------------------------------
Texture2D		myTexture : register( t0 );
SamplerState	Sampler1 : register( s0 );
SamplerState	Sampler2 : register (s1);

//--------------------------------------------------------------------------------------
// input structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
	float4 Norm : NORMAL;
    float4 Color : COLOR;
	float2 texCoord : TEXCOORD;
	float normalise : HEIGHTMULTIPLIER;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
	float4 worldPos: POSITION;
	float4 Norm : NORMAL;
    float4 Color : COLOR;
	float2 texCoord : TEXCOORD;
	float normalise : HEIGHTMULTIPLIER;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

	output.worldPos = mul( input.Pos, World );
    output.Pos = mul( output.worldPos, View );
    output.Pos = mul( output.Pos, Projection );

    output.Norm = mul( input.Norm, rot );

    output.Color = input.Color;

	output.texCoord = input.texCoord;

	output.normalise = input.normalise;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
	float4 vertexCol = input.Color * myTexture.Sample( Sampler1, input.texCoord );
	float posY = input.worldPos.y * input.normalise;

	//create colour constants
	float4 SNOW = float4(255.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f);
	float4 MOUNTAIN = float4(169.0f / 255, 169.0f / 255, 169.0f / 255, 1.0f);
	float4 CANYON = float4(205.0f / 255, 133.0f / 255, 63.0f / 255, 1.0f);
	float4 GRASS = float4(34.0f / 255, 139.0f / 255, 34.0f / 255, 1.0f);
	float4 SAND = float4(255.0f / 255, 165.0f / 255, 0.0f, 1.0f);
	float4 WATER = float4(0.0f, 0.0f, 204.0f / 255, 1.0f);
	float weight = 0;

	if (posY <= 255 && posY > 180)
	{
		vertexCol = SNOW;	//White
	}
	if (posY <= 180 && posY > 80)
	{
		vertexCol = MOUNTAIN;	//Dark Grey
	}
	if (posY <= 80 && posY > 25)
	{
		weight = (100 - 26) / posY;
		vertexCol = GRASS;		//Forest Green
	}
	if (posY <= 25 && posY > 20)
	{
		weight = (25 - 21) / posY;
		vertexCol = SAND;
	}
	if (posY <= 20)
	{
		weight = (20 - 0) / posY;
		vertexCol = WATER;
	}

	float3 lightDir = normalize( input.worldPos - lightPos );
	float4 diffuse =  saturate(max( 0.0f, dot( lightDir, normalize(input.Norm )) ) * lightCol);	//Increase the number value to reduce the detail of the vertex faces
    return saturate( (diffuse + ambientCol) * vertexCol );
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

//this can be used to do doubled sided rendering of an object with back culling switched off
float4 PS2(PS_INPUT input) : SV_Target
{
	float4 vertexCol = input.Color * myTexture.Sample(Sampler1, input.texCoord);
	float3 lightDir = normalize(input.worldPos - lightPos);
	float4 diffuse = saturate(abs( dot(lightDir, normalize(input.Norm))) * lightCol);
	return saturate((diffuse + ambientCol) * vertexCol);
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}