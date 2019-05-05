// --------------------------------------------------------------------------
//  BinaryBrain  -- binary network evaluation platform
//   CIFAR-10 sample
//
//                                Copyright (C) 2018-2019 by Ryuji Fuchikami
// --------------------------------------------------------------------------


#include <iostream>
#include <fstream>
#include <numeric>
#include <random>
#include <chrono>

#include "bb/RealToBinary.h"
#include "bb/BinaryToReal.h"
#include "bb/StochasticLut6.h"
#include "bb/StochasticLutBn.h"
#include "bb/BinaryLutN.h"
#include "bb/LoweringConvolution.h"
#include "bb/BatchNormalization.h"
#include "bb/ReLU.h"
#include "bb/MaxPooling.h"
#include "bb/Reduce.h"
#include "bb/LossSoftmaxCrossEntropy.h"
#include "bb/MetricsCategoricalAccuracy.h"
#include "bb/OptimizerAdam.h"
#include "bb/OptimizerAdaGrad.h"
#include "bb/OptimizerSgd.h"
#include "bb/LoadCifar10.h"
#include "bb/ShuffleSet.h"
#include "bb/Utility.h"
#include "bb/Sequential.h"
#include "bb/Runner.h"
#include "bb/ExportVerilog.h"
#include "bb/UniformDistributionGenerator.h"


#if 0

// CNN with LUT networks
void Cifar10Sparse6Cnn(int epoch_size, int mini_batch_size, int max_run_size, bool binary_mode, bool file_read)
{
    std::string net_name = "Cifar10Sparse6Cnn";

  // load cifar-10 data
#ifdef _DEBUG
    auto td = bb::LoadCifar10<>::Load(1);
    std::cout << "!!! debug mode !!!" << std::endl;
#else
    auto td = bb::LoadCifar10<>::Load();
#endif

    // create network
    auto layer_cnv0_sl0 = bb::StochasticLut6<>::Create(512);
    auto layer_cnv0_sl1 = bb::StochasticLut6<>::Create(384);
    auto layer_cnv0_sl2 = bb::StochasticLut6<>::Create(64);
    auto layer_cnv1_sl0 = bb::StochasticLut6<>::Create(512);
    auto layer_cnv1_sl1 = bb::StochasticLut6<>::Create(384);
    auto layer_cnv1_sl2 = bb::StochasticLut6<>::Create(64);
    auto layer_cnv2_sl0 = bb::StochasticLut6<>::Create(1024);
    auto layer_cnv2_sl1 = bb::StochasticLut6<>::Create(768);
    auto layer_cnv2_sl2 = bb::StochasticLut6<>::Create(128);
    auto layer_cnv3_sl0 = bb::StochasticLut6<>::Create(1024);
    auto layer_cnv3_sl1 = bb::StochasticLut6<>::Create(768);
    auto layer_cnv3_sl2 = bb::StochasticLut6<>::Create(64);
    auto layer_sl4      = bb::StochasticLut6<>::Create(2048);
    auto layer_sl5      = bb::StochasticLut6<>::Create(1024);
    auto layer_sl6      = bb::StochasticLut6<>::Create(420);
    auto layer_sl7      = bb::StochasticLut6<>::Create(70);

    {
        auto cnv0_sub = bb::Sequential::Create();
        cnv0_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv0_sub->Add(layer_cnv0_sl0);
        cnv0_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv0_sub->Add(layer_cnv0_sl1);
        cnv0_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv0_sub->Add(layer_cnv0_sl2);

        auto cnv1_sub = bb::Sequential::Create();
        cnv1_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv1_sub->Add(layer_cnv1_sl0);
        cnv1_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv1_sub->Add(layer_cnv1_sl1);
        cnv1_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv1_sub->Add(layer_cnv1_sl2);

        auto cnv2_sub = bb::Sequential::Create();
        cnv2_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv2_sub->Add(layer_cnv2_sl0);
        cnv2_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv2_sub->Add(layer_cnv2_sl1);
        cnv2_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv2_sub->Add(layer_cnv2_sl2);

        auto cnv3_sub = bb::Sequential::Create();
        cnv3_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv3_sub->Add(layer_cnv3_sl0);
        cnv3_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv3_sub->Add(layer_cnv3_sl1);
        cnv3_sub->Add(bb::StochasticBatchNormalization<>::Create());
        cnv3_sub->Add(layer_cnv3_sl2);
        
        auto net = bb::Sequential::Create();
        net->Add(bb::LoweringConvolution<>::Create(cnv0_sub, 3, 3));
        net->Add(bb::LoweringConvolution<>::Create(cnv1_sub, 3, 3));
//      net->Add(bb::StochasticMaxPooling2x2<>::Create());
        net->Add(bb::MaxPooling<>::Create(2, 2));
        net->Add(bb::LoweringConvolution<>::Create(cnv2_sub, 3, 3));
        net->Add(bb::LoweringConvolution<>::Create(cnv3_sub, 3, 3));
//      net->Add(bb::StochasticMaxPooling2x2<>::Create());
        net->Add(bb::MaxPooling<>::Create(2, 2));
        net->Add(bb::StochasticBatchNormalization<>::Create());
        net->Add(layer_sl4);
        net->Add(bb::StochasticBatchNormalization<>::Create());
        net->Add(layer_sl5);
        net->Add(bb::StochasticBatchNormalization<>::Create());
        net->Add(layer_sl6);
        net->Add(bb::StochasticBatchNormalization<>::Create());
        net->Add(layer_sl7);
        net->Add(bb::Reduce<>::Create(td.t_shape));
        net->SetInputShape(td.x_shape);

        if ( binary_mode ) {
            std::cout << "binary mode" << std::endl;
            net->SendCommand("binary true");
        }

        net->SendCommand("lut_binarize false");

//      net->SendCommand("batch_normalization false");

        /*
        net->SendCommand("fix_gamma true");
        net->SendCommand("fix_beta  true");
        net->SendCommand("set_gamma 0.2");
        net->SendCommand("set_beta  0.5");
        */

        // print model information
        net->PrintInfo();

        // run fitting
        bb::Runner<float>::create_t runner_create;
        runner_create.name               = net_name;
        runner_create.net                = net;
        runner_create.lossFunc           = bb::LossSoftmaxCrossEntropy<float>::Create();
        runner_create.metricsFunc        = bb::MetricsCategoricalAccuracy<float>::Create();
        runner_create.optimizer          = bb::OptimizerAdam<float>::Create();
//      runner_create.optimizer          = bb::OptimizerAdaGrad<float>::Create();
        runner_create.max_run_size       = max_run_size;    // 実際の1回の実行サイズ
        runner_create.file_read          = file_read;       // 前の計算結果があれば読み込んで再開するか
        runner_create.file_write         = true;            // 計算結果をファイルに保存するか
        runner_create.print_progress     = true;            // 途中結果を表示
        runner_create.initial_evaluation = false; // file_read;       // ファイルを読んだ場合は最初に評価しておく
        auto runner = bb::Runner<float>::Create(runner_create);
        runner->Fitting(td, epoch_size, mini_batch_size);
    }
}

#endif


#if 0

// CNN with LUT networks
void Cifar10Sparse6Cnn(int epoch_size, int mini_batch_size, int max_run_size, bool binary_mode, bool file_read)
{
    std::string net_name = "Cifar10Sparse6Cnn";

  // load cifar-10 data
#ifdef _DEBUG
    auto td = bb::LoadCifar10<>::Load(1);
    std::cout << "!!! debug mode !!!" << std::endl;
#else
    auto td = bb::LoadCifar10<>::Load();
#endif

    // create network
    auto layer_cnv0_sl0 = bb::StochasticLutBn<6>::Create(512);
    auto layer_cnv0_sl1 = bb::StochasticLutBn<6>::Create(384);
    auto layer_cnv0_sl2 = bb::StochasticLutBn<6>::Create(64);
    auto layer_cnv1_sl0 = bb::StochasticLutBn<6>::Create(512);
    auto layer_cnv1_sl1 = bb::StochasticLutBn<6>::Create(384);
    auto layer_cnv1_sl2 = bb::StochasticLutBn<6>::Create(64);
    auto layer_cnv2_sl0 = bb::StochasticLutBn<6>::Create(1024);
    auto layer_cnv2_sl1 = bb::StochasticLutBn<6>::Create(768);
    auto layer_cnv2_sl2 = bb::StochasticLutBn<6>::Create(128);
    auto layer_cnv3_sl0 = bb::StochasticLutBn<6>::Create(1024);
    auto layer_cnv3_sl1 = bb::StochasticLutBn<6>::Create(768);
    auto layer_cnv3_sl2 = bb::StochasticLutBn<6>::Create(64);
    auto layer_sl4      = bb::StochasticLutBn<6>::Create(2048);
    auto layer_sl5      = bb::StochasticLutBn<6>::Create(1024);
    auto layer_sl6      = bb::StochasticLutBn<6>::Create(420);
    auto layer_sl7      = bb::StochasticLutBn<6>::Create(70);

    {
        auto cnv0_sub = bb::Sequential::Create();
        cnv0_sub->Add(layer_cnv0_sl0);
        cnv0_sub->Add(layer_cnv0_sl1);
        cnv0_sub->Add(layer_cnv0_sl2);

        auto cnv1_sub = bb::Sequential::Create();
        cnv1_sub->Add(layer_cnv1_sl0);
        cnv1_sub->Add(layer_cnv1_sl1);
        cnv1_sub->Add(layer_cnv1_sl2);

        auto cnv2_sub = bb::Sequential::Create();
        cnv2_sub->Add(layer_cnv2_sl0);
        cnv2_sub->Add(layer_cnv2_sl1);
        cnv2_sub->Add(layer_cnv2_sl2);

        auto cnv3_sub = bb::Sequential::Create();
        cnv3_sub->Add(layer_cnv3_sl0);
        cnv3_sub->Add(layer_cnv3_sl1);
        cnv3_sub->Add(layer_cnv3_sl2);
        
        auto net = bb::Sequential::Create();
        net->Add(bb::LoweringConvolution<>::Create(cnv0_sub, 3, 3));
        net->Add(bb::LoweringConvolution<>::Create(cnv1_sub, 3, 3));
        net->Add(bb::MaxPooling<>::Create(2, 2));
        net->Add(bb::LoweringConvolution<>::Create(cnv2_sub, 3, 3));
        net->Add(bb::LoweringConvolution<>::Create(cnv3_sub, 3, 3));
        net->Add(bb::MaxPooling<>::Create(2, 2));
        net->Add(layer_sl4);
        net->Add(layer_sl5);
        net->Add(layer_sl6);
        net->Add(layer_sl7);
        net->Add(bb::Reduce<>::Create(td.t_shape));
        net->SetInputShape(td.x_shape);

        net->SendCommand("lut_binarize false");

        // print model information
        net->PrintInfo();

        // run fitting
        bb::Runner<float>::create_t runner_create;
        runner_create.name               = net_name;
        runner_create.net                = net;
        runner_create.lossFunc           = bb::LossSoftmaxCrossEntropy<float>::Create();
        runner_create.metricsFunc        = bb::MetricsCategoricalAccuracy<float>::Create();
//      runner_create.optimizer          = bb::OptimizerAdam<float>::Create();
        runner_create.optimizer          = bb::OptimizerAdaGrad<float>::Create();
        runner_create.max_run_size       = max_run_size;    // 実際の1回の実行サイズ
        runner_create.file_read          = file_read;       // 前の計算結果があれば読み込んで再開するか
        runner_create.file_write         = true;            // 計算結果をファイルに保存するか
        runner_create.print_progress     = true;            // 途中結果を表示
        runner_create.initial_evaluation = false; // file_read;       // ファイルを読んだ場合は最初に評価しておく
        auto runner = bb::Runner<float>::Create(runner_create);
        runner->Fitting(td, epoch_size, mini_batch_size);
    }
}

#endif


#if 0

// CNN with LUT networks
void Cifar10Sparse6Cnn(int epoch_size, int mini_batch_size, int max_run_size, bool binary_mode, bool file_read)
{
    std::string net_name = "Cifar10Sparse6Cnn";

  // load cifar-10 data
#ifdef _DEBUG
    auto td = bb::LoadCifar10<>::Load(1);
    std::cout << "!!! debug mode !!!" << std::endl;
#else
    auto td = bb::LoadCifar10<>::Load();
#endif

    // create network
    auto layer_cnv0_sl0 = bb::StochasticLutBn<6>::Create(192);
    auto layer_cnv0_sl1 = bb::StochasticLutBn<6>::Create(32);
    auto layer_cnv1_sl0 = bb::StochasticLutBn<6>::Create(192);
    auto layer_cnv1_sl1 = bb::StochasticLutBn<6>::Create(32);
    auto layer_cnv2_sl0 = bb::StochasticLutBn<6>::Create(384);
    auto layer_cnv2_sl1 = bb::StochasticLutBn<6>::Create(64);
    auto layer_cnv3_sl0 = bb::StochasticLutBn<6>::Create(384);
    auto layer_cnv3_sl1 = bb::StochasticLutBn<6>::Create(64);
    auto layer_sl4      = bb::StochasticLutBn<6>::Create(512);
    auto layer_sl5      = bb::StochasticLutBn<6>::Create(360);
    auto layer_sl6      = bb::StochasticLutBn<6>::Create(60);
    auto layer_sl7      = bb::StochasticLutBn<6>::Create(10);

    {
        auto cnv0_sub = bb::Sequential::Create();
        cnv0_sub->Add(layer_cnv0_sl0);
        cnv0_sub->Add(layer_cnv0_sl1);

        auto cnv1_sub = bb::Sequential::Create();
        cnv1_sub->Add(layer_cnv1_sl0);
        cnv1_sub->Add(layer_cnv1_sl1);

        auto cnv2_sub = bb::Sequential::Create();
        cnv2_sub->Add(layer_cnv2_sl0);
        cnv2_sub->Add(layer_cnv2_sl1);

        auto cnv3_sub = bb::Sequential::Create();
        cnv3_sub->Add(layer_cnv3_sl0);
        cnv3_sub->Add(layer_cnv3_sl1);
        
        auto net = bb::Sequential::Create();
        net->Add(bb::LoweringConvolution<>::Create(cnv0_sub, 3, 3));
        net->Add(bb::LoweringConvolution<>::Create(cnv1_sub, 3, 3));
        net->Add(bb::MaxPooling<>::Create(2, 2));
        net->Add(bb::LoweringConvolution<>::Create(cnv2_sub, 3, 3));
        net->Add(bb::LoweringConvolution<>::Create(cnv3_sub, 3, 3));
        net->Add(bb::MaxPooling<>::Create(2, 2));
        net->Add(layer_sl4);
        net->Add(layer_sl5);
        net->Add(layer_sl6);
        net->Add(layer_sl7);
        net->SetInputShape(td.x_shape);

        net->SendCommand("lut_binarize false");

        // print model information
        net->PrintInfo();

        // run fitting
        bb::Runner<float>::create_t runner_create;
        runner_create.name               = net_name;
        runner_create.net                = net;
        runner_create.lossFunc           = bb::LossSoftmaxCrossEntropy<float>::Create();
        runner_create.metricsFunc        = bb::MetricsCategoricalAccuracy<float>::Create();
        runner_create.optimizer          = bb::OptimizerAdam<float>::Create();
//      runner_create.optimizer          = bb::OptimizerAdaGrad<float>::Create();
        runner_create.max_run_size       = max_run_size;    // 実際の1回の実行サイズ
        runner_create.file_read          = file_read;       // 前の計算結果があれば読み込んで再開するか
        runner_create.file_write         = true;            // 計算結果をファイルに保存するか
        runner_create.print_progress     = true;            // 途中結果を表示
        runner_create.initial_evaluation = false; // file_read;       // ファイルを読んだ場合は最初に評価しておく
        auto runner = bb::Runner<float>::Create(runner_create);
        runner->Fitting(td, epoch_size, mini_batch_size);
    }
}


#endif


void Cifar10Sparse6Cnn(int epoch_size, int mini_batch_size, int max_run_size, bool binary_mode, bool file_read)
{
    std::string net_name = "Cifar10Sparse6Cnn";

  // load cifar-10 data
#ifdef _DEBUG
    auto td = bb::LoadCifar10<>::Load(1);
    std::cout << "!!! debug mode !!!" << std::endl;
#else
    auto td = bb::LoadCifar10<>::Load();
#endif

    /*
    // create network
    auto layer_cnv0_sl0 = bb::StochasticLutBn<6>::Create(192);
    auto layer_cnv0_sl1 = bb::StochasticLutBn<6>::Create(32);
    auto layer_cnv1_sl0 = bb::StochasticLutBn<6>::Create(192);
    auto layer_cnv1_sl1 = bb::StochasticLutBn<6>::Create(32);
    auto layer_cnv2_sl0 = bb::StochasticLutBn<6>::Create(384);
    auto layer_cnv2_sl1 = bb::StochasticLutBn<6>::Create(64);
    auto layer_cnv3_sl0 = bb::StochasticLutBn<6>::Create(384);
    auto layer_cnv3_sl1 = bb::StochasticLutBn<6>::Create(64);
    auto layer_sl4      = bb::StochasticLutBn<6>::Create(512);
    auto layer_sl5      = bb::StochasticLutBn<6>::Create(360);
    auto layer_sl6      = bb::StochasticLutBn<6>::Create(60);
    */

    std::cout << "binary mode : " << binary_mode << std::endl;

    {
        float bn_momentum = 0.01f;

        auto cnv0_sub = bb::Sequential::Create();
        cnv0_sub->Add(bb::StochasticLut6<>::Create(192));
        cnv0_sub->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { cnv0_sub->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }
        cnv0_sub->Add(bb::StochasticLut6<>::Create(32));
        cnv0_sub->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { cnv0_sub->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }

        auto cnv1_sub = bb::Sequential::Create();
        cnv1_sub->Add(bb::StochasticLut6<>::Create(192));
        cnv1_sub->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { cnv1_sub->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }
        cnv1_sub->Add(bb::StochasticLut6<>::Create(32));
        cnv1_sub->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { cnv1_sub->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }

        auto cnv2_sub = bb::Sequential::Create();
        cnv2_sub->Add(bb::StochasticLut6<>::Create(384));
        cnv2_sub->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { cnv2_sub->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }
        cnv2_sub->Add(bb::StochasticLut6<>::Create(64));
        cnv2_sub->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { cnv2_sub->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }

        auto cnv3_sub = bb::Sequential::Create();
        cnv3_sub->Add(bb::StochasticLut6<>::Create(384));
        cnv3_sub->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { cnv3_sub->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }
        cnv3_sub->Add(bb::StochasticLut6<>::Create(64));
        cnv3_sub->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { cnv3_sub->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }
        
        auto net = bb::Sequential::Create();
        if ( binary_mode ) {  net->Add(bb::RealToBinary<>::Create(7)); }
        net->Add(bb::LoweringConvolution<>::Create(cnv0_sub, 3, 3));
        net->Add(bb::LoweringConvolution<>::Create(cnv1_sub, 3, 3));
        net->Add(bb::MaxPooling<>::Create(2, 2));
        net->Add(bb::LoweringConvolution<>::Create(cnv2_sub, 3, 3));
        net->Add(bb::LoweringConvolution<>::Create(cnv3_sub, 3, 3));
        net->Add(bb::MaxPooling<>::Create(2, 2));
        net->Add(bb::StochasticLut6<>::Create(1024));
        net->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { net->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }
        net->Add(bb::StochasticLut6<>::Create(150));
        net->Add(bb::StochasticBatchNormalization<>::Create(bn_momentum));
        if ( binary_mode ) { net->Add(bb::Binarize<>::Create(0.5f, 0.0f, 1.0f)); }
        if ( binary_mode ) {
            net->Add(bb::BinaryToReal<>::Create(td.t_shape, 7));
        }
        else {
            net->Add(bb::Reduce<>::Create(td.t_shape));
        }
        net->SetInputShape(td.x_shape);

        net->SendCommand("lut_binarize false");

        // print model information
        net->PrintInfo();

        // run fitting
        bb::Runner<float>::create_t runner_create;
        runner_create.name               = net_name;
        runner_create.net                = net;
        runner_create.lossFunc           = bb::LossSoftmaxCrossEntropy<float>::Create();
        runner_create.metricsFunc        = bb::MetricsCategoricalAccuracy<float>::Create();
        runner_create.optimizer          = bb::OptimizerAdam<float>::Create();
//      runner_create.optimizer          = bb::OptimizerAdaGrad<float>::Create();
        runner_create.max_run_size       = max_run_size;    // 実際の1回の実行サイズ
        runner_create.file_read          = file_read;       // 前の計算結果があれば読み込んで再開するか
        runner_create.file_write         = true;            // 計算結果をファイルに保存するか
        runner_create.print_progress     = true;            // 途中結果を表示
        runner_create.initial_evaluation = false; // file_read;       // ファイルを読んだ場合は最初に評価しておく
        auto runner = bb::Runner<float>::Create(runner_create);
        runner->Fitting(td, epoch_size, mini_batch_size);
    }
}


// end of file
