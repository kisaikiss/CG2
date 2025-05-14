#include "Object3d.hlsli"

// PixelShader
// 実際に画面に撃つPixelの色を決めるShader
struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
};

struct DirectionalLight
{
    float4 color;       // ライトの色
    float3 direction;   // ライトの向き
    float intensity;    // 輝度
};

struct LightConstBuffer
{
    DirectionalLight light[3];
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<LightConstBuffer> gLight : register(b1);
struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

float4 CreateLight(VertexShaderOutput input)
{
    float4 result = 0;
    for (int i = 0; i < 3; i++)
    {
        float NdotL = dot(normalize(input.normal), -gLight.light[i].direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float4 color = gLight.light[i].color * cos * gLight.light[i].intensity;
        result += color;
    }
    return result;
}

//srvのレジスタ－はt
Texture2D<float4> gTexture : register(t0);
//サンプラーのレジスターはs
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    if (gMaterial.enableLighting != 0)
    {
        //float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        //float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color = gMaterial.color * textureColor * CreateLight(input);
        output.color.w = gMaterial.color.w;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}