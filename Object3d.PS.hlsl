#include "Object3d.hlsli"

// PixelShader
// 実際に画面に撃つPixelの色を決めるShader
struct Material
{
    float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

//srvのレジスタ－はt
Texture2D<float4> gTexture : register(t0);
//サンプラーのレジスターはs
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = gMaterial.color * textureColor;
    return output;
}