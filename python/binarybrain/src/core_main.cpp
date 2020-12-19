// --------------------------------------------------------------------------
//  Binary Brain  -- binary neural net framework
//
//                                Copyright (C) 2018-2019 by Ryuji Fuchikami
//                                https://github.com/ryuz
//                                ryuji.fuchikami@nifty.com
// --------------------------------------------------------------------------


#define BB_PYBIND11


#include <omp.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/numpy.h>


#include "bb/Version.h"
#include "bb/DataType.h"

#include "bb/Tensor.h"
#include "bb/FrameBuffer.h"
#include "bb/Variables.h"

#include "bb/Sequential.h"
#include "bb/DenseAffine.h"
#include "bb/DepthwiseDenseAffine.h"
#include "bb/DifferentiableLutN.h"
#include "bb/DifferentiableLutDiscreteN.h"
#include "bb/BinaryLutN.h"
#include "bb/Reduce.h"
#include "bb/LoweringConvolution.h"
#include "bb/Shuffle.h"
#include "bb/BitEncode.h"
#include "bb/RealToBinary.h"
#include "bb/BinaryToReal.h"
#include "bb/BinaryModulation.h"
#include "bb/Sigmoid.h"
#include "bb/ReLU.h"
#include "bb/HardTanh.h"
#include "bb/Dropout.h"
#include "bb/BatchNormalization.h"
#include "bb/StochasticBatchNormalization.h"

#include "bb/LossFunction.h"
#include "bb/LossSoftmaxCrossEntropy.h"
#include "bb/LossMeanSquaredError.h"

#include "bb/MetricsFunction.h"
#include "bb/MetricsCategoricalAccuracy.h"
#include "bb/MetricsMeanSquaredError.h"

#include "bb/Optimizer.h"
#include "bb/OptimizerSgd.h"
#include "bb/OptimizerAdam.h"
#include "bb/OptimizerAdaGrad.h"

#include "bb/ValueGenerator.h"
#include "bb/NormalDistributionGenerator.h"
#include "bb/UniformDistributionGenerator.h"

#include "bb/Runner.h"
#include "bb/LoadMnist.h"
#include "bb/LoadCifar10.h"
#include "bb/ExportVerilog.h"

#ifdef BB_WITH_CUDA
#include "bbcu/bbcu.h"
#include "bbcu/bbcu_util.h"
#endif


using Tensor                       = bb::Tensor;
using FrameBuffer                  = bb::FrameBuffer;
using Variables                    = bb::Variables;

using Model                        = bb::Model;
using SparseLayer                  = bb::SparseLayer;
using Sequential                   = bb::Sequential;
using DenseAffine                  = bb::DenseAffine<float>;
using DepthwiseDenseAffine         = bb::DepthwiseDenseAffine<float>;
using LutLayer                     = bb::LutLayer<float, float>;
using LutLayerBit                  = bb::LutLayer<bb::Bit, float>;

using BinaryLut2                   = bb::BinaryLutN<2, float, float>;
using BinaryLut2Bit                = bb::BinaryLutN<2, bb::Bit, float>;
using BinaryLut4                   = bb::BinaryLutN<4, float, float>;
using BinaryLut4Bit                = bb::BinaryLutN<4, bb::Bit, float>;
using BinaryLut6                   = bb::BinaryLutN<6, float, float>;
using BinaryLut6Bit                = bb::BinaryLutN<6, bb::Bit, float>;

using DifferentiableLut2           = bb::DifferentiableLutN<2, float, float>;
using DifferentiableLut2Bit        = bb::DifferentiableLutN<2, bb::Bit, float>;
using DifferentiableLut4           = bb::DifferentiableLutN<4, float, float>;
using DifferentiableLut4Bit        = bb::DifferentiableLutN<4, bb::Bit, float>;
using DifferentiableLut5           = bb::DifferentiableLutN<5, float, float>;
using DifferentiableLut5Bit        = bb::DifferentiableLutN<5, bb::Bit, float>;
using DifferentiableLut6           = bb::DifferentiableLutN<6, float, float>;
using DifferentiableLut6Bit        = bb::DifferentiableLutN<6, bb::Bit, float>;

using StochasticLut2               = bb::StochasticLutN<2, float, float>;
using StochasticLut2Bit            = bb::StochasticLutN<2, bb::Bit, float>;
using StochasticLut4               = bb::StochasticLutN<4, float, float>;
using StochasticLut4Bit            = bb::StochasticLutN<4, bb::Bit, float>;
using StochasticLut5               = bb::StochasticLutN<5, float, float>;
using StochasticLut5Bit            = bb::StochasticLutN<5, bb::Bit, float>;
using StochasticLut6               = bb::StochasticLutN<6, float, float>;
using StochasticLut6Bit            = bb::StochasticLutN<6, bb::Bit, float>;

using Reduce                       = bb::Reduce<float, float>; 
using BinaryModulation             = bb::BinaryModulation<float, float>;
using BinaryModulationBit          = bb::BinaryModulation<bb::Bit, float>;
using RealToBinary                 = bb::RealToBinary<float, float>;
using RealToBinaryBit              = bb::RealToBinary<bb::Bit, float>;
using BinaryToReal                 = bb::BinaryToReal<float, float>;
using BinaryToRealBit              = bb::BinaryToReal<bb::Bit, float>;

using BitEncode                    = bb::BitEncode<float, float>;
using BitEncodeBit                 = bb::BitEncode<bb::Bit, float>;
using Shuffle                      = bb::Shuffle;

using Filter2d                     = bb::Filter2d<float, float>;
using Filter2dBit                  = bb::Filter2d<bb::Bit, float>;
using ConvolutionCol2Im            = bb::ConvolutionCol2Im <float, float>;
using ConvolutionCol2ImBit         = bb::ConvolutionCol2Im <bb::Bit, float>;
using ConvolutionIm2Col            = bb::ConvolutionIm2Col <float, float>;
using ConvolutionIm2ColBit         = bb::ConvolutionIm2Col <bb::Bit, float>;
using LoweringConvolution          = bb::LoweringConvolution<float, float>;
using LoweringConvolutionBit       = bb::LoweringConvolution<bb::Bit, float>;
using MaxPooling                   = bb::MaxPooling<float, float>;
using MaxPoolingBit                = bb::MaxPooling<bb::Bit, float>;

using Activation                   = bb::Activation;
using Binarize                     = bb::Binarize<float, float>;
using BinarizeBit                  = bb::Binarize<bb::Bit, float>;
using Sigmoid                      = bb::Sigmoid<float>;
using ReLU                         = bb::ReLU<float, float>;
using ReLUBit                      = bb::ReLU<bb::Bit, float>;
using HardTanh                     = bb::HardTanh<float, float>;
using Dropout                      = bb::Dropout<float, float>;
using BatchNormalization           = bb::BatchNormalization<float>;
using StochasticBatchNormalization = bb::StochasticBatchNormalization<float>;

using LossFunction                 = bb::LossFunction;
using LossMeanSquaredError         = bb::LossMeanSquaredError<float>;
using LossSoftmaxCrossEntropy      = bb::LossSoftmaxCrossEntropy<float>;

using MetricsFunction              = bb::MetricsFunction;
using MetricsCategoricalAccuracy   = bb::MetricsCategoricalAccuracy<float>;
using MetricsMeanSquaredError      = bb::MetricsMeanSquaredError<float>;

using Optimizer                    = bb::Optimizer;
using OptimizerSgd                 = bb::OptimizerSgd<float>;
using OptimizerAdam                = bb::OptimizerAdam<float>;
using OptimizerAdaGrad             = bb::OptimizerAdaGrad<float>;

using ValueGenerator               = bb::ValueGenerator<float>;
using NormalDistributionGenerator  = bb::NormalDistributionGenerator<float>;
using UniformDistributionGenerator = bb::UniformDistributionGenerator<float>;

using TrainData                    = bb::TrainData<float>;
using LoadMnist                    = bb::LoadMnist<float>;
using LoadCifar10                  = bb::LoadCifar10<float>;
using RunStatus                    = bb::RunStatus;
using Runner                       = bb::Runner<float>;



int GetDeviceCount(void)
{
#if BB_WITH_CUDA
    return bbcu_GetDeviceCount();
#else
    return 0;
#endif
}

void SetDevice(int device)
{
#if BB_WITH_CUDA
    bbcu_SetDevice(device);
#endif
}

std::string GetDevicePropertiesString(int device)
{
#if BB_WITH_CUDA
    return bbcu::GetDevicePropertiesString(device);
#else
    return "host only\n"
#endif
}

std::string MakeVerilog_FromLut(std::string module_name, std::vector< std::shared_ptr< bb::LutLayer<float, float> > > layers)
{
    std::stringstream ss;
    bb::ExportVerilog_LutLayers<float, float>(ss, module_name, layers);
    return ss.str();
}

std::string MakeVerilog_FromLutBit(std::string module_name, std::vector< std::shared_ptr< bb::LutLayer<bb::Bit, float> > > layers)
{
    std::stringstream ss;
    bb::ExportVerilog_LutLayers<bb::Bit, float>(ss, module_name, layers);
    return ss.str();
}


std::string MakeVerilogAxi4s_FromLutFilter2d(std::string module_name, std::vector< std::shared_ptr< bb::Filter2d<float, float> > > layers)
{
    std::stringstream ss;
    bb::ExportVerilog_LutCnnLayersAxi4s(ss, module_name, layers);
    return ss.str();
}

std::string MakeVerilogAxi4s_FromLutFilter2dBit(std::string module_name, std::vector< std::shared_ptr< bb::Filter2d<bb::Bit, float> > > layers)
{
    std::stringstream ss;
    bb::ExportVerilog_LutCnnLayersAxi4s(ss, module_name, layers);
    return ss.str();
}






//////////////////////////////////////]
// PyBind11 module
//////////////////////////////////////]

namespace py = pybind11;
PYBIND11_MODULE(core, m) {
    m.doc() = "BinaryBrain ver " + bb::GetVersionString();

    m.attr("__version__") = py::cast(BB_VERSION);

    m.attr("TYPE_BIT")    = BB_TYPE_BIT;
    m.attr("TYPE_BINARY") = BB_TYPE_BINARY;
    m.attr("TYPE_FP16")   = BB_TYPE_FP16;
    m.attr("TYPE_FP32")   = BB_TYPE_FP32;
    m.attr("TYPE_FP64")   = BB_TYPE_FP64;
    m.attr("TYPE_INT8")   = BB_TYPE_INT8;
    m.attr("TYPE_INT16")  = BB_TYPE_INT16;
    m.attr("TYPE_INT32")  = BB_TYPE_INT32;
    m.attr("TYPE_INT64")  = BB_TYPE_INT64;
    m.attr("TYPE_UINT8")  = BB_TYPE_UINT8;
    m.attr("TYPE_UINT16") = BB_TYPE_UINT16;
    m.attr("TYPE_UINT32") = BB_TYPE_UINT32;
    m.attr("TYPE_UINT64") = BB_TYPE_UINT64;

    m.attr("BB_BORDER_CONSTANT")    = BB_BORDER_CONSTANT;
    m.attr("BB_BORDER_CONSTANT")    = BB_BORDER_CONSTANT;
    m.attr("BB_BORDER_REFLECT")     = BB_BORDER_REFLECT;
    m.attr("BB_BORDER_REFLECT_101") = BB_BORDER_REFLECT_101;
    m.attr("BB_BORDER_REPLICATE")   = BB_BORDER_REPLICATE;
    m.attr("BB_BORDER_WRAP")        = BB_BORDER_WRAP;

    
    m.def("dtype_get_bit_size", &bb::DataType_GetBitSize);
    m.def("dtype_get_byte_size", &bb::DataType_GetByteSize);
    

    // Tensor
    py::class_< Tensor >(m, "Tensor")
        .def(py::init< bb::indices_t, int, bool >(),
            py::arg("shape"),
            py::arg("type")=BB_TYPE_FP32,
            py::arg("host_only")=false)
        .def("get_type", &Tensor::GetType)
        .def("get_shape", &Tensor::GetShape)
        .def(py::self + py::self)
        .def(py::self + double())
        .def(double() + py::self)
        .def(py::self - py::self)
        .def(py::self - double())
        .def(double() - py::self)
        .def(py::self * py::self)
        .def(py::self * double())
        .def(double() * py::self)
        .def(py::self / py::self)
        .def(py::self / double())
        .def(double() / py::self)
        .def(py::self += py::self)
        .def(py::self += double())
        .def(py::self -= py::self)
        .def(py::self -= double())
        .def(py::self *= py::self)
        .def(py::self *= double())
        .def(py::self /= py::self)
        .def(py::self /= double())
        .def("numpy_int8",   &Tensor::Numpy<std::int8_t>)
        .def("numpy_int16",  &Tensor::Numpy<std::int16_t>)
        .def("numpy_int32",  &Tensor::Numpy<std::int32_t>)
        .def("numpy_int64",  &Tensor::Numpy<std::int64_t>)
        .def("numpy_uint8",  &Tensor::Numpy<std::int8_t>)
        .def("numpy_uint16", &Tensor::Numpy<std::uint16_t>)
        .def("numpy_uint32", &Tensor::Numpy<std::uint32_t>)
        .def("numpy_uint64", &Tensor::Numpy<std::uint64_t>)
        .def("numpy_fp32",   &Tensor::Numpy<float>)
        .def("numpy_fp64",   &Tensor::Numpy<double>)
        .def_static("from_numpy_int8",   &Tensor::FromNumpy<std::int8_t>)
        .def_static("from_numpy_int16",  &Tensor::FromNumpy<std::int16_t>)
        .def_static("from_numpy_int32",  &Tensor::FromNumpy<std::int32_t>)
        .def_static("from_numpy_int64",  &Tensor::FromNumpy<std::int64_t>)
        .def_static("from_numpy_uint8",  &Tensor::FromNumpy<std::uint8_t>)
        .def_static("from_numpy_uint16", &Tensor::FromNumpy<std::uint16_t>)
        .def_static("from_numpy_uint32", &Tensor::FromNumpy<std::uint32_t>)
        .def_static("from_numpy_uint64", &Tensor::FromNumpy<std::uint64_t>)
        .def_static("from_numpy_fp32",   &Tensor::FromNumpy<float>)
        .def_static("from_numpy_fp64",   &Tensor::FromNumpy<double>)
        ;

    // FrameBuffer
    py::class_< FrameBuffer >(m, "FrameBuffer")
        .def(py::init< bb::index_t, bb::indices_t, int, bool>(),
            py::arg("frame_size") = 0,
            py::arg("shape") = bb::indices_t(),
            py::arg("data_type") = 0,
            py::arg("host_only") = false)
        .def("resize",  (void (FrameBuffer::*)(bb::index_t, bb::indices_t, int))&bb::FrameBuffer::Resize,
                py::arg("frame_size"),
                py::arg("shape"),
                py::arg("data_type") = BB_TYPE_FP32)
        .def("get_type", &FrameBuffer::GetType)
        .def("get_frame_size", &FrameBuffer::GetFrameSize)
        .def("get_frame_stride", &FrameBuffer::GetFrameStride)
        .def("get_node_size", &FrameBuffer::GetNodeSize)
        .def("get_node_shape", &FrameBuffer::GetShape)
        .def("range", &FrameBuffer::Range)
        .def("concatenate", &FrameBuffer::Concatenate)
        .def(py::self + py::self)
        .def(py::self + double())
        .def(double() + py::self)
        .def(py::self - py::self)
        .def(py::self - double())
        .def(double() - py::self)
        .def(py::self * py::self)
        .def(py::self * double())
        .def(double() * py::self)
        .def(py::self / py::self)
        .def(py::self / double())
        .def(double() / py::self)
        .def(py::self += py::self)
        .def(py::self += double())
        .def(py::self -= py::self)
        .def(py::self -= double())
        .def(py::self *= py::self)
        .def(py::self *= double())
        .def(py::self /= py::self)
        .def(py::self /= double())
        .def("numpy_int8",   &FrameBuffer::Numpy<std::int8_t>)
        .def("numpy_int16",  &FrameBuffer::Numpy<std::int16_t>)
        .def("numpy_int32",  &FrameBuffer::Numpy<std::int32_t>)
        .def("numpy_int64",  &FrameBuffer::Numpy<std::int64_t>)
        .def("numpy_uint8",  &FrameBuffer::Numpy<std::int8_t>)
        .def("numpy_uint16", &FrameBuffer::Numpy<std::uint16_t>)
        .def("numpy_uint32", &FrameBuffer::Numpy<std::uint32_t>)
        .def("numpy_uint64", &FrameBuffer::Numpy<std::uint64_t>)
        .def("numpy_fp32",   &FrameBuffer::Numpy<float>)
        .def("numpy_fp64",   &FrameBuffer::Numpy<double>)
        .def_static("from_numpy_int8",   &FrameBuffer::FromNumpy<std::int8_t>)
        .def_static("from_numpy_int16",  &FrameBuffer::FromNumpy<std::int16_t>)
        .def_static("from_numpy_int32",  &FrameBuffer::FromNumpy<std::int32_t>)
        .def_static("from_numpy_int64",  &FrameBuffer::FromNumpy<std::int64_t>)
        .def_static("from_numpy_uint8",  &FrameBuffer::FromNumpy<std::uint8_t>)
        .def_static("from_numpy_uint16", &FrameBuffer::FromNumpy<std::uint16_t>)
        .def_static("from_numpy_uint32", &FrameBuffer::FromNumpy<std::uint32_t>)
        .def_static("from_numpy_uint64", &FrameBuffer::FromNumpy<std::uint64_t>)
        .def_static("from_numpy_fp32",   &FrameBuffer::FromNumpy<float>)
        .def_static("from_numpy_fp64",   &FrameBuffer::FromNumpy<double>)
        .def_static("calc_frame_stride", &FrameBuffer::CalcFrameStride)
        ;
    
    // Variables
    py::class_< Variables, std::shared_ptr<Variables> >(m, "Variables")
        .def(py::init<>())
        .def("push_back", (void (Variables::*)(Variables const &))&Variables::PushBack)
        ;



    // ------------------------------------
    //  Models
    // ------------------------------------
    
    // model
    py::class_< Model, std::shared_ptr<Model> >(m, "Model")
        .def("get_name", &Model::GetName)
        .def("get_class_name", &Model::GetClassName)
        .def("get_info", &Model::GetInfoString,
                py::arg("depth")    = 0,
                py::arg("columns")  = 70,
                py::arg("nest")     = 0)
        .def("get_input_shape", &Model::GetInputShape)
        .def("set_input_shape", &Model::SetInputShape)
        .def("get_output_shape", &Model::GetOutputShape)
        .def("get_input_node_size", &Model::GetInputNodeSize)
        .def("get_output_node_size", &Model::GetOutputNodeSize)
        .def("get_parameters", &Model::GetParameters)
        .def("get_gradients", &Model::GetGradients)
        .def("forward_node",  &Model::ForwardNode)
        .def("forward",  &Model::Forward, "Forward",
                py::arg("x_buf"),
                py::arg("train") = true)
        .def("backward", &Model::Backward, "Backward")
        .def("send_command",  &Model::SendCommand, "SendCommand",
                py::arg("command"),
                py::arg("send_to") = "all")
        .def("backward", &Model::Backward, "Backward")
        .def("save_binary", &Model::SaveBinary)
        .def("load_binary", &Model::LoadBinary)
        .def("save_json", &Model::SaveJson)
        .def("load_json", &Model::LoadJson);
    
    
    py::class_< Reduce, Model, std::shared_ptr<Reduce> >(m, "Reduce")
        .def_static("create",   &Reduce::CreateEx);

    py::class_< BinaryModulation, Model, std::shared_ptr<BinaryModulation> >(m, "BinaryModulation")
        .def_static("create", &BinaryModulation::CreateEx,
                py::arg("layer"),
                py::arg("output_shape")              = bb::indices_t(),
                py::arg("depth_modulation_size")     = 1,
                py::arg("training_modulation_size")  = 1,
                py::arg("training_value_generator")  = nullptr,
                py::arg("training_framewise")        = true,
                py::arg("training_input_range_lo")   = 0.0f,
                py::arg("training_input_range_hi")   = 1.0f,
                py::arg("inference_modulation_size") = -1,
                py::arg("inference_value_generator") = nullptr,
                py::arg("inference_framewise")       = true,
                py::arg("inference_input_range_lo")  = 0.0f,
                py::arg("inference_input_range_hi")  = 1.0f);

    py::class_< BinaryModulationBit, Model, std::shared_ptr<BinaryModulationBit> >(m, "BinaryModulationBit")
        .def_static("create", &BinaryModulationBit::CreateEx,
                py::arg("layer"),
                py::arg("output_shape")              = bb::indices_t(),
                py::arg("depth_modulation_size")     = 1,
                py::arg("training_modulation_size")  = 1,
                py::arg("training_value_generator")  = nullptr,
                py::arg("training_framewise")        = true,
                py::arg("training_input_range_lo")   = 0.0f,
                py::arg("training_input_range_hi")   = 1.0f,
                py::arg("inference_modulation_size") = -1,
                py::arg("inference_value_generator") = nullptr,
                py::arg("inference_framewise")       = true,
                py::arg("inference_input_range_lo")  = 0.0f,
                py::arg("inference_input_range_hi")  = 1.0f);

    py::class_< RealToBinary, Model, std::shared_ptr<RealToBinary> >(m, "RealToBinary")
        .def_static("create", &RealToBinary::CreateEx,
                py::arg("frame_modulation_size") = 1,
                py::arg("depth_modulation_size") = 1,
                py::arg("value_generator")  = nullptr,
                py::arg("framewise")        = false,
                py::arg("input_range_lo")   = 0.0f,
                py::arg("input_range_hi")   = 1.0f);

    py::class_< RealToBinaryBit, Model, std::shared_ptr<RealToBinaryBit> >(m, "RealToBinaryBit")
        .def_static("create", &RealToBinaryBit::CreateEx,
                py::arg("frame_modulation_size") = 1,
                py::arg("depth_modulation_size") = 1,
                py::arg("value_generator")  = nullptr,
                py::arg("framewise")        = false,
                py::arg("input_range_lo")   = 0.0f,
                py::arg("input_range_hi")   = 1.0f);

    py::class_< BinaryToReal, Model, std::shared_ptr<BinaryToReal> >(m, "BinaryToReal")
        .def_static("create", &BinaryToReal::CreateEx,
                py::arg("frame_modulation_size") = 1,
                py::arg("output_shape")          = bb::indices_t());

    py::class_< BinaryToRealBit, Model, std::shared_ptr<BinaryToRealBit> >(m, "BinaryToRealBit")
        .def_static("create", &BinaryToRealBit::CreateEx,
                py::arg("frame_modulation_size") = 1,
                py::arg("output_shape")          = bb::indices_t());


    // DenseAffine
    py::class_< DenseAffine, Model, std::shared_ptr<DenseAffine> >(m, "DenseAffine")
        .def_static("create",   &DenseAffine::CreateEx, "create",
            py::arg("output_shape"),
            py::arg("initialize_std") = 0.01f,
            py::arg("initializer")    = "he",
            py::arg("seed")           = 1)
        .def("W", ((Tensor& (DenseAffine::*)())&DenseAffine::W))
        .def("b", ((Tensor& (DenseAffine::*)())&DenseAffine::b))
        .def("dW", ((Tensor& (DenseAffine::*)())&DenseAffine::dW))
        .def("db", ((Tensor& (DenseAffine::*)())&DenseAffine::db));
    
    // DepthwiseDenseAffine
    py::class_< DepthwiseDenseAffine, Model, std::shared_ptr<DepthwiseDenseAffine> >(m, "DepthwiseDenseAffine")
        .def_static("create",   &DepthwiseDenseAffine::CreateEx, "create",
            py::arg("output_shape"),
            py::arg("input_point_size")=0,
            py::arg("depth_size")=0,
            py::arg("initialize_std") = 0.01f,
            py::arg("initializer")    = "he",
            py::arg("seed")           = 1)
        .def("W", ((Tensor& (DepthwiseDenseAffine::*)())&DepthwiseDenseAffine::W))
        .def("b", ((Tensor& (DepthwiseDenseAffine::*)())&DepthwiseDenseAffine::b))
        .def("dW", ((Tensor& (DepthwiseDenseAffine::*)())&DepthwiseDenseAffine::dW))
        .def("db", ((Tensor& (DepthwiseDenseAffine::*)())&DepthwiseDenseAffine::db));


    // SparseLayer
    py::class_< SparseLayer, Model, std::shared_ptr<SparseLayer> >(m, "SparseLayer")
         .def("get_connection_size", &SparseLayer::GetConnectionSize)
         .def("set_connection", &SparseLayer::SetConnectionIndices)
         .def("get_connection", &SparseLayer::GetConnectionIndices)
         .def("set_connection_index", &SparseLayer::SetConnectionIndex)
         .def("get_connection_index", &SparseLayer::GetConnectionIndex)
         .def("get_node_connection_size", &SparseLayer::GetNodeConnectionSize)
         .def("set_node_connection_index", &SparseLayer::SetNodeConnectionIndex)
         .def("get_node_connection_index", &SparseLayer::GetNodeConnectionIndex);
    
    // DifferentiableLut6
    py::class_< DifferentiableLut6, SparseLayer, std::shared_ptr<DifferentiableLut6> >(m, "DifferentiableLut6")
        .def_static("create", &DifferentiableLut6::CreateEx, "create DifferentiableLut6",
                py::arg("output_shape"),
                py::arg("batch_norm") = true,
                py::arg("connection") = "",
                py::arg("momentum")   = 0.0,
                py::arg("gamma")      = 0.3,
                py::arg("beta")       = 0.5,
                py::arg("seed")       = 1)
        ;


    py::class_< StochasticLut6, SparseLayer, std::shared_ptr<StochasticLut6> >(m, "StochasticLut6")
        .def_static("create", &StochasticLut6::CreateEx, "create StochasticLut6",
                py::arg("output_shape"),
                py::arg("connection") = "",
                py::arg("seed") = 1);


    // filter
    py::class_< Filter2d, Model, std::shared_ptr<Filter2d> >(m, "Filter2d");


    py::class_< ConvolutionIm2Col, Model, std::shared_ptr<ConvolutionIm2Col> >(m, "ConvolutionIm2Col")
        .def_static("create", &ConvolutionIm2Col::CreateEx,
                py::arg("filter_h_size")=1,
                py::arg("filter_w_size")=1,
                py::arg("y_stride")=1,
                py::arg("x_stride")=1,
                py::arg("padding")="valid",
                py::arg("border_mode")=BB_BORDER_REFLECT_101)
        ;

    py::class_< ConvolutionIm2ColBit, Model, std::shared_ptr<ConvolutionIm2ColBit> >(m, "ConvolutionIm2ColBit")
        .def_static("create", &ConvolutionIm2ColBit::CreateEx,
                py::arg("filter_h_size")=1,
                py::arg("filter_w_size")=1,
                py::arg("y_stride")=1,
                py::arg("x_stride")=1,
                py::arg("padding")="valid",
                py::arg("border_mode")=BB_BORDER_REFLECT_101)
        ;

    py::class_< ConvolutionCol2Im, Model, std::shared_ptr<ConvolutionCol2Im> >(m, "ConvolutionCol2Im")
        .def_static("create", &ConvolutionCol2Im::CreateEx,
                py::arg("h_size")=1,
                py::arg("w_size")=1)
        ;

    py::class_< ConvolutionCol2ImBit, Model, std::shared_ptr<ConvolutionCol2ImBit> >(m, "ConvolutionCol2ImBit")
        .def_static("create", &ConvolutionCol2Im::CreateEx,
                py::arg("h_size")=1,
                py::arg("w_size")=1)
        ;


    // activation
    py::class_< Activation, Model, std::shared_ptr<Activation> >(m, "Activation");

    py::class_< Binarize, Activation, std::shared_ptr<Binarize> >(m, "Binarize")
        .def_static("create", &Binarize::CreateEx,
                py::arg("binary_th")    =  0.0f,
                py::arg("hardtanh_min") = -1.0f,
                py::arg("hardtanh_max") = +1.0f);
    
    py::class_< BinarizeBit, Activation, std::shared_ptr<BinarizeBit> >(m, "BinarizeBit")
        .def_static("create", &BinarizeBit::CreateEx,
                py::arg("binary_th")    =  0.0f,
                py::arg("hardtanh_min") = -1.0f,
                py::arg("hardtanh_max") = +1.0f);

    py::class_< Sigmoid, Binarize, std::shared_ptr<Sigmoid> >(m, "Sigmoid")
        .def_static("create",   &Sigmoid::Create);

    py::class_< ReLU, Binarize, std::shared_ptr<ReLU> >(m, "ReLU")
        .def_static("create",   &ReLU::Create);

    py::class_< ReLUBit, BinarizeBit, std::shared_ptr<ReLUBit> >(m, "ReLUBit")
        .def_static("create",   &ReLUBit::Create);

    py::class_< HardTanh, Binarize, std::shared_ptr<HardTanh> >(m, "HardTanh")
        .def_static("create", &HardTanh::CreateEx,
                py::arg("hardtanh_min") = -1.0,
                py::arg("hardtanh_max") = +1.0);

    
    py::class_< Dropout, Activation, std::shared_ptr<Dropout> >(m, "Dropout")
        .def_static("create", &Dropout::CreateEx,
                py::arg("rate") = 0.5,
                py::arg("seed") = 1);

    py::class_< BatchNormalization, Activation, std::shared_ptr<BatchNormalization> >(m, "BatchNormalization")
        .def_static("create", &BatchNormalization::CreateEx,
                py::arg("momentum")  = 0.9f,
                py::arg("gamma")     = 1.0f,
                py::arg("beta")      = 0.0f,
                py::arg("fix_gamma") = false,
                py::arg("fix_beta")  = false);

    py::class_< StochasticBatchNormalization, Activation, std::shared_ptr<StochasticBatchNormalization> >(m, "StochasticBatchNormalization")
        .def_static("create", &StochasticBatchNormalization::CreateEx,
                py::arg("momentum")  = 0.9,
                py::arg("gamma")     = 0.2,
                py::arg("beta")      = 0.5);

    // Loss Functions
    py::class_< LossFunction, std::shared_ptr<LossFunction> >(m, "LossFunction")
        .def("clear",          &LossFunction::Clear)
        .def("get_loss",       &LossFunction::GetLoss)
        .def("calculate_loss", &LossFunction::CalculateLoss,
            py::arg("y_buf"),
            py::arg("t_buf"),
            py::arg("mini_batch_size"));

    py::class_< LossSoftmaxCrossEntropy, LossFunction, std::shared_ptr<LossSoftmaxCrossEntropy> >(m, "LossSoftmaxCrossEntropy")
        .def_static("create", &LossSoftmaxCrossEntropy::Create);

    py::class_< LossMeanSquaredError, LossFunction, std::shared_ptr<LossMeanSquaredError> >(m, "LossMeanSquaredError")
        .def_static("create", &LossMeanSquaredError::Create);


    // Metrics Functions
    py::class_< MetricsFunction, std::shared_ptr<MetricsFunction> >(m, "MetricsFunction")
        .def("clear",              &MetricsFunction::Clear)
        .def("get_metrics",        &MetricsFunction::GetMetrics)
        .def("calculate_metrics",  &MetricsFunction::CalculateMetrics)
        .def("get_metrics_string", &MetricsFunction::GetMetricsString);

    py::class_< MetricsCategoricalAccuracy, MetricsFunction, std::shared_ptr<MetricsCategoricalAccuracy> >(m, "MetricsCategoricalAccuracy")
        .def_static("create", &MetricsCategoricalAccuracy::Create);

    py::class_< MetricsMeanSquaredError, MetricsFunction, std::shared_ptr<MetricsMeanSquaredError> >(m, "MetricsMeanSquaredError")
        .def_static("create", &MetricsMeanSquaredError::Create);


    // Optimizer
    py::class_< Optimizer, std::shared_ptr<Optimizer> >(m, "Optimizer")
        .def("set_variables", &Optimizer::SetVariables)
        .def("update",        &Optimizer::Update);
    
    py::class_< OptimizerSgd, Optimizer, std::shared_ptr<OptimizerSgd> >(m, "OptimizerSgd")
        .def_static("create", (std::shared_ptr<OptimizerSgd> (*)(float))&OptimizerSgd::Create, "create",
            py::arg("learning_rate") = 0.01f);
    
    py::class_< OptimizerAdam, Optimizer, std::shared_ptr<OptimizerAdam> >(m, "OptimizerAdam")
        .def_static("create", &OptimizerAdam::CreateEx,
            py::arg("learning_rate") = 0.001f,
            py::arg("beta1")         = 0.9f,
            py::arg("beta2")         = 0.999f); 
    
    py::class_< OptimizerAdaGrad, Optimizer, std::shared_ptr<OptimizerAdaGrad> >(m, "OptimizerAdaGrad")
        .def_static("Create", (std::shared_ptr<OptimizerAdaGrad> (*)(float))&OptimizerAdaGrad::Create,
            py::arg("learning_rate") = 0.01f);


    // ValueGenerator
    py::class_< ValueGenerator, std::shared_ptr<ValueGenerator> >(m, "ValueGenerator");
    
    py::class_< NormalDistributionGenerator, ValueGenerator, std::shared_ptr<NormalDistributionGenerator> >(m, "NormalDistributionGenerator")
        .def_static("create", &NormalDistributionGenerator::Create,
            py::arg("mean")   = 0.0f,
            py::arg("stddev") = 1.0f,
            py::arg("seed")   = 1);
    
    py::class_< UniformDistributionGenerator, ValueGenerator, std::shared_ptr<UniformDistributionGenerator> >(m, "UniformDistributionGenerator")
        .def_static("Create", &UniformDistributionGenerator::Create,
            py::arg("a")    = 0.0f,
            py::arg("b")    = 1.0f,
            py::arg("seed") = 1);
    

    // ------------------------------------
    //  Others
    // ------------------------------------

    // OpenMP
    m.def("omp_set_num_threads", &omp_set_num_threads);

    // CUDA device
    m.def("get_device_count",      &GetDeviceCount);
    m.def("set_device",            &SetDevice,                 py::arg("device") = 0);
    m.def("get_device_properties", &GetDevicePropertiesString, py::arg("device") = 0);

    // verilog
    m.def("make_verilog_from_lut", &MakeVerilog_FromLut);
    m.def("make_verilog_from_lut_bit", &MakeVerilog_FromLutBit);
    m.def("make_verilog_axi4s_from_lut_cnn", &MakeVerilogAxi4s_FromLutFilter2d);
    m.def("make_verilog_axi4s_from_lut_cnn_bit", &MakeVerilogAxi4s_FromLutFilter2dBit);

    m.def("get_version", &bb::GetVersionString);
}






#if 0




//////////////////////////////////////]
// PyBind11 module
//////////////////////////////////////]

namespace py = pybind11;
PYBIND11_MODULE(core, m) {
    m.doc() = "BinaryBrain ver " + bb::GetVersionString();

    m.attr("__version__") = py::cast(BB_VERSION);

    m.attr("TYPE_BIT")    = BB_TYPE_BIT;
    m.attr("TYPE_BINARY") = BB_TYPE_BINARY;
    m.attr("TYPE_FP16")   = BB_TYPE_FP16;
    m.attr("TYPE_FP32")   = BB_TYPE_FP32;
    m.attr("TYPE_FP64")   = BB_TYPE_FP64;
    m.attr("TYPE_INT8")   = BB_TYPE_INT8;
    m.attr("TYPE_INT16")  = BB_TYPE_INT16;
    m.attr("TYPE_INT32")  = BB_TYPE_INT32;
    m.attr("TYPE_INT64")  = BB_TYPE_INT64;
    m.attr("TYPE_UINT8")  = BB_TYPE_UINT8;
    m.attr("TYPE_UINT16") = BB_TYPE_UINT16;
    m.attr("TYPE_UINT32") = BB_TYPE_UINT32;
    m.attr("TYPE_UINT64") = BB_TYPE_UINT64;

    m.attr("BB_BORDER_CONSTANT")    = BB_BORDER_CONSTANT;
    m.attr("BB_BORDER_CONSTANT")    = BB_BORDER_CONSTANT;
    m.attr("BB_BORDER_REFLECT")     = BB_BORDER_REFLECT;
    m.attr("BB_BORDER_REFLECT_101") = BB_BORDER_REFLECT_101;
    m.attr("BB_BORDER_REPLICATE")   = BB_BORDER_REPLICATE;
    m.attr("BB_BORDER_WRAP")        = BB_BORDER_WRAP;


    // Tensor
    py::class_< Tensor >(m, "Tensor")
        .def(py::init< bb::indices_t, int, bool >(),
            py::arg("shape"),
            py::arg("type")=BB_TYPE_FP32,
            py::arg("host_only")=false)
        .def("get_type", &Tensor::GetType, doc__Tensor__get_type)
        .def("get_shape", &Tensor::GetShape, doc__Tensor__get_shape)
        .def("set_data", &Tensor::SetData<float>, doc__Tensor__set_data)
        .def("get_data", &Tensor::GetData<float>, doc__Tensor__get_data)
        .def("set_data_int32", &Tensor::SetData<int>, doc__Tensor__set_data_int32)
        .def("get_data_int32", &Tensor::GetData<int>, doc__Tensor__get_data_int32)
        .def("numpy_int8",   &Tensor::Numpy<std::int8_t>)
        .def("numpy_int16",  &Tensor::Numpy<std::int16_t>)
        .def("numpy_int32",  &Tensor::Numpy<std::int32_t>)
        .def("numpy_int64",  &Tensor::Numpy<std::int64_t>)
        .def("numpy_uint8",  &Tensor::Numpy<std::int8_t>)
        .def("numpy_uint16", &Tensor::Numpy<std::uint16_t>)
        .def("numpy_uint32", &Tensor::Numpy<std::uint32_t>)
        .def("numpy_uint64", &Tensor::Numpy<std::uint64_t>)
        .def("numpy_fp32",   &Tensor::Numpy<float>)
        .def("numpy_fp64",   &Tensor::Numpy<double>)
        .def_static("from_numpy_int8",   &Tensor::FromNumpy<std::int8_t>)
        .def_static("from_numpy_int16",  &Tensor::FromNumpy<std::int16_t>)
        .def_static("from_numpy_int32",  &Tensor::FromNumpy<std::int32_t>)
        .def_static("from_numpy_int64",  &Tensor::FromNumpy<std::int64_t>)
        .def_static("from_numpy_uint8",  &Tensor::FromNumpy<std::uint8_t>)
        .def_static("from_numpy_uint16", &Tensor::FromNumpy<std::uint16_t>)
        .def_static("from_numpy_uint32", &Tensor::FromNumpy<std::uint32_t>)
        .def_static("from_numpy_uint64", &Tensor::FromNumpy<std::uint64_t>)
        .def_static("from_numpy_fp32",   &Tensor::FromNumpy<float>)
        .def_static("from_numpy_fp64",   &Tensor::FromNumpy<double>)
        ;


    // FrameBuffer
    py::class_< FrameBuffer >(m, "FrameBuffer")
        .def(py::init< bb::index_t, bb::indices_t, int, bool>(), doc__FrameBuffer__init,
            py::arg("frame_size") = 0,
            py::arg("shape") = bb::indices_t(),
            py::arg("data_type") = 0,
            py::arg("host_only") = false)
        .def("resize",  (void (FrameBuffer::*)(bb::index_t, bb::indices_t, int))&bb::FrameBuffer::Resize, doc__FrameBuffer__resize,
                "resize",
                py::arg("frame_size"),
                py::arg("shape"),
                py::arg("data_type") = BB_TYPE_FP32)
        .def("get_type", &FrameBuffer::GetType, doc__FrameBuffer_get_type)
        .def("get_frame_size", &FrameBuffer::GetFrameSize)
        .def("get_node_size", &FrameBuffer::GetNodeSize)
        .def("get_node_shape", &FrameBuffer::GetShape)
        .def("range", &FrameBuffer::Range)
        .def("concatenate", &FrameBuffer::Concatenate)

        .def("set_data", &FrameBuffer::SetData<float>,
R"(set data to frames

    set data to frames

Args:
    data(List[List[float]]): data
    offset(int): offset
)",
                py::arg("data"),
                py::arg("offset") = 0)

        .def("get_data", &FrameBuffer::GetData<float>,
R"(get data from frames

    set data to frames

Args:
    size(int): size (If you specify 0 or less, it will be the size to the end)
    offset(int): offset
)",
                py::arg("size") = 0,
                py::arg("offset") = 0);


    // Variables
    py::class_< Variables, std::shared_ptr<Variables> >(m, "Variables");



    // ------------------------------------
    //  Models
    // ------------------------------------
    
    // model
    py::class_< Model, std::shared_ptr<Model> >(m, "Model")
        .def("get_name", &Model::GetName)
        .def("get_class_name", &Model::GetClassName)
        .def("get_info", &Model::GetInfoString, doc__Model_get_info,
                py::arg("depth")    = 0,
                py::arg("columns")  = 70,
                py::arg("nest")     = 0)
        .def("get_input_shape", &Model::GetInputShape)
        .def("set_input_shape", &Model::SetInputShape)
        .def("get_output_shape", &Model::GetOutputShape)
        .def("get_input_node_size", &Model::GetInputNodeSize)
        .def("get_output_node_size", &Model::GetOutputNodeSize)
        .def("get_parameters", &Model::GetParameters)
        .def("get_gradients", &Model::GetGradients)
        .def("forward_node",  &Model::ForwardNode)
        .def("forward",  &Model::Forward, "Forward",
                py::arg("x_buf"),
                py::arg("train") = true)
        .def("backward", &Model::Backward, "Backward")
        .def("send_command",  &Model::SendCommand, "SendCommand",
                py::arg("command"),
                py::arg("send_to") = "all")
        .def("backward", &Model::Backward, "Backward")
        .def("save_binary", &Model::SaveBinary)
        .def("load_binary", &Model::LoadBinary)
        .def("save_json", &Model::SaveJson)
        .def("load_json", &Model::LoadJson);
    
    // DenseAffine
    py::class_< DenseAffine, Model, std::shared_ptr<DenseAffine> >(m, "DenseAffine")
        .def_static("create",   &DenseAffine::CreateEx, "create",
            py::arg("output_shape"),
            py::arg("initialize_std") = 0.01f,
            py::arg("initializer")    = "he",
            py::arg("seed")           = 1)
        .def("W", ((Tensor& (DenseAffine::*)())&DenseAffine::W))
        .def("b", ((Tensor& (DenseAffine::*)())&DenseAffine::b))
        .def("dW", ((Tensor& (DenseAffine::*)())&DenseAffine::dW))
        .def("db", ((Tensor& (DenseAffine::*)())&DenseAffine::db));
    
    // DepthwiseDenseAffine
    py::class_< DepthwiseDenseAffine, Model, std::shared_ptr<DepthwiseDenseAffine> >(m, "DepthwiseDenseAffine")
        .def_static("create",   &DepthwiseDenseAffine::CreateEx, "create",
            py::arg("output_shape"),
            py::arg("input_point_size")=0,
            py::arg("depth_size")=0,
            py::arg("initialize_std") = 0.01f,
            py::arg("initializer")    = "he",
            py::arg("seed")           = 1)
        .def("W", ((Tensor& (DepthwiseDenseAffine::*)())&DepthwiseDenseAffine::W))
        .def("b", ((Tensor& (DepthwiseDenseAffine::*)())&DepthwiseDenseAffine::b))
        .def("dW", ((Tensor& (DepthwiseDenseAffine::*)())&DepthwiseDenseAffine::dW))
        .def("db", ((Tensor& (DepthwiseDenseAffine::*)())&DepthwiseDenseAffine::db));

    // SparseLayer
    py::class_< SparseLayer, Model, std::shared_ptr<SparseLayer> >(m, "SparseLayer")
         .def("get_connection_size", &SparseLayer::GetConnectionSize)
         .def("set_connection", &SparseLayer::SetConnectionIndices)
         .def("get_connection", &SparseLayer::GetConnectionIndices)
         .def("set_connection_index", &SparseLayer::SetConnectionIndex)
         .def("get_connection_index", &SparseLayer::GetConnectionIndex)
         .def("get_node_connection_size", &SparseLayer::GetNodeConnectionSize)
         .def("set_node_connection_index", &SparseLayer::SetNodeConnectionIndex)
         .def("get_node_connection_index", &SparseLayer::GetNodeConnectionIndex);


    py::class_< LutLayer, SparseLayer, std::shared_ptr<LutLayer> >(m, "LutLayer")
        .def("import_parameter", &LutLayer::ImportLayer);

    py::class_< LutLayerBit, SparseLayer, std::shared_ptr<LutLayerBit> >(m, "LutLayerBit")
        .def("import_parameter", &LutLayerBit::ImportLayer);
       
    py::class_< Sequential, Model, std::shared_ptr<Sequential> >(m, "Sequential")
        .def_static("create",   &Sequential::Create)
        .def("add",             &Sequential::Add);

    py::class_< Reduce, Model, std::shared_ptr<Reduce> >(m, "Reduce")
        .def_static("create",   &Reduce::CreateEx);

    py::class_< BinaryModulation, Model, std::shared_ptr<BinaryModulation> >(m, "BinaryModulation")
        .def_static("create", &BinaryModulation::CreateEx,
                py::arg("layer"),
                py::arg("output_shape")              = bb::indices_t(),
                py::arg("depth_modulation_size")     = 1,
                py::arg("training_modulation_size")  = 1,
                py::arg("training_value_generator")  = nullptr,
                py::arg("training_framewise")        = true,
                py::arg("training_input_range_lo")   = 0.0f,
                py::arg("training_input_range_hi")   = 1.0f,
                py::arg("inference_modulation_size") = -1,
                py::arg("inference_value_generator") = nullptr,
                py::arg("inference_framewise")       = true,
                py::arg("inference_input_range_lo")  = 0.0f,
                py::arg("inference_input_range_hi")  = 1.0f);

    py::class_< BinaryModulationBit, Model, std::shared_ptr<BinaryModulationBit> >(m, "BinaryModulationBit")
        .def_static("create", &BinaryModulationBit::CreateEx,
                py::arg("layer"),
                py::arg("output_shape")              = bb::indices_t(),
                py::arg("depth_modulation_size")     = 1,
                py::arg("training_modulation_size")  = 1,
                py::arg("training_value_generator")  = nullptr,
                py::arg("training_framewise")        = true,
                py::arg("training_input_range_lo")   = 0.0f,
                py::arg("training_input_range_hi")   = 1.0f,
                py::arg("inference_modulation_size") = -1,
                py::arg("inference_value_generator") = nullptr,
                py::arg("inference_framewise")       = true,
                py::arg("inference_input_range_lo")  = 0.0f,
                py::arg("inference_input_range_hi")  = 1.0f);

    py::class_< RealToBinary, Model, std::shared_ptr<RealToBinary> >(m, "RealToBinary")
        .def_static("create", &RealToBinary::CreateEx,
                py::arg("frame_modulation_size") = 1,
                py::arg("depth_modulation_size") = 1,
                py::arg("value_generator")  = nullptr,
                py::arg("framewise")        = false,
                py::arg("input_range_lo")   = 0.0f,
                py::arg("input_range_hi")   = 1.0f);

    py::class_< RealToBinaryBit, Model, std::shared_ptr<RealToBinaryBit> >(m, "RealToBinaryBit")
        .def_static("create", &RealToBinaryBit::CreateEx,
                py::arg("frame_modulation_size") = 1,
                py::arg("depth_modulation_size") = 1,
                py::arg("value_generator")  = nullptr,
                py::arg("framewise")        = false,
                py::arg("input_range_lo")   = 0.0f,
                py::arg("input_range_hi")   = 1.0f);

    py::class_< BinaryToReal, Model, std::shared_ptr<BinaryToReal> >(m, "BinaryToReal")
        .def_static("create", &BinaryToReal::CreateEx,
                py::arg(" modulation_size") = 1,
                py::arg("output_shape")     = bb::indices_t());

    py::class_< BinaryToRealBit, Model, std::shared_ptr<BinaryToRealBit> >(m, "BinaryToRealBit")
        .def_static("create", &BinaryToRealBit::CreateEx,
                py::arg(" modulation_size") = 1,
                py::arg("output_shape")     = bb::indices_t());


    py::class_< BinaryLut6, LutLayer, std::shared_ptr<BinaryLut6> >(m, "BinaryLut6")
        .def_static("create", &BinaryLut6::CreateEx,
R"(create BinaryLut6 object

    Args:
        output_shape (List[int]): shape of output frame
        seed (int): seed of random
)",
                py::arg("output_shape"),
                py::arg("seed") = 1);
    
    py::class_< BinaryLut6Bit, LutLayerBit, std::shared_ptr<BinaryLut6Bit> >(m, "BinaryLut6Bit")
        .def_static("create", &BinaryLut6Bit::CreateEx, "create",
                py::arg("output_shape"),
                py::arg("seed") = 1);
    
    py::class_< DifferentiableLut6, SparseLayer, std::shared_ptr<DifferentiableLut6> >(m, "DifferentiableLut6")
        .def_static("create", &DifferentiableLut6::CreateEx, "create DifferentiableLut6",
                py::arg("output_shape"),
                py::arg("batch_norm") = true,
                py::arg("connection") = "",
                py::arg("momentum")   = 0.0,
                py::arg("gamma")      = 0.3,
                py::arg("beta")       = 0.5,
                py::arg("seed")       = 1);

    py::class_< DifferentiableLut6Bit, SparseLayer, std::shared_ptr<DifferentiableLut6Bit> >(m, "DifferentiableLut6Bit")
        .def_static("create", &DifferentiableLut6Bit::CreateEx, "create DifferentiableLut6Bit",
                py::arg("output_shape"),
                py::arg("batch_norm") = true,
                py::arg("connection") = "",
                py::arg("momentum")   = 0.0,
                py::arg("gamma")      = 0.3,
                py::arg("beta")       = 0.5,
                py::arg("seed")       = 1);
    
    
    py::class_< StochasticLut6, SparseLayer, std::shared_ptr<StochasticLut6> >(m, "StochasticLut6")
        .def_static("create", &StochasticLut6::CreateEx, "create StochasticLut6",
                py::arg("output_shape"),
                py::arg("connection") = "",
                py::arg("seed") = 1);
    
    py::class_< StochasticLut6Bit, SparseLayer, std::shared_ptr<StochasticLut6Bit> >(m, "StochasticLut6Bit")
        .def_static("create", &StochasticLut6Bit::CreateEx, "create StochasticLut6Bit",
                py::arg("output_shape"),
                py::arg("connection") = "",
                py::arg("seed") = 1);
    
    py::class_< BitEncode, Model, std::shared_ptr<BitEncode> >(m, "BitEncode")
        .def_static("create", &BitEncode::CreateEx,
                py::arg("bit_size")=1,
                py::arg("output_shape") = bb::indices_t());

    py::class_< BitEncodeBit, Model, std::shared_ptr<BitEncodeBit> >(m, "BitEncodeBit")
        .def_static("create", &BitEncodeBit::CreateEx,
                py::arg("bit_size")=1,
                py::arg("output_shape") = bb::indices_t());

    py::class_< Shuffle, Model, std::shared_ptr<Shuffle> >(m, "Shuffle")
        .def_static("create", &Shuffle::CreateEx,
                py::arg("shuffle_unit"),
                py::arg("output_shape") = bb::indices_t());


    // filter
    py::class_< Filter2d, Model, std::shared_ptr<Filter2d> >(m, "Filter2d");

    py::class_< Filter2dBit, Model, std::shared_ptr<Filter2dBit> >(m, "Filter2dBit");

    py::class_< LoweringConvolution, Filter2d, std::shared_ptr<LoweringConvolution> >(m, "LoweringConvolution")
        .def_static("create", &LoweringConvolution::CreateEx,
                py::arg("layer"),
                py::arg("filter_h_size"),
                py::arg("filter_w_size"),
                py::arg("y_stride")      = 1,
                py::arg("x_stride")      = 1,
                py::arg("padding")       = "valid",
                py::arg("border_mode")   = BB_BORDER_REFLECT_101,
                py::arg("border_value")  = 0.0);

    py::class_< LoweringConvolutionBit, Filter2dBit, std::shared_ptr<LoweringConvolutionBit> >(m, "LoweringConvolutionBit")
        .def_static("create", &LoweringConvolutionBit::CreateEx,
                py::arg("layer"),
                py::arg("filter_h_size"),
                py::arg("filter_w_size"),
                py::arg("y_stride")      = 1,
                py::arg("x_stride")      = 1,
                py::arg("padding")       = "valid",
                py::arg("border_mode")   = BB_BORDER_REFLECT_101,
                py::arg("border_value")  = 0.0);
    
    py::class_< MaxPooling, Filter2d, std::shared_ptr<MaxPooling> >(m, "MaxPooling")
        .def_static("create", &MaxPooling::CreateEx,
                py::arg("filter_h_size"),
                py::arg("filter_w_size"));
    
    py::class_< MaxPoolingBit, Filter2dBit, std::shared_ptr<MaxPoolingBit> >(m, "MaxPoolingBit")
        .def_static("create", &MaxPoolingBit::CreateEx,
                py::arg("filter_h_size"),
                py::arg("filter_w_size"));

    // activation
    py::class_< Activation, Model, std::shared_ptr<Activation> >(m, "Activation");

    py::class_< Binarize, Activation, std::shared_ptr<Binarize> >(m, "Binarize")
        .def_static("create", &Binarize::CreateEx,
                py::arg("binary_th")    =  0.0f,
                py::arg("hardtanh_min") = -1.0f,
                py::arg("hardtanh_max") = +1.0f);
    
    py::class_< BinarizeBit, Activation, std::shared_ptr<BinarizeBit> >(m, "BinarizeBit")
        .def_static("create", &BinarizeBit::CreateEx,
                py::arg("binary_th")    =  0.0f,
                py::arg("hardtanh_min") = -1.0f,
                py::arg("hardtanh_max") = +1.0f);

    py::class_< Sigmoid, Binarize, std::shared_ptr<Sigmoid> >(m, "Sigmoid")
        .def_static("create",   &Sigmoid::Create);

    py::class_< ReLU, Binarize, std::shared_ptr<ReLU> >(m, "ReLU")
        .def_static("create",   &ReLU::Create);

    py::class_< ReLUBit, BinarizeBit, std::shared_ptr<ReLUBit> >(m, "ReLUBit")
        .def_static("create",   &ReLUBit::Create);

    py::class_< HardTanh, Binarize, std::shared_ptr<HardTanh> >(m, "HardTanh")
        .def_static("create", &HardTanh::CreateEx,
                py::arg("hardtanh_min") = -1.0,
                py::arg("hardtanh_max") = +1.0);

    
    py::class_< Dropout, Activation, std::shared_ptr<Dropout> >(m, "Dropout")
        .def_static("create", &Dropout::CreateEx,
                py::arg("rate") = 0.5,
                py::arg("seed") = 1);

    py::class_< BatchNormalization, Activation, std::shared_ptr<BatchNormalization> >(m, "BatchNormalization")
        .def_static("create", &BatchNormalization::CreateEx,
                py::arg("momentum")  = 0.9f,
                py::arg("gamma")     = 1.0f,
                py::arg("beta")      = 0.0f,
                py::arg("fix_gamma") = false,
                py::arg("fix_beta")  = false);

    py::class_< StochasticBatchNormalization, Activation, std::shared_ptr<StochasticBatchNormalization> >(m, "StochasticBatchNormalization")
        .def_static("create", &StochasticBatchNormalization::CreateEx,
                py::arg("momentum")  = 0.9,
                py::arg("gamma")     = 0.2,
                py::arg("beta")      = 0.5);

    // Loss Functions
    py::class_< LossFunction, std::shared_ptr<LossFunction> >(m, "LossFunction")
        .def("clear",          &LossFunction::Clear)
        .def("get_loss",       &LossFunction::GetLoss)
        .def("calculate_loss", &LossFunction::CalculateLoss,
            py::arg("y_buf"),
            py::arg("t_buf"),
            py::arg("mini_batch_size"));

    py::class_< LossSoftmaxCrossEntropy, LossFunction, std::shared_ptr<LossSoftmaxCrossEntropy> >(m, "LossSoftmaxCrossEntropy")
        .def_static("create", &LossSoftmaxCrossEntropy::Create);

    py::class_< LossMeanSquaredError, LossFunction, std::shared_ptr<LossMeanSquaredError> >(m, "LossMeanSquaredError")
        .def_static("create", &LossMeanSquaredError::Create);


    // Metrics Functions
    py::class_< MetricsFunction, std::shared_ptr<MetricsFunction> >(m, "MetricsFunction")
        .def("clear",              &MetricsFunction::Clear)
        .def("get_metrics",        &MetricsFunction::GetMetrics)
        .def("calculate_metrics",  &MetricsFunction::CalculateMetrics)
        .def("get_metrics_string", &MetricsFunction::GetMetricsString);

    py::class_< MetricsCategoricalAccuracy, MetricsFunction, std::shared_ptr<MetricsCategoricalAccuracy> >(m, "MetricsCategoricalAccuracy")
        .def_static("create", &MetricsCategoricalAccuracy::Create);

    py::class_< MetricsMeanSquaredError, MetricsFunction, std::shared_ptr<MetricsMeanSquaredError> >(m, "MetricsMeanSquaredError")
        .def_static("create", &MetricsMeanSquaredError::Create);


    // Optimizer
    py::class_< Optimizer, std::shared_ptr<Optimizer> >(m, "Optimizer")
        .def("set_variables", &Optimizer::SetVariables)
        .def("update",        &Optimizer::Update);
    
    py::class_< OptimizerSgd, Optimizer, std::shared_ptr<OptimizerSgd> >(m, "OptimizerSgd")
        .def_static("create", (std::shared_ptr<OptimizerSgd> (*)(float))&OptimizerSgd::Create, "create",
            py::arg("learning_rate") = 0.01f);
    
    py::class_< OptimizerAdam, Optimizer, std::shared_ptr<OptimizerAdam> >(m, "OptimizerAdam")
        .def_static("create", &OptimizerAdam::CreateEx,
            py::arg("learning_rate") = 0.001f,
            py::arg("beta1")         = 0.9f,
            py::arg("beta2")         = 0.999f); 
    
    py::class_< OptimizerAdaGrad, Optimizer, std::shared_ptr<OptimizerAdaGrad> >(m, "OptimizerAdaGrad")
        .def_static("Create", (std::shared_ptr<OptimizerAdaGrad> (*)(float))&OptimizerAdaGrad::Create,
            py::arg("learning_rate") = 0.01f);


    // ValueGenerator
    py::class_< ValueGenerator, std::shared_ptr<ValueGenerator> >(m, "ValueGenerator");
    
    py::class_< NormalDistributionGenerator, ValueGenerator, std::shared_ptr<NormalDistributionGenerator> >(m, "NormalDistributionGenerator")
        .def_static("create", &NormalDistributionGenerator::Create,
            py::arg("mean")   = 0.0f,
            py::arg("stddev") = 1.0f,
            py::arg("seed")   = 1);
    
    py::class_< UniformDistributionGenerator, ValueGenerator, std::shared_ptr<UniformDistributionGenerator> >(m, "UniformDistributionGenerator")
        .def_static("Create", &UniformDistributionGenerator::Create,
            py::arg("a")    = 0.0f,
            py::arg("b")    = 1.0f,
            py::arg("seed") = 1);


    // TrainData
    py::class_< TrainData >(m, "TrainData")
        .def_readwrite("x_shape", &TrainData::x_shape)
        .def_readwrite("t_shape", &TrainData::t_shape)
        .def_readwrite("x_train", &TrainData::x_train)
        .def_readwrite("t_train", &TrainData::t_train)
        .def_readwrite("x_test",  &TrainData::x_test)
        .def_readwrite("t_test",  &TrainData::t_test)
        .def("empty", &TrainData::empty);

    // LoadMNIST
    py::class_< LoadMnist >(m, "LoadMnist")
        .def_static("load", &LoadMnist::Load,
            py::arg("max_train") = -1,
            py::arg("max_test")  = -1,
            py::arg("num_class") = 10);
    
    // LoadCifar10
    py::class_< LoadCifar10 >(m, "LoadCifar10")
        .def_static("load", &LoadCifar10::Load,
            py::arg("num") = 5);

    
    // RunStatus
    py::class_< RunStatus >(m, "RunStatus")
        .def_static("WriteJson", &RunStatus::WriteJson)
        .def_static("ReadJson",  &RunStatus::ReadJson);


    // Runnner
    py::class_< Runner, std::shared_ptr<Runner> >(m, "CRunner")
        .def_static("create", &Runner::CreateEx,
            py::arg("name"),
            py::arg("net"),
            py::arg("lossFunc"),
            py::arg("metricsFunc"),
            py::arg("optimizer"),
            py::arg("max_run_size") = 0,
            py::arg("print_progress") = true,
            py::arg("print_progress_loss") = true,
            py::arg("print_progress_accuracy") = true,
            py::arg("log_write") = true,
            py::arg("log_append") = true,
            py::arg("file_read") = false,
            py::arg("file_write") = false,
            py::arg("write_serial") = false,
            py::arg("initial_evaluation") = false,
            py::arg("seed") = 1)
        .def("fitting", &Runner::Fitting,
            py::arg("td"),
            py::arg("epoch_size"),
            py::arg("batch_size"));

    
    // OpenMP
    m.def("omp_set_num_threads", &omp_set_num_threads);

    // CUDA device
    m.def("get_device_count",      &GetDeviceCount);
    m.def("set_device",            &SetDevice,                 py::arg("device") = 0);
    m.def("get_device_properties", &GetDevicePropertiesString, py::arg("device") = 0);

    // verilog
    m.def("make_verilog_from_lut", &MakeVerilog_FromLut);
    m.def("make_verilog_from_lut_bit", &MakeVerilog_FromLutBit);
    m.def("make_verilog_axi4s_from_lut_cnn", &MakeVerilogAxi4s_FromLutFilter2d);
    m.def("make_verilog_axi4s_from_lut_cnn_bit", &MakeVerilogAxi4s_FromLutFilter2dBit);

    m.def("get_version", &bb::GetVersionString);
}

#endif


// end of file
