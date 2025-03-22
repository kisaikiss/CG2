// VertexShader
// 与えられた座標を同次クリップ空間に変換する


struct VertexShaderOutput {
    float4 position : SV_POSITION;
};

struct VertexShaderInput {
    float4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = input.position;
    return output;
}