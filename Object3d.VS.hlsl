#include "Object3d.hlsli"

// VertexShader
// 与えられた座標を同次クリップ空間に変換する

struct TransformationMatrix
{
    float4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texcoord = input.texcord;
    return output;
}