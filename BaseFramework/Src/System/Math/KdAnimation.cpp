//-----------------------------------------------------------------------------
// File: KdAnimation.h
//
// Edit: 2021/03/19 野筒隼輔
//-----------------------------------------------------------------------------
#include "KdAnimation.h"

//--------------------------------------------------
// Name: BinarySearchNextAnimKey()
// Desc: アニメーションキーの検索
//--------------------------------------------------
template<class T>
int BinarySearchNextAnimKey(const std::vector<T>& a_list, float a_time)
{
    int low = 0;
    int high = static_cast<int>(a_list.size());

    while (low < high)
    {
        int mid = (low + high) / 2;
        float midTime = a_list[mid].m_time;

        if (midTime <= a_time) low = mid + 1;
        else high = mid;
    }

    return low;
}

//-----------------------------------------------------------------------------
// KdAnimationData
//-----------------------------------------------------------------------------

void KdAnimationData::Node::Interpolate(KdMatrix& a_dst, float a_time)
{
    // クォータニオンによる回転補完
    KdMatrix rotate;
    KdQuaternion resultQuat;
    if (InterpolateRotation(resultQuat, a_time))
    {
        rotate.CreateFromQuaternion(resultQuat);
    }

    // ベクターによる座標補完
    KdMatrix trans;
    KdVector3 resultVec;
    if (InterpolateTranslation(resultVec, a_time))
    {
        trans.CreateTranslation(resultVec);
    }

    a_dst = rotate * trans;
}

bool KdAnimationData::Node::InterpolateTranslation(KdVector3& a_result, float a_time)
{
    if (m_translations.size() == 0) return false;

    // キー位置検索
    UINT keyIdx = BinarySearchNextAnimKey(m_translations, a_time);

    // 先頭キーなら最後のデータを返す
    if (keyIdx == 0)
    {
        a_result = m_translations.front().m_vec;
        return true;
    }
    // 配列外のキーなら最後のデータを返す
    else if (keyIdx >= m_translations.size())
    {
        a_result = m_translations.back().m_vec;
        return true;
    }
    // それ以外ならその時間の値を補完計算で求める
    else
    {
        auto& prev = m_translations[keyIdx - 1];// 前のキー
        auto& next = m_translations[keyIdx];    // 次のキー

        // 前のキーと次のキーの時間から 0～1の間の時間を求める
        float f = (a_time - prev.m_time) / (next.m_time - prev.m_time);

        // 補完
        a_result = DirectX::XMVectorLerp(prev.m_vec, next.m_vec, f);
    }

    return true;
}

bool KdAnimationData::Node::InterpolateRotation(KdQuaternion& a_result, float a_time)
{
    if (m_rotations.size() == 0) return false;

    // キー位置検索
    UINT keyIdx = BinarySearchNextAnimKey(m_rotations, a_time);

    // 先頭キーなら最後のデータを返す
    if (keyIdx == 0)
    {
        a_result = m_rotations.front().m_quat;
    }
    // 配列外のキーなら最後のデータを返す
    else if (keyIdx >= m_rotations.size())
    {
        a_result = m_rotations.back().m_quat;
    }
    // それ以外ならその時間の値を補完計算で求める
    else
    {
        auto& prev = m_rotations[keyIdx - 1];// 前のキー
        auto& next = m_rotations[keyIdx];    // 次のキー

        // 前のキーと次のキーの時間から 0～1の間の時間を求める
        float f = (a_time - prev.m_time) / (next.m_time - prev.m_time);

        // 補完
        a_result = DirectX::XMQuaternionSlerp(prev.m_quat, next.m_quat, f);
    }

    return true;
}

//-----------------------------------------------------------------------------
// KdAnimator
//-----------------------------------------------------------------------------

KdAnimator::KdAnimator()
    : m_spAnimation(nullptr)
    , m_time(0.0f)
    , m_isLoop(true)
{
}

inline void KdAnimator::SetAnimation(std::shared_ptr<KdAnimationData>& a_spData, bool a_isLoop)
{
    m_spAnimation = a_spData;
    m_time = 0.0f;
    m_isLoop = a_isLoop;
}

bool KdAnimator::IsAnimationEnd() const
{
    // アニメーションデータ確認
    if (m_spAnimation == nullptr) { return true; }
    // 経過時間確認
    if (m_time >= m_spAnimation->m_maxLength) { return true; }

    return false;
}

void KdAnimator::AdvanceTime(std::vector<KdModel::Node>& a_nodeVector, float a_speed, float a_brendWeight)
{
    // アニメーションデータ確認
    if (!m_spAnimation) return;

    // 全てのアニメーションノード(モデルの行列を補完する情報)の行列補完を実施
    for (auto& rAnimNode : m_spAnimation->m_nodes)
    {
        // 対応するノードのインデックス
        UINT idx = rAnimNode.m_nodeOffset;

        // ブレンドの比重が1以上ならそのまま補完
        if (a_brendWeight >= 1.0f)
        {
            // アニメーションデータによる行列補完
            rAnimNode.Interpolate(a_nodeVector[idx].m_localTransform, m_time);
        }
        // ブレンド比重に従ってブレンディング補完
        else
        {
            //--------------------------------------------------
            // 補完用の情報生成
            //--------------------------------------------------

            KdMatrix brendAnim = {};

            // ブレンド用行列生成
            rAnimNode.Interpolate(brendAnim, m_time);

            // 補完拡大率生成
            KdVector3 scale = DirectX::XMVectorLerp(
                a_nodeVector[idx].m_localTransform.GetScales(),
                brendAnim.GetScales(), a_brendWeight
            );

            // 補完回転クォータニオン生成
            KdQuaternion rot = DirectX::XMQuaternionSlerp(
                KdQuaternion().CreateFromMatrix(a_nodeVector[idx].m_localTransform),
                KdQuaternion().CreateFromMatrix(brendAnim),
                a_brendWeight
            );

            // 補完座標生成
            KdVector3 trans = DirectX::XMVectorLerp(
                a_nodeVector[idx].m_localTransform.GetTranslation(),
                brendAnim.GetTranslation(), a_brendWeight
            );

            //--------------------------------------------------
            // 補完
            //--------------------------------------------------

            // 拡大率の補完
            a_nodeVector[idx].m_localTransform.CreateScalling(scale);

            // 回転角度の補完
            KdMatrix rotate = {};
            rotate.CreateFromQuaternion(rot);
            a_nodeVector[idx].m_localTransform *= rotate;

            // 座標の補完
            a_nodeVector[idx].m_localTransform.SetTranslation(trans);
        }
    }

    // アニメーションのフレームを1進める
    m_time += a_speed;

    // アニメーションが最後のフレームを超えていたら
    if (m_time >= m_spAnimation->m_maxLength)
    {
        m_time = m_isLoop ? 0.0f : m_spAnimation->m_maxLength;
    }
}

//-----------------------------------------------------------------------------
// KdBrendAnimator
//-----------------------------------------------------------------------------

void KdBrendAnimator::AdvanceTime(std::vector<KdModel::Node>& a_node, float a_speed)
{
    if (m_pNowAnimator)
    {
        // アニメーション更新
        m_pNowAnimator->AdvanceTime(a_node, a_speed);
    }

    //--------------------------------------------------
    // ブレンド
    //--------------------------------------------------

    // 遷移先のアニメーション確認 ※なければブレンドしない
    if (m_pNextAnimator)
    {
        // 進行度をタイマーから所得
        float brendProgress = m_brendTimer.GetProgress();

        // ブレンドアニメーション
        m_pNextAnimator->AdvanceTime(a_node, a_speed, brendProgress);

        // タイマー更新
        m_brendTimer.Update();

        // タイマーの終了確認
        if (m_brendTimer.IsEnd())
        {
            // 遷移先のアニメーションユニットを再生中のアニメーションに置き換え
            m_pNowAnimator = std::move(m_pNextAnimator);
            // 遷移先(ブレンド先)をクリア
            m_pNextAnimator = nullptr;

            // タイマーのリセット
            m_brendTimer.Reset();
        }
    }
}

void KdBrendAnimator::SetAnimation(std::shared_ptr<KdAnimationData>& a_spData, float a_changeSec, bool a_isLoop)
{
    // アニメーションデータ確認
    if (a_spData == nullptr) return;

    // 遷移中のアニメーションがあったら、一気に遷移してしまう
    if (m_pNextAnimator)
    {
        m_pNextAnimator = std::move(m_pNextAnimator);
    }

    // 生成, アニメーション設定
    m_pNextAnimator = std::make_unique<KdAnimator>();
    m_pNextAnimator->SetAnimation(a_spData, a_isLoop);

    // ブレンド用のタイマー設定
    if (a_changeSec <= 0.0f) a_changeSec = 0.1f;
    m_brendTimer.SetTime(a_changeSec);
}

bool KdBrendAnimator::IsAnimationEnd() const
{
    // 実行中のアニメがある & アニメが再生中
    if (m_pNowAnimator && !m_pNextAnimator->IsAnimationEnd())
    {
        // 終わってない
        return false;
    }

    // 終わった
    return false;
}