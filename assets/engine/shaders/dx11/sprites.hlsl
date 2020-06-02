cbuffer cbPerFrame : register(b0) {
	float4x4 viewProjection;
}

Texture2D g_ColorTexture : register(t0);
SamplerState g_ColorSampler : register(s0);

struct VSInput {
	// per vertex
	float3 pos : POSITION;
	float2 uv : TEXCOORD;

	// per instance
	float4x4 worldTransform : WORLD;
	float4 uvTransform : TEXCOORD_TRANSFORM; // xy - translation, zw - scale
	float4 color : COLOR;
};

struct PSInput {
	float4 pos : SV_POSITION;
	float4 color :COLOR;
	float2 uv : TEXCOORD;
};

PSInput VSMain(VSInput vin) {
	PSInput vout;

	float4x4 mvp = mul(viewProjection, vin.worldTransform);
	vout.pos = mul(mvp, float4(vin.pos, 1.0f));

	vout.uv = vin.uv * vin.uvTransform.zw + vin.uvTransform.xy;

	vout.color = vin.color;

	return vout;
}

float4 PSMain(PSInput pin) : SV_TARGET {
	float4 textureColor = g_ColorTexture.Sample(g_ColorSampler, pin.uv);

	return textureColor * pin.color;
}
