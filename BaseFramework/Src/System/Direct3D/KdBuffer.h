//-----------------------------------------------------------------------------
// File: KdBuffer.h
//
// Edit: 2021/03/29 野筒隼輔 (初期配布)
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Name: class KdBuffer
// Desc: 様々なバッファを作成・操作するクラス ※ID3D11Bufferを簡略化
//-----------------------------------------------------------------------------
class KdBuffer
{
public:

	// コンストラクタ
	KdBuffer();
	// デストラクタ
	~KdBuffer() { Release(); }
	// バッファを作成
	// ・bindFlags		… どのバッファとしてDirect3Dへバインドするかのフラグ　D3D11_BIND_FLAG定数を指定する
	// ・bufferSize		… 作成するバッファのサイズ(byte)
	// ・bufferUsage	… バッファの使用法　D3D11_USAGE定数を指定する
	// ・initData		… 作成時に書き込むデータ nullptrだと何も書き込まない
	bool Create(UINT bindFlags, UINT bufferSize, D3D11_USAGE bufferUsage, const D3D11_SUBRESOURCE_DATA* initData);
	// 解放
	void Release();

	//--------------------------------------------------
	// 操作
	//--------------------------------------------------

	// バッファへ指定データを書き込み
	// Dynamicの場合はD3D11_MAP_WRITE_DISCARDでMap。
	// Defaultの場合はUpdateSubResource。
	// Stagingの場合はD3D11_MAP_READ_WRITEでMap。
	// ・pSrcData		… 書き込みたいデータの先頭アドレス
	// ・size			… 書き込むサイズ(byte)
	void WriteData(const void* pSrcData, UINT size);

	// GPU上でバッファのコピーを実行する
	// ※詳細はDeviceContextのCopyResource()参照 https://msdn.microsoft.com/ja-jp/library/ee419574(v=vs.85).aspx
	//  一部抜粋
	//  ※異なるリソースである必要があります。
	//  ※同じ種類である必要があります。
	//  ※次元 (幅、高さ、深度、サイズなど) が同じである必要があります。
	//  ※単純なコピーのみが行われます。CopyResource では、引き伸ばし、カラー キー、ブレンド、フォーマット変換はサポートされません。
	//  ※DXGIフォーマットの互換性が必要です。
	// ・srcBuffer		… コピー元バッファ
	void CopyFrom(const KdBuffer& srcBuffer);

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------
	// バッファインターフェイスを取得
	ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
	ID3D11Buffer* const* GetAddress() const { return &m_pBuffer; }
	// バッファのサイズを取得
	UINT GetBufferSize() const { return m_bufSize; }

protected:
	ID3D11Buffer*	m_pBuffer;	// バッファ本体
	UINT			m_bufSize;	// バッファのサイズ(byte)
	D3D11_USAGE		m_bufUsage;	// バッファの使用法

private:
	// コピー禁止用
	KdBuffer(const KdBuffer& src) = delete;
	void operator=(const KdBuffer& src) = delete;
};

//-----------------------------------------------------------------------------
// Name: class KdConstantBuffer
// Desc: 作業データ付き 定数バッファクラス
//       Bufferに、バッファと同じサイズの作業データを持たせたり、
//       更新した時だけバッファに書き込みを行ったりと、管理を楽にしたクラス
//-----------------------------------------------------------------------------
template<class DataType>
class KdConstantBuffer
{
public:

	// デストラクタ
	~KdConstantBuffer() { Release(); }
	// なんすかこれ
	KdConstantBuffer() = default;

	// DataType型のサイズの定数バッファを作成
	// ・initData		… 作成時にバッファに書き込むデータ　nullptrで何も書き込まない
	bool Create(const DataType* initData = nullptr)
	{
		if (initData) { m_work = *initData; }

		//----------------------------------
		// バッファ作成
		//----------------------------------

		// 書き込むデータ
		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = &m_work;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;

		// 作成
		return m_buffer.Create(D3D11_BIND_CONSTANT_BUFFER, sizeof(DataType), D3D11_USAGE_DYNAMIC, &srd);
	}

	// 解放
	void Release()
	{
		m_buffer.Release();
		m_isDirty = true;
	}

	// m_workを定数バッファへ書き込む
	// ※m_isDirtyがtrueの時のみ、バッファに書き込まれる
	void Write()
	{
		// 変更がある時だけ書き込む
		if (m_isDirty) {
			m_buffer.WriteData(&m_work, m_buffer.GetBufferSize());
			m_isDirty = false;
		}
	}

	//--------------------------------------------------
	//
	// 取得・設定
	//
	//--------------------------------------------------

	// 作業領域取得　※変更フラグがONになります
	DataType& Work() { m_isDirty = true; return m_work; }
	// 作業領域取得　※読み取り専用　変更フラグは変化しません
	const DataType& GetWork() const { return m_work; }
	// バッファアドレス取得
	ID3D11Buffer* const* GetAddress() const { return m_buffer.GetAddress(); }

private:
	KdBuffer	m_buffer;			// 定数バッファ
	DataType	m_work;				// 作業用 定数バッファ ※この内容がWrite関数で定数バッファ本体に方に書き込まれる
	bool		m_isDirty = true;	// データ更新フラグ ※パフォーメンス向上のため、trueの時だけWrite()でデータ書き込み実行

private:
	// コピー禁止用
	KdConstantBuffer(const KdConstantBuffer& src) = delete;
	void operator=(const KdConstantBuffer& src) = delete;
};