﻿// --------------------------------------------------------------------------
//  Binary Brain  -- binary neural net framework
//
//                                Copyright (C) 2018-2019 by Ryuji Fuchikami
//                                https://github.com/ryuz
//                                ryuji.fuchikami@nifty.com
// --------------------------------------------------------------------------


#pragma once

#include <cmath>
#include <array>
#include <vector>

#include "bb/SparseLayer.h"


namespace bb {


// LUT方式基底クラス
template <typename FT = Bit, typename BT = float>
class LutLayer : public SparseLayer<FT, BT>
{
public:
    // LUT操作の定義
    virtual int   GetLutTableSize(index_t node) const = 0;
    virtual void  SetLutTable(index_t node, int bitpos, bool value) = 0;
    virtual bool  GetLutTable(index_t node, int bitpos) const = 0;

    virtual bool  GetLutInput(index_t frame, index_t node, int bitpos) const = 0;
    virtual int   GetLutInputIndex(index_t frame, index_t node) const
    {
        int index = 0;
        int lut_table_size = GetLutTableSize(node);
        for (int bitpos = 0; bitpos < lut_table_size; ++bitpos) {
            index |= (GetLutInput(frame, node, bitpos) ? (1 << bitpos) : 0);
        }
        return index;
    }

protected:
    void InitializeLutTable(std::uint64_t seed)
    {
        std::mt19937_64                     mt(seed);
        std::uniform_int_distribution<int>  rand(0, 1);
        
        index_t node_size = GetShapeSize(GetOutputShape());

        // LUTテーブルをランダムに初期化
        for ( index_t node = 0; node < node_size; ++node) {
            int lut_table_size = GetLutTableSize(node);
            for (int i = 0; i < lut_table_size; i++) {
                this->SetLutTable(node, i, rand(mt) != 0);
            }
        }
    }
    
public:
    // 形状が同一のSparceLayerをテーブル化して取り込む
    template <typename SFT, typename SBT>
    void ImportLayer(const SparseLayer<SFT, SBT>& src)
    {
        auto node_size  = GetShapeSize(GetInputShape());

        BB_ASSERT(GetShapeSize(src.GetInputShape()) == node_size);

        for (index_t node = 0; node < node_size; ++node) {
            auto input_size = GetNodeInputSize(node);
            auto table_size = GetLutTableSize(node);
            
            BB_ASSERT(src.GetNodeInputSize(node) == input_size);
            
            // 入力をコピー
            for (int input_index = 0; input_index < input_size; ++input_index) {
                SetNodeInput(node, input_index, src.GetNodeInput(node, input_index));
            }

            // 係数をバイナリ化
            std::vector<SFT> vec(input_size);
            for (int index = 0; index < table_size; ++index) {
                for (int bit = 0; bit < input_size; ++bit) {
                    vec[bit] = (index & (1 << bit)) ? (SFT)1.0 : (SFT)0.0;
                }
                auto v = src.ForwardNode(node, vec);
                SetLutTable(node, index, (v[0] > 0));
            }
        }
    }
};


}

// end of file