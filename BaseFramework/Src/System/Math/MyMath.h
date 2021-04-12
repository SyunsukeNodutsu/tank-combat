//-----------------------------------------------------------------------------
// File: MyMath.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdVector3
// Desc: 自作の3Dベクトルクラス XMFLOAT3継承
//-----------------------------------------------------------------------------
class KdVector3 : public DirectX::XMFLOAT3
{
public:

	//--------------------------------------------------
	// Name: KdVector3()
	// Desc: デフォルトコンストラクタ 0クリア
	//--------------------------------------------------
	KdVector3()
	{
		x = 0.0f; y = 0.0f; z = 0.0f;
	}

	//--------------------------------------------------
	// Name: KdVector3()
	// Desc: コンストラクタ x.y.zの値設定
	//--------------------------------------------------
	KdVector3(float _x, float _y, float _z)
	{
		x = _x; y = _y; z = _z;
	}

	//--------------------------------------------------
	// Name: KdVector3()
	// Desc: コンストラクタ XMVECTORから代入
	//--------------------------------------------------
	KdVector3(const DirectX::XMVECTOR& aVec)
	{
		// 計算用->保存用
		DirectX::XMStoreFloat3(this, aVec);
	}

	//--------------------------------------------------
	// Name: KdVector3()
	// Desc: コンストラクタ XMFLOAT3から代入
	//--------------------------------------------------
	KdVector3(const DirectX::XMFLOAT3& V)
	{
		x = V.x; y = V.y; z = V.z;
	}

	//--------------------------------------------------
	// Name: Effekseer::Vector3D()
	// Desc: Effekseer::Vector3Dとの互換性
	//--------------------------------------------------
	operator Effekseer::Vector3D()
	{
		return Effekseer::Vector3D(x, y, z);
	}

	//--------------------------------------------------
	// Name: TransformCoord()
	// Desc: 指定行列でVectorを変換
	//--------------------------------------------------
	KdVector3& TransformCoord(const DirectX::XMMATRIX& aMat)
	{
		return *this = XMVector3TransformCoord(*this, aMat);
	}

	//--------------------------------------------------
	// Name: TransformNormal()
	// Desc: 指定行列(回転)でVectorを変換
	//--------------------------------------------------
	KdVector3& TransformNormal(const DirectX::XMMATRIX& aMat)
	{
		return *this = XMVector3TransformNormal(*this, aMat);
	}

	//--------------------------------------------------
	// Name: operator DirectX::XMVECTOR()
	// Desc: XMVECTORへ変換
	//--------------------------------------------------
	operator DirectX::XMVECTOR() const
	{
		return DirectX::XMLoadFloat3(this);
	}

	//--------------------------------------------------
	// Name: operator Math::Vector3()
	// Desc: Math::Vector3へ変換
	//--------------------------------------------------
	operator Math::Vector3& ()
	{
		return *(Math::Vector3*)this;
	}

	//--------------------------------------------------
	// Name: operator+=()
	// Desc: 算術演算子 +
	//--------------------------------------------------
	KdVector3& operator+= (KdVector3 aVec)
	{
		*this = DirectX::XMVectorAdd(*this, aVec);
		return *this;
	}

	//--------------------------------------------------
	// Name: operator-=()
	// Desc: 算術演算子 -
	//--------------------------------------------------
	KdVector3& operator-= (KdVector3 aVec)
	{
		*this = DirectX::XMVectorSubtract(*this, aVec);
		return *this;
	}

	//--------------------------------------------------
	// Name: operator*=()
	// Desc: 算術演算子 *
	//--------------------------------------------------
	KdVector3& operator*= (float s)
	{
		*this = DirectX::XMVectorScale(*this, s);
		return *this;
	}

	//--------------------------------------------------
	// Name: Normalize()
	// Desc: 正規化
	//--------------------------------------------------
	KdVector3& Normalize()
	{
		*this = DirectX::XMVector3Normalize(*this);
		return *this;
	}

	//--------------------------------------------------
	// Name: Length()
	// Desc: 長さを所得
	//--------------------------------------------------
	float Length() const
	{
		return DirectX::XMVector3Length(*this).m128_f32[0];
	}

	//--------------------------------------------------
	// Name: LengthSquared()
	// Desc: 長さの2乗を所得
	//--------------------------------------------------
	float LengthSquared() const
	{
		return DirectX::XMVector3LengthSq(*this).m128_f32[0];
	}

	//--------------------------------------------------
	// Name: Dot()
	// Desc: 内積を所得
	//--------------------------------------------------
	static float Dot(const KdVector3& v1, const KdVector3& v2)
	{
		return DirectX::XMVector3Dot(v1, v2).m128_f32[0];
	}

	//--------------------------------------------------
	// Name: Cross()
	// Desc: 外積を所得
	//--------------------------------------------------
	static KdVector3 Cross(const KdVector3& v1, const KdVector3& v2)
	{
		return DirectX::XMVector3Cross(v1, v2);
	}

	//--------------------------------------------------
	// Name: Complement()
	// Desc: 渡されたベクトルに渡された値分,自身を向ける
	//--------------------------------------------------
	inline void Complement(const KdVector3& aTarget, float aDegree)
	{
		// 回転軸作成(この軸で回転する)
		KdVector3 vRotAxis = KdVector3::Cross(*this, aTarget);

		// ベクトルの内積
		float dot = KdVector3::Dot(*this, aTarget);

		// 誤差を補正(クランプ)
		dot = std::clamp(dot, -1.0f, 1.0f);

		// ベクトル間の角度
		float radian = acos(dot);

		// 角度制限
		const float RAD_LIMIT = aDegree * KdToRadians;
		if (radian > RAD_LIMIT) radian = RAD_LIMIT;

		// KdMatrix使えないからXMMATRIXで
		DirectX::XMMATRIX matrix = {};
		matrix = DirectX::XMMatrixRotationAxis(vRotAxis, radian);

		// 軸回転行列をベクトルに変換
		TransformNormal(matrix);
	}

	//--------------------------------------------------
	// Name: GetDistance()
	// Desc: 2点間の距離算出
	// Ret : 2点間の距離
	//--------------------------------------------------
	static float GetDistance(KdVector3 aPosition, KdVector3 aTargetPos)
	{
		float distance = sqrtf(
			(aTargetPos.x - aPosition.x) * (aTargetPos.x - aPosition.x) +
			(aTargetPos.z - aPosition.z) * (aTargetPos.z - aPosition.z));

		return distance;
	}

	//--------------------------------------------------
	// Name: GetLaunchPoint()
	// Desc: 引数angleの角度に引数distance分離れた座標を所得
	//--------------------------------------------------
	static KdVector3 GetLaunchPoint(const float angle, const float distance)
	{
		// check launch point
		float axis_x = cosf(angle * KdToRadians) * distance;
		float axis_y = sinf(angle * KdToRadians) * distance;
		// done
		return KdVector3(axis_x, 0.0f, axis_y);
	}

	//--------------------------------------------------
	// Name: GetAngle()
	// Desc: 2点間の角度算出
	//--------------------------------------------------
	static float GetAngle(const Math::Vector2 a, const Math::Vector2 b)
	{
		float dot = a.x * b.x + a.y * b.y;	// dot product
		float det = a.x * b.y - a.y * b.x;	// determinant
		float angle = atan2f(det, dot);		// atan2(y, x) or atan2(sin, cos)
		if (angle < 0) {
			angle += 6.283185307f/*XM_2PI*/;
		}
		return angle;
	}

	//--------------------------------------------------
	// Name: CalcAngle()
	// Desc: 2線のなす角算出
	// 参考: https://www.hiramine.com/programming/graphics/2d_vectorangle.html
	//--------------------------------------------------
	static const bool CalcAngle(const Math::Vector2& vectorA, const Math::Vector2& vectorB, float& angle)
	{
		// check zero vector
		float length = vectorA.Length() * vectorB.Length();
		if (length == 0) { return false; }

		float cosAlpha = vectorA.Dot(vectorB) / length;
		if (cosAlpha > 1.0) { cosAlpha = 1.0; }
		if (cosAlpha < -1.0) { cosAlpha = -1.0; }
		angle = acosf(cosAlpha);

		return true;
	}

	//--------------------------------------------------
	// Name: CalcAngle2()
	// Desc: -π～π：ベクトルＢがベクトルＡに対して時計方向回りを向いているとき負の値が返ります。
	//--------------------------------------------------
	static const bool CalcAngle2(const Math::Vector2& vectorA, const Math::Vector2& vectorB, float& angle)
	{
		if (!CalcAngle(vectorA, vectorB, angle)) { return false; }
		Math::Vector2 dOuterProduct = vectorA.Cross(vectorB);
		if (dOuterProduct.x < 0.0) { angle = -angle; }

		return true;
	}

	//--------------------------------------------------
	// Name: Up()
	// Desc: 上
	//--------------------------------------------------
	static const KdVector3 Up()
	{
		return KdVector3(0.0f, 1.0f, 0.0f);
	}
};

//-----------------------------------------------------------------------------
// Name: class KdMatrix
// Desc: 自作の4x4の行列クラス XMFLOAT4X4継承
//-----------------------------------------------------------------------------
class KdMatrix : public DirectX::XMFLOAT4X4
{
public:
	//--------------------------------------------------
	// Name: KdMatrix()
	// Desc: デフォルトコンストラクタ 単位行列
	//--------------------------------------------------
	KdMatrix()
	{
		*this = DirectX::XMMatrixIdentity();
	}

	//--------------------------------------------------
	// Name: KdMatrix()
	// Desc: コンストラクタ XMMATRIXから代入
	//--------------------------------------------------
	KdMatrix(const DirectX::XMMATRIX& m)
	{
		DirectX::XMStoreFloat4x4(this, m);
	}

	//--------------------------------------------------
	// Name: KdMatrix()
	// Desc: XMFLOAT4X4,Math::Matrixから代入
	//--------------------------------------------------
	KdMatrix(const DirectX::XMFLOAT4X4& m)
	{
		memcpy_s(this, sizeof(float) * 16, &m, sizeof(XMFLOAT4X4));
	}

	//--------------------------------------------------
	// Name: DirectX::XMMATRIX()
	// Desc: XXMMATRIXへ変換
	//--------------------------------------------------
	operator DirectX::XMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(this);
	}

	//--------------------------------------------------
	// Name: Math::Matrix& ()
	// Desc: Math::Matrixと互換性を持つための関数
	//--------------------------------------------------
	operator Math::Matrix& ()
	{
		return *(Math::Matrix*)this;
	}

	//--------------------------------------------------
	// Name: Effekseer::Matrix44()
	// Desc: Effekseer::Matrix44と互換性を持つための関数
	//--------------------------------------------------
	operator Effekseer::Matrix44()
	{
		Effekseer::Matrix44 result = {};
		for (int height = 0; height < 4; height++)
		{
			for (int width = 0; width < 4; width++)
			{
				result.Values[height][width] = this->m[height][width];
			}
		}
		return result;
	}

	//--------------------------------------------------
	// Name: Effekseer::Matrix43()
	// Desc: Effekseer::Matrix43と互換性を持つための関数
	//--------------------------------------------------
	operator Effekseer::Matrix43()
	{
		Effekseer::Matrix43 result = {};
		for (int height = 0; height < 4; height++)
		{
			for (int width = 0; width < 3; width++)
			{
				result.Value[height][width] = this->m[height][width];
			}
		}
		return result;
	}

	//--------------------------------------------------
	// Name: operator*=()
	// Desc: 代入演算子 *
	//--------------------------------------------------
	KdMatrix& operator*= (const KdMatrix& m)
	{
		*this = DirectX::XMMatrixMultiply(*this, m);
		return *this;
	}

	//-----------------------------------------------------------------------------
	//
	// 作成
	//
	//-----------------------------------------------------------------------------

	//--------------------------------------------------
	// Name: CreateTranslation()
	// Desc: 移動行列作成
	//--------------------------------------------------
	void CreateTranslation(float x, float y, float z)
	{
		*this = DirectX::XMMatrixTranslation(x, y, z);
	}
	void CreateTranslation(KdVector3 aVec)
	{
		*this = DirectX::XMMatrixTranslation(aVec.x, aVec.y, aVec.z);
	}

	//--------------------------------------------------
	// Name: CreateRotationX
	// Desc: X軸の回転行列作成
	//--------------------------------------------------
	void CreateRotationX(float aAngle)
	{
		*this = DirectX::XMMatrixRotationX(aAngle);
	}
	
	//--------------------------------------------------
	// Name: CreateScalling()
	// Desc: 拡縮行列作成
	//--------------------------------------------------
	void CreateScalling(float aSize)
	{
		*this = DirectX::XMMatrixScaling(aSize, aSize, aSize);
	}
	void CreateScalling(float x, float y, float z)
	{
		*this = DirectX::XMMatrixScaling(x, y, z);
	}
	void CreateScalling(KdVector3 vec)
	{
		*this = DirectX::XMMatrixScaling(vec.x, vec.y, vec.z);
	}

	//--------------------------------------------------
	// Name: CreateRotationAxis()
	// Desc: 指定軸回転行列作成
	//--------------------------------------------------
	void CreateRotationAxis(const KdVector3& axis, float angle)
	{
		*this = DirectX::XMMatrixRotationAxis(axis, angle);
	}

	//--------------------------------------------------
	// Name: CreateProjectionPerspectiveFov()
	// Desc: 透視影行列の作成
	//--------------------------------------------------
	KdMatrix& CreateProjectionPerspectiveFov(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
		return *this;
	}

	//--------------------------------------------------
	// Name: CreateProjection_Orthographic()
	// Desc: 正射影行列作成
	//--------------------------------------------------
	KdMatrix& CreateProjection_Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
		return *this;
	}

	//--------------------------------------------------
	// Name: LookTo()
	// Desc: Z軸を指定方向に向ける
	//--------------------------------------------------
	KdMatrix& LookTo(const KdVector3& dir, const KdVector3& up)
	{
		KdVector3 vZ = dir;
		vZ.Normalize();
		KdVector3 vX = KdVector3::Cross(up, vZ).Normalize();
		if (vX.LengthSquared() == 0)
		{
			vX = { 1,0,0 };
		}
		KdVector3 vY = KdVector3::Cross(vZ, vX).Normalize();

		float scaleX = GetAxisX().Length();
		float scaleY = GetAxisY().Length();
		float scaleZ = GetAxisZ().Length();

		SetAxisX(vX * scaleX);
		SetAxisY(vY * scaleY);
		SetAxisZ(vZ * scaleZ);

		return *this;
	}

	//--------------------------------------------------
	// Name: CreateFromQuaternion()
	// Desc: クォータニオンから回転行列作成
	//--------------------------------------------------
	void CreateFromQuaternion(const Math::Quaternion& quat)
	{
		*this = DirectX::XMMatrixRotationQuaternion(quat);
	}

	//-----------------------------------------------------------------------------
	//
	// 操作
	//
	//-----------------------------------------------------------------------------

	//--------------------------------------------------
	// Name: RotateX()
	// Desc: X軸回転
	//--------------------------------------------------
	void RotateX(float angle)
	{
		*this *= DirectX::XMMatrixRotationX(angle);
	}

	//--------------------------------------------------
	// Name: RotateY()
	// Desc: Y軸回転
	//--------------------------------------------------
	void RotateY(float angle)
	{
		*this *= DirectX::XMMatrixRotationY(angle);
	}

	//--------------------------------------------------
	// Name: RotateZ()
	// Desc: Z軸回転
	//--------------------------------------------------
	void RotateZ(float angle)
	{
		*this *= DirectX::XMMatrixRotationZ(angle);
	}

	//--------------------------------------------------
	// Name: Scale()
	// Desc: 拡縮
	//--------------------------------------------------
	void Scale(float x, float y, float z)
	{
		*this *= DirectX::XMMatrixScaling(x, y, z);
	}

	//--------------------------------------------------
	// Name: Inverse()
	// Desc: 逆行列にする
	//--------------------------------------------------
	void Inverse()
	{
		*this = DirectX::XMMatrixInverse(nullptr, *this);
	}

	//--------------------------------------------------
	// Name: Move()
	// Desc: 移動
	//--------------------------------------------------
	void Move(const KdVector3& v)
	{
		_41 += v.x; _42 += v.y; _43 += v.z;
	}
	void Move(const float& aX, const float& aY, const float& aZ)
	{
		_41 += aX; _42 += aY; _43 += aZ;
	}

	//--------------------------------------------------
	// Name: SetBillboard()
	// Desc: 正面向くように
	//--------------------------------------------------
	inline void SetBillboard(const KdMatrix& aTarget)
	{
		// 逆行列にする
		KdMatrix targetMatrix = aTarget;
		targetMatrix.Inverse();

		// 移動成分を所得
		float x = _41, y = _42, z = _43;

		// 合成
		*this *= targetMatrix;

		// 移動成分の設定
		_41 = x; _42 = y; _43 = z;
	}

	//-----------------------------------------------------------------------------
	//
	// プロパティ
	//
	//-----------------------------------------------------------------------------

	//--------------------------------------------------
	// Name: GetAxisX()
	// Desc: X軸所得
	//--------------------------------------------------
	KdVector3 GetAxisX() const { return { _11 ,_12 ,_13 }; }

	//--------------------------------------------------
	// Name: GetAxisY()
	// Desc: Y軸所得
	//--------------------------------------------------
	KdVector3 GetAxisY() const { return { _21 ,_22 ,_23 }; }

	//--------------------------------------------------
	// Name: GetAxisZ()
	// Desc: Z軸所得
	//--------------------------------------------------
	KdVector3 GetAxisZ() const { return { _31 ,_32 ,_33 }; }

	//--------------------------------------------------
	// Name: SetAxisX()
	// Desc: X軸セット
	//--------------------------------------------------
	void SetAxisX(const KdVector3& v)
	{
		_11 = v.x;
		_12 = v.y;
		_13 = v.z;
	}

	//--------------------------------------------------
	// Name: SetAxisZ()
	// Desc: Y軸セット
	//--------------------------------------------------
	void SetAxisY(const KdVector3& v)
	{
		_21 = v.x; _22 = v.y; _23 = v.z;
	}

	//--------------------------------------------------
	// Name: SetAxisZ()
	// Desc: Z軸セット
	//--------------------------------------------------
	void SetAxisZ(const KdVector3& v)
	{
		_31 = v.x; _32 = v.y; _33 = v.z;
	}

	//--------------------------------------------------
	//
	// 座標所得
	KdVector3 GetTranslation() const { return { _41 ,_42 ,_43 }; }

	//--------------------------------------------------
	// Name: SetTranslation()
	// Desc: 座標セット
	//--------------------------------------------------
	void SetTranslation(const KdVector3& v)
	{
		_41 = v.x; _42 = v.y; _43 = v.z;
	}

	//--------------------------------------------------
	// Name: GetAngle()
	// Desc: XYZの順で合成した時の、回転角度を算出 ※Xしかとれん
	//--------------------------------------------------
	KdVector3 GetAngle() const
	{
		KdMatrix matrix = *this;

		// 各軸所得
		KdVector3 axisX = matrix.GetAxisX();
		KdVector3 axisY = matrix.GetAxisY();
		KdVector3 axisZ = matrix.GetAxisZ();

		// 各軸正規化
		axisX.Normalize();
		axisY.Normalize();
		axisZ.Normalize();

		// 行列正規化
		matrix.SetAxisX(axisX);
		matrix.SetAxisY(axisY);
		matrix.SetAxisZ(axisZ);

		KdVector3 angle = {};
		angle.x = atan2f(matrix.m[1][2], matrix.m[2][2]);
		angle.y = atan2f(-matrix.m[0][2], sqrtf(matrix.m[1][2] * matrix.m[1][2] + matrix.m[2][2] * matrix.m[2][2]));
		angle.z = atan2f(matrix.m[0][1], matrix.m[0][0]);

		return angle;
	}

	//--------------------------------------------------
	// Name: GetScale()
	// Desc: 拡大成分所得
	//--------------------------------------------------
	KdVector3 GetScale() const
	{
		KdMatrix matrix = *this;

		// 各軸の長さ所得
		float x = matrix.GetAxisX().Length();
		float y = matrix.GetAxisY().Length();
		float z = matrix.GetAxisZ().Length();

		KdVector3 result = {};
		result.x = x;
		result.y = y;
		result.z = z;

		return result;
	}

	//--------------------------------------------------
	// Name: GetScales()
	// Desc:
	//--------------------------------------------------
	KdVector3 GetScales() const
	{
		return {
			this->GetAxisX().Length(),
			this->GetAxisY().Length(),
			this->GetAxisZ().Length(),
		};
	}

	//--------------------------------------------------
	// Name: ExtractRotation()
	// Desc: ワールド行列から回転成分のみを抽出する
	//--------------------------------------------------
	DirectX::XMMATRIX ExtractRotation(const DirectX::XMMATRIX& mWorld)
	{
		DirectX::XMMATRIX mOffset = DirectX::XMMatrixTranslation(mWorld.r[3].m128_f32[0], mWorld.r[3].m128_f32[1], mWorld.r[3].m128_f32[2]);
		DirectX::XMMATRIX mScaling = DirectX::XMMatrixScaling(
			DirectX::XMVector3Length(DirectX::XMVECTOR{ mWorld.r[0].m128_f32[0],mWorld.r[0].m128_f32[1],mWorld.r[0].m128_f32[2] }).m128_f32[0],
			DirectX::XMVector3Length(DirectX::XMVECTOR{ mWorld.r[1].m128_f32[0],mWorld.r[1].m128_f32[1],mWorld.r[1].m128_f32[2] }).m128_f32[0],
			DirectX::XMVector3Length(DirectX::XMVECTOR{ mWorld.r[2].m128_f32[0],mWorld.r[2].m128_f32[1],mWorld.r[2].m128_f32[2] }).m128_f32[0]
		);

		// 左からScaling、右からOffsetの逆行列をそれぞれかける。
		DirectX::XMVECTOR det = {};
		return DirectX::XMMatrixInverse(&det, mScaling) * mWorld * XMMatrixInverse(&det, mOffset);
	}
};

//--------------------------------------------------
// Name: operator*
// Desc: KdMatrix同士の合成
//--------------------------------------------------
inline KdMatrix operator* (const KdMatrix& m1, const KdMatrix& m2)
{
	using namespace DirectX;
	return XMMatrixMultiply(m1, m2);
}

//-----------------------------------------------------------------------------
// Name: class KdQuaternion
// Desc: 自作のクォータニン操作クラス XMFLOAT4継承
//-----------------------------------------------------------------------------
class KdQuaternion: public DirectX::XMFLOAT4
{
public:
	//--------------------------------------------------
	// Name: KdQuaternion()
	// Desc: デフォルトコンストラクタ 初期値代入
	//--------------------------------------------------
	KdQuaternion() { x = y = z = 0.0f; w = 1.0f; }

	//--------------------------------------------------
	// Name: KdQuaternion()
	// Desc: コンストラクタ XMVECTORから代入
	//--------------------------------------------------
	KdQuaternion(const DirectX::XMVECTOR& v) { DirectX::XMStoreFloat4(this, v); }

	//--------------------------------------------------
	// Name: DirectX::XMVECTOR()
	// Desc: XMVECTORに変換
	//--------------------------------------------------
	operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat4(this); }

	//--------------------------------------------------
	// Name: CreateFromMatrix()
	// Desc: 行列から生成
	//--------------------------------------------------
	KdQuaternion& CreateFromMatrix(const Math::Matrix& a_matrix)
	{
		*this = DirectX::XMQuaternionRotationMatrix(a_matrix);
		return *this;
	}
};

/*
{
	// ベクトルの定義
	class Vector2D
	{
	public:
		float x;
		float y;
	};

	// ベクトルの長さを計算する
	float get_vector_length(Vector2D v) { return pow((v.x * v.x) + (v.y * v.y), 0.5f); }

	// ベクトル内積
	float dot_product(Vector2D vl, Vector2D vr) { return vl.x * vr.x + vl.y * vr.y; }

	// 2つのベクトルABのなす角度θを求める
	float AngleOf2Vector(Vector2D a_vector01, Vector2D a_vector02)
	{
		// ベクトルAとBの長さを計算する
		float length_A = get_vector_length(a_vector01);
		float length_B = get_vector_length(a_vector02);

		// 内積とベクトル長さを使ってcosθを求める
		float cos_sita = dot_product(a_vector01, a_vector02) / (length_A * length_B);

		// cosθからθを求める
		float sita = acosf(cos_sita);
		sita *= KdToDegrees;

		return sita;
	}
}
*/