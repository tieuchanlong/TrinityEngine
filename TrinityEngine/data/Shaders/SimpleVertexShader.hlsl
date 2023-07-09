/* cbuffer is constant buffer */
/* register to is to assign the PerApplication object to b0 variable */
cbuffer PerApplication : register(b0)
{
    matrix projectionMatrix;
}

cbuffer PerFrame : register(b1)
{
    matrix viewMatrix;
}

cbuffer PerObject : register(b2)
{
    matrix worldMatrix;
}

struct AppData
{
    float3 position : POSITION;
    float3 color: COLOR;
};

struct VertexShaderOutput
{
    float4 color : COLOR;
    float4 position : SV_POSITION;
};

/* The entry point name has to match what's defined in the HLSL compiled gneral section*/
VertexShaderOutput SimpleVertexShader(AppData IN)
{
    VertexShaderOutput OUT;

    /* Get the output matrix by multiplying projection matrix, camera view matrix and world matrix */
    matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    OUT.position = mul(mvp, float4(IN.position, 1.0f));
    OUT.color = float4(IN.color, 1.0f);

    return OUT;
}