#ifndef _SHADER_H_
#define _SHADER_H_

#include<d3d11.h>

HRESULT create_vs_from_cso(ID3D11Device *device, const char *cso_name, ID3D11VertexShader **vertexShader, ID3D11InputLayout **inputLayout, D3D11_INPUT_ELEMENT_DESC *inputElementDesc, UINT numElements);
HRESULT create_ps_from_cso(ID3D11Device *device, const char *cso_name, ID3D11PixelShader **pixelShader);

#endif //!_SHADER_H_
