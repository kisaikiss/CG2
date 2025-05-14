#include "Object3d.hlsli"


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    float offsetAmount = 0.02; // 拡張する距離

    // 各頂点を法線方向に移動
    float3 normalOffset = normalize(input.normal) * offsetAmount;
    
    float3 expandedPosition = input.position.xyz + normalOffset;
    
    output.position = mul(float4(expandedPosition, 1.0), gTransformationMatrix.WVP);

    output.texcoord = input.texcord;
    output.normal = normalize(input.normal);

    return output;
}

//VertexShaderOutput main(VertexShaderInput input)
//{
//    VertexShaderOutput output;
//    output.position = mul(input.position, gTransformationMatrix.WVP);
//    output.texcoord = input.texcord;
//    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));
//    return output;
//}