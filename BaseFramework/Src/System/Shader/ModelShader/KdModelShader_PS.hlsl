//-----------------------------------------------------------------------------
// File: KdModelShader_PS.hlsl
//
// Edit: 授業
//-----------------------------------------------------------------------------
#include "../inc_KdCommon.hlsli"
#include "inc_KdModelShader.hlsli"

//--------------------------------------------------
// テクスチャ（画像データ※そのままでは使えない）
//--------------------------------------------------
Texture2D g_baseTex			: register(t0);		// ベースカラーテクスチャ
Texture2D g_emissiveTex		: register(t1);		// エミッシブテクスチャ
Texture2D g_mrTex			: register(t2);		// メタリック/ラフネステクスチャ
Texture2D g_normalTex		: register(t3);		// 法線マップ
Texture2D g_dirShadowMap	: register(t102);	// 平行光シャドウマップ(0～127使用可能で、今回は102を使用)

//--------------------------------------------------
// サンプラ（テクスチャのデータを扱う役目）
//--------------------------------------------------
SamplerState			g_ss				: register(s0);
SamplerComparisonState	g_ssShadowMapComp	: register(s10);

//--------------------------------------------------
// BlinnPhong NDF
// ・lightDir    … ライトの方向
// ・vCam        … ピクセルからカメラへの方向
// ・normal      … 法線
// ・specPower   … 反射の鋭さ
//--------------------------------------------------
float BlinnPhong(float3 lightDir, float3 vCam, float3 normal, float specPower)
{
	float3 H = normalize(-lightDir + vCam);
	float NdotH = saturate(dot(normal, H)); // カメラの角度差(0～1)
	float spec = pow(NdotH, specPower);

	// 正規化Blinn-Phong
	return spec * ((specPower + 2) / (2 * 3.1415926535));
}

//
float CheckShadow(VSOutput In)
{
	// 最終的な値
	float shadow = 1.0f;

	// ピクセルの3D座標から、シャドウマップ空間へ変換
	float4 liPos = mul(float4(In.wPos, 1), g_DL_mLightVP);

	// 自身で射影座標に変換する場合は奥行で変換してやる(長さが求まる)
	liPos.xyz /= liPos.w;
	float shadowX = abs(liPos.x);
	float shadowY = abs(liPos.y);

	// 深度マップの範囲内か確認
	if (shadowX <= 1 && shadowY <= 1 && liPos.z <= 1)
	{
		// 射影座標 -> UV座標へ変換 ※右下を拡大すればいいだけ(イメージ)
		float2 uv = liPos.xy * float2(1, -1) * 0.5 + 0.5;

		// ライトカメラからの距離 ※シャドウアクネ対策(ImGuiで変更できるように修正する)
		float z = liPos.z - 0.002f;

		// ぼかし
		float width, height;
		g_dirShadowMap.GetDimensions(width, height);
		float tw = 1.0f / width;
		float th = 1.0f / height;

		// UVの周辺3x3も考慮し、平均値を算出
		shadow = 0;
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				// 影判定
				shadow += g_dirShadowMap.SampleCmpLevelZero(g_ssShadowMapComp, uv + float2(x * tw, y * th), z);
			}
		}
		shadow *= 0.11f;

		// UV座標から離れていると薄く
		shadow = min(1.0f, shadow + pow(shadowX, 3.0f) + pow(shadowY, 3.0f));
	}

	return shadow;
}

//--------------------------------------------------
// ピクセルシェーダ
//--------------------------------------------------
float4 main(VSOutput In) : SV_Target0
{
	// カメラへの方向
	float3 vCam = g_CamPos - In.wPos;
	float camDist = length(vCam); // カメラ - ピクセル距離
	vCam = normalize(vCam);

	// ３種の法線から法線行列を作成
	row_major float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN),
	};

	// 法線マップから法線ベクトル取得
	float3 wN = g_normalTex.Sample(g_ss, In.UV).rgb;

	// 0～1　から -1～1　へ変換
	wN = wN * 2.0f - 1.0f;

	// 法線ベクトルをこのピクセル空間へ変換
	wN = normalize(mul(wN, mTBN));

	//------------------------------------------
	// 材質色
	//------------------------------------------

	// 材質の色
	float4 baseColor = g_baseTex.Sample(g_ss, In.UV) * g_Material.BaseColor * In.Color;

	//
	float4 mr = g_mrTex.Sample(g_ss, In.UV);

	// 金属性
	float metallic = mr.b * g_Material.Metallic;

	// 粗さ
	float roughness = mr.b * g_Material.Roughness;

	// ラフネスから、反射光用のSpecularPowerを求める
	// Call of Duty : Black OpsやFarCry3での手法を参考
	// specPower = 2^(13*g)　グロス値g = 0～1で1～8192のスペキュラ強度を求める
	// ここでは specPower = 2^(11*g)　1～2048を使用します。
	// 参考：https://hanecci.hatenadiary.org/entry/20130511/p1
	float smoothness = 1.0f - roughness; // ラフネスを逆転させ「滑らか」さにする
	float specPower = pow(2.0f, 11.0f * smoothness); // 1～2048

	//------------------------------------------
	// ライティング
	//------------------------------------------

	// 最終的な色
	float3 color = 0.0f;

	// 材質の拡散色　非金属ほど材質の色になり、金属ほど拡散色は無くなる
	const float3 baseDiffuse = lerp(baseColor.rgb, float3(0.0f, 0.0f, 0.0f), metallic);

	// 材質の反射色　非金属ほど光の色をそのまま反射し、金属ほど材質の色が乗る
	const float3 baseSpecular = lerp(0.04f, baseColor.rgb, metallic);

	//--------------------------------------------------
	// シャドウマッピング(影判定)
	//--------------------------------------------------
	float shadow = CheckShadow(In);

	//------------------------------------------
	// 平行光
	//------------------------------------------

	// Diffuse(拡散光)
	{
		// 光の方向と法線の方向との角度さが光の強さになる
		float lightDiffuse = dot(-g_DL_Dir, wN);
		lightDiffuse = saturate(lightDiffuse);  // マイナス値は0にする　0(暗)～1(明)になる

		// 正規化Lambert
		lightDiffuse /= 3.1415926535;

		// 光の色 * 材質の拡散色 * 透明率
		color += (g_DL_Color * lightDiffuse) * baseDiffuse * baseColor.a * shadow;
	}

	// Specular(反射色) ※反射した光の強さを求める
	{
		// Blinn-Phong NDF
		float spec = BlinnPhong(g_DL_Dir, vCam, wN, specPower);

		// 光の色 * 反射光の強さ * 材質の反射色 * 透明率 * 適当な調整値
		color += (g_DL_Color * spec) * baseSpecular * baseColor.a * shadow;
	}

	//------------------------------------------
	// 点光
	//------------------------------------------
	for (int pi = 0; pi < g_PL_Cnt.x; pi++)
	{
		// ピクセルから点光への方向
		float3 dir = g_PL[pi].Pos - In.wPos;

		// 距離を算出
		float dist = length(dir);

		// 正規化
		dir /= dist;

		// 点光の判定以内
		if (dist < g_PL[pi].Radius)
		{
			// 半径をもとに、距離の比率を求める
			float atte = 1.0 - saturate(dist / g_PL[pi].Radius);
			// 逆２乗の法則
			atte *= atte;

			// Diffuse(拡散光)
			{
				// 光の方向と法線の方向との角度さが光の強さになる
				float lightDiffuse = dot(dir, wN);
				lightDiffuse = saturate(lightDiffuse); // マイナス値は0にする　0(暗)～1(明)になる

				lightDiffuse *= atte; // 減衰

				// 正規化Lambert
				lightDiffuse /= 3.1415926535;

				// 光の色 * 材質の拡散色 * 透明率
				color += (g_PL[pi].Color * lightDiffuse) * baseDiffuse * baseColor.a;
			}

			// Specular(反射色)
			{
				// 反射した光の強さを求める
				// Blinn-Phong NDF
				float spec = BlinnPhong(-dir, vCam, wN, specPower);

				spec *= atte; // 減衰

				// 光の色 * 反射光の強さ * 材質の反射色 * 透明率 * 適当な調整値
				color += (g_PL[pi].Color * spec) * baseSpecular * baseColor.a * 0.5;
			}
		}
	}

	// 自己発光
	color += g_emissiveTex.Sample(g_ss, In.UV).rgb * g_Material.Emissive;

	// 環境光 ※光の当たっていない部分が真っ黒になるのを防ぐ
	color += g_AmbientLight * baseColor.rgb * baseColor.a;

	//------------------------------------------
	// 距離フォグ
	//------------------------------------------
	if (g_DistanceFogEnable)
	{
		// フォグ ※1...近い ～ 0...遠い
		// 急にカーブさせたり、高さフォグも実装したい
		float fog = saturate(1.0 / exp(camDist * g_DistanceFogDensity));
		// 適用
		color.rgb = lerp(g_DistanceFogColor, color.rgb, fog);
	}

	//------------------------------------------
	// 出力
	//------------------------------------------
	return float4(color, baseColor.a);
}