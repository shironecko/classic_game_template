cbuffer cbPerFrame : register(b0)
{
    float4x4 viewProjection;
}

Texture2D g_ColorTexture : register(t0);
SamplerState g_ColorSampler : register(s0);

struct VSInput
{
    // per vertex
    float2 pos : QUAD_POSITION;
    float2 uv : TEXCOORD;

    // per instance
    float4 colorTint : COLOR;
    float2 position : SPRITE_POSITION;
    float2 uvMin : TEXCOORD_MIN;
    float2 uvMax : TEXCOORD_MAX;
    float2 scale : SCALE;
    float rotation : ROTATION;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 colorTint : COLOR;
    float2 uv : TEXCOORD;
};

PSInput VSMain(VSInput vin)
{
    PSInput vout;

    float2 pos = vin.pos * vin.scale;
    float angleCos = cos(vin.rotation);
    float angleSin = sin(vin.rotation);
    pos = float2(
        angleCos * pos.x - angleSin * pos.y,
        angleSin * pos.x + angleCos * pos.y);
    pos += vin.position;

    vout.pos = mul(viewProjection, float4(pos, 0.0f, 1.0f));

    float2 uvScale = float2(vin.uvMax.x - vin.uvMin.x, vin.uvMax.y - vin.uvMin.y);
    float2 uvTranslation = float2(0.5f, 0.5f) * uvScale + vin.uvMin;
    vout.uv = vin.uv * uvScale + uvTranslation;

    vout.colorTint = vin.colorTint;

    return vout;
}

float4 PSMain(PSInput pin) : SV_TARGET
{
    float4 textureColor = g_ColorTexture.Sample(g_ColorSampler, pin.uv);

    return textureColor * pin.colorTint;
}
