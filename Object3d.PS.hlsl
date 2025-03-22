// PixelShader
// 実際に画面に撃つPixelの色を決めるShader

struct PixelShaderOutput {
    float4 color : SV_TARGET0;
};

PixelShaderOutput main() {
    PixelShaderOutput output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}